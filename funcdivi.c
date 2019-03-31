#include "9cc.h"
void arg_parse(Func *func);
int pos=0;
Vector *vec_func;
Func *new_func(char *name){
	Func *func=malloc(sizeof(Func));
	func->name = new_node_ident(name);
	func->map=new_map();
	func->cnt=0;
	func->code=new_vector();
	return func;
}

void funcsp(){//関数を分割
	vec_func = new_vector();
	while(((Token *)vec_token->data[pos])->ty != TK_EOF)vec_push(vec_func,(void *)con());
	vec_push(vec_func,(void *)NULL);
}

Func *con(){//パース
	Token * tkn = ((Token *)vec_token->data[pos]);
	char *func_name=tkn->name;pos++;
	if(!func_name){
	fprintf(stderr,"構文エラー:関数名がありません:%c\n",tkn->ty);
	exit(1);
	}
	Func *func=new_func(func_name);
	func->name = func_parse(func->name);
	arg_parse(func);
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
	return func;
}
void arg_parse(Func *func){
	if(func->name->rhs == NULL)return;
	Vector *arg = func->name->rhs->args;
	for(int i=0;arg->data[i] && i<6;i++){
		func->cnt++;
		map_put(func->map,(char *)arg->data[i],(void *)(intptr_t)(func->cnt*8));
	}
}
