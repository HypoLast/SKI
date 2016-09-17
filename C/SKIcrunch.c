#include <stdio.h>
#include <stdlib.h>
#include "SKIlib.h"
#include "leaker.h"

TreeNode *process(Branch *b);
QN *buildQ(TreeNode *tn, TreeNode *parent, QN *q);
TreeNode *copyLeaf(TreeNode *leaf);

char buffer[1024*1024] = { 0 };
int bufferIdx = 0;

void execute(char *expression, ExecOpts options) {
	TreeNode *tn = makeTree(expression);
	if (!tn) return;
	CrunchStep cs = { tn, NULL };
	do {
		cs = simplify(cs.root, cs.processQ);
	} while(cs.processQ);
	printf(" = ");
	printAST(cs.root);
	freeTree(cs.root);
}

CrunchStep simplify(TreeNode *root, QN *processQ) {
	if (!processQ) {
		processQ = buildQ(root, NULL, NULL);
	}
	if (!processQ) {
		return (CrunchStep){ root, NULL };
	}
	QN *next = processQ;
	processQ = processQ->next;
	TreeNode *tn = process(next->proc);
	if (tn->type == TYPE_BRANCH) {
		processQ = buildQ(tn, (TreeNode *)next->parent, processQ);
	}
	if (next->parent) {
		if (next->parent->left == (TreeNode *)next->proc) next->parent->left = tn;
		else next->parent->right = tn;
	} else {
		root = tn;
	}
	lfree(next);
	return (CrunchStep){ root, processQ };
}

TreeNode *process(Branch *b) {
	TreeNode *result;
	switch(b->left->type) {
		case TYPE_I: {
			result = b->right;
			lfree(b->left);
			break;
		}
		case TYPE_K:
		case TYPE_S: {
			Kp *pp = lmalloc(sizeof(Kp));
			pp->super.type = b->left->type == TYPE_K ? TYPE_KP : TYPE_SP;
			pp->partial = b->right;
			result = (TreeNode *)pp;
			lfree(b->left);
			break;
		}
		case TYPE_PRINT: {
			char s = ((SymbolLeaf *)b->left)->symbol;
			buffer[bufferIdx] = s;
			bufferIdx ++;
			if (s == '\n') {
				printf(buffer);
				bufferIdx = 0;
			}
			buffer[bufferIdx] = '\0';
			result = b->right;
			lfree(b->left);
			break;
		}
		case TYPE_VAR:
		case TYPE_APP: {
			Application *app = lmalloc(sizeof(Application));
			app->super.type = TYPE_APP;
			app->f = b->left;
			app->x = b->right;
			result = (TreeNode *)app;
			break;
		}
		case TYPE_KP: {
			result = ((Kp *)b->left)->partial;
			lfree(b->left);
			freeTree(b->right);
			break;
		}
		case TYPE_SP: {
			Spp *spp = lmalloc(sizeof(Spp));
			spp->super.type = TYPE_SPP;
			spp->partial1 = ((Sp *)b->left)->partial;
			spp->partial2 = b->right;
			result = (TreeNode *)spp;
			lfree(b->left);
			break;
		}
		case TYPE_SPP: {
			Spp *spp = (Spp *)b->left;
			TreeNode *x = spp->partial1;
			TreeNode *y = spp->partial2;
			TreeNode *z1 = b->right;
			TreeNode *z2 = copyLeaf(b->right);
			Branch *b1 = lmalloc(sizeof(Branch));
			b1->super.type = TYPE_BRANCH;
			Branch *b2 = lmalloc(sizeof(Branch));
			b2->super.type = TYPE_BRANCH;
			Branch *b3 = lmalloc(sizeof(Branch));
			b3->super.type = TYPE_BRANCH;
			b1->left = (TreeNode *)b2;
			b1->right = (TreeNode *)b3;
			b2->left = x;
			b2->right = z1;
			b3->left = y;
			b3->right = z2;

			result = (TreeNode *)b1;
			lfree(spp);
			break;
		}
		default: {
			fprintf(stderr, "Fatal error in processing...");
			exit(1);
		}
	}
	lfree(b);
	return result;
}

TreeNode *copyLeaf(TreeNode *leaf) {
	switch(leaf->type) {
		case TYPE_S:
		case TYPE_K:
		case TYPE_I: {
			TreeNode *tn = lmalloc(sizeof(TreeNode));
			tn->type = leaf->type;
			return tn;
		}
		case TYPE_PRINT:
		case TYPE_VAR: {
			SymbolLeaf *tn = lmalloc(sizeof(SymbolLeaf));
			tn->super.type = leaf->type;
			tn->symbol = ((SymbolLeaf *)leaf)->symbol;
			return (TreeNode *)tn;
		}
		case TYPE_KP:
		case TYPE_SP: {
			Kp *tn = lmalloc(sizeof(Kp));
			tn->super.type = leaf->type;
			tn->partial = copyLeaf(((Kp *)leaf)->partial);
			return (TreeNode *)tn;
		}
		case TYPE_SPP: {
			Spp *tn = lmalloc(sizeof(Spp));
			tn->super.type = leaf->type;
			tn->partial1 = copyLeaf(((Spp *)leaf)->partial1);
			tn->partial2 = copyLeaf(((Spp *)leaf)->partial2);
			return (TreeNode *)tn;
		}
		case TYPE_APP: {
			Application *tn = lmalloc(sizeof(Application));
			tn->super.type = leaf->type;
			tn->f = copyLeaf(((Application *)leaf)->f);
			tn->x = copyLeaf(((Application *)leaf)->x);
			return (TreeNode *)tn;
		}
		default: {
			TreeNode *tn = malloc(sizeof(TreeNode));
			tn->type = '?';
			return tn;
		}
	}
}

QN *buildQ(TreeNode *tn, TreeNode *parent, QN *q) {
	if (tn->type != TYPE_BRANCH) return q;
	QN *qn = lmalloc(sizeof(QN));
	qn->proc = (Branch *)tn;
	qn->parent = (Branch *)parent;
	qn->next = q;
	Branch *b = (Branch *)tn;
	qn = buildQ(b->right, tn, qn);
	qn = buildQ(b->left, tn, qn);
	return qn;
}