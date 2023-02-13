#include <iostream>

/// @brief register
enum Register {
    R_R0 = 0,
    R_R1,
    R_R2,
    R_R3,
    R_R4,
    R_R5,
    R_R6,
    R_R7,
    R_PC,
    R_COND,
    R_COUNT
};

/// @brief options
enum Options {
    OP_ADD = 0, /* add  */
    OP_AND,     /* bitwise and */
    OP_BR,      /* branch */
    OP_JMP,     /* jump */
    OP_JSR,     /* jump register */
    OP_LD,      /* load */
    OP_LDI,     /* load indirect */
    OP_LDR,     /* load register */
    OP_LEA,     /* load effective address */
    OP_NOT,     /* bitwise not */
    OP_RES,     /* reserved (unused) */
    OP_RTI,     /* unused */
    OP_ST,      /* store */
    OP_STI,     /* store indirect */
    OP_STR,     /* store register */
    OP_TRAP     /* execute trap */
};

/// @brief R_COND Condition - 存储「条件标志位」
enum Condition {
    FL_POS = 1 << 0, /* Positive */
    FL_ZRO = 1 << 1, /* Zero */
    FL_NEG = 1 << 2, /* Negative */
};

uint16_t memory[UINT16_MAX];
uint16_t reg[R_COUNT];

using namespace std;

uint16_t sign_extend(uint16_t x, int bit_count) {
    if ((x >> (bit_count - 1)) & 1) {
        x |= (0xFFFF << bit_count);
    }
    return x;
}

/// @brief setcc()
void update_flags(uint16_t r) {
    if (reg[r] == 0) {
        reg[R_COND] = FL_ZRO;
    } else if (reg[r] >> 15) {
        reg[R_COND] = FL_NEG;
    } else {
        reg[R_COND] = FL_POS;
    }
}

uint16_t mem_read(uint16_t address) {
    return 0;
}

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        /* show usage string */
        printf("lc3 [image-file1] ...\n");
        exit(2);
    }

    //   for (int i = 1; i < argc; ++i) {
    //     if(!read_)
    //   }

    /* set the PC to starting position */
    /* 0x3000 is the default */
    enum { PC_START = 0x3000 };
    reg[R_PC] = PC_START;
    cout << reg[R_PC]++ << endl;
    cout << reg[R_PC]++ << endl;

    int running = 1;
    while (running) {
        /* FETCH */
        uint16_t instr = mem_read(reg[R_PC]++);
        uint16_t op = instr >> 12;
        switch (op) {
        case OP_ADD: {
            uint16_t dr = instr >> 9 & 0x7;
            uint16_t sr1 = instr >> 6 & 0x7;
            uint16_t imm_flag = instr >> 5 & 0x1;
            if (imm_flag) {
                uint16_t imm5 = instr & 0x1F;
                reg[dr] = reg[sr1] + sign_extend(imm5, 5);
            } else {
                uint16_t sr2 = instr & 0x7;
                reg[dr] = reg[sr1] + reg[sr2];
            }
            update_flags(dr);
            break;
        }
        case OP_AND: {
            uint16_t dr = instr >> 9 & 0x7;
            uint16_t sr1 = instr >> 6 & 0x7;
            uint16_t imm_flag = instr >> 5 & 0x1;
            if (imm_flag) {
                uint16_t imm5 = instr & 0x1F;
                reg[dr] = reg[sr1] & sign_extend(imm5, 5);
            } else {
                uint16_t sr2 = instr & 0x7;
                reg[dr] = reg[sr1] & reg[sr2];
            }
            update_flags(dr);
            break;
        }
        case OP_BR: {
            // uint16_t n_flag = (instr >> 11) & 0x1;
            // uint16_t z_flag = (instr >> 10) & 0x1;
            // uint16_t p_flag = (instr >> 9) & 0x1;
            uint16_t nzp_flag = instr >> 9 & 0x7;
            uint16_t pc_offset = instr & 0x1FF;
            if (nzp_flag & reg[R_COND]) {
                reg[R_PC] += sign_extend(pc_offset, 9);
            }
            break;
        }
        case OP_JMP: {
            uint16_t base_r = instr >> 6 & 0x7;
            reg[R_PC] = reg[base_r];
            break;
        }
        case OP_JSR: {
            reg[R_R7] = reg[R_PC];
            if ((instr >> 11 & 1) == 0) {
                uint16_t base_r = instr >> 6 & 0x7;
                reg[R_PC] = reg[base_r];
            } else {
                uint16_t pc_offset_11 = instr & 0x7FF;
                reg[R_PC] += sign_extend(pc_offset_11, 11);
            }
            break;
        }
        case OP_LD: {
            uint16_t dr = instr >> 9 & 0x7;
            uint16_t pc_offset_9 = instr & 0x1FF;
            reg[dr] = mem_read(reg[R_PC] + sign_extend(pc_offset_9, 9));
            update_flags(dr);
            break;
        }
        case OP_LDI: {
            uint16_t dr = instr >> 9 & 0x7;
            uint16_t pc_offset_9 = instr & 0x1FF;
            reg[dr] = mem_read(mem_read(reg[R_PC] + sign_extend(pc_offset_9, 9)));
            update_flags(dr);
            break;
        }
        case OP_LDR: {
            uint16_t dr = instr >> 9 & 0x7;
            uint16_t base_r = instr >> 6 & 0x7;
            uint16_t offset_6 = instr & 0x3F;
            reg[dr] = mem_read(reg[base_r] + sign_extend(offset_6, 6));
            update_flags(dr);
            break;
        }
        case OP_LEA: {
            break;
        }
        case OP_NOT: {
            break;
        }
        case OP_RES: {
            break;
        }
        case OP_RTI: {
            break;
        }
        case OP_ST: {
            break;
        }
        case OP_STI: {
            break;
        }
        case OP_STR: {
            break;
        }
        case OP_TRAP: {
            break;
        }

        default: {
            break;
        }
        }
    }

    return 0;
}
