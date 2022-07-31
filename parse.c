#include "9cc.h"

Node *term();
Node *mul();
Node *add();
Node *stmt();
Node *expr();
Node *assign();
Node *func(Node *node);
Node *relational();
void checkval(Node *node);


Vector *vec_code;
Map *val_map;
int *val_cnt;
char *val_func;
int pos=0;
Vector *vec_func;

Map *global_map;
int *gval_cnt;



Func *new_func(char *name){
    val_func = name;
    Func *func = malloc(sizeof(Func));
    func->name = name;
    
    func->map=new_map();
    func->valcnt = 0;

    func->args=new_vector();
    func->argcnt = 0;

    func->code=new_vector();
    return func;
}

Node *new_node(int ty, Node *lhs, Node *rhs){
    Node *node = malloc(sizeof(Node));
    node->ty = ty;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}
Type *new_type(){
    Type *type = malloc(sizeof(Type));
    return type;
}
Type *new_type_ptr(){
    Type *t1 = new_type();
    if(consume('*')){
        t1->ty = PTR;
        t1->ptr_to = new_type_ptr(); 
    }
    else{
        t1->ty = INT;
        }
    return t1;
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

Vector *new_vector_stmts(){
    Vector *vec = new_vector();
    while(!consume('}')){
        vec_push(vec,(void *)stmt());
    }
    vec_push(vec,(void *)NULL);
    return vec;
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
    if(consume('[')){
        Node *index_node = term();
        if(!consume(']')){
            fprintf(stderr,"開きカッコに対応する閉じカッコがありません:%s\n",((Token *)vec_token->data[pos])->input);
            exit(1);
        }
        return index_node;
    }

    if(((Token *)vec_token->data[pos])->ty == TK_NUM){
		 Node *node = new_node_num(((Token *)vec_token->data[pos++])->val);//数値
        if(((Token *)vec_token->data[pos])->ty == '['){
            Node *index_node = term();
            Node *add_node = new_node('+',node,index_node);
            return new_node(ND_DEREF,add_node,NULL);
        }
        return node;
	}

    if(((Token *)vec_token->data[pos])->ty == TK_IDENT){
        Node *node = new_node_ident(((Token *)vec_token->data[pos++])->name);
		Vector *vec = new_vector();

        /*関数コールとして処理*/
		if(consume('(')){
			node->ty   = ND_CALL;
			while(!consume(')')){ vec_push(vec,(void *)expr()); }
			vec_push(vec,(void *)NULL);
			node->args = vec;
		}
        /* 添え字処理 */
        else if(((Token *)vec_token->data[pos])->ty == '['){
            Node *index_node = term();
            Node *add_node   = new_node('+',node,index_node);
            return new_node(ND_DEREF,add_node,NULL);
        }
		return node;
	}
    if(consume(TK_INT)){
        Node *node = calloc(1, sizeof(Node));
        Type *type  = new_type();

        if(consume('*')){ 
            type->ty = PTR; 
            type->ptr_to = new_type_ptr();
        }
        else{ type->ty = INT; }
        node->t = type;
        
        if(((Token *)vec_token->data[pos])->ty == TK_IDENT){
            node->ty   = ND_IDENT;
            node->name = ((Token *)vec_token->data[pos])->name;//変数名
            pos++;
           if(((Token *)vec_token->data[pos])->ty == '['){
            Node *index_node = term();
                if(index_node->ty == TK_NUM){
                  type->ty = ARRAY;
                  type->array_size = index_node->val;   
                }
            }
            checkval(node);
        }
        return node;
	}
	
    fprintf(stderr,"数値でも開きカッコでもないトークンです:%s\n",((Token *)vec_token->data[pos])->input);
    exit(1);
}

Node *unary(){
    if(consume(TK_SIZEOF)){
        Node *node =  unary();
        if(node->t->ty == INT) 
            { return new_node_num(4); }
        else
            { return new_node_num(8); }
    }
    if(consume('+')){
        return term();
    }
    if(consume('-')){
        return new_node('-',new_node_num(0),term());
    }
    if(consume('*')){
        return new_node(ND_DEREF,unary(),NULL);
    }
    if(consume('&')){
        return new_node(ND_ADDR,unary(),NULL);
    }
    return term();
}

Node *mul(){
    Node *node = unary();

    for(;;){
        if(consume('*')){
            node = new_node('*',node,unary());
        }
        else if(consume('/')){
            node = new_node('/',node,unary());
        }
        else{
            return node;
        }
    }
}


Node *add(){
    Node *node = mul();

    for(;;){
        if(consume('+')){
            node = new_node('+',node,mul());
        }
        else if(consume('-')){
            node = new_node('-',node,mul());
        }
        else{
            return node;
        }
    }
}
void program(){
    while(((Token *)vec_token->data[pos])->ty != '}')vec_push(vec_code,(void *)stmt());
    vec_push(vec_code,(void *)NULL);
}
Node *stmt(){
    Node *node;

    if(consume('{')){
        node = calloc(1, sizeof(Node));
        node->ty = ND_BLOCK;
        node->stmts = new_vector_stmts();
        
    }
    else if(consume(TK_IF)){
        node = calloc(1, sizeof(Node));
        if(consume('(')){
            node->ty = ND_IF;
            node->cond = expr(); //if([cond])
            if(!consume(')')){
                fprintf(stderr,"開きカッコに対応する閉じカッコがありません:%s\n",((Token *)vec_token->data[pos])->input);
                exit(1);
            }
            node->then = stmt();//if([cond]) [then]
            if(consume(TK_ELSE)){
                node->els = stmt();//if([cond]) [then] else [els]
            }
        }
        else{
            fprintf(stderr,"開きカッコがありません:%s\n",((Token *)vec_token->data[pos])->input);
            exit(1);
        }
    }
    else if(consume(TK_WHILE)){
        node = calloc(1, sizeof(Node));
        if(consume('(')){
            node->ty = ND_WHILE;
            node->cond = expr();
            if(!consume(')')){
                fprintf(stderr,"開きカッコに対応する閉じカッコがありません:%s\n",((Token *)vec_token->data[pos])->input);
                exit(1);
            }
            node->body = stmt();
        }
        else{
            fprintf(stderr,"開きカッコがありません:%s\n",((Token *)vec_token->data[pos])->input);
            exit(1);
        }
    }
    else if(consume(TK_FOR)){
        node = calloc(1, sizeof(Node));
        if(consume('(')){
            //for" ( init; cond; inc ) body
            node->ty = ND_FOR;
            node->init = expr();
            if(!consume(';')){
                fprintf(stderr,"';'ではないトークンです:%s\n",((Token *)vec_token->data[pos])->input);
                exit(1);
            }
            node->cond = expr();
            if(!consume(';')){
                fprintf(stderr,"';'ではないトークンです:%s\n",((Token *)vec_token->data[pos])->input);
                exit(1);
            }
            node->inc  = expr();
            if(!consume(')')){
                fprintf(stderr,"開きカッコに対応する閉じカッコがありません:%s\n",((Token *)vec_token->data[pos])->input);
                exit(1);
            }
            node->body = stmt();
        }
        else{
            fprintf(stderr,"開きカッコがありません:%s\n",((Token *)vec_token->data[pos])->input);
            exit(1);
        }
    }
    else{
        if (consume(TK_RETURN)) {
            node = calloc(1, sizeof(Node));
            node->ty = ND_RETURN;
            node->lhs = expr();
          }
        else{
            node = expr();
        }
        if(!consume(';')){
            fprintf(stderr,"';'ではないトークンです:%s\n",((Token *)vec_token->data[pos])->input);
            exit(1);
        }
    }
    return node;
}

Node *expr() {
    return assign();
}

Node *assign(){
    Node* node = relational();

    for(;;){
        if(consume('!')){
            if(consume('='))
                return new_node(ND_NEQ,node,assign());//!=
            else{
                fprintf(stderr,"式が不正です\n");
                exit(1);
            }
        }

        if(consume('=')){
            if(consume('=')){
				return new_node(ND_EQ,node,assign());//==
			}
            node = new_node('=',node,assign());
        }
        else { return node; }
    }
}

Node *relational(){
	Node* node = add();
    for(;;){
        if(consume('<')){
            if(consume('=')){
				return new_node(ND_LE,node,assign());//==
			}
            node = new_node('<',node,assign());
        }

        if(consume('>')){
             /*評価順を逆にする*/
            if(consume('=')){
				return new_node(ND_LE,assign(),node);// >=
			}
            node = new_node('<',assign(),node);// >
        }
        else { return node; }
    }
}

void checkval(Node *node){
    if(!map_get(val_map,node->name)){
        int allosize = 1;
        if(node->t->ty == ARRAY){
           allosize = (node->t->array_size);
        }
        *val_cnt+=allosize;
        Variable *t = calloc(1, sizeof(Variable));
        t->type   = node->t;
        t->scope = LOCAL;
        
        t->offset = (void*)(intptr_t)(*val_cnt * 8);
        map_put(val_map, node->name, (void*)t);
    }
}
void checkglobalval(Node *node){
    if(!map_get(global_map,node->name)){
        Variable *t = calloc(1, sizeof(Variable));
        t->type   = node->t;
        t->scope = GLOBAL;
        map_put(global_map, node->name, (void*)t);
    }
}

/* 型チェック */
Node* checkTyping(Token *tk){
    Node *node = calloc(1, sizeof(Node));
    Type *type = new_type();
    switch(tk->ty){
        case TK_INT:
            type->ty = INT;
            node->t  = type;
        break;
        case TK_CHAR:
            type->ty = CHAR;
            node->t  = type; 
        break;
        default:
            fprintf(stderr,"型がありません:%s\n",((Token *)vec_token->data[pos])->input);
            exit(1);
    }
    if(consume('*')){ 
        type->ty = PTR; 
        type->ptr_to = new_type_ptr();
    }
    return node;
}
char *ident(){
    if(((Token *)vec_token->data[pos])->ty == TK_IDENT){
        return ((Token *)vec_token->data[pos++])->name;
    }
    fprintf(stderr,"有効な名前ではありません:%s\n",((Token *)vec_token->data[pos])->input);
    exit(1);
}



Func *con(){ //パース
    char *name;
    for(;;){
        Node *node = checkTyping((Token *)vec_token->data[pos++]);
        name = ident();
        node->ty   = ND_IDENT;
        node->name = name;
        if(consume('[')){
            node->t->ty = ARRAY;
            if(((Token *)vec_token->data[pos])->ty == TK_NUM){
                node->t->array_size = ((Token *)vec_token->data[pos++])->val;
            }
            if(!consume(']')){
                fprintf(stderr,"対応する閉じカッコがありません:%s\n",((Token *)vec_token->data[pos])->input);
                exit(1);
            }
        }
        if(consume(';')){
            checkglobalval(node);
            continue;
        }
        if(consume('(')){ break; }
        fprintf(stderr,"例外構文エラー:%s\n",((Token *)vec_token->data[pos])->input);
        exit(1);
    }
    
    Func *func = new_func(name);
    val_map  = func->map;       /* 変数マップ */
    val_cnt  = &(func->valcnt); /* 変数カウント */
    vec_code = func->code;     /* コード格納 */
    
    /*引数チェック*/
    while(!consume(')')){
        if(consume(TK_INT)){
            Node *node = calloc(1, sizeof(Node));
            Type *type  = new_type();
            if(consume('*')){ 
                type->ty = PTR; 
                type->ptr_to = new_type_ptr();
            }
            else
                { type->ty = INT; }
            
            if(((Token *)vec_token->data[pos])->ty == TK_IDENT){
                node->ty   = ND_IDENT;
                node->name = ((Token *)vec_token->data[pos])->name; /* 変数名 */
                node->t = type;
                pos++;
	    	    func->valcnt += 1;
                func->argcnt += 1;
                vec_push(func->args,(void *)node);
                if(!map_get(func->map, node->name)){
                    Variable *t = calloc(1, sizeof(Variable));
                    t->type     = node->t;
                    t->offset   = (void*)(intptr_t)(func->argcnt*8);
                    map_put(func->map, node->name, (void*)t);
                }
            }else{
                fprintf(stderr,"変数名が不正です:%s\n",((Token *)vec_token->data[pos])->input);
                exit(1);
            }
	    }
    }
    if(consume('{')){
        program();//パース
        if(!consume('}')){
            fprintf(stderr,"閉じカッコがありません\n");
            exit(1);
        }
    }
    else{
        fprintf(stderr,"開きカッコがありません\n");
        exit(1);
    }
    return func;
}

void funcsp(){
    vec_func = new_vector();
    gval_cnt = malloc(sizeof(int));
	global_map = new_map();

    while(((Token *)vec_token->data[pos])->ty != TK_EOF){
        vec_push(vec_func,(void *)con());
    }

    vec_push(vec_func,(void *)NULL);
}