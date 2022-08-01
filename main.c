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
	
	funcsp();//関数の分割と解析

	printf(".intel_syntax noprefix\n");
	printf(".global main");
	printf("\n");
	for(int i=global_map->keys->len - 1;i>=0;i--){
		Type *type= (Type*)((Variable *)global_map->vals->data[i])->type;
		switch(type->ty){
			case INT:
				printf(".comm   %s,%d,%d\n",(char*)global_map->keys->data[i],4,4); 
				break;
			case PTR:
				printf(".comm   %s,%d,%d\n",(char*)global_map->keys->data[i],8,8); 
				break;
			case ARRAY:
				printf(".comm   %s,%d,%d\n",(char*)global_map->keys->data[i],4,8); 
				break;
			case CHAR:
				printf(".comm   %s,%d,%d\n",(char*)global_map->keys->data[i],1,1); 
				break;
			default: 
				printf("ERROR\n");
				exit(1);
		}
	}
	for(int i=0;vec_str->data[i];i++){
		printf(".LC%d:\n",i);
		printf("    .string \"%s\"\n",(char*)vec_str->data[i]);
	}
	for(int i=0;vec_func->data[i];i++){
		func_gen((Func *)vec_func->data[i]);
	}
	return 0;
}
