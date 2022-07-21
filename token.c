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
Token *new_token_ident(int ty,char* input,char* name){
	Token *tkn = malloc(sizeof(Token));
	tkn->ty=ty;
	tkn->input=input;
	tkn->name=name;
	return tkn;
}

int is_alnum(char c) {
  return ('a' <= c && c <= 'z') ||
         ('A' <= c && c <= 'Z') ||
         ('0' <= c && c <= '9') ||
         (c == '_');

	//return isalpha(c) || isdigit(c) || c == '_'
}

Vector *vec_token;

void tokenize(char *p){
	vec_token=new_vector();
	int i = 0;
	while(*p){

		if(isspace(*p) || *p == ','){
			p++;
			continue;
		}

		if(strchr("+-*/;=()!{}",*p)){
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

		if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6])) {
			vec_push(vec_token,(void *)new_token(TK_RETURN,p));
 			i++;
  			p += 6;
  			continue;
		}
		if (strncmp(p, "if", 2) == 0 && !is_alnum(p[2])) {
			vec_push(vec_token,(void *)new_token(TK_IF,p));
 			i++;
  			p += 2;
  			continue;
		}
		if (strncmp(p, "else", 4) == 0 && !is_alnum(p[4])) {
			vec_push(vec_token,(void *)new_token(TK_ELSE,p));
 			i++;
  			p += 4;
  			continue;
		}
		if (strncmp(p, "while", 5) == 0 && !is_alnum(p[5])) {
			vec_push(vec_token,(void *)new_token(TK_WHILE,p));
 			i++;
  			p += 5;
  			continue;
		}

		if(isalpha(*p)){ //文字探し
			int strcnt=1;
			while(isalpha(p[strcnt]))strcnt++;
			char *val=malloc(sizeof(char) * strcnt+1);
			strncpy(val,p,strcnt);//変数名部分をトークン化
			val[strcnt]='\0';//終端
			vec_push(vec_token,(void *)new_token_ident(TK_IDENT,p,val));
			i++;
			p+=strcnt;
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
