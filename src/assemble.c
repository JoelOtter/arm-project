#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

void writeBinary(char *path, char *write){
    FILE *fp;
    int k = 3;
    int zeroHit = 0;
    int zero = 0;

    fp = fopen(path, "wb");
    while(1) {
        if (write[k] == 0){
            fwrite(&zero, sizeof(char), 1, fp);
            zeroHit = 1;
        }
        else {
            fwrite(&write[k], sizeof(write[k]), 1, fp);
        }
        if (k % 4 == 0 && zeroHit) break;
        else if (k % 4 == 0) k += 7;
        else --k;
    }
}

int main(int argc, char **argv) {
    assert(argc == 3);

    char *srcpath = argv[1];
    char *destpath = argv[2];

    char *toWrite = malloc(4 * sizeof(char));
    toWrite[0] = 1;
    toWrite[1] = 2;
    toWrite[2] = 3;
    toWrite[3] = 4;
    writeBinary(destpath, toWrite);
}
