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
	
	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	con();
	return 0;
}
