#include "9cc.h"
int pos=0;
Func *new_func(char *name){
	Func *func=malloc(sizeof(Func));
	func->name = new_node_ident(name);
	func->map=new_map();
	func->cnt=0;
	func->code=new_vector();
	return func;
}
void con(){
	char *func_name=((Token *)vec_token->data[pos++])->name;
	if(!func_name){
	fprintf(stderr,"構文エラー:関数名がありません\n");
	exit(1);
	}
	Func *func=new_func(func_name);
	func->name = func_parse(func->name);
	if(consume('{')){
		program(func->map,&(func->cnt),func->code);//パース
		if(consume('}')){

		}else{
			fprintf(stderr,"閉じカッコがありません\n");
			exit(1);
		}

	}
	else{
		fprintf(stderr,"開きカッコがありません\n");
		exit(1);
	}
	printf("%s:\n",func->name->lhs->name);
	printf("	push rbp\n");
	printf("	mov rbp, rsp\n");
	printf("	sub rsp, %d\n",func->cnt*8);

	set_valmap(func->map);//gen関数に必要な変数データを渡す

	for(int i=0;(Node *)func->code->data[i];i++){
		gen((Node *)func->code->data[i]);
		printf("	pop rax\n");
	}
	printf("	mov rsp, rbp\n");
	printf("	pop rbp\n");
	printf("	ret\n");
}
