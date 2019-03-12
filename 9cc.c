#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//トークンの型を表す値
enum{
	TK_NUM = 256,
	TK_IDENT,
	TK_EOF,
};

//トークン型定義
typedef struct {
	int ty;
	int val;
	char *input;
}Token;
int pos=0;
enum{
	ND_NUM = 256,	
};
Token tokens[100];

typedef struct Node {
	int ty;
	struct Node *lhs; //left hand side
	struct Node *rhs; //right hand side
	int val;
}Node;
//可変長ベクター
Node *add();
Node *mul();
Node *term();
void error(int i);

typedef struct{
	void **data;
	int capacity;
	int len;
}Vector;

Node *new_node(int ty, Node *lhs, Node *rhs){
	Node *node = malloc(sizeof(Node));
	node->ty = ty;
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

Node *new_node_num(int val){
	Node *node = malloc(sizeof(Node));
	node->ty=ND_NUM;
	node->val=val;
	return node;
}

Vector *new_vector(){
	Vector *vec = malloc(sizeof(Vector));
	vec->data = malloc(sizeof(void *) * 16);
	vec->capacity = 16;
	vec->len=0;
	return vec;
}
void vec_push(Vector *vec, void *elem){
	if(vec->capacity == vec->len){
		vec->capacity *=2;
		vec->data = realloc(vec->data,sizeof(void *) * vec->capacity);
	}
	vec->data[vec->len++] = elem;
}

void expect(int line, int expected, int actual){
	if(expected == actual)return;
	fprintf(stderr,"%d: %d expected, but got %d\n",line,expected,actual);
	exit(1);
}

void runtest(){
	Vector *vec = new_vector();
	expect(__LINE__,0,vec->len);

	for(int i=0;i < 100;i++)vec_push(vec,(void *)i);

	expect(__LINE__,100,vec->len);
	expect(__LINE__,0,(int)vec->data[0]);
	expect(__LINE__,50,(int)vec->data[50]);
	expect(__LINE__,99,(int)vec->data[99]);

	printf("OK\n");
	
}
int consume(int ty){
	if(tokens[pos].ty != ty)return 0;
	pos++;
	return 1;
}

Node *term(){
	if(consume('(')){
		Node *node = add();
		if(!consume(')'))printf("開きカッコに対応する閉じカッコがありません:%s",tokens[pos].input);
		return node;
	}

	if(tokens[pos].ty == TK_NUM)return new_node_num(tokens[pos++].val);
	printf("数値でも開きカッコでもないトークンです:%s",tokens[pos++].input);
}

Node *mul(){
	Node *node = term();

	for(;;){
		if(consume('*'))node = new_node('*',node,term());
		else if(consume('/'))node = new_node('/',node,term());
		else return node;
	}
}


Node *add(){
	Node *node = mul();

	for(;;){
		if(consume('+'))node = new_node('+',node,mul());
		else if(consume('-'))node = new_node('-',node,mul());
		else return node;
	}
}

void gen(Node *node){
	if(node->ty == ND_NUM){
		printf("	push %d\n",node->val);
		return;
	}
	gen(node->lhs);
	gen(node->rhs);

	printf("	pop rdi\n");
	printf("	pop rax\n");

	switch(node->ty){
		case '+':
			printf("	add rax, rdi\n");
			break;
		case '-':
			printf("	sub rax, rdi\n");
			break;
		case '*':
			printf("	mul rdi\n");
			break;
		case '/':
			printf("	mov rdx, 0\n");
			printf("	div rdi\n");
	}
	printf("	push rax\n");
}

void tokenize(char *p){
	int i = 0;
	while(*p){

		if(isspace(*p)){
			p++;
			continue;
		}

		if(*p == '+' || *p == '-'|| *p == '*'|| *p == '/' || *p == '(' || *p == ')'){
			tokens[i].ty = *p;
			tokens[i].input = p;
			i++;
			p++;
			continue;
		}

		if(isdigit(*p)){
			tokens[i].ty = TK_NUM;
			tokens[i].input = p;
			tokens[i].val = strtol(p,&p,10);
			i++;
			continue;
		}
		if('a' <= *p && *p <= 'z'){
			tokens[i].ty = TK_IDENT;
			tokens[i].input = p;
			i++;
			p++;
			continue;
		}

		fprintf(stderr,"トークナイズできません: %s\n",p);
		exit(1);
	}

	tokens[i].ty = TK_EOF;
	tokens[i].input = p;
}

void error(int i){
	fprintf(stderr,"予期しないトークンです: %s\n",tokens[i].input);
	exit(1);
}
int main(int argc,char **argv){
	if(argc != 2){
		fprintf(stderr,"引数の個数が正しくありません\n");
		return 1;
	}
	if(!strcmp(argv[1],"-test")){
		runtest();
		return 0;
	}
	tokenize(argv[1]);
	Node *node = add();

	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");

	gen(node);

	printf("	pop rax\n");
	printf("	ret\n");
	return 0;
}
