#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

enum {
	MIN_ARGS = 1
};

int
try_exit_usage(int num_of_args)
{
	if (num_of_args < MIN_ARGS) {
		fprintf(stderr, "usage: pinger ip/name [ip/name ...]\n");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int
exec_ping(char *direction)
{
	char *ping_command[] =
	    { "/bin/ping", "-c", "1", "-W", "5", direction, NULL };
	if (execv(ping_command[0], ping_command) == -1) {
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

pid_t
create_child(char *direction)
{
	pid_t pid;

	pid = fork();
	if (pid == 0) {		//HIJO
		return exec_ping(direction);
	} else if (pid < 0) {	//ERROR EN FORK
		return -1;
	}
	return pid;
}

int
try_concurrent_pings(int n_directions, pid_t * children_pids,
		     char *directions[])
{
	int i, status, function_status = EXIT_SUCCESS;
	pid_t child_pid;

	// Para cada parametro introducido, ejecutamos create_child
	for (i = 0; i < n_directions; i++) {
		child_pid = create_child(directions[i + 1]);
		if (child_pid == -1) {
			perror("fork");
			return EXIT_FAILURE;
		}
		children_pids[i] = child_pid;
	}
	// Iteramos sobre los pid de los hijos, esperando a cada uno
	for (i = 0; i < n_directions; i++) {
		if (waitpid(children_pids[i], &status, 0) == -1) {
			perror("waitpid");
			return EXIT_FAILURE;
		}
		// Verificamos si el hijo terminó de forma normal y su estado
		if (WIFEXITED(status)) {
			if (WEXITSTATUS(status) != EXIT_SUCCESS) {
				function_status = EXIT_FAILURE;
			}
		} else {
			fprintf(stderr,
				"err: child returned with unexpected behavior");
			function_status = EXIT_FAILURE;
		}
	}
	return function_status;
}

int
main(int argc, char *argv[])
{
	int n_directions = argc - 1, exit_status = EXIT_SUCCESS;
	pid_t *children_pids = malloc(n_directions * sizeof(pid_t));

	if (children_pids == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	// Si hay error en numero de argumentos o algun ping, saldra error
	if (try_exit_usage(n_directions)
	    || try_concurrent_pings(n_directions, children_pids, argv)) {
		exit_status = EXIT_FAILURE;
	}
	// Liberamos memoria y terminamos con el estado pertinente 
	free(children_pids);
	exit(exit_status);
}
