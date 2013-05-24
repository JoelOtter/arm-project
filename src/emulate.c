#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

unsigned char* loadbinary(const char *filepath) {

    unsigned char *memory = malloc(65536);

    FILE *fp;
    unsigned char k;
    int first = 1;
    unsigned char j;
    int count = 0;

    if ((fp = fopen(filepath, "rb")) == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    while(1 == fread(&k,sizeof(k),1,fp)) {
        if (first) {
            j = k;
        }
        else {
            memory[count] = k;
            memory[++count] = j;
            ++count;
        }
        first = 1 - first;
    }

    fclose(fp);

    return memory;
}

int main(int argc, char **argv) {

    assert(argc == 2);

    //sizeof char is always 1, so safe to assume malloc size is just total number of bytes

    unsigned char *memory = loadbinary(argv[1]);
    printf("%x\n", memory[6]);
}
