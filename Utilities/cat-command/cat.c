#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 4096

int main(int argc, char *argv[]) {
    FILE *file;
    char buffer[BUFFER_SIZE];
    size_t bytesRead;

    if (argc != 2) {
        fprintf(stderr, "Uso: %s arquivo\n", argv[0]);
        return 1;
    }

    file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        fwrite(buffer, 1, bytesRead, stdout);
    }

    fclose(file);
    return 0;
}