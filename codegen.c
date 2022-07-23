#include "9cc.h"
Map *gen_map;
int branch_label_no = 0;

void set_valmap(Map *map){
    gen_map = map;
}
int new_branch_label_no(){
    return branch_label_no++;
}
void gen_lval(Node *node){
    if(node->ty != ND_IDENT){
        fprintf(stderr,"代入の左辺値が変数ではありません");
        exit(1);
    }
    if(!map_get(gen_map,node->name)){
        fprintf(stderr,"未定義の変数です:%s\n",node->name);
        exit(1);
    }
    int offset = (int)(intptr_t)map_get(gen_map,node->name); 
    printf("    mov rax, rbp\n");
    printf("    sub rax, %d\n",offset);
    printf("    push rax\n");
}

void gen_cmp(char *op){
    printf("    cmp rax, rdi\n");
    printf("    %s al\n",op);
    printf("    movzb rax, al\n");
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
            printf("    push %d\n",node->val);
            return;

        case ND_WHILE:
            label = branch_label_no;
            branch_label_no++;
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
            label = branch_label_no;
            branch_label_no++;
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
            label = branch_label_no;
            branch_label_no++;
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
            printf("    pop rax\n");
            printf("    mov rax, [rax]\n");
            printf("    push rax\n");
            return;

        case ND_CALL:
            if(node->args != NULL){
                char *rgsr[6]={"rdi","rsi","rdx","rcx","r8","r9"};//レジスタ一覧
                for(int i=0;i<6 && node->args->data[i];i++){
                    gen((Node *)node->args->data[i]);
                    printf("    pop rax\n");
                    printf("    mov %s, rax\n",rgsr[i]);
                }
            }
            printf("    call %s\n",node->name);
            return;

        case '=':
            gen_lval(node->lhs);
            gen(node->rhs);

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
    printf("%s:\n",func->name->lhs->name);//変数名
    printf("    push rbp\n");
    printf("    mov rbp, rsp\n");
    printf("    sub rsp, %d\n",func->cnt*8);
    
    set_valmap(func->map);//gen関数に必要な変数データを渡す

    if(func->name->rhs != NULL){
        int offset;
        char *rgsr[6]={"rdi","rsi","rdx","rcx","r8","r9"};//レジスタ一覧
        printf("    mov rax, rbp\n");
        for(int i=0;i<6&&func->name->rhs->args->data[i];i++){
            offset =(int)map_get(func->map,(char *)func->name->rhs->args->data[i]);
            printf("    sub rax, %d\n",offset);
            printf("    mov rax, %s\n",rgsr[i]);
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