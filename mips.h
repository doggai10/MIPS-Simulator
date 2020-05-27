#ifndef __MIPS_H__
#define __MIPS_H__


typedef struct{
	int number;
	char *name;
	char arguments[32];
} mips_instruction;


int disassemble(mips_instruction* instruction, uint32_t num);
int mipsrun(uint32_t number, int8_t data[]);
void registers();
static const char * const MIPS_REG_NAME[35]= {
"$0", "$1", "$2", "$3", "$4", "$5", "$6", "$7",
"$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15",
"$16", "$17", "$18", "$19", "$20", "$21", "$22", "$23",
"$24", "$25", "$26", "$27", "$28", "$29", "$30", "$31","HI","LO","PC"
};

static char * const MIPS_REG_INST_NAME[8][8]= {
	{"sll",NULL,"srl","sra","sllv",NULL,"srlv","srav"},
	{"jr","jalr","","","syscall"},
	{"mfhi","mthi","mflo","mtlo"},
	{"mult","multu","div","divu"},
	{"add","addu","sub","subu","and","or","xor","nor"},
	{NULL,NULL,"slt","sltu"},
	
};

static char * const MIPS_ROOT_INST_NAME[8][8]={
	{NULL,NULL,"j","jal","beq","bne","blez","bgtz"},
	{"addi","addiu","slti","sltiu","andi","ori","xori","lui"},
	{},
	{},
	{"lb","lh","lwl","lw","lbu","lhu","lwr"},
	{"sb","sh","swl","sw",NULL,NULL,"swr"}
};

#define MIPS_TYPE_R 'R';
#define MIPS_TYPE_J 'J';
#define MIPS_TYPE_I 'I';

#define MIPS_REG_C_TYPE_MUL 0
#define MIPS_REG_C_TYPE_COU 4

#define MIPS_REG_TYPE_SHIFT 0
#define MIPS_REG_TYPE_JUMP 1
#define MIPS_REG_TYPE_MOVE 2
#define MIPS_REG_TYPE_DIVMULT 3
#define MIPS_REG_TYPE_ARITH 4
#define MIPS_REG_TYPE_SYS 6


#define MIPS_ROOT_TYPE_JUMP_OR_BRANCH 0
#define MIPS_ROOT_TYPE_ARITHLOGI 1
#define MIPS_ROOT_TYPE_LOADSTORE 4

#endif


