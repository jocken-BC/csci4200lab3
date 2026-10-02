#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// ============================================================================
// SIMULATED HARDWARE STATE (Global Primitives - 256-Byte Memory Space)
// ============================================================================
uint16_t R[16];          // General Purpose Registers R0 through R15 (16-bit)
uint8_t  PC = 0;         // Program Counter (8-bit address for 256-byte MEM)
uint8_t  FLAG_Z = 0;     // Zero Flag (1 if last arithmetic result was 0, else 0)
uint8_t  FLAG_N = 0;     // Negative Flag (1 if high bit 15 was set, else 0)
bool     HALTED = false; // CPU Execution State

// 256-Byte Main RAM Array
uint8_t  MEM[256];

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

// Displays raw instruction hex word
void print_opcode_and_pc(uint16_t raw_inst) {
    printf("==================================================\n");
    printf(" EXECUTING INST: [0x%04X]  |  PC: 0x%02X\n", raw_inst, PC);
    printf("--------------------------------------------------\n");
}

// Displays current CPU state
void print_cpu_state(uint16_t raw_inst) {
    printf(" Flags: [Z=%d, N=%d]\n", FLAG_Z, FLAG_N);
    for (int i = 0; i < 16; i += 2) {
        printf("  R%2d: 0x%04X (%5d)  |  R%2d: 0x%04X (%5d)\n", 
                i, R[i], R[i], i+1, R[i+1], R[i+1]);
    }
    printf("==================================================\n\n");
}

// Bootloader: Reads a raw binary machine-code file into MEM starting at 0x00
void load_binary_file(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Error: Could not open binary file '%s'\n", filename);
        exit(1);
    }

    size_t bytes_read = fread(MEM, sizeof(uint8_t), 256, file);
    fclose(file);

    printf("Bootloader: Loaded %zu bytes into RAM from '%s'\n\n", bytes_read, filename);
}

// ============================================================================
// STUDENT TASK 1: FETCH INSTRUCTION (Little-Endian Assembly)
// ============================================================================

// Reads a 16-bit instruction word from MEM starting at address 'addr'
// Note: Instructions are stored in Little-Endian format in MEM[]!
uint16_t fetch_instruction(uint8_t addr) {
    // TODO: Read the low-order byte from MEM[addr]
    // TODO: Read the high-order byte from MEM[addr + 1]
    // TODO: Combine low and high bytes into a single 16-bit word and return it
    
    return 0x0000; // Placeholder
}

// ============================================================================
// STUDENT TASK 2: DECODE & EXECUTE
// ============================================================================

void decode_and_execute(uint16_t raw_inst) {
    // ------------------------------------------------------------------------
    // Step 1: Extract Instruction Fields using Bitwise Shifts (>>) and Masks (&)
    // ------------------------------------------------------------------------
    // TODO: Extract standard fields (opcode, rd, rs1, rs2_imm)
    // TODO: Extract 8-bit target address field for JMP (addr8)
    uint8_t opcode;
    uint8_t rd;
    uint8_t rs1;
    uint8_t rs2_imm;
    uint8_t addr8;
    
    // Default step: advance Program Counter to the next instruction (2 bytes)
    uint8_t next_pc = PC + 2;

    // ------------------------------------------------------------------------
    // Step 2: Hardware Execution & Dispatch
    // ------------------------------------------------------------------------
    // TODO: Implement switch (opcode) for:
    //       0x0: NOP
    //       0x1: ADD
    //       0x2: SUB
    //       0x3: AND
    //       0x4: OR
    //       0x6: LOAD
    //       0x7: STORE
    //       0x8: MOVI
    //       0x9: BEQ (PC-Relative)
    //       0xA: JMP (Absolute Address)
    //       0xF: HALT
    switch (opcode) {
        default:
            printf("Hardware Fault: Invalid opcode 0x%X at PC 0x%02X!\n", opcode, PC);
            HALTED = true;
            break;
    }

    // ------------------------------------------------------------------------
    // Step 3: Pre-provided Hardware Enforcements
    // ------------------------------------------------------------------------
    // Register R0 is hardwired to 0x0000. Any write to R0 is discarded.
    R[0] = 0;

    // Commit Program Counter update for the next instruction cycle
    PC = next_pc;
}

// ============================================================================
// MAIN EXECUTION DRIVER
// ============================================================================
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <program.bin>\n", argv[0]);
        return 1;
    }

    // 1. Initialize RAM array with zeros
    for (int i = 0; i < 256; i++) {
        MEM[i] = 0;
    }

    // 2. Load program file into memory
    load_binary_file(argv[1]);

    printf("Starting CPU Simulation...\n\n");

    // 3. Fetch-Decode-Execute Loop
    while (!HALTED) {
        uint16_t raw_inst = fetch_instruction(PC);
        print_opcode_and_pc(raw_inst);
        decode_and_execute(raw_inst);
        print_cpu_state(raw_inst);
    }

    printf("CPU Execution Halted Successfully.\n");
    return 0;
}
