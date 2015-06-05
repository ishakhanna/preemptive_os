#include <sys/tarfs.h>
#include <sys/sbunix.h>

char * tarfs_start = &_binary_tarfs_start;
char * iterator = &_binary_tarfs_start;
char * tarfs_end = &_binary_tarfs_end;
struct posix_header_ustar * header;

uint64_t file_size;
uint64_t header_size = sizeof(struct posix_header_ustar);
int f_count = 1;

char files[100][100];
uint64_t file_sizes[100];
int file_type[100];
int file_des[100];
char * file_address[100];

void inc(int global_id)
{
	if(global_id >= 0)
	{
		file_des[global_id] = file_des[global_id] + 1;
	}
}

void increment_file_desc(pcb * process)
{
	int i = 3;
	for(;i < MAX_OPEN_FILES;i++)
	{
		if(process->mapping[i] != 0)
		{
			int local_id = get_direct_local_fd(process, i);
			int global_id = process->file_descriptor[local_id];

			inc(global_id);
		}
	}
}

void init_tarfs()
{
//	printf("Initializing tarfs.. Please wait.\n");

	/* Special case handling for rootfs directory */
	strcpy(files[0], "");
	file_sizes[0] = 0;
	file_type[0] = 5;

	int j = 1;
	while(iterator < tarfs_end && j < 100)
	{
		header = (struct posix_header_ustar *) iterator;
		file_size = oct_to_dec(atoi(header->size));

		if(strlen(header->name) != 0)
		{
			strcpy(files[j], header->name);
			file_sizes[j] = file_size;
			file_type[j] = atoi(header->typeflag);
			file_address[j] = (char *)(iterator);
		}

		if (file_size % header_size)
		{
			file_size = ((file_size/header_size)+1) * header_size;
		}

		iterator += header_size + file_size;
		j++;
	}
//	printf("Tarfs initialized.\n");
}

int global_id(char * filename)
{
	int i = 0;

	while(files[i] != NULL)
	{
		if(strcmp(files[i], filename) == 0)
		{
			return i;
		}
		i++;
	}
	return -1;
}

void display_all_files()
{
//	printf("Displaying all files in tarfs...\n");

	int i = 0;

	while(strlen(files[i]) != 0)
	{
		printf("%s :%d\t", files[i], file_type[i]);
		i++;
	}
}

char * parse_filename(char * filename, pcb * process)
{
	char * final_filename = (char *)kmalloc1(100 * (sizeof(char)));

	if(filename == NULL || strlen(filename) == 0)
	{
		return NULL;
	}
	else
	{
		if(filename[0] == '/')
		{
			/* Should start from /rootfs/ */
			//printf("(Absolute)Parsed file: %s\n", filename + 8);
			return filename + 8;
		}
		else
		{
			char * cwd = get_current_working_directory(process);
			//printf("Current working directory =%s\n", cwd);
			int cwd_len = strlen(cwd);

			strncpy(final_filename, cwd + 8, cwd_len - 8);
			int len = strlen(final_filename);
			int f_len = strlen(filename);

			if(f_len == 1 && filename[0] == '.')
			{
				//printf("(relative)Parsed file: %s\n", final_filename);
				return final_filename;
			}
			else
			{
				strncpy(final_filename + len, filename, strlen(filename));
				//printf("(relative)Parsed file: %s\n", final_filename);
				return final_filename;

			}
		}
	}
}

int get_file_fd(char * filename)
{
	int fd = -1;

	int i = 1;
	while(strlen(files[i]) != 0 )
	{
		if(strcmp(files[i], filename) == 0)
		{
			fd = i;
			break;
		}
		i++;
	}

	return fd;
}

int tarfs_open(char * filename, int flags, pcb * process)
{
	if(flags == O_RDONLY)
	{
		filename = parse_filename(filename, process);

		int fd = get_file_fd(filename); // get file fd will give global fd

		if(fd != -1)
		{
			file_des[fd] = file_des[fd] + 1;
			return fd;
		}
		else
		{
			return -2; //MYENOENT
		}
	}
	else if(flags == O_DIRECTORY)
	{
		filename = parse_filename(filename, process);

		int len = strlen(filename);

		if(len == 0)
		{
			//printf("You are trying to open rootfs directory\n");
			file_des[0] = file_des[0] + 1;
			return 0;
		}

		if(filename[len - 1] != '/')
		{
			filename[len] = '/';
			filename[len + 1] = '\0';
		}
		else
		{
			filename[len] = '\0';
		}

		int fd = get_file_fd(filename);

		if(fd != -1)
		{
			file_des[fd] = file_des[fd] + 1;
			return fd;
		}
		else
		{
			return -2; //MYENOENT
		}
	}
	else
	{
		return -13; //MYEACCES;
	}
}

int tarfs_close(int fd)
{
	if(fd < 0)
	{
		return -9; //MYEBADF
	}

	int count = file_des[fd];

	if(count > 0)
	{
		file_des[fd] = count - 1;
		return 0;
	}
	else
	{
		return -9; //MYEBADF
	}
}

int tarfs_read_file(int global_fd,char * buf, int bytes_to_read, pcb * process, int local_fd)
{
	int bytes_read = 0;

	if(file_type[global_fd] == 0)
	{
		char * filename = files[global_fd];
		//printf("Reading the file: %s\n", filename);

		char * file = search_tarfs(filename);

		int start = process->f_pos[local_fd];

		for(;(bytes_read < bytes_to_read) && ((start + bytes_read) < file_sizes[global_fd]); bytes_read++)
		{
			//write_to_buffer(file[start + bytes_read]);
			strncpy(buf + bytes_read, file+(start+bytes_read), 1);
		}
		//write_to_buffer('\0');
		*(buf + bytes_read) = '\0';

		process->f_pos[local_fd] = start + bytes_read;

		return bytes_read;
	}
	else if(file_type[global_fd] == 5)
	{
		return -12; //EISDIR;
	}
	else
	{
		return -9; //EBADF
	}
}

int contains(char * filename, char * folder_name)
{/*
	if(strcmp(filename, folder_name))
	{
		return 1;
	}
*/

	int i = 0;
	while(folder_name[i] != '\0')
	{
		if(folder_name[i] != filename[i])
		{
			return -1;
		}
		i++;
	}

	//	printf("FIlename: %s, folder name: %s\n", filename, folder_name);

	if(folder_name[i] == '\0' && filename[i] == '\0')
	{
		return 2;
	}

	//printf("SEARCHING: %s\n",filename);

	while(1)
	{
		if(filename[i] != '\0' && filename[i] != '/')
		{
			i++;
			continue;
		}
		if(filename[i] == '\0')
		{
			// should be a file
			//printf("G=HERE for : %s\n", filename);
			return 1;
		}
		if(filename[i] == '/')
		{
			// check if the path continues.I fyes, then return -1 or if the path ends after '/' return 1;
			if(filename[i + 1] == '\0')
			{
				return 1;
			}
			else
			{
				return -1;
			}
		}
	}

	return 1;
}

char * remove_folder_name(char * filename, char * foldername)
{
	int len = strlen(foldername);
	return filename + len;
}

uint64_t tarfs_read_dir(int global_fd, char * buffer,  uint64_t bytes_to_read, pcb * process, int local_fd)
{
	int bytes_read = 0;
	int num_of_files_to_read = bytes_to_read / sizeof(struct dirent);

	int files_to_skip = process->f_pos[local_fd];

	if(file_type[global_fd] == 5)
	{
		char * filename = files[global_fd];

		int file_count = 0;
		int i = 1;
		int count = 0;

		for(;i < 100 ; i++)
		{
			if(strlen(files[i]) == 0)
			{
				break;
			}

			int result = 0;
			if((result = contains(files[i], filename)) >= 1 )
			{
				if(count < files_to_skip)
				{
					count++;
					continue;
				}
				if(file_count < num_of_files_to_read)
				{
					//printf("Reading the file: %s, result : %d\n", files[i], result);

					if(result == 2)
					{
						struct dirent * entry = (struct dirent *)kmalloc1(sizeof(struct dirent *));
						entry->d_ino = 0;
						strcpy(entry->d_name, ".");
						entry->d_off = 0; // need to modify this
						entry->d_reclen = sizeof(struct dirent); // need to modify this

						memcpy(buffer + bytes_read, (void * )entry, sizeof(struct dirent));
						//printf("Memory add: start: %p, bytes_read: %d, Res mem add: %p\n",buffer, bytes_read, (buffer + bytes_read));
						bytes_read += sizeof(struct dirent);
						file_count++;


						if(strcmp(process->cur_working_dir , "/rootfs/") != 0 || strcmp(process->cur_working_dir , "/rootfs") != 0)
						{
							struct dirent * entry = (struct dirent *)kmalloc1(sizeof(struct dirent *));
							entry->d_ino = 0;
							strcpy(entry->d_name, "..");
							entry->d_off = 0; // need to modify this
							entry->d_reclen = sizeof(struct dirent); // need to modify this

							memcpy(buffer + bytes_read, (void * )entry, sizeof(struct dirent));
							//printf("Memory add: start: %p, bytes_read: %d, Res mem add: %p\n",buffer, bytes_read, (buffer + bytes_read));
							bytes_read += sizeof(struct dirent);
							file_count++;
						}
					}
					else
					{
						if((result == 1) && (strcmp(process->cur_working_dir , "/rootfs/") != 0 || strcmp(process->cur_working_dir , "/rootfs") != 0))
						{

						}

						struct dirent * entry = (struct dirent *)kmalloc1(sizeof(struct dirent *));
						entry->d_ino = 0;

						char * file = remove_folder_name(files[i], filename);
						strcpy(entry->d_name, file);
						entry->d_off = 0; // need to modify this
						entry->d_reclen = sizeof(struct dirent); // need to modify this

						memcpy(buffer + bytes_read, (void * )entry, sizeof(struct dirent));
						//printf("Memory add: start: %p, bytes_read: %d, Res mem add: %p\n",buffer, bytes_read, (buffer + bytes_read));
						bytes_read += sizeof(struct dirent);
						file_count++;
					}
				}
			}						//i++;
		}
		process->f_pos[local_fd] += num_of_files_to_read;
		//printf("No of files read : %d\n", process->f_pos[global_fd]);
		return bytes_read;
	}
	else
	{
		return -9; //EBADF
	}
}


int tarfs_lseek(int global_fd, pcb * process, int local_fd, int pos, int whence)
{
	int cur_pos = process->f_pos[local_fd];
	int file_size = file_sizes[global_fd];

	switch(whence)
	{
		case 0:
			// From current
			cur_pos = pos;
			break;
		case 1:
			cur_pos = cur_pos + pos;
			break;
		case 2:
			cur_pos = file_size + pos;
			break;
	}

	process->f_pos[local_fd] = cur_pos;
	return cur_pos;
}

int string_len(char * str)
{
	int i = 0;
	while(str[i] != '\0')
	{
		i++;
	}
	return i;
}

int compare_path(char * tarfs_path, char * path)
{
	int i = 0;
	while(path[i] != '\0')
	{
		if(path[i] != tarfs_path[i])
		{
			return -1;
		}
		i++;
	}

	if(path[i - 1] == '/' && tarfs_path[i - 1] == '/')
	{
		return 0;
	}

	if(tarfs_path[i] == '/')
	{
		return 0;
	}
	else
		return -1;
}

int is_valid_dir(char * path)
{
	int i = 1;

	while(strlen(files[i]) != 0)
	{
		if(file_type[i] == 5)
		{
			if(strcmp(files[i], path) == 0)
			{
				return 0;
			}
		}
		i++;
	}

	if(strlen(path) == 0) // denotes "" directory
	{
		return 0;
	}

	printf("%s: is not a valid directory.\n", path);
	return -1;
}

char * remove_trailing_spaces(char * pathname)
{
	char * answer = (char *)kmalloc1(PATH_MAX * sizeof(char));
	memset(answer, 0, PATH_MAX);

	int i = 0;
	int j = 0;

	while(pathname[i] == ' ')
	{
		i++;
	}

	while(pathname[i] != ' ')
	{
		answer[j] = pathname[i];
		j++;
		i++;
	}

	answer[j] = '\0';
	return answer;
}

char * final_processing(char * full_path)
{
	char * answer = (char *) kmalloc1(PATH_MAX * sizeof(char));
	memset(answer, 0, PATH_MAX);

	int i = 0;
	int j = 0;

	while(full_path[i] != '\0')
	{
		if(full_path[i] != '.')
		{
			answer[j] = full_path[i];
			j++;
			i++;
			continue;
		}
		// skip './', do that we stay in same directory
		if(full_path[i] == '.' && ((full_path[i + 1] != '\0') && (full_path[i + 1] == '/')))
		{
			i++;
			i++;
			continue;
		}
		if(full_path[i] == '.' && ((full_path[i + 1] != '\0') && (full_path[i + 1] == '.'))
				&& ((full_path[i + 2] != '\0') && (full_path[i + 2] == '/')))
		{
			i++;
			i++;
			i++;
			// go above one directory
			if(j == 0)
			{
				// cannot go up anymore, so continue
				continue;
			}
			j--; // skip '/' just before
			j--;

			while(answer[j] != '/' && j != -1)
			{
				answer[j] = '\0';
				j--;
			}
			j++;

			continue;
		}
		else
		{
			answer[j] = full_path[i];
			i++;
			j++;
		}
	}
	answer[j] = '\0';
	return answer;
}

int tarfs_change_directory(pcb * process, char * pathname)
{
	if(pathname == NULL || pathname[0] == '\0')
	{
		printf("%s: is not a valid directory.\n", pathname);
		return -2; //ENOENT
	}

	pathname  = remove_trailing_spaces(pathname);

	if(pathname[0] == '/')
	{
		if(strlen(pathname) == 1 && pathname[0] == '/')
		{
			strcpy(process->cur_working_dir, "/rootfs/");
			return 0;
		}

		if(string_len(pathname) < 7)
		{
			printf("%s: is not a valid directory.\n", pathname);
			return -20; //ENOTDIR
		}

		if(string_len(pathname) == 7)
		{
			if(strcmp(pathname, "/rootfs") == 0)
			{
				strcpy(process->cur_working_dir, "/rootfs");
				return 0;
			}
			else
			{
				printf("%s: is not a valid directory.\n", pathname);
				return -20;
			}
		}

		if(strlen(pathname) == 8 && strcmp(pathname, "/rootfs/") == 0)
		{
			strcpy(process->cur_working_dir, "/rootfs/");
			return 0;
		}

		// skip /rootfs/, length =(char *)pathname + 8
		char * path_to_search = (char *)kmalloc1(PATH_MAX * sizeof(char));
		memset(path_to_search, 0, PATH_MAX);
		strncpy(path_to_search, pathname + 8, strlen(pathname) - 8);


		int full_len = strlen(path_to_search);

		if(path_to_search[full_len - 1] != '/')
		{
			path_to_search[full_len] = '/';
			path_to_search[++full_len] = '\0';
		}

		path_to_search = final_processing(path_to_search);

		if(is_valid_dir(path_to_search) == 0)
		{
			strcpy(process->cur_working_dir, "/rootfs/");
			strcpy(process->cur_working_dir + 8, path_to_search);
			return 0;
		}
		else
		{
			return -20;
		}
	}
	else
	{
		// relative pathname
		//printf("Relative pathname: %s, %d\n", pathname, strlen(pathname));
		char * cur_dir = process->cur_working_dir;
		int cur_len = strlen(cur_dir);

		if(cur_len < 7)
		{
			printf("Error in current working directory.\n");
			return -1;
		}

		char * full_path = (char *)kmalloc1(sizeof(char) * PATH_MAX);
		memset(full_path, 0, PATH_MAX);

		if(strcmp(cur_dir, "/rootfs") == 0 || strcmp(cur_dir, "/rootfs/") == 0)
		{
			// do nothing
		}
		else
		{
			strcpy(full_path, cur_dir + 8);  // skip /rootfs/ in
		}

		int full_len = strlen(full_path);
		strncpy(full_path + full_len, pathname, strlen(pathname));

		full_len  = strlen(full_path);

		if(full_path[full_len - 1] != '/')
		{
			full_path[full_len] = '/';
			full_path[++full_len] = '\0';
		}

		full_path = final_processing(full_path);

		if(is_valid_dir(full_path) == 0)
		{
			strcpy(process->cur_working_dir, "/rootfs/");
			strcpy(process->cur_working_dir + 8, full_path);
			return 0;
		}
		else
		{
			return -20;
		}
	}
	return 0;
}

