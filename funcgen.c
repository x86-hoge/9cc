#include "9cc.h"

void built(Func *func){
	printf("%s:\n",func->name->lhs->name);//変数名
	printf("	push rbp\n");
	printf("	mov rbp, rsp\n");
	printf("	sub rsp, %d\n",func->cnt*8);
	
	set_valmap(func->map);//gen関数に必要な変数データを渡す

	if(func->name->rhs != NULL){
		int offset;
		char *rgsr[6]={"rdi","rsi","rdx","rcx","r8","r9"};//レジスタ一覧
		printf("	mov rax, rbp\n");
		for(int i=0;i<6&&func->name->rhs->args->data[i];i++){
			offset =(int)map_get(func->map,(char *)func->name->rhs->args->data[i]);
			printf("	sub rax, %d\n",offset);
			printf("	mov rax, %s\n",rgsr[i]);
		}
	}

	for(int i=0;(Node *)func->code->data[i];i++){
		gen((Node *)func->code->data[i]);
		printf("	pop rax\n");
	}
	printf("	mov rsp, rbp\n");
	printf("	pop rbp\n");
	printf("	ret\n");
}
