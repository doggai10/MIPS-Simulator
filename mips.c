#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "mips.h"

#define bitmask(n) ((1<<n) -1)
#define signextension(n) ((int32_t)n)
#define unsign(n) n & 0xFFFFFFFF
#define extension(n) 0x00000000^n
int32_t reg[34]={0,};
int disassemble(mips_instruction *instruction, uint32_t num){
	uint8_t op,op_lower,op_upper,rs,rd,rt,rt_upper,rt_lower,
		   sa,funct,funct_upper,funct_lower;

	int16_t imm;
	int32_t target;

	op=num>>26;
	op_upper= (op>>3) & bitmask(3);
	op_lower= op & bitmask(3);
	rs=(num >> 21) & bitmask(5);
	rt=(num>>16) & bitmask(5);
	rt_upper=rt>>3;
	rt_lower=rt&bitmask(3);
	rd=(num>>11)&bitmask(5);
	sa=(num>>6)&bitmask(5);
	funct=num&bitmask(6);
	funct_upper=(funct>>3)&bitmask(3);
	funct_lower=funct&bitmask(3);
	imm=num&bitmask(16);
	target=num&bitmask(26);
	if( target > bitmask(25)){
		target |=0xfc000000;
	}

	if(op==0){
		instruction->name=MIPS_REG_INST_NAME[funct_upper][funct_lower];
	}
	else{
		instruction->name=MIPS_ROOT_INST_NAME[op_upper][op_lower];
	}
	
	if(instruction-> name == NULL){
		return 0;
	}
	if(num==0){
		sprintf(instruction->arguments, "%s, %s, %d", MIPS_REG_NAME[rd],MIPS_REG_NAME[rt],sa);
	}else if(op==0){
		switch(funct_upper){
		case MIPS_REG_TYPE_SHIFT:
			if(funct_lower <4){
				sprintf(instruction->arguments,"%s, %s, %d",
						MIPS_REG_NAME[rd],
						MIPS_REG_NAME[rt],
						sa);
					}
			 else{
				sprintf(instruction->arguments, "%s, %s, %s",
						MIPS_REG_NAME[rd],
						MIPS_REG_NAME[rt],
						MIPS_REG_NAME[rs]);
			}
			break;
		case MIPS_REG_TYPE_JUMP:
			if(funct_lower < 1){
				sprintf(instruction->arguments, "%s", MIPS_REG_NAME[rs]);
			}
			else {
				sprintf(instruction->arguments, "%s, %s", MIPS_REG_NAME[rd], MIPS_REG_NAME[rs]);
			}
			break;
		case MIPS_REG_TYPE_MOVE:
			if(funct_lower % 2 == 0 ){
				sprintf(instruction->arguments, "%s", MIPS_REG_NAME[rd]);
			}
			else{
				sprintf(instruction->arguments, "%s", MIPS_REG_NAME[rs]);
			}
			break;
		case MIPS_REG_TYPE_DIVMULT:
			sprintf(instruction->arguments, "%s, %s",
					MIPS_REG_NAME[rs],
					MIPS_REG_NAME[rt]);
			break;
		case MIPS_REG_TYPE_ARITH:
		case MIPS_REG_TYPE_ARITH + 1:
			sprintf(instruction->arguments, "%s, %s, %s",
					MIPS_REG_NAME[rd],
					MIPS_REG_NAME[rs],
					MIPS_REG_NAME[rt]);
			break;


		default:
			return 0;
	}
	}
	
	else{
		switch(op_upper){
		case MIPS_ROOT_TYPE_JUMP_OR_BRANCH:
			if(op_lower < 4){
				sprintf(instruction->arguments, "%d", target);
			} else {
				if(op_lower < 6){
				 sprintf(instruction->arguments, "%s, %s, 0x%08x",
						 MIPS_REG_NAME[rs],
						 MIPS_REG_NAME[rt],
						 imm);
				} else {
					sprintf(instruction->arguments, "%s, 0x%08x",
							MIPS_REG_NAME[rt], imm);
				}
		}
			break;

		case MIPS_ROOT_TYPE_ARITHLOGI:
			if(op_lower < 7) {
				sprintf(instruction->arguments, "%s, %s, 0x%08x",
						MIPS_REG_NAME[rt],
						MIPS_REG_NAME[rs],
						imm);
			} else{
				sprintf(instruction->arguments, "%s, 0x%08x",
						MIPS_REG_NAME[rt], imm);
			}
			break;

		case MIPS_ROOT_TYPE_LOADSTORE:
		case MIPS_ROOT_TYPE_LOADSTORE+1:
			sprintf(instruction->arguments, "%s, %d(%s)",
					MIPS_REG_NAME[rt],imm,MIPS_REG_NAME[rs]);
			break;

		default:
			return 0;

		}		
	}


	return 1;

}

int mipsrun(uint32_t num, int8_t data[]){
	uint8_t op,op_lower,op_upper,rs,rd,rt,rt_upper,rt_lower,
		   sa,funct,funct_upper,funct_lower;

	int16_t imm;	
	int32_t target;
	op=num>>26;
	op_upper= (op>>3) & bitmask(3);
	op_lower= op & bitmask(3);
	rs=(num >> 21) & bitmask(5);
	rt=(num>>16) & bitmask(5);
	rt_upper=rt>>3;
	rt_lower=rt&bitmask(3);
	rd=(num>>11)&bitmask(5);
	sa=(num>>6)&bitmask(5);
	funct=num&bitmask(6);
	funct_upper=(funct>>3)&bitmask(3);
	funct_lower=funct&bitmask(3);
	imm=num&bitmask(16);	
	target=num&bitmask(26);
	if( target > bitmask(25)){
		target |=0xfc000000;
	}

	if(num==0xff){
		reg[34]+=4;
		return 1;
		//reg[34]=count*4;
	}else if(op==0){
		switch(funct_upper){
		case MIPS_REG_TYPE_SHIFT:
			if(funct_lower==0){
				unsigned int temp=(unsigned int) reg[rt] ;
				reg[rd]=temp<<sa;
				reg[0]=0;
				reg[34]+=4;
				return 1;
				//break;
			
			}else if(funct_lower==2){
				unsigned int temp=(unsigned int) reg[rt] ;
				reg[rd]=temp>>sa;
				reg[0]=0;
				reg[34]+=4;
				return 1;
				//break;
				break;
			}else if(funct_lower==3){
				reg[rd]=reg[rt]>>sa;
				reg[0]=0;
				reg[34]+=4;
				return 1;
				//break;
			}else if(funct_lower==4){
				unsigned int temp=(unsigned int) reg[rt];
				reg[rd]=temp<<reg[rs];
				reg[0]=0;
				reg[34]+=4;
				return 1;
				//break;
			}else if(funct_lower==6){
				unsigned int temp=(unsigned int) reg[rt] ;
				reg[rd]=temp>>reg[rs];
				reg[0]=0;
				reg[34]+=4;				
				return 1;
				//break;
			}else if(funct_lower==7){
				reg[rd]=reg[rt]>>reg[rs];
				reg[0]=0;
				reg[34]+=4;				
				return 1;
				//break;
			}else{
				return 1;
				//break;
			}
		case MIPS_REG_TYPE_JUMP:
			if(funct_lower== 0){ //jr
				reg[34]=reg[rs];
				return reg[34];
			}
			else if(funct_lower==1) { //jalr
				reg[rd]=reg[34]+4;
				reg[34]=reg[rs];
				return reg[34];
			} else if(funct_lower==4){ //syscall
				if(reg[2]==1){
					//print $a0($4)
					printf("%d", reg[4]);
					reg[34]+=4;
					return 1;
				}else if (reg[2]==4){
					//print string in the data memory $a0($4)
					int index=(reg[4]-0x10000000);	
					reg[34]+=4;			
					while(data[index]>0){
						char result=data[index];
						printf("%c", result);
						index++;
					}
					return 1;				
						
				}else if(reg[2]==10){
					printf("EXIT syscall\n");
					reg[34]+=4;
					int tem=10;
					return 0;
				}else {
					printf("Invalid syscall\n");
					return -1;
				}
			}else{
				return 1;
				//break;
			}
		case MIPS_REG_TYPE_MOVE:
			if(funct_lower== 0 ){ //mfhi
				reg[rd]=reg[32];
				reg[34]+=4;
				//reg[34]=count*4;
				return 1;
				//break;
			}
			else if(funct_lower==1){ //mthi
				reg[32]=reg[rs];
				reg[34]+=4;
				//reg[34]=count*4;
				return 1;
				//break;
			}
			else if(funct_lower==2){//mflo
				reg[rd]=reg[33];
				reg[34]+=4;
				//reg[34]=count*4;				
				return 1;
				//break;			
			}
			else{ // mtlo
				reg[33]=reg[rs];
				reg[34]+=4;
				//reg[34]=count*4;
				return 1;
				//break;
			}
		case MIPS_REG_TYPE_DIVMULT:
			if(funct_lower==0){ //mult
				int64_t result= (int64_t)reg[rs]*(int64_t)reg[rt];
				reg[33]=result;
				reg[32]=(result>>32);
				reg[34]+=4;
				return 1;
				//break;
			}else if(funct_lower==1){ //multu
				uint32_t a= unsign(reg[rs]);
				uint32_t b= unsign(reg[rt]);
				uint64_t result= (uint64_t)a*(uint64_t)b;
				reg[33]=result;
				reg[32]=(result>>32);
				reg[34]+=4;
				return 1;
				//break;
			}else if(funct_lower==2){ //div
				reg[32]=reg[rs]%reg[rt];
				reg[33]=reg[rs]/reg[rt];
				reg[34]+=4;
				return 1;
				//break;
			}else if(funct_lower==3){ //divu
				uint32_t a= unsign(reg[rs]);
				uint32_t b= unsign(reg[rt]);
				reg[32]=a%b;
				reg[33]=a/b;
				reg[34]+=4;
				return 1;
				//break;
			}else{
				return 1;
				//break;
			}
		case MIPS_REG_TYPE_ARITH:
			if(funct_lower<=1){
				reg[rd]=reg[rs]+reg[rt];
				reg[0]=0;
				reg[34]+=4;
				return 1;
				//break;
			}else if(funct_lower<=3){
				reg[rd]=reg[rs]-reg[rt];
				reg[0]=0;
				reg[34]+=4;
				return 1;
				//break;
			}else if(funct_lower==4){
				reg[rd]=reg[rs]&reg[rt];
				reg[0]=0;
				reg[34]+=4;
				return 1;
				//break;
			}else if(funct_lower==5){
				reg[rd]=(reg[rs]|reg[rt]);
				reg[0]=0;
				reg[34]+=4;
				return 1;
				//break;
			}else if(funct_lower==6){
				reg[rd]=reg[rs]^reg[rt];
				reg[0]=0;
				reg[34]+=4;
				return 1;
				//break;
			}else if(funct_lower==7){
				reg[rd]=~(reg[rs]|reg[rt]);
				reg[0]=0;
				reg[34]+=4;
				return 1;
				//break;
			}else{
				return 1;
				//break;
			}
		
		case MIPS_REG_TYPE_ARITH + 1:
			if(funct_lower==2){
				if(reg[rs]<reg[rt]){
					reg[rd]=1;
					reg[0]=0;
					reg[34]+=4;
					return 1;
					//break;
				} else {
					reg[rd]=0;
					reg[0]=0;
					reg[34]+=4;
					return 1;
					//break;					
					}
			}else if(funct_lower==3){
					unsigned int temp=reg[rs];
					unsigned int temp2=reg[rt];
					if(temp<temp2){
						reg[rd]=1;
						reg[0]=0;
						reg[34]+=4;
						return 1;
						//break;
					} else {
						reg[rd]=0;
						reg[0]=0;
						reg[34]+=4;					
						return 1;
						//break;
					}
			}else{
					return 1;
					//break;
			}
		default:
			return 1;
			//break;
		}
	}
	
	else{
		switch(op_upper){
		case MIPS_ROOT_TYPE_JUMP_OR_BRANCH:
			if(op_lower ==2){ //j
				reg[34]=((reg[34])&0xf0000000)|(target<<2);				
				//int32_t pc_upper= (reg[34]>>16);
				//reg[34]= (pc_upper<<16)|(target<<2);
				return reg[34];
				//break;
			}else if(op_lower==3){//jal
				reg[31]=reg[34]+4;
				reg[34]=((reg[34])&0xf0000000)|(target<<2);
				return reg[34];
			}else if(op_lower==4){//beq
				if(reg[rs]==reg[rt]){
					int32_t offset=(int32_t)imm<<2;
					reg[34]+=offset;
					return reg[34];
					//break;				
					//return reg[34];
				} else {
					reg[34]+=4;
					return 1;
					//break;
				}
				
			}else if(op_lower==5){//bne
				if(reg[rs]!=reg[rt]){
					int32_t offset=(int32_t)imm<<2;
					reg[34]+=offset;
					return reg[34];
				} else {
					reg[34]+=4;
					return 1;
					//break;
				}
			}else{
				return 1;
				//break;
			}
		
		case MIPS_ROOT_TYPE_ARITHLOGI:
			if(op_lower <= 1) {
				//uint16_t temp= unsign(imm);
				int32_t temp=num&bitmask(16);
				//uint32_t tem=unsign(temp);
				//int32_t res=reg[rs];
				//uint32_t reg=unsign(res);
				uint64_t res= reg[rs]+imm;
				reg[rt]=res;
				reg[0]=0;
				reg[34]+=4;
				return 1;
				
			}else if(op_lower==2){
				if(reg[rs]<imm){
					reg[rt]=1;
					reg[0]=0;
					reg[34]+=4;
					return 1;
					//break;
				}else{
					reg[rt]=0;
					reg[0]=0;
					reg[34]+=4;					
					return 1;
					//break;
				}	
			}else if(op_lower==3){
				if(num&bitmask(16)<0xf0000000){
					int32_t temp=num&bitmask(16);
					if(reg[rs]<temp){
						reg[rt]=1;
						reg[0]=0;
						reg[34]+=4;
						return 1;
						//break;
					}else{
						reg[rt]=0;
						reg[0]=0;
						reg[34]+=4;						
						return 1;
						//break;
					}
				}else{
					uint32_t temp=unsign(num&bitmask(16));
					if(reg[rs]<temp){
						reg[rt]=1;
						reg[0]=0;
						reg[34]+=4;
						return 1;
						//break;
					}else{
						reg[rt]=0;
						reg[0]=0;
						reg[34]+=4;						
						return 1;
						//break;
					}
				}	
				
			}else if(op_lower==4){
				reg[rt]=reg[rs]&imm;
				reg[0]=0;
				reg[34]+=4;
				return 1;
				//break;
			} else if(op_lower==5){
				int32_t temp= num&bitmask(16);
				reg[rt]=reg[rs]|temp;				
				reg[0]=0;
				reg[34]+=4;				
				return 1;
				//break;
			} else if(op_lower==6){
				int32_t temp=num&bitmask(16);
				reg[rt]=reg[rs]^temp;
				reg[0]=0;
				reg[34]+=4;
				return 1;
				//break;
			} else if(op_lower==7){	
				reg[rt]=imm;
				reg[rt]=reg[rt]<<16;
				reg[34]+=4;				
				reg[0]=0;
				return 1;
				//break;
			}else{
				return 1;
				//break;
			}
		case MIPS_ROOT_TYPE_LOADSTORE:
			if(op_lower==0){ //lb0
				int32_t temp= (num&bitmask(16));
				int32_t addr=reg[rs]+temp;
				int index=((reg[rs]+temp)-0x10000000);
				if(index <0 || index >65536){
					if(index>65536){
						printf("Memory address out of range: 0x00010000\n");
						reg[34]+=4;
						return 0;
					} else{
						printf("Memory address out of range: 0x00000000\n");
						reg[34]+=4;
						return 0;
					}						
				}else {
					int8_t result=data[index];	
					reg[rt]=result;
					reg[34]+=4;
					return 1;
				}
			} else if(op_lower==1){ //lh0
				int32_t temp= (num&bitmask(16));
				int32_t addr=reg[rs]+temp;
				int index=((reg[rs]+temp)-0x10000000);
				if(index %2 !=0){
					if(index <0 || index >65536){
						if(index>65536){
						printf("Memory address out of range: 0x00010000\n");
						reg[34]+=4;
						return 0;
						} else{
						printf("Memory address out of range: 0x00000000\n");
						reg[34]+=4;
						return 0;
						}					
					}else {
						printf("Memory address alignment error: %08x\n",addr);
						reg[34]+=4;
						return 0;
					}
				} else {
					if(index <0 || index >65536){
						if(index>65536){
						printf("Memory address out of range: 0x00010000\n");
						reg[34]+=4;
						return 0;
						} else{
						printf("Memory address out of range: 0x00000000\n");
						reg[34]+=4;
						return 0;
						}
					}else {
						int8_t a=data[index];
						uint8_t b= data[index+1];
						reg[rt]=(a<<8)+b;
						reg[34]+=4;
						return 1;
					}

				}
			} else if(op_lower==3){ //lw0
				int32_t temp= (num&bitmask(16));
				int32_t addr=reg[rs]+temp;
				int index=((reg[rs]+temp)-0x10000000);
				if(index %4 !=0){
					if(index <0 || index >65536){
						if(index>65536){
						printf("Memory address out of range: 0x00010000\n");
						reg[34]+=4;
						return 0;
						} else{
						printf("Memory address out of range: 0x00000000\n");
						reg[34]+=4;
						return 0;
						}					
					}else {
						printf("Memory address alignment error: %08x\n",addr);
						reg[34]+=4;
						return 0;
					}
				} else {
					if(index <0 || index >65536){
						if(index>65536){
						printf("Memory address out of range: 0x00010000\n");
						reg[34]+=4;
						return 0;
						} else{
						printf("Memory address out of range: 0x00000000\n");
						reg[34]+=4;
						return 0;
						}					
					}else {
						int8_t a=data[index];
						uint8_t b=data[index+1];
						uint8_t c=data[index+2];
						uint8_t d=data[index+3];
						uint32_t result=(a<<24)+(b<<16)+(c<<8)+d;
						reg[rt]=result;
						reg[34]+=4;
						return 1;
					}

				}
				
				
				//break;
			} else if(op_lower==4){ //lbu0
				int32_t tem= (num&bitmask(16));
				uint32_t temp= (uint32_t)tem;
				int32_t addr=reg[rs]+temp;
				int index=((reg[rs]+temp)-0x10000000);
				if(index <0 || index >65536){
					if(index>65536){
						printf("Memory address out of range: 0x00010000\n");
						reg[34]+=4;
						return 0;
					} else{
						printf("Memory address out of range: 0x00000000\n");
						reg[34]+=4;
						return 0;
						}							
				}else {
					uint8_t result=data[index];	
					reg[rt]=result;
					reg[34]+=4;
					return 1;
				}
			} else if(op_lower==5){ //lhu0
				int32_t im=(num&bitmask(16));
				uint32_t temp=(uint32_t)im;
				int32_t tem=(reg[rs]+temp);
				uint32_t addr=unsign(tem);
				int index=addr-0x10000000;
				if(index %2 !=0){
					if(index <0 || index >65536){
						if(index>65536){
						printf("Memory address out of range: 0x00010000\n");
						reg[34]+=4;
						return 0;
						} else{
						printf("Memory address out of range: 0x00000000\n");
						reg[34]+=4;
						return 0;
						}					
					}else {
						printf("Memory address alignment error: %08x\n",addr);
						reg[34]+=4;
						return 0;
					}

				} else {
					if(index <0 || index >65536){
						if(index>65536){
						printf("Memory address out of range: 0x00010000\n");
						reg[34]+=4;
						return 0;
						} else{
						printf("Memory address out of range: 0x00000000\n");
						reg[34]+=4;
						return 0;
						}						
					}else {
						uint8_t a=data[index];
						uint8_t b= data[index+1];
						reg[rt]=(a<<8)+b;
						reg[34]+=4;
						return 1;
					}

				}
			}
		case MIPS_ROOT_TYPE_LOADSTORE+1:
			if(op_lower==0){ //sb0
				int32_t temp= (num&bitmask(16));
				int32_t addr=reg[rs]+temp;
				int index=addr-0x10000000;
				if(index <0 || index >65536){
					if(index>65536){
						printf("Memory address out of range: 0x10010000\n");
						reg[34]+=4;
						return 0;
					} else{
						printf("Memory address out of range: 0x10000000\n");
						reg[34]+=4;
						return 0;
						}						
				}else {
					uint32_t temp=(uint32_t)reg[rt];
					uint8_t a=(temp);
					data[index]=a;
					reg[34]+=4;
					return 1;
				}
			} else if(op_lower==1){ //sh0
				int32_t temp= (num&bitmask(16));
				int32_t addr=reg[rs]+temp;
				int index=addr-0x10000000;
				if(index %4 !=0){
					if(index <0 || index >65536){
						if(index>65536){
						printf("Memory address out of range: 0x10010000\n");
						reg[34]+=4;
						return 0;
						} else{
						printf("Memory address out of range: 0x10000000\n");
						reg[34]+=4;
						return 0;
						}					
					}else {
						printf("Memory address alignment error: %08x\n",addr);
						reg[34]+=4;
						return 0;
					}
				} else {
					if(index <0 || index >65536){
						if(index>65536){
						printf("Memory address out of range: 0x10010000\n");
						reg[34]+=4;
						return 0;
						} else{
						printf("Memory address out of range: 0x10000000\n");
						reg[34]+=4;
						return 0;
						}					
					}else {
						//uint32_t temp=unsign(reg[rt]);
						uint32_t temp=(uint32_t)reg[rt];
						uint8_t a=((temp>>8));
						uint8_t b=(temp);
						data[index]=a;
						data[index+1]=b;
						reg[34]+=4;
						return 1;
					}

				}
			} else if(op_lower==3){ //sw0
				int32_t temp= (num&bitmask(16));
				int32_t addr=reg[rs]+temp;
				int index=addr-0x10000000;
				if(index %4 !=0){
					if(index <0 || index >65536){
						if(index>65536){
						printf("Memory address out of range: 0x10010000\n");
						reg[34]+=4;
						return 0;
						} else{
						printf("Memory address out of range: 0x10000000\n");
						reg[34]+=4;
						return 0;
						}					
					}else {
						printf("Memory address alignment error: %08x\n",addr);
						reg[34]+=4;
						return 0;
					}
				} else {
					if(index <0 || index >65536){
						if(index>65536){
						printf("Memory address out of range: 0x10010000\n");
						reg[34]+=4;
						return 0;
						} else{
						printf("Memory address out of range: 0x10000000\n");
						reg[34]+=4;
						return 0;
						}					
					}else {
						uint32_t temp=(uint32_t)reg[rt];
						uint8_t a=(temp>>24);
						uint8_t b=((temp>>16));
						uint8_t c=((temp>>8));
						uint8_t d=(temp);
						//printf("%08x %08x %08x %08x\n",a,b,c,d);
						data[index]=a;
						data[index+1]=b;
						data[index+2]=c;
						data[index+3]=d;
						reg[34]+=4;
						return 1;
					}

				}
			} else {
				return 1;
				//break;
			}
		default:
			return 1;
			//break;

		}		
	}


}

void registers(){
	for(int i=0; i<=34;i++){
        printf("%s: ", MIPS_REG_NAME[i]);
	printf("0x%08x\n",reg[i]);
	}

	for(int i=0; i<=34;i++)
	{
		reg[i]=0;
	}



}

