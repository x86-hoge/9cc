#include "9cc.h"

Token *new_token(int ty,char* input){
	Token *tkn = malloc(sizeof(Token));
	tkn->ty=ty;
	tkn->input=input;
	return tkn;
}

Token *new_token_num(int ty,int val,char* input){
	Token *tkn = malloc(sizeof(Token));
	tkn->ty=ty;
	tkn->val=val;
	tkn->input=input;
	return tkn;
}

Vector *vec_token;

void tokenize(char *p){
	vec_token=new_vector();
	int i = 0;
	while(*p){

		if(isspace(*p)){
			p++;
			continue;
		}

		if(*p == '+' || *p == '-'|| *p == '*'|| *p == '/' 
				|| *p == '(' || *p == ')' || *p == '=' || *p == ';'){
			vec_push(vec_token,(void *)new_token(*p,p));
			i++;
			p++;
			continue;
		}

		if(isdigit(*p)){
			vec_push(vec_token,(void *)new_token_num(TK_NUM,strtol(p,&p,10),p));
			i++;
			continue;
		}
		if('a' <= *p && *p <= 'z'){
			//tokens[i].ty = TK_IDENT;
			//tokens[i].input = p;
			vec_push(vec_token,(void *)new_token(TK_IDENT,p));
			i++;
			p++;
			continue;
		}

		fprintf(stderr,"トークナイズできません: %s\n",p);
		exit(1);
	}
	vec_push(vec_token,new_token(TK_EOF,p));

}

void error(int i){
	fprintf(stderr,"予期しないトークンです: %s\n",((Token *)vec_token->data[vec_token->len])->input);
	exit(1);
}
