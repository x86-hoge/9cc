#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

enum{
	TK_NUM = 256,
	TK_IDENT,
	TK_EOF,
	TK_RETURN,
	TK_IF,
	TK_ELSE,
	TK_WHILE,
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
	ND_FUNC,
	ND_ARG,
	ND_RETURN,
	ND_IF,
	ND_ELSE,
	ND_BLOCK,
	ND_WHILE,
};

extern int pos;

typedef struct{
	void **data;
	int capacity;
	int len;
}Vector;

typedef struct Node {
	int ty;
	struct Node *lhs; //左ノード
	struct Node *rhs; //右ノード
	struct Node *expr; //評価
	int val;
	char *name;
	Vector *stmts;//複数式　
	Vector *args;
}Node;

extern Node *code[];

typedef struct{
	Vector *keys;
	Vector *vals;
}Map;

typedef struct{
	Node *name;//関数名
	Map *map;//変数保存
	int cnt;//変数カウント
	Vector *code;//コード格納
}Func;

Node *new_node(int ty, Node *lhs, Node *rhs);

Node *new_node_ident(char* na);

Node *new_node_num(int val);

Node *func_parse(Node *node);

Vector *new_vector();

void vec_push(Vector *vec, void *elem);

extern Vector *vec_token;//トークンを格納する

extern Vector *vec_func;//関数を格納する

Map *new_map();

void map_put(Map *map, char *key, void *val);

void *map_get(Map *map,char *key);

void expect(int line, int expected, int actual);

void runtest();

void program(Map *map,int *cnt,Vector *code);

void gen_lval(Node *node);

void gen(Node *node);

void tokenize(char *p);

int consume(int ty);

Func *con();//関数

void set_valmap(Map *map);//マップを関数に渡す

void built(Func *func);

void funcsp();
