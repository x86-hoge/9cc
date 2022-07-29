#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

enum{
	TK_NUM = 256,
	TK_IDENT,
	TK_EOF,
	TK_RETURN,
	TK_IF,
	TK_ELSE,
	TK_WHILE,
	TK_FOR,
	TK_INT,
	TK_SIZEOF,
};



typedef struct {
	int ty;
	int val;
	char *input;
	char *name;
}Token;

enum{
	ND_NUM = 256,
	ND_IDENT,
	ND_EQ,
	ND_NEQ,
	ND_FUNC,//260
	ND_CALL,
	ND_ARG,
	ND_RETURN,
	ND_IF,
	ND_ELSE,
	ND_BLOCK,
	ND_WHILE,
	ND_FOR,
	ND_LE,
	ND_INC,//270
	ND_SUB,
	ND_ADDR,
	ND_DEREF,
	ND_INT,
};

extern int pos;

typedef struct{
	void **data;
	int capacity;
	int len;
}Vector;

typedef struct Node Node;

typedef struct Type{
	enum { INT, PTR, ARRAY} ty;
  	struct Type *ptr_to;
	size_t array_size;
}Type;

typedef struct Variable{
	Type *type;
  	void *offset;
	enum { LOCAL=1,GLOBAL} scope;
}Variable;

typedef struct Node {
	int ty;
	Node *lhs; //左ノード
	Node *rhs; //右ノード
	Node *expr; 


	int val;
	char *name;
	Vector *stmts;//複数式　
	Vector *args;//関数コール時の引数


	/* 実装済 */
  	// "if" ( cond ) then "else" els
	// "while" ( cond ) body
  	// "for" ( init; cond; inc ) body

	Node *cond;
  	Node *then;
  	Node *els;
  	Node *init;
  	Node *inc;
 	Node *body;

	/* 未実装 */
  	// "do" body "while" ( cond )
  	// "switch" ( cond ) body
  	// "case" val ":" body
	Type *t;
}Node;

extern Node *code[];

typedef struct{
	Vector *keys;
	Vector *vals;
}Map;

typedef struct{
	char *name;//関数名
	Map *map;//ローカル変数マップ
	int valcnt;//変数カウント
	Vector *args;//引数格納
	int argcnt;//引数カウント
	Vector *code;//コード格納
}Func;

Node *new_node(int ty, Node *lhs, Node *rhs);

Node *new_node_ident(char* na);

Node *new_node_num(int val);

Node *func_parse(Node *node);

Vector *new_vector();

void vec_push(Vector *vec, void *elem);

extern Vector *vec_token;//トークンを格納する

extern Vector *vec_func;//関数単位を格納する

Map *new_map();

void map_put(Map *map, char *key, void *val);

void *map_get(Map *map,char *key);

void expect(int line, int expected, int actual);

void runtest();

void program();

void gen_lval(Node *node);

void gen(Node *node);

void tokenize(char *p);

int consume(int ty);

Func *con();//関数

void set_valmap(Map *map);//マップを関数に渡す

void func_gen(Func *func);

void funcsp();

extern Map *global_map;//ローカル変数マップ

extern int *gval_cnt;