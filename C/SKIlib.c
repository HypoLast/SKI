#include <stdio.h>
#include <stdlib.h>
#include "SKIlib.h"
#include "leaker.h"

typedef struct Token {
	char token[2];
	struct Token *next;
} Token;

struct TreeNodeToken {
	TreeNode *tn;
	Token *t;
};

Token *tokenize(char *input);
Token *validate(Token *tokens);
void freeToken(Token *token);
struct TreeNodeToken makeTreeNode(Token *token);

void printASTRec(TreeNode *tn) {
	if (tn) {
		switch(tn->type) {
			case TYPE_BRANCH: {
				Branch *b = (Branch *)tn;
				printf("`");
				printASTRec(b->left);
				printASTRec(b->right);
				break;
			}
			case TYPE_S:
			case TYPE_K:
			case TYPE_I: {
				printf("%c", tn->type);
				break;
			}
			case TYPE_VAR: {
				printf("%c", ((SymbolLeaf *)tn)->symbol);
				break;
			}
			case TYPE_PRINT: {
				((SymbolLeaf *)tn)->symbol == '\n' ?
					printf("R") :
					printf(".%c", ((SymbolLeaf *)tn)->symbol);
				break;
			}
			case TYPE_KP: {
				printf("(K'");
				printASTRec(((Kp *)tn)->partial);
				printf(")");
				break;
			}
			case TYPE_SP: {
				printf("(S'");
				printASTRec(((Sp *)tn)->partial);
				printf(")");
				break;
			}
			case TYPE_SPP: {
				printf("(S''");
				printASTRec(((Spp *)tn)->partial1);
				printASTRec(((Spp *)tn)->partial2);
				printf(")");
				break;
			}
			case TYPE_APP: {
				printf("(");
				printASTRec(((Application *)tn)->f);
				printASTRec(((Application *)tn)->x);
				printf(")");
				break;
			}
			default: printf("?"); 
		}
	}
}

void printAST(TreeNode *tn) {
	printASTRec(tn);
	printf("\n");
}

TreeNode *makeTree(char *input) {
	Token *tokens = validate(tokenize(input));
	if (!tokens) return NULL;

	TreeNode *tn = makeTreeNode(tokens).tn;
	freeToken(tokens);
	return tn;
}

void freeTree(TreeNode *root) {
	switch(root->type) {
		case TYPE_S:
		case TYPE_K:
		case TYPE_I:
		case TYPE_PRINT:
		case TYPE_VAR: {
			lfree(root);
			break;
		}
		case TYPE_KP: {
			freeTree(((Kp *)root)->partial);
			lfree(root);
			break;
		}
		case TYPE_SP: {
			freeTree(((Sp *)root)->partial);
			lfree(root);
			break;
		}
		case TYPE_SPP: {
			freeTree(((Spp *)root)->partial1);
			freeTree(((Spp *)root)->partial2);
			lfree(root);
			break;
		}
		case TYPE_APP: {
			freeTree(((Application *)root)->f);
			freeTree(((Application *)root)->x);
			lfree(root);
			break;
		}
	}
}

struct TreeNodeToken makeTreeNode(Token *token) {
	if (token->token[0] == '`') {
		struct TreeNodeToken tnt = makeTreeNode(token->next);
		TreeNode *left = tnt.tn;
		tnt = makeTreeNode(tnt.t);
		Branch *branch = lmalloc(sizeof(Branch));
		branch->super.type = TYPE_BRANCH;
		branch->left = left;
		branch->right = tnt.tn;
		return (struct TreeNodeToken){ (TreeNode *)branch, tnt.t };
	} else {
		char t = token->token[0];
		TreeNode *leaf;
		if (t == 'S') {
			leaf = lmalloc(sizeof(Leaf));
			leaf->type = TYPE_S;
		} else if (t == 'K') {
			leaf = lmalloc(sizeof(Leaf));
			leaf->type = TYPE_K;
		} else if (t == 'I') {
			leaf = lmalloc(sizeof(Leaf));
			leaf->type = TYPE_I;
		} else if (t == '.') {
			SymbolLeaf *l = lmalloc(sizeof(SymbolLeaf));
			leaf = (TreeNode *)l;
			l->super.type = TYPE_PRINT;
			l->symbol = token->token[1];
		} else if (t == 'v') {
			SymbolLeaf *l = lmalloc(sizeof(SymbolLeaf));
			leaf = (TreeNode *)l;
			l->super.type = TYPE_VAR;
			l->symbol = token->token[1];
		}
		return (struct TreeNodeToken){ leaf, token->next };
	}
}

Token *validate(Token *tokens) {
	if (!tokens) return NULL;
	int count = 0;
	int implicit = 0;
	Token *curr = tokens;
	while(curr) {
		if (curr->token[0] == '`') count ++;
		else if (curr->next) {
			if (count == 0) {
				implicit ++;
			} else count --;
		}
		curr = curr->next;
	}
	if (count > 0) {
		fprintf(stderr, "Parse failed, found ` with insufficient arguments.\n");
		return NULL;
	}
	Token *tick;
	for (; implicit > 0; implicit --) {
		tick = lmalloc(sizeof(Token));
		tick->token[0] = '`';
		tick->next = tokens;
		tokens = tick;
	}
	return tokens;
}

Token *tokenize(char *input) {
	if (!input) return NULL;
	int i;
	Token *tok;
	Token *tokens = NULL;
	Token *curr = NULL;

	for (i = 0;; i ++) {
		if (input[i] == '\0') {
			return tokens; 
		}
		if (input[i] == ' ' || input[i] == '\n' || input[i] == '\t' || input[i] == '\r') continue;

		tok = lmalloc(sizeof(Token));
		tok->next = NULL;
		if (!tokens) tokens = tok;
		if (curr) curr->next = tok;
		curr = tok;
		
		if (input[i] == 'I' || input[i] == 'K' || input[i] == 'S' || input[i] == '`') {
			tok->token[0] = input[i];
		} else if (input[i] == '.') {
			tok->token[0] = '.';
			if (input[i + 1] == '\0') {
				fprintf(stderr, "Parse failed, invalid output with no character at position %d.\n", i);
				return NULL;
			}
			tok->token[1] = input[i + 1];
			i ++;
		} else if (input[i] == 'R') {
			tok->token[0] = '.';
			tok->token[1] = '\n';
		} else if ((input[i] >= 'a' && input[i] <= 'z') || input[i] == '_') {
			tok->token[0] = 'v';
			tok->token[1] = input[i];
		} else {
			fprintf(stderr, "Parse failed, illegal character %c at position %d.\n", input[i], i);
			return NULL;
		}
	}
}

void freeToken(Token *token) {
	if (!token) return;
	if (token->next) freeToken(token->next);
	lfree(token);
}