#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>


void listFiles(const char *path) {
    DIR *dir;
    struct dirent *entry;
    struct stat file_info;
    char abs_path[MAX];

    if ((dir = opendir(path)) == NULL) {
        perror("Error al abrir directorio");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            snprintf(abs_path, sizeof(abs_path), "%s/%s", path, entry->d_name);
            if (lstat(abs_path, &file_info) == -1) {
                perror("Error al obtener información del archivo");
                exit(EXIT_FAILURE);
            }
            printf("%s\n", abs_path);
            if (S_ISDIR(file_info.st_mode)) {
                listFiles(abs_path);
            }
        }
    }

    closedir(dir);
}

int main() {
    char path[PATH_MAX];

    // Leer el path desde la entrada estándar
    if (fgets(path, PATH_MAX, stdin) == NULL) {
        perror("Error al leer el path");
        exit(EXIT_FAILURE);
    }

    // Eliminar el carácter de nueva línea si está presente
    if (path[strlen(path) - 1] == '\n') {
        path[strlen(path) - 1] = '\0';
    }

    printf("Contenido de %s:\n", path);
    listFiles(path);

    exit(EXIT_SUCCESS);
}
