#include "9cc.h"
Map *gen_map;
int branch_label_no = 0;

void set_valmap(Map *map){
	gen_map = map;
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
	printf("	mov rax, rbp\n");
	printf("	sub rax, %d\n",offset);
	printf("	push rax\n");
}

/* コード発行 */
void gen(Node *node){

	if (node->ty == ND_RETURN) {
    	gen(node->lhs);
    	printf("	pop rax\n");
    	printf("	mov rsp, rbp\n");
    	printf("	pop rbp\n");
    	printf("	ret\n");
    	return;
  	}

	if(node->ty == ND_NUM){
		printf("	push %d\n",node->val);
		return;
	}
	if(node->ty == ND_WHILE){
		printf(".Lbegin%d:\n",branch_label_no);
		gen(node->cond);
		printf("	pop rax\n");
		printf("	cmp rax, 0\n");
		printf("	je .Lend%d\n",branch_label_no);
		gen(node->body);
		printf("	push rax\n");
		printf("	jmp .Lbegin%d\n",branch_label_no);
		printf(".Lend%d:\n",branch_label_no);
		branch_label_no++;
		return;
	}

	
	if(node->ty == ND_IF){
		gen(node->cond);
		printf("	pop rax\n");
		printf("	cmp rax, 0\n");

		if(node->els){
			printf("	je .Lelse%d\n",branch_label_no);
			gen(node->then);
			printf(".Lelse%d:\n",branch_label_no);
			gen(node->els);
		}
		else{
			printf("	je .Lend%d\n",branch_label_no);
			gen(node->then);
		}
		printf("	push rax\n");
		printf(".Lend%d:\n",branch_label_no);
		branch_label_no++;
		return;
	}

	if(node->ty == ND_BLOCK){
		for(int i=0;(Node *)node->stmts->data[i];i++){
			gen((Node *)node->stmts->data[i]);
			printf("	pop rax\n");
		}
		return;
	}

	if(node->ty == ND_IDENT){
		gen_lval(node);
		printf("	pop rax\n");
		printf("	mov rax, [rax]\n");
		printf("	push rax\n");
		return;
	}

	if(node->ty == ND_FUNC){
		if(node->rhs != NULL){
			char *rgsr[6]={"rdi","rsi","rdx","rcx","r8","r9"};//レジスタ一覧
			Vector *arg = node->rhs->args;
			for(int i=0;i<6 && arg->data[i] != NULL;i++)
				printf("	mov %s, %d\n",rgsr[i],(int)(intptr_t)arg->data[i]);
		}

		printf("	call %s\n",node->lhs->name);
		return;
	}

	if(node->ty == '='){
		gen_lval(node->lhs);
		gen(node->rhs);

		printf("	pop rdi\n");
		printf("	pop rax\n");
		printf("	mov [rax], rdi\n");
		printf("	push rdi\n");
		return;
	}
	if(node->ty != ND_FUNC){
		gen(node->lhs);
		gen(node->rhs);
	}

	printf("	pop rdi\n");
	printf("	pop rax\n");

	if(node->ty == ND_EQ){
		printf("	cmp rdi, rax\n");
		printf("	sete al\n");
		printf("	movzb rax, al\n");
	}
	
	else if(node->ty == ND_NEQ){
		printf("	cmp rdi, rax\n");
		printf("	setne al\n");
		printf("	movzb rax, al\n");//５６ビットをクリア
	}

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
