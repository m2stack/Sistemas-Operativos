#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

enum {
	MIN_ARGS = 3,
	MAX_ARGS = 4
};

int
try_exit_usage(int argc)
{
	if (argc < MIN_ARGS || argc > MAX_ARGS) {
		fprintf(stderr,
			"usage: ./copybytes origin_file destination_file buffer [bytes]\n");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

void
try_open(const char *path1, const char *path2, int *fd_origin, int *fd_dest)
{
	// If the first arg is '-' reads from standart input
	if (*path1 == '-') {
		*fd_origin = STDIN_FILENO;
	} else {
		// Tries to open origin file
		*fd_origin = open(path1, O_RDONLY);
		if (*fd_origin == -1) {
			perror("err: could not open origin file correctly");
		}
	}
	// If the second arg is '-' writes through standart output
	if (*path2 == '-') {
		*fd_dest = STDOUT_FILENO;
	} else {
		// Tries to open destination file
		*fd_dest =
		    open(path2, O_CREAT | O_WRONLY | O_TRUNC,
			 S_IWUSR | S_IRUSR);
		if (*fd_dest == -1) {
			perror
			    ("err: could not open destination file correctly");
			if (*fd_origin != -1) {
				close(*fd_origin);	// Close origin file
			}
		}
	}
}

void
try_close(int *fd)
{
	// Checks if the file descriptor is not null and not stdin or stdout
	if (fd != NULL && *fd >= 2) {
		if (close(*fd) == -1) {
			perror
			    ("err: something went wrong closing a file descriptor");
		}
	}
}

int
try_write(int *fd_origin, int *fd_dest, int buffsize, int maxbytes)
{
	int nread, nwritten = 0, write_exit_status = EXIT_SUCCESS;
	char *buffer = (char *)malloc(buffsize * sizeof(char));

	if (buffer == NULL) {
		fprintf(stderr,
			"err: unable to allocate memory for the buffer\n");
		return EXIT_FAILURE;
	}
	// Tries to read from the origin file descriptor specified
	while ((nread = read(*fd_origin, buffer, buffsize)) > 0) {
		// Controls the number of bytes to write
		if (maxbytes > 0 && nwritten + nread > maxbytes) {
			nread = maxbytes - nwritten;
		}
		// Tries to write the proper bytes
		if (write(*fd_dest, buffer, nread) != nread) {
			fprintf(stderr,
				"err: could not write to target file\n");
			write_exit_status = EXIT_FAILURE;
			break;
		}
		nwritten += nread;
		// If we reached the maximum bytes specified to copy, its over
		if (nwritten == maxbytes) {
			break;
		}
	}
	// We inform and exit with failure if something went wrong reading 
	if (nread == -1) {
		fprintf(stderr, "err: could not read correctly from origin\n");
		write_exit_status = EXIT_FAILURE;
	}

	free(buffer);
	return write_exit_status;
}

int
try_to_copy(int argnum, char **args)
{
	char *endbuffptr, *endbyteptr;
	int buffsize, maxbytes = -1;
	int fd_origin, fd_dest, status_of_write;

	// Initializes the buffer size and the max bytes to copy if introduced
	buffsize = (int)strtol(args[3], &endbuffptr, 10);
	if (argnum == MAX_ARGS) {
		maxbytes = (int)strtol(args[4], &endbyteptr, 10);
		if (maxbytes < 0 || (maxbytes != -1 && *endbyteptr != '\0')) {
			fprintf(stderr, "err: invalid num of bytes to copy\n");
			return EXIT_FAILURE;
		}
	}
	if (*endbuffptr != '\0' || buffsize < 0) {
		fprintf(stderr, "err: invalid buffer size\n");
		return EXIT_FAILURE;
	}
	// Tries to open the introduced files or standart input/output
	try_open(args[1], args[2], &fd_origin, &fd_dest);
	if (fd_origin == -1 || fd_dest == -1) {
		if (fd_dest != -1) {
			close(fd_dest);	// Closes the destination file if needed
		}
		return EXIT_FAILURE;
	}
	// Tries to write with the buffer and max bytes detailed before
	status_of_write = try_write(&fd_origin, &fd_dest, buffsize, maxbytes);
	try_close(&fd_origin);
	try_close(&fd_dest);
	return status_of_write;	// Returns the status given by the try_write function
}

int
main(int argc, char *argv[])
{
	int exit_status, argnum = argc - 1;

	// If there is an error in the number of arguments, exits with FAILURE status
	if (try_exit_usage(argnum)) {
		exit_status = EXIT_FAILURE;
	} else {
		exit_status = try_to_copy(argnum, argv);
	}
	exit(exit_status);
}
