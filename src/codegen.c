#include <codegen.h>
#include <symbol.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <colors.h>

// 2022 Ian Moffett

static char* regs[4] = {"%r8", "%r9", "%r10", "%r11"};
static char* bregs[4] = {"%r8b", "%r9b", "%r10b", "%r11b"};
static uint8_t reg_bmp = 0xF;
static FILE* out = NULL;

#define ASM_OUT "/tmp/kcc_" __TIME__ ".s"

void clean_and_exit(void);


// Jump types.
static char* jmps[] = {"jne", "je", "jge", "jle", "jg", "jl"};


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
    "\t# Start of program (prologue).\n"
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
            "\t# End of program (epilogue).\n"
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


static int rcmp(uint8_t r1, uint8_t r2, char* how) {
    fprintf(out, "\tcmpq\t%s, %s\n", regs[r2], regs[r1]);
    fprintf(out, "\t%s\t%s\n", how, bregs[r2]);
    fprintf(out, "\tandq\t$0xFF,%s\n", regs[r2]);
    return r2;
}


static int requal(int r1, int r2) {
    return rcmp(r1, r2, "sete");
}


static int rnotequal(int r1, int r2) {
    return rcmp(r1, r2, "setne");
}


static int rlessthan(int r1, int r2) {
    return rcmp(r1, r2, "setl");
}


static int rgreaterthan(int r1, int r2) {
    return rcmp(r1, r2, "setg");
}


static int rlessequal(int r1, int r2) {
    return rcmp(r1, r2, "setle");
}


static int rgreaterequal(int r1, int r2) {
    return rcmp(r1, r2, "setge");
}


// Strores a global variable that is in a register
// into a variable.
static int rsglob(uint8_t r, const char* identifier) {
    fprintf(out, "\tmovq\t%s, %s(\%%rip)\n", regs[r], identifier);
    return r;
}


static size_t gen_label(void) {
    static int id = 1;
    return id++;
}


static void rlabel(int l) {
    fprintf(out, "_%d:\n", l);
}

static int gencode_if(struct ASTNode* node) {
    int lFalse = gen_label();

    // Generate condition code followed
    // by a jump to false label.
    interpret_ast(node->left, lFalse, node->op);
    freeall_regs();

    // Generate true compound statement.
    interpret_ast(node->mid, -1, node->op);
    freeall_regs();
    
    rlabel(lFalse);

    return -1;
}


// Make space for a global symbol.
void rmkglob_sym(char* symbol, uint8_t sz_bytes) {
    fprintf(out, "\t.comm\t%s,%d,%d\n", symbol, sz_bytes, sz_bytes);
}


// Prints an integer that is inside
// of a register.
void codegen_print_int(uint8_t reg) {
    fprintf(out, "\tmovq\t%s, %%rdi\n", regs[reg]);
    fprintf(out, "\tcall\tprintint\n");
    free_reg(reg);
}


// Moves a global variable into a register.
// Returns register glob was moved into.
uint8_t rload_glob(char* identifier) {
    uint8_t r = alloc_reg();
    fprintf(out, "\tmovq\t%s(\%%rip), %s\n", identifier, regs[r]);
    return r;
}


static int compare_n_jmp(int ast_op, uint8_t r1, uint8_t r2, size_t label) {
    if (ast_op < A_CMP || ast_op > A_GE) {
        printf(COLOR_ERROR "Bad ast_op in compare_n_jmp()\n");
        fclose(out);
        clean_and_exit();
    }

    fprintf(out, "\tcmpq\t%s, %s\n", regs[r2], regs[r1]);
    fprintf(out, "\t%s\t_%ld\n", jmps[ast_op - A_CMP], label);
    freeall_regs();
    return -1;
}


int interpret_ast(struct ASTNode* root, uint8_t reg, int parent_ast_top) {
    int leftreg, rightreg;
    
    switch (root->op) {
        case A_IF:
            return gencode_if(root);
        case A_GLUE:
            // Handle each child statement, and free 
            // the registers after each child.
            interpret_ast(root->left, -1, root->op);
            freeall_regs();
            interpret_ast(root->right, -1, root->op);
            freeall_regs();
            return -1;
        default: break;
    }
    
    // Get left, right sub-tree values.
    if (root->left)
        leftreg = interpret_ast(root->left, -1, root->op);

    if (root->right)
        rightreg = interpret_ast(root->right, leftreg, root->op);

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
        case A_CMP:
        case A_NOT_EQ:
        case A_LT:
        case A_GT:
        case A_LE:
        case A_GE:
            /*
             *  If parent_ast_top is an IF statement we will
             *  compare and jump, otherwise we will re-check the
             *  operators and set whatever the result is.
             *
             */

            if (parent_ast_top == A_IF)
                return compare_n_jmp(root->op,leftreg, rightreg, reg);
            else {
                switch (root->op) {
                    case A_CMP:
                        return requal(leftreg, rightreg);
                    case A_NOT_EQ:
                        return rnotequal(leftreg, rightreg);
                    case A_LT:
                        return rlessthan(leftreg, rightreg);
                    case A_GT:
                        return rgreaterthan(leftreg, rightreg);
                    case A_LE:
                        return rlessequal(leftreg, rightreg);
                    case A_GE:
                        return rgreaterequal(leftreg, rightreg);
                    default: break;
                }
            }

            break;
        case A_PRINT:
            codegen_print_int(leftreg);
        case A_INTLIT:
            return rload(root->val_int);
        case A_LVIDENT:
            {
                extern struct SymbolTable globl_sym_tbl[NSYMBOLS];
                return rsglob(reg, globl_sym_tbl[root->symbol_id].name);
            }
        case A_ASSIGN:
            return rightreg; 
        default:
            return 0;
    }

    return 0;
}


void codegen_init(void) {
    extern uint8_t error;
    if (error) return;
    out = fopen(ASM_OUT, "w");

    
    fputs("/*\n* Assembly automatically generated by the KessC compiler.\n* KessC, made by Ian Marco Moffett.\n*\n*/\n\n", out);

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
