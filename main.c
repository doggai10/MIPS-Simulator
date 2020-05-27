#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "mips.h"

uint32_t swap_uint32(uint32_t val) {
	val=((val<<8)&0xFF00FF00) | ((val >> 8) & 0xFF00FF);
	return (val<<16) | (val>>16);

}
int main(){
	//char line[100];
	int line_num=0;
	char command[10];
	char file_name[100];
	uint32_t inst[5000];
	uint8_t data[65537];
	for(int i=0; i<65537;i++){
		data[i]=0xff;		
	}
	for(int i=0 ;i <5000 ; i++){
		inst[i]=0xff;
	}
	size_t size;
	printf("mips-sim> ");
	scanf("%s", command);
	if(!(strcmp(command,"exit"))){
	return 0;
	}
	while((strcmp(command, "exit"))) {

		
		if(!(strcmp(command, "read"))){
			int a;	
			scanf("%s", file_name);
			FILE *fp=fopen(file_name,"rb");
			while(0<(size=fread(&a, sizeof(int), 1, fp))){
				mips_instruction* instruction = calloc(1, sizeof(mips_instruction));
				uint32_t value=(uint32_t) a;	
				uint32_t number= swap_uint32(value);		
				printf("inst %d: ",line_num);
				if(disassemble(instruction,number)){
					printf("%08x  ",number);
					printf("%s %s", instruction->name, instruction->arguments);
				}
				else {
					printf("%08x  ",number);
 					printf("unknown instruction");
				}
				line_num+=1;
				printf("\n");
				free(instruction);

			}
			line_num=0;
			fclose(fp);
			printf("mips-sim> ");
			scanf("%s", command);
			if(!(strcmp(command,"exit"))){
				break;
			}
		}else if(!(strcmp(command,"loadinst"))){
			for(int i=0 ;i <100 ; i++){
			inst[i]=0xff;
			}
			int a;
			scanf("%s", file_name);
			FILE* fp=fopen(file_name,"rb");
			while(0<(size=fread(&a, sizeof(int), 1, fp))){
				mips_instruction* instruction = calloc(1, sizeof(mips_instruction));
				uint32_t value=(uint32_t) a;	
				uint32_t number= swap_uint32(value);
				if(disassemble(instruction,number)){
					instruction->number=number;
					//printf("%08x  ",instruction->number);
					//printf("%s %s", instruction->name, instruction->arguments);
				}
				else {
					instruction->name="unknown instruction";
					//printf("%08x  ",instruction->number);
 					//printf("unknown instruction");
				}
				inst[line_num]=number;
				line_num+=1;
				//printf("\n");
				free(instruction);
				}
			line_num=0;
			fclose(fp);
			printf("mips-sim> ");
			scanf("%s", command);
			if(!(strcmp(command,"exit"))){
				break;
			}
		} else if(!(strcmp(command,"loaddata"))){
			for(int i=0; i<65537;i++){
				data[i]=0xff;			
			}	
			uint8_t a;
			int i=0;
			scanf("%s",file_name);
			FILE *fp=fopen(file_name, "rb");
			while(0<(size=fread(&a,sizeof(uint8_t),1,fp))){
				data[i]= a;
				//printf("%d  %d\n", data[i],i);
				i++;
			}	
			fclose(fp);
			printf("mips-sim> ");
			scanf("%s", command);
			if(!(strcmp(command,"exit"))){
				break;
			}
		} else if(!(strcmp(command,"run"))){
         		int num;
			int count=0;
			int du=0;
			scanf("%d",&num);
			int i=0, j=0;
			for(i=0;i<=num;i++){			
				uint32_t number= inst[i];
				if(number !=0xff){
					int result=mipsrun(number,data);
					if(result==1){
						
					}//else if(result !=0 && result !=1){
					//	i=(result/4)-1;
					//	du++;
					//}
					else if(result==0){
						count++;
						break;
						
					}else if(result==-1){
						break;
						
					} else {
						i=(result/4)-1;
						du++;
						//break;
					}
					count++;
				} 
				else{
					
					if(du==0){
						count++;
					}	
					mipsrun(number,data);				
					printf("unknown instruction\n");
					break;
				}
				
		    }
		printf("Executed %d instructions\n",count);
		printf("mips-sim> ");
		scanf("%s", command);
		if(!(strcmp(command,"exit"))){
		break;
		}

		}else if(!(strcmp(command,"registers"))){
	
		registers();
		printf("mips-sim> ");
		scanf("%s", command);
		if(!(strcmp(command,"exit"))){
		break;
		}

		}
		else{	
			printf("잘못입력하셨습니다\n");
			printf("mips-sim> ");
			scanf("%s", command);
			if(!(strcmp(command,"exit"))){
			break;
			}
		}
	
	}
}




