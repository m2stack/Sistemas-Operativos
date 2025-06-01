#include <stdlib.h>
#include <stdio.h>
#include <string.h>

enum {
	SORT_FORWARD = 1,
	SORT_BACKWARD = 0
};

int
set_direction()
{
	char *env_var_value;

	env_var_value = getenv("DIRECTION");
	// Comprobamos si la variable de entorno está definida y no vale "forward"
	if (env_var_value != NULL && strcmp(env_var_value, "forward") != 0) {
		// Si la variable de entorno está definida con un valor incorrecto, el programa sale con error
		if (strcmp(env_var_value, "backward") != 0) {
			fprintf(stderr,
				"ERROR: valor incorrecto de la variable DIRECTION\n");
			exit(EXIT_FAILURE);
		}
		return SORT_BACKWARD;
	}
	return SORT_FORWARD;
}

int
if_relocate(char *arg1, char *arg2, int if_forward)
{
	// Se decide en qué orden se comparan los argumentos según la ordenación
	char *first = arg2, *second = arg1;

	if (if_forward) {
		first = arg1;
		second = arg2;
	}
	// Comparamos y devolvemos el valor pertinente
	if (strcmp(first, second) > 0) {
		return 1;
	}
	return 0;
}

void
sort(int argnum, char *args[], int if_forward)
{
	int i, j;
	char *tmp;

	for (i = 1; i < argnum; i++) {
		for (j = i + 1; j < argnum; j++) {
			if (if_relocate(args[i], args[j], if_forward)) {
				// En caso de que if_relocate devuelva 1 (TRUE), se intercambian posiciones
				tmp = args[j];
				args[j] = args[i];
				args[i] = tmp;
			}
		}
	}
}

int
main(int argc, char *argv[])
{
	int index = 1;

	sort(argc, argv, set_direction());
	// Fragmento de codigo encargado de imprimir el contenido de argv
	while (index < argc) {
		printf("%s\n", argv[index]);
		index++;
	}
	exit(EXIT_SUCCESS);
}
