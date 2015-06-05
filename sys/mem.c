#include <sys/sbunix.h>

// Pointer to physical address of pml4 base address
pml4* table_base_ptr=0;
unsigned char *bitmap; // bitmap to keep track of free/allocated memory
unsigned char *frame_ref_count;
void *kern_max;        // below it the memory is reserved
extern char physbase;
extern volatile char *video;
extern volatile char * VIDEO_ADDR;

int init_virtual_space_proc(pcb *proc){
	write_cr3((uint64_t)table_base_ptr);
    uint64_t pml4_process = (uint64_t)get_free_page();
    
   if(!pml4_process){
        return -1;
    }
    pml4 *vir_add, *pml4e_temp;
    
    // Get kernel pml4 physical address using self referencing trick.
    pml4e_temp = (pml4 *)get_self_ref_add(0x1FE,0x1FE,0x1FE,0x1FE);

    int i = 509;
    // Find first available slot in kernel pml4 to save the process pml4 address
//    for(i = 509; i>=0; i--){
//        if(!pml4e_temp->entries[i]){
            pml4e_temp->entries[i] = (((uint64_t) pml4_process) | PR | RW | US);
//	    break;
//        }
//    }
    
vir_add = (pml4 *)(get_self_ref_add(0x1FE, 0x1FE, 0x1FE, i));

memset(vir_add,0,sizeof(pml4)); 
    
vir_add->entries[511] = pml4e_temp->entries[511]; 

    // Self Referencing
    vir_add->entries[0x1FE] = (uint64_t)pml4_process | PR | RW | US;
	proc->cr3 = (uint64_t)pml4_process;
//	proc->kernel_pml4_offset = i;
	return 0;
}

uint64_t* get_free_page()
{
    for(uint64_t i=0; i<MAX_CHUNK; i++)
    {
        if(bitmap[i]!=255)
        {
            for(int j=0; j<8; j++)
            {
               if(!(bitmap[i] & (1<<j))){
                    bitmap[i] |= 1<<(j);
		    uint64_t phys_addr = (uint64_t)((i*8+j)*PAGE_SIZE); 
		    incr_ref_count(phys_addr);
                    return (uint64_t*)phys_addr;
                }
            }
        }
    }
    return (uint64_t*)-1;
}

uint64_t get_pml4_offset(uint64_t virt_addr)
{
    return 0x1ff & (virt_addr >> L4_SHIFT);
}

uint64_t get_pdp_offset(uint64_t virt_addr)
{
    return 0x1ff & (virt_addr >> L3_SHIFT);
}

uint64_t get_pd_offset(uint64_t virt_addr)
{
    return 0x1ff & (virt_addr >> L2_SHIFT);
}

uint64_t get_pt_offset(uint64_t virt_addr)
{
    return 0x1ff & (virt_addr >> L1_SHIFT);
}

void bitmap_init(void *physfree)
{
    kern_max= physfree + (uint64_t)(sizeof(char)*MAX_CHUNK);
    bitmap= (unsigned char *)((uint64_t)physfree);
    
    // Keeping Page descriptors just above the bit map. 
    frame_ref_count = (unsigned char *)((uint64_t)kern_max);
    kern_max += (uint64_t)(MAX_CHUNK*8);

    // Initializing bitmap
    int i=0;
    while(i<MAX_CHUNK)
    {
        *(bitmap+i)= 255;
        i++;
    }

    // Initializing Pgae ref count
    memset(frame_ref_count, 0, MAX_CHUNK*8);
}

/*void init_page_desc(void * physfree){
	uint32_t page_desc[MAX_CHUNK];
}*/
void set_bitmap(int page_no)
{
    int chunk_no= page_no/8;
    bitmap[chunk_no]|= 1<<(page_no%8);
}

void clear_bitmap(int page_no)
{
    int chunk_no= page_no/8;
    bitmap[chunk_no]&= ~(1<<(page_no%8));
}

int get_bitmap(int page_no)
{
    int chunk_no= page_no/8;
    return ((bitmap[chunk_no] & (1<<(page_no%8))) >> (page_no%8));
}

void incr_ref_count(uint64_t phys_add){
	frame_ref_count[phys_add/PAGE_SIZE]++;
}

void decr_ref_count(uint64_t phys_add){
	frame_ref_count[phys_add/PAGE_SIZE]--;
}

int get_ref_count(uint64_t phys_add)
{
	return frame_ref_count[phys_add/PAGE_SIZE];
}

void update_video_loc( uint64_t video_va)
{
    uint64_t trav = (uint64_t)(video - (VIDEO_ADDR));
    video = (volatile char*)(video_va + trav);
    VIDEO_ADDR = (volatile char *)(video_va);
}

void mem_track_init(uint64_t sbase, uint64_t slength, void *physfree)
{
    for(uint64_t page = sbase; page < sbase+slength; page+=PAGE_SIZE)
    {
        // page > 4096(any random number e.g. 100 will do) coz we dont want lower memories to be available.
        if(page > 4096 && (page<(uint64_t)&physbase ||  page>(uint64_t)kern_max))
            clear_bitmap(page/PAGE_SIZE);
        if(page >= 0xB8000 &&  page <= 0xBC096)
            set_bitmap(page/PAGE_SIZE);
    }
}

void map_page(uint64_t phys, uint64_t virt){
    pdp* pdpe;
    pd* pde;
    pt* pte;
    uint64_t* page;
    uint64_t* pde_entry;
    uint64_t* pdpe_entry;
    pml4* pml4_dir = (pml4*)table_base_ptr;
    
    uint64_t* e = &(pml4_dir->entries[get_pml4_offset(virt)]);
    
    if( !(*e & PR) ){
        pdp* pdpe_dir = (pdp*)get_free_page();
        memset(pdpe_dir, 0, sizeof(pdp));
        if(! pdpe_dir ){
//            printf("Out of memory while trying to allocate PDPE\n");
            return;
        }
        
        *e = (uint64_t)(pdpe_dir) | PR | RW | US;
    }
    
    pdpe = (pdp*)PAGE_PHYSICAL_ADDRESS(e);
    pdpe_entry = &(pdpe->entries[ get_pdp_offset(virt) ]);
    
    if( !(*pdpe_entry & PR) ){
        pd* pde_dir = (pd*)get_free_page();
        memset(pde_dir, 0, sizeof(pd));
        if( !pde_dir ){
//            printf("Out of memory while trying to allocate PDE\n");
            return;
        }
        *pdpe_entry = (uint64_t)(pde_dir) | PR | RW | US;

    }
    
    pde = (pd*)PAGE_PHYSICAL_ADDRESS(pdpe_entry);
    pde_entry = &(pde->entries[ get_pd_offset(virt) ]);
    
    if( !(*pde_entry & PR) ){
        pt* pte_dir = (pt*)get_free_page(); // Create lower level page table
        memset(pte_dir, 0, sizeof(pte_dir));
        if( !pte_dir ){
//            printf("Out of memory while trying to allocate PTE\n");
            return;
        }
        uint64_t* e2 = &(pde->entries[ get_pd_offset(virt) ]);
        *e2 = (uint64_t)(pte_dir) | PR | RW | US;
    }
    
    pte = (pt*)PAGE_PHYSICAL_ADDRESS(pde_entry);
    page = &(pte->entries[ get_pt_offset(virt) ]);
    *page = (uint64_t)phys | PR | RW | US;
}

void set_kernel_video_page(){
    uint64_t frame;
    
    pml4* pml4table = (pml4*)get_free_page();
    if(!pml4table){
        return;
    }
    memset(pml4table,0,sizeof(pml4));
    
    // Self Referencing
    uint64_t* e= &(pml4table->entries[0x1FE]);
    *e = (uint64_t)pml4table | PR | RW | US;
 
    table_base_ptr = pml4table;
    
    for(frame=(uint64_t)&physbase; frame<=(uint64_t)kern_max;frame=frame+PAGE_SIZE){
        map_page(frame, 0xFFFFFFFF80000000+frame);
    }
    
    map_page(0xB8000, 0xFFFFFFFF80100000);
//    map_page((uint64_t)bitmap, (uint64_t)(0xFFFFFFFF80000000 + bitmap)); 
	update_video_loc(0xFFFFFFFF80100000);
    bitmap = (unsigned char *)(0xFFFFFFFF80000000 + bitmap);
    frame_ref_count = (unsigned char *)(0xFFFFFFFF80000000 + frame_ref_count);
    kmalloc_bump_ptr = (uint64_t)(0xFFFFFFFF80000000 + kmalloc_bump_ptr);
    write_cr3((uint64_t)table_base_ptr);
}

uint64_t get_self_ref_add(uint64_t pml4_offset, uint64_t pdp_offset, uint64_t pd_offset, uint64_t pt_offset){
    uint64_t virt_add;
    virt_add = 0xFFFF000000000000;
     if(pml4_offset<256)
    virt_add = 0x0000000000000000;
    virt_add = (((virt_add >> (39)) | pml4_offset) << (39));
    virt_add = (((virt_add >> (30)) | pdp_offset) << (30));
    virt_add = (((virt_add >> (21)) | pd_offset) << (21));
    virt_add = (((virt_add >> (12)) | pt_offset) << (12));
    return virt_add;
}

uint64_t page_lookup(uint64_t vadd)
{
    int pml4eindex = get_pml4_offset(vadd);
    int pdpeindex = get_pdp_offset(vadd);
    int pdeindex = get_pd_offset(vadd);
    int pteindex = get_pt_offset(vadd);

  pml4 *pml4eAdd;
  pdp  *pdpeAdd;
  pd *pdeAdd;
  pt *pteAdd;

  pml4eAdd   = (pml4 *)(get_self_ref_add(0x1FE, 0x1FE, 0x1FE, 0x1FE));
  pdpeAdd    = (pdp *)(get_self_ref_add(0x1FE, 0x1FE, 0x1FE, pml4eindex));
  pdeAdd     = (pd *)(get_self_ref_add(0x1FE, 0x1FE, pml4eindex, pdpeindex));
  pteAdd     = (pt *)(get_self_ref_add(0x1FE, pml4eindex, pdpeindex, pdeindex));
  if(pml4eAdd->entries[pml4eindex]==0 || pdpeAdd->entries[pdpeindex]==0 || pdeAdd->entries[pdeindex]==0 || pteAdd->entries[pteindex]==0)
    return 0;
  return pteAdd->entries[pteindex];
}

void assign_page_with_self_ref(uint64_t virt_add)
{
    int pml4_offset = get_pml4_offset(virt_add);
    int pdp_offset = get_pdp_offset(virt_add);
    int pd_offset = get_pd_offset(virt_add);
    int pt_offset = get_pt_offset(virt_add);

    uint64_t *pml4e=0, *pdpe=0, *pde=0, *pte=0, *paddr=0;

    pml4e = (uint64_t *)get_self_ref_add(0x1FE,0x1FE,0x1FE,0x1FE);
    pdpe = (uint64_t *)pml4e[pml4_offset];

    if(!((uint64_t)pdpe & (uint64_t)0x1)){
        pdpe = (uint64_t *)get_free_page();
        pml4e[pml4_offset] = (((uint64_t)pdpe) & ALIGN_PAGE) | PR | RW | US;
        pdpe = (uint64_t *)get_self_ref_add(0x1FE, 0x1FE, 0x1FE, pml4_offset);
        memset(pdpe, 0, sizeof(pdp));
    }
    pdpe = (uint64_t *)get_self_ref_add(0x1FE, 0x1FE, 0x1FE, pml4_offset);

    pde = (uint64_t*)pdpe[pdp_offset];

    if(!((uint64_t)pde & (uint64_t)0x1)){
   pde = (uint64_t *)get_free_page();
        pdpe[pdp_offset] = (((uint64_t)pde) & ALIGN_PAGE) | PR | RW | US;
        pde = (uint64_t *)get_self_ref_add(0x1FE, 0x1FE, pml4_offset, pdp_offset);
        memset(pde, 0, sizeof(pd));
    }

    pde = (uint64_t *)get_self_ref_add(0x1FE, 0x1FE, pml4_offset, pdp_offset);

    pte = (uint64_t*)pde[pd_offset];
    
    if(!((uint64_t)pte & (uint64_t)0x1)){
   pte = (uint64_t *)get_free_page();
        pde[pd_offset] = (((uint64_t)pte) & ALIGN_PAGE) | PR | RW | US;
        pte = (uint64_t *)get_self_ref_add(0x1FE, pml4_offset, pdp_offset, pd_offset);
        memset(pte, 0, sizeof(pt));
    }
    pte = (uint64_t *)get_self_ref_add(0x1FE, pml4_offset, pdp_offset, pd_offset);
	paddr = (uint64_t*)pte[pt_offset];
    if(!((uint64_t)paddr & (uint64_t)0x1)){
    uint64_t paddr_new = (uint64_t )get_free_page();
       	pte[pt_offset] = (((uint64_t)paddr_new) & ALIGN_PAGE) | PR | RW | US;
       	//pte[pt_offset] = (((uint64_t)paddr_new)) | PR | RW | US;
    }

}

void assign_page_cow(uint64_t virt_add){
    int pml4_offset = get_pml4_offset(virt_add);
    int pdp_offset = get_pdp_offset(virt_add);
    int pd_offset = get_pd_offset(virt_add);
    int pt_offset = get_pt_offset(virt_add);
    char temp[4096];

    uint64_t *pte=0, *paddr=0;

    pte = (uint64_t *)get_self_ref_add(0x1FE, pml4_offset, pdp_offset, pd_offset);

    //paddr = (uint64_t*)pte[pt_offset];
    paddr = (uint64_t*)(pte[pt_offset] & ALIGN_PAGE); // FOR PAGE ALIGNMENT
    if(get_ref_count((uint64_t)paddr) > 1){
 	memcpy((char *)temp, (char *)virt_add, 4096);
    	uint64_t *paddr_new = (uint64_t *)get_free_page();
	decr_ref_count((uint64_t)paddr);
       	pte[pt_offset] = ((((uint64_t)paddr_new) & ALIGN_PAGE) | PR | RW | US) & ~COW_MASK;
	memcpy((char *)virt_add, (char *)temp, 4096);
    }
    else{
        pte[pt_offset] = ((((uint64_t)paddr) & ALIGN_PAGE) | PR | RW | US) & ~COW_MASK;
    }
}

 int calls=0;
void free_frame(void *virt_add, uint64_t phys_add){
	calls++;
    decr_ref_count(phys_add);
    // Means this was the last process referencing this physical frame.
    if(get_ref_count(phys_add) < 1){
 //       memset(virt_add, 0, PAGE_SIZE);
        clear_bitmap(phys_add/PAGE_SIZE);
    }
}

void delete_page_tables(pcb *parent){
write_cr3(parent->cr3);
    // Get parents virtual address
    pml4 *pml4_parent_virt_add = (pml4 *)(get_self_ref_add(0x1FE, 0x1FE, 0x1FE, 0x1FE));
    
    for(int j = 509; j >=0; j--){
        if(pml4_parent_virt_add->entries[j] & (uint64_t)PR){
            pdp *pdpe_parent_virt_add = (pdp *)get_self_ref_add(0x1FE,0x1FE, 0x1FE,j);
            // Loop for next table level i.e. pdpe
            for(int k = 511; k >= 0; k--){
                if(pdpe_parent_virt_add->entries[k] & PR){
                    pd *pde_parent_virt_add = (pd *)get_self_ref_add(0x1FE, 0x1FE,j,k);
                    
                    // Loop for next table level i.e. pd
                    for(int m = 511; m >= 0; m--){
                        if(pde_parent_virt_add->entries[m] & PR){
			     pt *pte_parent_virt_add = (pt *)get_self_ref_add(0x1FE,j,k,m);
                            // Loop the innermost ptf(j == 0) printf("m = %d", m);e to set the actual values and mark them as COW
			     for(int n = 511; n >= 0; n--){
				if(j == 0 && k == 0 && m == 0 && n == 0)
				{
					continue;
                                }
				    if(pte_parent_virt_add->entries[n] & PR){
                                    uint64_t frame_phys_addr = pte_parent_virt_add->entries[n] & ALIGN_PAGE;
				    free_frame((void *)get_self_ref_add(j,k,m,n), frame_phys_addr);
			//	    pte_parent_virt_add->entries[n] = 0x0;	
                                }
                              }
                            uint64_t frame_phys_addr = pde_parent_virt_add->entries[m] & ALIGN_PAGE;
                            free_frame((void *)get_self_ref_add(0x1FE,j,k,m), frame_phys_addr);
			  //  pde_parent_virt_add->entries[m] = 0x0;
                        }
                    }
                    
                    uint64_t frame_phys_addr = pdpe_parent_virt_add->entries[k] & ALIGN_PAGE;
                    free_frame((void *)get_self_ref_add(0x1FE,0x1FE,j,k), frame_phys_addr);
		  //  pdpe_parent_virt_add->entries[k] = 0x0;
                }
            }
            
            uint64_t frame_phys_addr = pml4_parent_virt_add->entries[j] & ALIGN_PAGE;
            free_frame((void *)get_self_ref_add(0x1FE,0x1FE,0x1FE,j), frame_phys_addr);
//	    pml4_parent_virt_add->entries[j] = 0x0;
        }
    }
//    free_frame((void *)pml4_parent_virt_add, parent->cr3);
    write_cr3((uint64_t)table_base_ptr);
    return;
}