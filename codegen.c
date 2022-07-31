#include "9cc.h"
static char *rgsr[6]={"rdi","rsi","rdx","rcx","r8","r9"};//レジスタ一覧
Map *local_map;
Map *global_map;
int branch_label_no = 0;
void gen(Node *node);
void ptr_gen(Node *node,Type *t);
void set_valmap(Map *map){
    local_map = map;
}

int roundup(int x, int align) {
  return (x + align - 1) & ~(align - 1);
}



Variable *get_var(Map *map,Node *node){
    if(map_get(map,node->name)){
        return (Variable*)map_get(map,node->name);
    }
    return NULL;
}
Variable *get_global_var(Node *node){ return get_var(global_map,node); }

Variable *get_local_var(Node *node) { return get_var(local_map,node);  }

Type *get_ident_type(Node *node){
    if(!get_local_var(node)){
        if(!get_global_var(node)){
           return NULL;
        }
        return (Type*)get_global_var(node)->type; 
    }
    return (Type*)get_local_var(node)->type;
}



int get_local_ident_offset(Node *node){
    Variable *v = map_get(local_map,node->name);
    int offset  = (int)(intptr_t)v->offset; 
    return offset;
}

int ident_check(Node *node){
    if(node->ty != ND_IDENT){
        fprintf(stderr,"代入の左辺値が変数ではありません:%d\n",node->ty);
        exit(1);
    }
    return 1;
}

void gen_lval(Node *node){
    ident_check(node);
    /* ローカル変数 */
    if(map_get(local_map,node->name)){
        printf("    mov rax, rbp\n");
        printf("    sub rax, %d\n",get_local_ident_offset(node));
        printf("    push rax\n");
    }
    /* グローバル変数 */
    else{
         if(!map_get(global_map,node->name)){
            fprintf(stderr,"未定義の変数です:%s\n",node->name);
            exit(1);
        }
        if(get_ident_type(node)->ty == CHAR){
            printf("    lea rax, BYTE PTR %s[rip]\n", node->name);
        }else{
            printf("    lea rax, DWORD PTR %s[rip]\n", node->name);
        }
        printf("    push rax\n");
    }
}

void gen_cmp(char *op){
    printf("    cmp rax, rdi\n");
    printf("    %s al\n",op);
    printf("    movzb rax, al\n");
}

int gen_num(Node *node){
    if(node->ty == ND_NUM){
        return node->val;
    }
    return 0;
}

void ptr_gen(Node *node,Type *t){
    switch(node->ty){
        case ND_NUM:
            printf("    push %d\n",(node->val));
            if(t->ptr_to->ty == INT){
                printf("    pop rax\n");
                printf("    push 4\n");
                printf("    pop rdi\n");
                printf("    mul rdi\n");
                printf("    push rax\n");
            }
            else{
                printf("    pop rax\n");
                printf("    push 8\n");
                printf("    pop rdi\n");
                printf("    push rax\n");
            }
            return;
            
        case ND_IDENT:
            gen_lval(node);
            if(get_local_var(node) && get_ident_type(node)->ty != ARRAY){
                printf("    pop rax\n");
                printf("    mov rax, [rax]\n");
                printf("    push rax\n");
            }
            return;

        case ND_ADDR:
            gen_lval(node->lhs);
            return;

        case ND_DEREF:
            gen(node->lhs);
            printf("    pop rax\n");
            printf("    mov rax, [rax]\n");
            printf("    push rax\n");
            return;
        case ND_CALL:
            if(node->args != NULL){
                for(int i=0;i<6 && node->args->data[i];i++){
                    gen((Node *)node->args->data[i]);
                    printf("    pop rax\n");
                    printf("    mov %s, rax\n",rgsr[i]);
                }
            }
            
            printf("    call %s\n",node->name);
            printf("    push rax\n");
            return;
    }
    if(node->ty != ND_FUNC){
        if(node->lhs->ty == ND_NUM)
            { ptr_gen(node->lhs,t); }
        else
            { gen(node->lhs); }
        if(node->rhs->ty == ND_NUM)
            { ptr_gen(node->rhs,t); }
        else
            { gen(node->rhs); }
    }
    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch(node->ty){
        case ND_EQ:
            gen_cmp("sete");
            break;
        case ND_NEQ:
            gen_cmp("setne");
            break;
        case ND_LE:
            gen_cmp("setle");
            break;
        case '<':
            gen_cmp("setl");
            break;
        case '+':
            printf("    add rax, rdi\n");
            break;
        case '-':
            printf("    sub rax, rdi\n");
            break;
        case '*':
            printf("    mul rdi\n");
            break;
        case '/':
            printf("    mov rdx, 0\n");
            printf("    div rdi\n");
    }
    printf("    push rax\n");
}
/* コード発行 */
void gen(Node *node){
    int label;
    switch(node->ty){
        case ND_RETURN:
            gen(node->lhs);
            printf("    pop rax\n");
            printf("    mov rsp, rbp\n");
            printf("    pop rbp\n");
            printf("    ret\n");
            return;

        case ND_NUM:
            printf("    push %d\n",gen_num(node));
            return;

        case ND_WHILE:
            label = branch_label_no++;
            printf(".Lbegin%d:\n",label);
            gen(node->cond);
            printf("    pop rax\n");
            printf("    cmp rax, 0\n");
            printf("    je .Lend%d\n",label);
            gen(node->body);
            printf("    push rax\n");
            printf("    jmp .Lbegin%d\n",label);
            printf(".Lend%d:\n",label);
            return;
            
        case ND_ADDR:
            gen_lval(node->lhs);
            return;

        case ND_DEREF:
            gen(node->lhs);
            printf("    pop rax\n");
            printf("    mov rax, [rax]\n");
            printf("    push rax\n");
            return;

        case ND_FOR:
            label = branch_label_no++;
            gen(node->init);
            printf(".Lbegin%d:\n",label);
            gen(node->cond);
            printf("    pop rax\n");
            printf("    cmp rax, 0\n");
            printf("    je .Lend%d\n",label);
            gen(node->body);
            gen(node->inc);
            printf("    push rax\n");
            printf("    jmp .Lbegin%d\n",label);
            printf(".Lend%d:\n",label);
            return;

        case ND_IF:
            label = branch_label_no++;
            gen(node->cond);
            printf("    pop rax\n");
            printf("    cmp rax, 0\n");

            if(node->els){
                printf("    je .Lelse%d\n",label);
                gen(node->then);
                printf("    jmp .Lend%d\n",label);
                printf(".Lelse%d:\n",label);
                gen(node->els);
            }
            else{
                printf("    je .Lend%d\n",label);
                gen(node->then);
            }
            printf("    push rax\n");
            printf(".Lend%d:\n",label);
            return;

        case ND_BLOCK:
            for(int i=0;(Node *)node->stmts->data[i];i++){
                gen((Node *)node->stmts->data[i]);
                printf("    pop rax\n");
            }
            return;
            
        case ND_IDENT:
            gen_lval(node);
            if(get_ident_type(node)->ty != ARRAY){
                printf("    pop rax\n");
                printf("    mov rax, [rax]\n");
                printf("    push rax\n");
            }
            return;

        case ND_CALL:
            if(node->args != NULL){
                for(int i=0;i<6 && node->args->data[i];i++){
                    gen((Node *)node->args->data[i]);
                    printf("    pop rax\n");
                    printf("    mov %s, rax\n",rgsr[i]);
                }
            }
            
            printf("    call %s\n",node->name);
            printf("    push rax\n");
            return;

        case '=':
            if(node->lhs->ty == ND_DEREF){
                    gen(node->lhs->lhs);
                    gen(node->rhs);
                    printf("    pop rdi\n");
                    printf("    pop rax\n");
                    printf("    mov [rax], rdi\n");
                    printf("    push rdi\n");
                    return;
            }
            if(node->lhs->ty == ND_IDENT){
                gen_lval(node->lhs);
                if(get_ident_type(node->lhs)->ty == PTR){
                    ptr_gen(node->rhs,get_ident_type(node->lhs));
                }else{
                    gen(node->rhs);
                }
            }
            else{
                gen_lval(node->lhs);
                gen(node->rhs);
            }
            printf("    pop rdi\n");
            printf("    pop rax\n");
            printf("    mov [rax], rdi\n");
            printf("    push rdi\n");
            return;
    }
    if(node->ty != ND_FUNC){
        gen(node->lhs);
        gen(node->rhs);
    }

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch(node->ty){
        case ND_EQ:
            gen_cmp("sete");
            break;
        case ND_NEQ:
            gen_cmp("setne");
            break;
        case ND_LE:
            gen_cmp("setle");
            break;
        case '<':
            gen_cmp("setl");
            break;
        case '+':
            printf("    add rax, rdi\n");
            break;
        case '-':
            printf("    sub rax, rdi\n");
            break;
        case '*':
            printf("    mul rdi\n");
            break;
        case '/':
            printf("    mov rdx, 0\n");
            printf("    div rdi\n");
    }
    printf("    push rax\n");
}

void func_gen(Func *func){
    printf("%s:\n",func->name);//変数名
    printf("    push rbp\n");
    printf("    mov rbp, rsp\n");
    printf("    sub rsp, %d\n",roundup(func->valcnt*8,16));
    printf("    push 1\n");
    printf("    push 1\n");
    printf("    push 1\n");
    printf("    push 1\n");
    set_valmap(func->map);//gen関数に必要な変数データを渡す
    if(func->argcnt >0){
        
        for(int i=0; i<6 && i<func->argcnt ;i++){
            printf("    mov rax, rbp\n");
            printf("    sub rax, %d\n",get_local_ident_offset((Node *)func->args->data[i]));
            printf("    mov [rax],%s\n",rgsr[i]);
        }
    }
    for(int i=0;(Node *)func->code->data[i];i++){
        gen((Node *)func->code->data[i]);
        printf("    pop rax\n");
    }
    
    printf("    mov rsp, rbp\n");
    printf("    pop rbp\n");
    printf("    ret\n");
}