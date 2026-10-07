#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MEM_SIZE 256
#define NUM_REGS 15

// Condition Codes (CS:APP Chapter 4 standard)
uint8_t   ZF = 0; // Zero Flag
uint8_t   SF = 0; // Sign Flag
uint8_t   OF = 0; // Overflow Flag (Extra Credit)

// Architectural State
uint8_t   MEM[MEM_SIZE];
uint16_t  R[NUM_REGS];
uint8_t   PC = 0;
bool      halted = false;

// Register Identifiers mapping to Book Naming
#define RAX  0
#define RCX  1
#define RDX  2
#define RBX  3
#define RSP  4
#define RBP  5
#define RSI  6
#define RDI  7
#define R8   8
#define R9   9
#define R10  10
#define R11  11
#define R12  12
#define R13  13
#define R14  14
#define RNONE 15 // 0xF indicates no register present

const char* reg_names[] = {
    "%rax", "%rcx", "%rdx", "%rbx", "%rsp", "%rbp", "%rsi", "%rdi",
    "%r8",  "%r9",  "%r10", "%r11", "%r12", "%r13", "%r14"
};

// Y86 Instruction Codes (icode)
#define I_NOP    0x0
#define I_HALT   0x1
#define I_RRMOVQ 0x2
#define I_IRMOVQ 0x3
#define I_RMMOVQ 0x4
#define I_MRMOVQ 0x5
#define I_OPQ    0x6
#define I_JXX    0x7

// Decoding Structure
typedef struct {
    uint8_t icode;
    uint8_t ifun;
    uint8_t rA;
    uint8_t rB;
    uint8_t imm_dest; // Holds either Imm4 or Dest8 address depending on format
} Instruction;

void print_trace() {
    printf("PC: 0x%02X | ZF: %d | SF: %d | OF: %d\n", PC, ZF, SF, OF);
    printf("Registers:\n");
    for (int i = 0; i < NUM_REGS; i++) {
        printf("%-4s: 0x%04X  ", reg_names[i], R[i]);
        if ((i + 1) % 5 == 0) printf("\n");
    }
    printf("\n------------------------------------------------------------\n");
}

void dump_memory(void) {
    printf("\n===================== MEMORY DUMP =====================\n");
    printf("Addr | 00 01 02 03 04 05 06 07  08 09 0A 0B 0C 0D 0E 0F\n");
    printf("-----+-------------------------------------------------\n");

    for (int i = 0; i < MEM_SIZE; i += 16) {
        // Print the 8-bit base address for the row
        printf("0x%02X | ", i);

        // Print 16 bytes per row, split with a middle visual separator for readability
        for (int j = 0; j < 16; j++) {
            printf("%02X ", MEM[i + j]);
            if (j == 7) {
                printf(" "); // Visual break between bytes 7 and 8
            }
        }
        printf("\n");
    }
    printf("=======================================================\n\n");
}


// --- SIMULATION STAGES ---

uint16_t fetch() {
    // TODO: Little-Endian Instruction Fetch (2 Bytes)
    // Return 16-bit raw instruction word from MEM[PC] and MEM[PC+1]
    // Be sure to implement a bounds check to prevent memory segmentation faults.
    return 0x1000;
}

Instruction decode(uint16_t instr_word) {
    Instruction inst;
    
    // TODO: Extract basic icode and ifun fields
    inst.icode = 0x1;
    inst.ifun  = 0x0;
    
    // TODO: Conditionally process layouts based on instruction class type
    // If it's a Jump (I_JXX), extract target address from lower byte (Layout B).
    // If standard layout, unpack rA, rB, and small immediate fields (Layout A).
    inst.rA = RNONE;
    inst.rB = RNONE;
    inst.imm_dest = 0;

    return inst;
}

void execute(Instruction inst) {
    uint8_t next_pc = PC + 2; 

    switch (inst.icode) {
        case I_NOP:
            break;

        case I_HALT:
            halted = true;
            break;

        case I_RRMOVQ:
            // TODO: Move value between registers: R[rB] = R[rA]
            break;

        case I_IRMOVQ:
            // TODO: Move immediate to register file: R[rB] = ZeroExtend(Imm4)
            break;

        case I_RMMOVQ:
            // TODO: Store 16-bit register context into byte-addressable memory at address R[rB]
            break;

        case I_MRMOVQ:
            // TODO: Load 16-bit word from memory at address R[rB] into register rA
            break;

        case I_OPQ:
            // Core Lab Requirements:
            if (inst.ifun == 0x0) { // addq
                // TODO: Perform R[rB] = R[rB] + R[rA]. Evaluate outcome to assert ZF and SF.
                // Evaluate and set the Overflow Flag (OF).
            } else if (inst.ifun == 0x1) { // subq
                // TODO: Perform R[rB] = R[rB] - R[rA]. Evaluate outcome to assert ZF and SF.
                // Evaluate and set the Overflow Flag (OF).
            }
            // EXTRA CREDIT:
            else if (inst.ifun == 0x2) { // andq
                // TODO: Perform bitwise AND. Clear OF to 0. Update ZF and SF.
            } else if (inst.ifun == 0x3) { // xorq
                // TODO: Perform bitwise XOR. Clear OF to 0. Update ZF and SF.
            }
            break;

        case I_JXX:
            // Core Lab Requirements:
            if (inst.ifun == 0x0) { // jmp (Unconditional)
                next_pc = inst.imm_dest;
            } else if (inst.ifun == 0x1) { // je (Conditional Equal / Zero)
                // TODO: Evaluate control branch: If ZF == 1, next_pc = inst.imm_dest;
            } 
            // EXTRA CREDIT: Implement remaining conditional jumps using textbook logic matrices
            else {
                bool take_branch = false;
                switch (inst.ifun) {
                    case 0x2: // jne
                        // TODO: take_branch = ...
                        break;
                    case 0x3: // js
                        // TODO: take_branch = ...
                        break;
                    case 0x4: // jns
                        // TODO: take_branch = ...
                        break;
                    case 0x5: // jle
                        // TODO: take_branch = ...
                        break;
                    case 0x6: // jl
                        // TODO: take_branch = ...
                        break;
                    case 0x7: // jge
                        // TODO: take_branch = ...
                        break;
                    case 0x8: // jg
                        // TODO: take_branch = ...
                        break;
                    default:
                        printf("Hardware Exception: Invalid branch type.\n");
                        break;
                }
                if (take_branch) next_pc = inst.imm_dest;
            }
            break;

        default:
            printf("Invalid Instruction Exception: icode 0x%X\n", inst.icode);
            halted = true;
            break;
    }

    PC = next_pc;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <binary_file.bin>\n", argv[0]);
        return 1;
    }

    for (int i = 0; i < MEM_SIZE; i++) MEM[i] = 0x00;
    for (int i = 0; i < NUM_REGS; i++) R[i] = 0x0000;

    FILE *file = fopen(argv[1], "rb");
    if (!file) {
        perror("Failed to parse target program stream");
        return 1;
    }
    size_t bytes_read = fread(MEM, 1, MEM_SIZE, file);
    fclose(file);
    printf("Loaded %zu bytes into CPU Memory.\n", bytes_read);

    while (!halted) {
        uint16_t raw_instr = fetch();
        Instruction inst = decode(raw_instr);
        
        printf("Executing Inst: [0x%04X] -> icode:0x%X ifun:0x%X rA:%d rB:%d Imm:0x%X\n", 
               raw_instr, inst.icode, inst.ifun, inst.rA, inst.rB, inst.imm_dest);
               
        execute(inst);
        print_trace();
        dump_memory();
    }
    return 0;
}

