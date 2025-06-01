#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>
#include <dirent.h>

enum {
	MAX_PATH_SIZE = 1024
};

struct PathData {
	char path[MAX_PATH_SIZE];
	int c_filenum;
	int h_filenum;
	off_t bytenum;
	int checkstat;		// contains EXIT_SUCCESS if all the data could be extracted correctly
};
typedef struct PathData PathData;

// The function below initializes a new PathData struct, allocating memory set to 0
PathData *
new_PathData()
{
	PathData *pdata = malloc(sizeof(PathData));

	if (!pdata) {
		fprintf(stderr,
			"Memory allocation failed while initializing PathData struct\n");
		exit(EXIT_FAILURE);
	}
	memset(pdata, 0, sizeof(PathData));
	return pdata;
}

void
update_path_data(PathData *pdata, int cnum, int hnum, off_t bnum)
{
	pdata->c_filenum += cnum;
	pdata->h_filenum += hnum;
	pdata->bytenum += bnum;
}

void
try_print_path_data(PathData *pdata)
{
	if (pdata->checkstat == EXIT_SUCCESS) {
		printf("%s\t%d\t%d\t%ld\n", pdata->path, pdata->c_filenum,
		       pdata->h_filenum, pdata->bytenum);
	}
}

DIR *
safe_open_dir(const char *path, PathData *pdata)
{
	DIR *dir = opendir(path);

	// When failing to open as a dir the path received, updates pdata->checkstat
	if (!dir) {
		printf("WARNING: non-valid path introduced: %s\n", path);
		pdata->checkstat = EXIT_FAILURE;
		return NULL;
	} else {
		strncpy(pdata->path, path, sizeof(pdata->path) - 1);
	}
	return dir;
}

void
try_count_ch_files(struct dirent *dir_entry, struct stat *filestat,
		   PathData *pdata)
{
	char *filextension = strrchr(dir_entry->d_name, '.');

	if (filextension && !S_ISLNK(filestat->st_mode)) {
		if (strcmp(filextension, ".c") == 0) {
			update_path_data(pdata, 1, 0, filestat->st_size);
		} else if (strcmp(filextension, ".h") == 0) {
			update_path_data(pdata, 0, 1, filestat->st_size);
		}
	}
}

PathData *
try_read_path(char *path)
{
	char filepath[MAX_PATH_SIZE];
	struct stat filestat;
	struct dirent *dir_entry;
	struct PathData *pdata = new_PathData();	// path data (will be returned)
	struct PathData *sddata = new_PathData();	// struct for subdirectories
	DIR *dir = safe_open_dir(path, pdata);	// opens the directory introduced

	while (pdata->checkstat != EXIT_FAILURE
	       && (dir_entry = readdir(dir)) != NULL) {
		// concats the path received with each element inside the entry
		snprintf(filepath, MAX_PATH_SIZE, "%s/%s", path,
			 dir_entry->d_name);
		// if the path is correct and is NOT . or .. then checks it
		if (stat(filepath, &filestat) != -1
		    && strcmp(dir_entry->d_name, ".") != 0
		    && strcmp(dir_entry->d_name, "..") != 0) {
			if (S_ISDIR(filestat.st_mode)) {
				// If the current entry is another directory, this function calls itself
				sddata = try_read_path(filepath);
				pdata->checkstat = sddata->checkstat;
				update_path_data(pdata, sddata->c_filenum,
						 sddata->h_filenum,
						 sddata->bytenum);
			} else {
				// else, check the .c and .h files and updates pdata if necessary
				try_count_ch_files(dir_entry, &filestat, pdata);
			}
		}
	}
	// closes the dir opened if necessary
	if (dir != NULL) {
		closedir(dir);
	}
	// free the memory allocated for sddata
	free(sddata);
	return pdata;
}

int
try_exit_usage(int argc)
{
	if (argc != 1) {
		fprintf(stderr, "usage: ./sourcefiles\n");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int
main(int argc, char *argv[])
{
	int exit_status;
	char path[MAX_PATH_SIZE];
	PathData *path_data;

	if ((exit_status = try_exit_usage(argc)) == EXIT_SUCCESS) {
		while (fgets(path, sizeof(path), stdin) != NULL) {
			// Remove trailing newline character
			path[strcspn(path, "\n")] = 0;
			// Extracts the data to a PathData struct and tries to print it
			path_data = try_read_path(path);
			try_print_path_data(path_data);
			exit_status = path_data->checkstat;
			// Free the memory allocated for the previous struct
			free(path_data);
		}
	}
	exit(exit_status);
}
