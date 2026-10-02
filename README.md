# Lab 3: Building a Fetch-Decode-Execute CPU Simulator in C

## 1. Overview
In this lab, you will build a functional, cycle-accurate instruction set simulator for a 16-bit RISC processor. By completing this assignment, you will bridge the gap between high-level code, machine instructions, and physical hardware behavior.

Rather than relying on pre-written abstractions, you will write the hardware logic for the **Fetch**, **Decode**, and **Execute** stages directly in C.

---

## 2. Learning Objectives
By the end of this lab, you will be able to:
1. **Manipulate Little-Endian Memory**: Reassemble multi-byte instruction words stored across 8-bit memory locations.
2. **Decode Machine Code**: Use C bitwise operators (`>>`, `&`, `|`) to extract opcodes, register specifiers, and immediate values from raw hexadecimal instruction words.
3. **Simulate CPU Architecture**: Implement hardware state transitions, ALU operations, register updates, and status flags (`FLAG_Z`, `FLAG_N`).
4. **Implement Control Flow**: Program absolute jumps and PC-relative conditional branching.

---

## 3. Hardware Architecture & System Specifications

### System Memory & Registers
* **Main Memory (`MEM`)**: 256 bytes of byte-addressable RAM.
* **General-Purpose Registers (`R0`–`R15`)**: Sixteen 16-bit registers.
  * **Rule**: Register `R0` is **hardwired to 0x0000**. Any write operation to `R0` is discarded by hardware.
* **Program Counter (`PC`)**: 8-bit register storing the memory address of the current instruction word.
* **Status Flags**:
  * **`FLAG_Z` (Zero Flag)**: Set to `1` if the result of an arithmetic operation is `0x0000`; otherwise `0`.
  * **`FLAG_N` (Negative Flag)**: Set to `1` if the most significant bit (bit 15) of an arithmetic result is `1`; otherwise `0`.

---

## 4. Instruction Set Architecture (ISA) Reference

Every instruction word is **16 bits wide** (2 bytes) and uses one of two binary layouts:

### Layout A: Standard Format (16-bit)
| Bits 15–12 | Bits 11–8 | Bits 7–4 | Bits 3–0 |
| :---: | :---: | :---: | :---: |
| **Opcode** (4 bits) | **Rd** / Target (4 bits) | **Rs1** (4 bits) | **Rs2** / **Imm4** (4 bits) |

### Layout B: Absolute Jump Format (`JMP`)
| Bits 15–12 | Bits 11–4 | Bits 3–0 |
| :---: | :---: | :---: |
| **Opcode** (`0xA`) | **Target Address `addr8`** (8 bits) | Unused (`0x0`) |

---

### Instruction Table

| Opcode | Mnemonic | Syntax | Hardware Operation |
| :---: | :--- | :--- | :--- |
| `0x0` | **NOP** | `NOP` | No operation. Advances `PC` by 2 bytes. |
| `0x1` | **ADD** | `ADD Rd, Rs1, Rs2` | R[Rd] = R[Rs1] + R[Rs2] ; Updates Z & N flags. |
| `0x2` | **SUB** | `SUB Rd, Rs1, Rs2` | R[Rd] = R[Rs1] - R[Rs2] ; Updates Z & N flags. |
| `0x3` | **AND** | `AND Rd, Rs1, Rs2` | R[Rd] = R[Rs1] & R[Rs2] ; Updates Z & N flags. |
| `0x4` | **OR** | `OR Rd, Rs1, Rs2` | R[Rd] = R[Rs1] \| R[Rs2] ; Updates Z & N flags. |
| `0x6` | **LOAD** | `LOAD Rd, [Rs1]` | Loads 16-bit word from MEM[R[Rs1]] into R[Rd]. |
| `0x7` | **STORE**| `STORE Rd, [Rs1]`| Stores 16-bit word from R[Rd] into MEM[R[Rs1]]. |
| `0x8` | **MOVI** | `MOVI Rd, Imm4` | R[Rd] = ZeroExtend(Imm4). |
| `0x9` | **BEQ** | `BEQ Imm4` | If FLAG_Z == 1, PC = PC + (Imm4 * 2). |
| `0xA` | **JMP** | `JMP addr8` | Unconditional jump: PC = addr8. |
| `0xF` | **HALT**| `HALT` | Halts CPU execution loop. |

---
 
## 5. Compiling and Testing

### Step 1: Compile Your Program
Build your C source file using `gcc`:

```bash
gcc -Wall cpu_decoder.c -o cpu_decoder
```

### Step 2: Execute with Test Binary
Run the compiled CPU binary by passing a pre-assembled machine-code file as an argument:

```bash
./cpu_decoder test_program.bin
```

---

## 6. Expected Execution Trace Sample

You are provided with several sample binary files to use as inputs to your program. The expected output for the `test_program.bin` file can be found in `text_program_output.txt`.

---

## 7. Post-Lab Deliverable Questions
Answer the following questions in your lab report submission:

1. **Little-Endian Analysis:** If `MEM[0x00] = 0x08` and `MEM[0x01] = 0x81`, what is the complete 16-bit hexadecimal instruction word fetched by `fetch_instruction(0x00)`? What instruction opcode does this represent?
2. **Branching Mechanics:** Explain why the branch instruction `BEQ` multiplies `rs2_imm` by 2 when calculating the branch target address in `next_pc`.
3. **Hardware Enforcement:** Describe what happens when an instruction attempts to execute `MOVI R0, 0x5`. Which line in the starter code prevents `R0` from storing `0x0005`?
4. **Sign & Zero Flags:** Write out the binary representation of `0xFFFF`. If an arithmetic operation produces `0xFFFF`, what values should be assigned to `FLAG_Z` and `FLAG_N`?

---

## 8. Project Submission
Zip together both your completed `cpu_decoder.c` file and a PDF containing the answer to the above questions, and submit the Zip file on Canvas.

---

## Optional Extra Credit: Reverse Assembly (up to +10 pts)
For up to 10 extra credit points, reverse-assemble the machine code contained in any of the provided binary files (other than `test_program.bin`) back into its human-readable Assembly source code.

For every 16-bit instruction word in your chosen file, provide a table in your report with the following columns:

| Memory Address | Hex Word | Bit Breakdown <br> (Opcode \| Rd \| Rs1 \| Rs2_Imm) | Assembly Mnemonic | Brief Hardware Explanation |
| :--- | :--- | :--- | :--- | :--- |
| **0x00** | 0x8101 | 1000 \| 0001 \| 0000 \| 0001 | `MOVI R1, 0x1` | Sets \$R[1] = 1. |
| **0x02** | *...* | *...* | *...* | *...* |
