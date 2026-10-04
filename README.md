# Lab 3: Building a Y86-16 Mini CPU Simulator in C

## 1. Overview
In this lab, you will build a functional, cycle-accurate instruction set simulator for the **Y86-16 Mini processor**. This custom architecture bridges the gap between high-level code, machine instructions, and physical hardware behavior. It utilizes the register naming conventions, condition codes, and instruction semantics introduced in **Chapter 4 of CS:APP**, but uses a fixed-width 16-bit format to simplify byte-stream parsing.

Rather than relying on pre-written abstractions, you will write the hardware logic for the **Fetch**, **Decode**, and **Execute** stages directly in C.

---

## 2. Learning Objectives
By the end of this lab, you will be able to:
1. **Manipulate Little-Endian Memory**: Reassemble multi-byte instruction words stored across 8-bit memory locations.
2. **Decode Machine Code**: Use C bitwise operators (`>>`, `&`, `|`) to extract opcodes (`icode`), function specifiers (`ifun`), and register IDs from raw machine words.
3. **Simulate CPU Architecture**: Implement hardware state transitions, ALU operations, register updates, and status flags (`ZF`, `SF`, and `OF`).
4. **Implement Control Flow**: Program absolute jumps and conditional branching based on combinational flag logic.

---

## 3. Hardware Architecture & System Specifications

### System Memory & Registers
* **Main Memory (`MEM`)**: 256 bytes of byte-addressable RAM.
* **The Register File (`R`)**: 15 architectural general-purpose registers (`%rax` to `%r14`) mapped to IDs `0` through `14`. 
  * Register ID `15` (`0xF`) represents **`RNONE`**, indicating the absence of a register specifier.
* **Program Counter (`PC`)**: 8-bit register storing the memory address of the current instruction word.
* **Condition Codes**:
  * **`ZF` (Zero Flag)**: Set to `1` if the result of an arithmetic operation is exactly `0x0000`; otherwise `0`.
  * **`SF` (Sign Flag)**: Set to `1` if the most significant bit (bit 15) of an arithmetic result is `1` (negative); otherwise `0`.
  * **`OF` (Overflow Flag)**: Set to `1` if an arithmetic operation results in a signed overflow; otherwise `0`.

---

## 4. Instruction Set Architecture (ISA) Reference

Every instruction word is **exactly 16 bits wide** (2 bytes) and adheres to one of three structural binary layouts:

### Layout A: Standard Register / Memory Format (16-bit)
Used by `nop`, `halt`, `rrmovq`, `rmmovq`, `mrmovq`, and `addq`/`subq`.

| Bits 15–12 | Bits 11–8 | Bits 7–4 | Bits 3–0 |
| :---: | :---: | :---: | :---: |
| **icode** (Opcode) | **ifun** (Function) | **rA** (Source Register) | **rB** (Destination Register) |

### Layout B: Register-Immediate Format (`irmovq`) (16-bit)
Used exclusively by `irmovq`. To maximize bit-budget efficiency, the literal 4-bit value is stored where `rA` would normally be, and `rB` acts as the target destination register.

| Bits 15–12 | Bits 11–8 | Bits 7–4 | Bits 3–0 |
| :---: | :---: | :---: | :---: |
| **icode** (`0x3`) | **ifun** (`0x0`) | **Imm4** (4-bit Immediate Value) | **rB** (Destination Register) |

### Layout C: Jump Format (`jXX`) (16-bit)

| Bits 15–12 | Bits 11–8 | Bits 7–0 |
| :---: | :---: | :---: |
| **icode** (`0x7`) | **ifun** (Condition Code) | **Target Address `dest8`** |

---

### Y86-16 Mini Instruction Matrix

| icode | ifun | Mnemonic | Syntax | Hardware Operation |
| :---: | :---: | :--- | :--- | :--- |
| `0x0` | `0x0` | **nop** | `nop` | No operation. `PC += 2`. |
| `0x1` | `0x0` | **halt** | `halt` | Halts execution loop. |
| `0x2` | `0x0` | **rrmovq** | `rrmovq rA, rB` | `R[rB] = R[rA]` ; `PC += 2`. |
| `0x3` | `0x0` | **irmovq** | `irmovq Imm4, rB` | `R[rB] = ZeroExtend(Imm4)` ; `PC += 2`. |
| `0x4` | `0x0` | **rmmovq** | `rmmovq rA, (rB)` | `MEM[R[rB]] = R[rA]` (16-bit store) ; `PC += 2`. |
| `0x5` | `0x0` | **mrmovq** | `mrmovq (rB), rA` | `R[rA] = MEM[R[rB]]` (16-bit load) ; `PC += 2`. |
| `0x6` | `0x0` <br> `0x1` | **addq** <br> **subq** | `addq rA, rB` <br> `subq rA, rB` | `R[rB] = R[rB] + R[rA]` ; Updates ZF, SF, OF ; `PC += 2`. <br> `R[rB] = R[rB] - R[rA]` ; Updates ZF, SF, OF ; `PC += 2`. |
| `0x7` | `0x0` <br> `0x1` | **jmp** <br> **je** | `jmp dest8` <br> `je dest8` | Unconditional jump: `PC = dest8`. <br> Conditional branch: If `ZF == 1`, `PC = dest8`; else `PC += 2`. |

---

## 5. Compiling and Testing

### Step 1: Compile Your Program
Build your C source file using `gcc`:

```bash
gcc -Wall y86_mini_sim.c -o y86_mini_sim
```

### Step 2: Execute with Test Binaries
Run the compiled simulator by passing one of the provided pre-assembled machine-code files as a command-line argument:

```bash
./y86_mini_sim test_basic.bin
./y86_mini_sim test_alu_flags.bin
./y86_mini_sim test_mem_control.bin
```

Compare your execution trace output against the provided sample text traces (`test_basic_trace.txt`, etc.) to verify that your register states and condition codes update correctly on a cycle-by-cycle basis.

---

## 6. Post-Lab Deliverable Questions
Answer the following questions in your lab report submission:

1. **Little-Endian Reconstruction:** If `MEM[0x04] = 0x51` and `MEM[0x05] = 0x30`, what is the complete 16-bit hexadecimal instruction word fetched when `PC = 0x04`? Translate this word into its exact assembly mnemonic, immediate value, and target register destination name.
2. **Byte Separation Constraints:** Explain how your simulator isolates the `rA` and `rB` register identifiers from a single byte of layout memory. What bitwise mask and shift operations did you use?
3. **Byte Separation Constraints:** Explain how your simulator dynamically checks the instruction opcode (`icode`) before decoding register IDs. Why does the instruction `irmovq` handle bits 7–4 differently than an `addq` instruction?
4. **Condition Code Activation:** Suppose `%rax` holds `0x0005` and `%rcx` holds `0x0005`. If the instruction `subq %rax, %rcx` executes, describe the resulting mathematical subtraction context, which register field gets updated with the result, and list the final values assigned to `ZF` and `SF`.
5. **Memory Swapping Alignment:** Because our simulated memory is byte-addressable (`uint8_t`), storing a 16-bit register value via `rmmovq` spans two slots. If `%rax = 0xABCD` and `%rbx = 0x10`, outline exactly what values are written to `MEM[0x10]` and `MEM[0x11]`.

---

## 7. Project Submission
Zip together your completed `y86_mini_sim.c` file and a PDF containing the answers to the above post-lab questions, and submit the Zip archive on Canvas.

---

## Optional Extra Credit: Complete ISA Emulation (up to +15 pts)
For extra credit, expand your implementation to support the full combinational matrix of logical operations and relative inequality jumps specified in Chapter 4 of the textbook.

### 1. Extended Logicals (`icode = 0x6`)
Extend your `OPq` decoder switch statement to support the bitwise logic function codes:
* `ifun = 0x2`: **andq rA, rB** → `R[rB] = R[rB] & R[rA]` (Updates `ZF`, `SF`; explicitly clears `OF = 0`)
* `ifun = 0x3`: **xorq rA, rB** → `R[rB] = R[rB] ^ R[rA]` (Updates `ZF`, `SF`; explicitly clears `OF = 0`)

### 2. Full Conditional Jumps (`icode = 0x7`)
Extend your `jXX` branch evaluation to parse and validate signed integer inequalities by implementing the full combinational logic gating matrices:

| `ifun` | Mnemonic | Jump Condition Logic |
| :---: | :--- | :--- |
| `0x2` | **jne** | `ZF == 0` |
| `0x3` | **js** | `SF == 1` |
| `0x4` | **jns** | `SF == 0` |
| `0x5` | **jle** | `(SF ^ OF) \| ZF` |
| `0x6` | **jl** | `SF ^ OF` |
| `0x7` | **jge** | `~(SF ^ OF)` |
| `0x8` | **jg** | `~(SF ^ OF) & ~ZF` |

To verify your extra credit implementation, run your simulator against `test_extra_credit.bin`. Your simulator must execute the entire verification loop block and terminate successfully without throwing an unknown instruction exception.