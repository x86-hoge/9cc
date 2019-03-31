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
	printf(".global main\n");
	for(int i=0;vec_func->data[i];i++)built((Func *)vec_func->data[i]);
	return 0;
}
