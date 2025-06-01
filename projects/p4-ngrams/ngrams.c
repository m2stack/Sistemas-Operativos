#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

enum {
	BUFFER_SIZE = 1024,
	REQUIRED_ARGNUM = 3
};

// Circular buffer structure 
struct CircularBuffer {
	char data[BUFFER_SIZE];
	int start;
	int end;
	int count;
};
typedef struct CircularBuffer CircularBuffer;

int
try_exit_usage(int argc)
{
	if (argc != REQUIRED_ARGNUM) {
		fprintf(stderr, "usage: ./ngrams <file_route> <ngram_size>\n");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

void
init_circ_buffer(CircularBuffer *c_buffer)
{
	c_buffer->start = 0;
	c_buffer->end = 0;
	c_buffer->count = 0;
}

void
update_circ_buffer(CircularBuffer *c_buffer)
{
	c_buffer->start = (c_buffer->start + 1) % BUFFER_SIZE;	// Advance position
	c_buffer->count--;	// Reduce count
}

void
add_to_buffer(CircularBuffer *c_buffer, char c)
{
	c_buffer->data[c_buffer->end] = c;
	c_buffer->end = (c_buffer->end + 1) % BUFFER_SIZE;
	if (c_buffer->count < BUFFER_SIZE) {
		c_buffer->count++;
	} else {
		c_buffer->start = (c_buffer->start + 1) % BUFFER_SIZE;
	}
}

void
print_buffer_ngram(CircularBuffer *c_buffer, int nsize)
{
	int i;

	// Prints the current data from the buffer (only if the size matches)
	if (c_buffer->count == nsize) {
		printf("[");
		for (i = c_buffer->start; i < c_buffer->start + nsize; i++) {
			printf("%c", c_buffer->data[i % BUFFER_SIZE]);
		}
		printf("]\n");
		update_circ_buffer(c_buffer);
	}
}

int
try_print_ngrams(char *args[])
{
	char current_char, *filename = args[1];
	int fd, nsize = atoi(args[2]), ret_status = EXIT_FAILURE;
	CircularBuffer buffer;

	if (nsize <= 0 || nsize > BUFFER_SIZE) {
		// If a zero or negative number is introduced as ngram_size, return failure
		fprintf(stderr,
			"err: invalid ngram_size introduced: %d (max is 1024)\n",
			nsize);
	} else {
		fd = open(filename, O_RDONLY);
		if (fd == -1) {
			// If the file cannot be opened, we print an error and return failure
			perror("err: could not open origin file");
		} else {
			// In case the file was open correctly, we proceed as follows
			init_circ_buffer(&buffer);
			while (read(fd, &current_char, sizeof(char)) != 0) {
				// Add the current char read too the buffer and print the data
				add_to_buffer(&buffer, current_char);
				print_buffer_ngram(&buffer, nsize);
			}
			close(fd);
			ret_status = EXIT_SUCCESS;
		}
	}
	return ret_status;
}

int
main(int argc, char *argv[])
{
	int exit_status;

	if (try_exit_usage(argc)) {
		exit_status = EXIT_FAILURE;
	} else {
		exit_status = try_print_ngrams(argv);
	}
	exit(exit_status);
}
