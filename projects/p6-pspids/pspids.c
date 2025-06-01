#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>

enum {
	BUFFER_SIZE = 1024
};

int
try_exit_usage(int argc)
{
	if (argc != 3) {
		fprintf(stderr, "usage: ./pspids PID0 PID1\n");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

// returns SUCCESS only if the pids introduced are valid numbers
int
test_pids(int pid0, int pid1)
{
	int test_status = EXIT_SUCCESS;

	if (pid0 > pid1) {
		fprintf(stderr,
			"error: PID1 should be greater or equal than PID0\n");
		test_status = EXIT_FAILURE;
	} else if (pid0 <= 0) {
		fprintf(stderr, "error: PIDs should be positive numbers\n");
		test_status = EXIT_FAILURE;
	}
	return test_status;
}

void
safe_pipe(int fd[])
{
	if (pipe(fd) == -1) {
		perror("Error al crear el pipe");
		exit(EXIT_FAILURE);
	}
}

pid_t
safe_fork()
{
	pid_t pid = fork();

	if (pid < 0) {
		perror("error: could not fork correctly");
		exit(EXIT_FAILURE);
	}
	return pid;
}

// function executed by the child process
void
execute_ps(int fd[])
{
	close(fd[0]);
	dup2(fd[1], STDOUT_FILENO);	// redirects the command output to the pipe
	execlp("ps", "ps", "-eo", "%p %U %x %c", NULL);	// executes 'ps -eo "%p %U %x %c"'
	perror("Error en execlp()");
	exit(EXIT_FAILURE);
}

// function executed by the father process before waiting for his son
int
print_processes(int fd[], int pid_from, int pid_to)
{
	char process_buffer[BUFFER_SIZE], *line_data, *rest, *line_copy;
	int ppid;

	close(fd[1]);		// closes the pipe output and reads from its entry
	dup2(fd[0], STDIN_FILENO);
	while (fgets(process_buffer, BUFFER_SIZE, stdin) != NULL) {
		line_copy = strdup(process_buffer);
		if ((line_data = strtok_r(process_buffer, " ", &rest)) != NULL) {
			// convert PID string to integer
			ppid = atoi(line_data);
			// print the line only if the PID is within the range
			if (ppid >= pid_from && ppid <= pid_to) {
				printf("%s", line_copy);
			}
		}
		free(line_copy);
	}
	return EXIT_SUCCESS;
}

void
wait4son(pid_t son_pid)
{
	int son_status;

	if (waitpid(son_pid, &son_status, 0) == -1) {
		perror("error: waitpid");
		exit(EXIT_FAILURE);
	}
	// Check how the son finished executing
	if (WIFEXITED(son_status)) {
		// Prints the proper warning if the son ended with non-success status or by any signal
		if (WEXITSTATUS(son_status) != EXIT_SUCCESS) {
			fprintf(stderr,
				"WARNING: Command executed exited with non-zero status: %d\n",
				WEXITSTATUS(son_status));
		}
	} else if (WIFSIGNALED(son_status)) {
		fprintf(stderr,
			"WARNING: Command executed terminated by signal: %d\n",
			WTERMSIG(son_status));
	}
}

int
main(int argc, char *argv[])
{
	int exit_status, pid_from, pid_to, fd[2];
	pid_t process_pid;

	// checks if the number of arguments introduced is correct
	if ((exit_status = try_exit_usage(argc)) == EXIT_SUCCESS) {
		pid_from = atoi(argv[1]);
		pid_to = atoi(argv[2]);
		// unless both pids are valid numbers, does not execute but ends with failure
		if ((exit_status = test_pids(pid_from, pid_to)) != EXIT_FAILURE) {
			// creates a pipe and forks
			safe_pipe(fd);
			process_pid = safe_fork();
			if (process_pid == 0) {	// SON : executes the ps command
				execute_ps(fd);
			} else {	// FATHER : reads what the son sends through the pipe
				exit_status =
				    print_processes(fd, pid_from, pid_to);
				wait4son(process_pid);	// waits until his son finishes
				close(fd[0]);	// closes the reading file descriptor
			}
		}
	}
	exit(exit_status);
}
