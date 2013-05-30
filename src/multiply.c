#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

enum opcodeType    {AND,
                    EOR,
                    SUB,
                    RSB,
                    ADD,
                    TST,
                    TEQ,
                    CMP,
                    ORR,
                    MOV};

int *registers;

// All the masks are up here
const uint32_t mask1 =   1; // 0000 0001
const uint32_t mask4 =  15; // 0000 1111

uint32_t generateMask(uint32_t length){

    uint32_t mask = 0;

    for(uint32_t i = 0; i < length; i ++){

        mask <<= 1;
        mask += 1;

    }
    return mask;
}

void printBits(uint32_t x) {
    
    int i;

    uint32_t mask = 1 << 31;

    for(i=0; i<32; ++i) {
        if((x & mask) == 0){
            printf("0");
        }else {
            printf("1");
        }
        mask >>= 1;
    }

    printf("\n");
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

void setFlagsMultiply(uint32_t result, uint32_t S) {
    //update flags
   
   if(S) {

    uint32_t cpsr = registers[16];

    // Set N if result is negative
    uint32_t N = result & (1 << 31); 
    // Set Z if result is zero
    uint32_t Z = (result == 0) << 30 ;
    printf("(Result == 0) = %d\n", (result == 0));
    //generate mask of 30 since V and C are unchanged
    uint32_t cpsrMask = generateMask(30);
    cpsr &= cpsrMask;

    printf("N    = "); printBits(N);
    printf("Z    = "); printBits(Z);
    printf("cpsr = "); printBits(cpsr);
    cpsr |= N | Z;
    printf("cpsr = "); printBits(cpsr);

    registers[16] = cpsr;
   }

}

void executeMultiply(uint32_t instruction) {

    uint32_t Rd = (instruction >> 16) & mask4;
    uint32_t Rn = (instruction >> 12) & mask4;
    uint32_t Rs = (instruction >> 8) & mask4;
    uint32_t Rm = instruction & mask4;
    uint32_t S = (instruction >> 20) & mask1;

    uint32_t temp = registers[Rm] * registers[Rs];
    printf("%u\n%u\n%u\n", registers[Rm], registers[Rs], temp);

    // if accumulate bit is set, then also add value of Rn
    if ((instruction >> 21) & mask1) {
        printf("ACCUMULATOR\n");
        temp += registers[Rn];
    } 
    registers[Rd] = temp;

    setFlagsMultiply(temp, S);



}

int main(int argc, char const *argv[]) {

    uint32_t instruction = atoi(argv[1]);

    registers = malloc(17 * sizeof(int)); // just put it here so it compiles but 
                       // can change it later 

    for(int x = 0; x < 17; ++x){
        registers[x] = 0;
    }

    /* testing with example from notes.
    use argument of 3759157650 for R0 = R1 * R2
    use argument of 3761254802 for R0 = R1 * R2 + R3
    */
    registers[1] = 4294967293;
    registers[2] = 15;
    registers[3] = 45;

    if ( checkCondition(instruction) == 1 ) {
        executeMultiply(instruction);
    } 

    printf("Registers[0] = %d\n", registers[0]);
    printf("CPSR = "); printBits(registers[16]);
    
    return 0;
}