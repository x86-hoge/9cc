#include "9cc.h"
Node *term();
Node *mul();
Node *add();
Node *stmt();
Node *assign();
void checkval(Node *node);

int pos=0;
Vector *vec_code;
Map *val_map;
int val_cnt;

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
Node *new_node_ident(char* name){
	Node *node = malloc(sizeof(Node));
	node->ty = ND_IDENT;
	node->name=name;
	return node;
}
Node *new_node_arg(){
	Node *node = malloc(sizeof(Node));
	node->ty = ND_ARG;
	node->args=new_vector();
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
		return new_node_num(((Token *)vec_token->data[pos++])->val);//数値
	if(((Token *)vec_token->data[pos])->ty == TK_IDENT)
		return new_node_ident(((Token *)vec_token->data[pos++])->name);//変数名
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
	val_map = new_map();//変数マップ
	val_cnt=0;//変数カウント
	vec_code = new_vector();//コード格納
	while(((Token *)vec_token->data[pos])->ty != TK_EOF) vec_push(vec_code,(void *)stmt());
	vec_push(vec_code,(void *)NULL);
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

	if(node->ty == ND_IDENT && consume('(')){
		if(!consume(')')){
			node = new_node(ND_FUNC,node,new_node_arg());//関数
			while(!consume(')')){
				vec_push(node->rhs->args,((Token *)vec_token->data[pos++])->val);
			}
			return node;
		}else
			return new_node(ND_FUNC,node,NULL);
	}
	for(;;){
		if(consume('!')){
			if(consume('='))
				return new_node(ND_NEQ,node,assign());//!=
			else{
				fprintf(stderr,"式が不正です");
				exit(1);
			}
		}

	if(consume('=')){
		if(consume('='))return new_node(ND_EQ,node,assign());//==
		checkval(node);
		node = new_node('=',node,assign());
	}

	else return node;
	}
}

void checkval(Node *node){
	if(!map_get(val_map,node->name)){
		val_cnt++;
		map_put(val_map, node->name,(void *)(val_cnt*8));
	}
}
