#ifndef __SKILIB_H__
#define __SKILIB_H__

#define TYPE_BRANCH 0
#define TYPE_I 'I'
#define TYPE_K 'K'
#define TYPE_S 'S'
#define TYPE_PRINT '.'
#define TYPE_VAR 'v'
#define TYPE_KP 'L'
#define TYPE_SP 'T'
#define TYPE_SPP 'U'
#define TYPE_APP 'A'

typedef struct TreeNode {
	char type;
} TreeNode;

typedef TreeNode Leaf;

typedef struct SymbolLeaf {
	Leaf super;
	char symbol;
} SymbolLeaf;

typedef struct Application {
	Leaf super;
	Leaf *f;
	Leaf *x;
} Application;

typedef struct Kp {
	Leaf super;
	Leaf *partial;
} Kp;

typedef Kp Sp;

typedef struct Spp {
	Leaf super;
	Leaf *partial1;
	Leaf *partial2;
} Spp;

typedef struct Branch {
	TreeNode super;
	TreeNode *left;
	TreeNode *right;
} Branch;

TreeNode *makeTree(char *input);
void freeTree(TreeNode *root);
void printAST(TreeNode *tn);

typedef struct QueueNode {
	Branch *proc;
	Branch *parent;
	struct QueueNode *next;
} QN;

typedef struct CrunchStep {
	TreeNode *root;
	QN *processQ;
} CrunchStep;

typedef struct ExecOpts {
	int step;
	int maxIterations;
} ExecOpts;

CrunchStep simplify(TreeNode *root, QN *processQ);
void execute(char *root, ExecOpts options);

#endif