#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

int32_t *registers = calloc(17, sizeof(int32_t));

unsigned char* loadbinary(const char *filepath) {

    unsigned char *memory = malloc(65536);

    // should malloc fail, print error message and return failure
    if (memory == NULL) {
        perror("malloc for memory failed");
        exit(EXIT_FAILURE);
    }

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

int checkCondition(uint32_t instruction) {

    //CSPR register values
    uint32_t cspr = registers[16];
    uint32_t N = mask1 & (cspr >> 31);
    uint32_t Z = mask1 & (cspr >> 30);
    uint32_t V = mask1 & (cspr >> 28);

    uint32_t condNumber = mask4 & (instruction >> 28);
    int condflag = 0;

    printf("instruction = %d\n", instruction);
    printf("Con Num = %d\n", condNumber);
    switch(condNumber) {

        case(0):
            // eq
            condflag = Z;
            break;
        case(1):
            // ne
            condflag = Z;
            break;
        case(10):
            // ge
            condflag = (N == V);
            break;
        case(11): 
            // lt
            condflag = (N != V);
            break;
        case(12):
            // gt
            condflag = ((Z == 0) && (N == V));
            break;
        case(13):
            //le
            condflag = ((Z == 1) || (N != V));
            break;
        case(14):
            // al (always 1)
            condflag = 1;
            break;
    }
    printf("ConditionFlag = %d\n", condflag);
    return condflag;
}

int main(int argc, char **argv) {

    assert(argc == 2);

    //sizeof char is always 1, so safe to assume malloc size is just total number of bytes

    unsigned char *memory = loadbinary(argv[1]);
    printf("%x\n", memory[6]);
}
