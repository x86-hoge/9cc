#include "9cc.h"
Node *term();
Node *mul();
Node *add();
Node *stmt();
Node *assign();

int pos=0;
Vector *vec_code;

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
Node *new_node_ident(char* na){
	Node *node = malloc(sizeof(Node));
	node->ty = ND_IDENT;
	node->name=na;
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

int consume(int ty){
	if(((Token *)vec_token->data[pos])->ty != ty)return 0;
	pos++;
	return 1;
}

Node *term(){
	if(consume('(')){
		Node *node = add();
		if(!consume(')')){
			fprintf(stderr,"開きカッコに対応する閉じカッコがありません:%s\n",((Token *)vec_token->data[pos])->input);
			exit(1);
		}
		return node;
	}

	if(((Token *)vec_token->data[pos])->ty == TK_NUM)
		return new_node_num(((Token *)vec_token->data[pos++])->val);//plus
	if(((Token *)vec_token->data[pos])->ty == TK_IDENT)
		return new_node_ident(((Token *)vec_token->data[pos++])->input);//plus
	fprintf(stderr,"数値でも開きカッコでもないトークンです:%s\n",((Token *)vec_token->data[pos])->input);
	exit(1);
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

void program(){
	int i=0;
	vec_code = new_vector();
	while(((Token *)vec_token->data[pos])->ty != TK_EOF) vec_push(vec_code,(void *)stmt());
	vec_push(vec_code,(void *)NULL);
	//code[i] = NULL;
}

Node *stmt(){
	Node *node = assign();
	if(!consume(';')){
	fprintf(stderr,"';'ではないトークンです:%s\n",((Token *)vec_token->data[pos])->input);
	exit(1);
	}
	return node;
}

Node *assign(){
	Node* node = add();
	for(;;){
	if(consume('='))node = new_node('=',node,assign());
	else return node;
	}
}
