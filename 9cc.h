#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum{
	TK_NUM = 256,
	TK_IDENT,
	TK_EOF,
};

typedef struct {
	int ty;
	int val;
	char *input;
}Token;

enum{
	ND_NUM = 256,	
	ND_IDENT,
};
extern int pos;
typedef struct Node {
	int ty;
	struct Node *lhs; //left hand side
	struct Node *rhs; //right hand side
	int val;
	char name;
}Node;
extern Node *code[];
typedef struct{
	void **data;
	int capacity;
	int len;
}Vector;

typedef struct{
	Vector *keys;
	Vector *vals;
}Map;


Node *code[100];

Node *new_node(int ty, Node *lhs, Node *rhs);

Node *new_node_ident(char* na);

Node *new_node_num(int val);

Vector *new_vector();

void vec_push(Vector *vec, void *elem);

extern Vector *vec_token;

Map *new_map();

void map_put(Map *map, char *key, void *val);

void *map_get(Map *map,char *key);

void expect(int line, int expected, int actual);

void runtest();

void program();

void gen_lval(Node *node);

void gen(Node *node);

void tokenize(char *p);
