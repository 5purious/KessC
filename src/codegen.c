#include <codegen.h>
#include <stdio.h>
#include <stdlib.h>

// 2022 Ian Moffett

static char* regs[4] = {"%r8", "%r9", "%r10", "%r11"};
static uint8_t reg_bmp = 0xF;
static FILE* out = NULL;

#define ASM_OUT "/tmp/kcc_" __TIME__ ".s"


// If a register is free, the bit is set, otherwise clear.
static int alloc_reg(void) {
    for (int i = 0; i < 4; ++i) {
        if (reg_bmp & (1 << i)) {
            reg_bmp &= ~(1 << i);
            return i;
        }
    }

    return -1;
}


// Frees a single register.
static void free_reg(uint8_t reg) {
    if (reg > 4) return;
    reg_bmp |= (1 << reg);
}


// De-allocates all registers.
static inline void freeall_regs(void) {
    reg_bmp = 0xFF;
}

static void program_prologue() {
  freeall_regs();
  fputs(
	"\t.text\n"
	".LC0:\n"
	"\t.string\t\"%d\\n\"\n"
	"printint:\n"
	"\tpushq\t%rbp\n"
	"\tmovq\t%rsp, %rbp\n"
	"\tsubq\t$16, %rsp\n"
	"\tmovl\t%edi, -4(%rbp)\n"
	"\tmovl\t-4(%rbp), %eax\n"
	"\tmovl\t%eax, %esi\n"
	"\tleaq	.LC0(%rip), %rdi\n"
	"\tmovl	$0, %eax\n"
	"\tcall	printf@PLT\n"
	"\tnop\n"
	"\tleave\n"
	"\tret\n"
	"\n"
	"\t.globl\tmain\n"
	"\t.type\tmain, @function\n"
	"main:\n"
	"\tpushq\t%rbp\n"
	"\tmovq	%rsp, %rbp\n",
    out);
}

static void epilogue(void) {
    fputs(
            "\tmovl\t$0, %eax\n"
            "\tpopq\t%rbp\n"
            "\tretq\n", out);
}



// Loads a value into a register.
static int rload(int value) {
    int reg = alloc_reg();
    if (reg < 0) return -1;
    fprintf(out, "\tmovq\t$%d, %s\n", value, regs[reg]); 
    return reg;
}


// Add two registers together and return 
// # of register with result.
static int radd(uint8_t r1, uint8_t r2) {
    fprintf(out, "\taddq\t%s, %s\n", regs[r1], regs[r2]);
    free_reg(r1);
    return r2;
}

// Multiply two registers together and return 
// # of register with result.
static int rmul(uint8_t r1, uint8_t r2) {
    fprintf(out, "\timulq\t%s, %s\n", regs[r1], regs[r2]);
    free_reg(r1);
    return r2;
}


// Subtract second register from first and return 
// # of register with result.
static int rsub(uint8_t r1, uint8_t r2) {
    fprintf(out, "\tsubq\t%s, %s\n", regs[r2], regs[r1]);
    free_reg(r2);
    return r1;
}

/*
 *  Divide from first register
 *  by the second and return # of register
 *  with result.
 *
 */


static int rdiv(uint8_t r1, uint8_t r2) {
    fprintf(out, "\tmovq\t%s,%%rax\n", regs[r1]);
    fprintf(out, "\tcqo\n");
    fprintf(out, "\tidivq\t%s\n", regs[r2]);
    fprintf(out, "\tmovq\t%%rax,%s\n", regs[r1]);
    free_reg(r2);
    return r1;
}


// Prints an integer that is inside
// of a register.
void codegen_print_int(uint8_t reg) {
    fprintf(out, "\tmovq\t%s, %%rdi\n", regs[reg]);
    fprintf(out, "\tcall\tprintint\n");
    free_reg(reg);
}


int interpret_ast(struct ASTNode* root) {
    int leftreg, rightreg;

    // Get left, right sub-tree values.
    if (root->left)
        leftreg = interpret_ast(root->left);

    if (root->right)
        rightreg = interpret_ast(root->right);

    switch (root->op) {
        case A_ADD:
            {
                return radd(leftreg, rightreg);
            }
            break;
        case A_SUB:
            {
                return rsub(leftreg, rightreg);
            }
            break;
        case A_MUL: 
            {
                return rmul(leftreg, rightreg);
            }
            break;
        case A_DIV:
            {
                return rdiv(leftreg, rightreg);
            }
            break;
        case A_INTLIT:
            return rload(root->val_int);
    }

    return 0;
}


void codegen_init(void) {
    extern uint8_t error;
    if (error) return;
    out = fopen(ASM_OUT, "w");
    program_prologue(); 
 
}


void codegen_done(void) {
    epilogue();
    fclose(out);
    
    extern uint8_t only_assembly;

    if (!(only_assembly)) {                 // If the user didn't request ONLY asm.
        // Output an ELF and remove the junk files.
        system("as " ASM_OUT " -o /tmp/kcc-out.o");
        system("gcc /tmp/kcc-out.o -o ./a.out");
        remove("/tmp/kcc-out.o");
        remove(ASM_OUT);
    } else {
        system("mv " ASM_OUT " ./");
    }
}
