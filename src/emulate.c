#include <stdlib.h>
#include <stdio.h>

void loadbinary(const char *filepath, char *dest) {

    FILE *fp;
    unsigned char k;
    int first = 1;
    unsigned char j;

    if ((fp = fopen(filepath, "rb")) == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    while(1 == fread(&k,sizeof(k),1,fp)) {
        if (first) {
            j = k;
            first = 1 - first;
        }
        else {
            printf("%x %x ", k, j);
            first = 1 - first;
        }
    }

    printf("\n");

    fclose(fp);
}

int main(int argc, char **argv) {

    char *memory = malloc(65536 * 8);

    loadbinary("../../testsuite/test_cases/add01", memory);
}
