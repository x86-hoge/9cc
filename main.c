#include "9cc.h"


int main(int argc,char **argv){
	if(argc != 2){
		fprintf(stderr,"引数の個数が正しくありません\n");
		return 1;
	}
	if(!strcmp(argv[1],"-test")){
		runtest();
		return 0;
	}

	tokenize(argv[1]);//トークンに分割
	program();//パース
	
	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");

	printf("	push rbp\n");
	printf("	mov rbp, rsp\n");
	printf("	sub rsp, %d\n",val_cnt*8);
	
	
	for(int i=0;(Node *)vec_code->data[i];i++){
		gen((Node *)vec_code->data[i]);
		printf("	pop rax\n");
	}
	
	printf("	mov rsp, rbp\n");
	printf("	pop rbp\n");
	printf("	ret\n");
	return 0;
}
