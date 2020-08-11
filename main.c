#include <stdio.h>
#include <stdlib.h>

typedef struct Tree* tree_pointer;

enum Color {
    RED, BLACK
};

struct Tree{

    int key;
    char string;
    enum Color color;

    tree_pointer root, parent, left, right;

};


tree_pointer root = NULL;


// CLASSIC TREE
tree_pointer insertTree(tree_pointer T, struct Tree t);
void printTree(tree_pointer T);



// RED BLACK TREE
void leftRotate(tree_pointer T, tree_pointer x);
void rightRotate(tree_pointer T, tree_pointer x);
void rbInsert(tree_pointer T, tree_pointer x);
void rbInsertFixup(tree_pointer T, tree_pointer x);

int main() {
    printf("Hello, World!\n");
    //return 0;

    /*tree_pointer root = malloc(sizeof(typeof(struct Tree)));
    root->root = NULL;
    root->key = 0;
    root->string = 0;*/

    tree_pointer tree = malloc(sizeof(typeof(struct Tree)));

    for (int i = 10; i > 0; i--){
        tree_pointer x = malloc(sizeof(typeof(struct Tree)));
        x->key = i;
        x->string = i;

        rbInsert(tree, x);
    }

    printf("Inoder Traversal of Created Tree\n");
    printTree(tree->root);

    // while (righe del documento)

    // prendo il comando:
    /*
     * case c (change):
     * FORMATO: (ind1,ind2)c
     *
     * for (int i = 0; i < (ind2 - ind1); i++){
     *      prendo la riga successiva e l'aggiungo alla lista all'indirizzo ind1 + i
     * }
     * _____________________
     *
     * case d (delete):
     * FORMATO: (ind1,ind2)d
     *
     * _____________________
     * case p (print):
     * FORMATO: (ind1,ind2)p
     *
     * _____________________
     * case u (undo):
     * FORMATO: (numero)u
     *
     * _____________________
     * case r (redo):
     * FORMATO: (numero)r
     *
     * _____________________
     */
}


void printTree(tree_pointer T){

    if(T != NULL){
        printTree(T->left);
        printf("%d\n", T->key);
        printTree(T->right);
    }
}

tree_pointer insertTree(tree_pointer T, struct Tree t){

    if(T != NULL){

        if(t.key > T->key){
            T->right = insertTree(T->right, t);
        }
        else if(t.key < T->key){
            T->left = insertTree(T->left, t);
        }
        else if(t.key == T->key){
            // change
        }

    }
    else {

        /*T = (tree_pointer)malloc(sizeof(struct tree));
        T->key = t.key;
        T->string = t.string;
        T->left = NULL;
        T->right = NULL;*/

        T = &t;

    }

    return T;
}



void leftRotate(tree_pointer T, tree_pointer x){
    tree_pointer y = x->right;
    x->right = y->left;

    if(y->left != NULL){
        y->left->parent = x;
    }

    y->parent = x->parent;

    if(x->parent == NULL){
        T->root = y;
    } else if(x == x->parent->left){
        x->parent->left = y;
    } else{
        x->parent->right = y;
    }

    y->left = x;
    x->parent = y;
}
void rightRotate(tree_pointer T, tree_pointer x){
    tree_pointer y = x->left;
    x->left = y->right;

    if(y->right != NULL){
        y->right->parent = x;
    }

    y->parent = x->parent;

    if(x->parent == NULL){
        T->root = y;
    } else if(x == x->parent->right){
        x->parent->right = y;
    } else{
        x->parent->left = y;
    }

    y->right = x;
    x->parent = y;
}
void rbInsert(tree_pointer T, tree_pointer z){

    tree_pointer y = NULL;
    tree_pointer x = T->root;

    while (x != NULL){
        y = x;
        if(z->key < x->key){
            x = x->left;
        } else{
            x = x->right;
        }
    }

    z->parent = y;

    if(y == NULL){
        T->root = z;
    } else if(z->key < y->key){
        y->left = z;
    } else{
        y->right = z;
    }

    z->left = NULL;
    z->right = NULL;
    z->color = RED;

    rbInsertFixup(T, z);
}
void rbInsertFixup(tree_pointer T, tree_pointer z){
    if(z == T->root){
        T->root->color = BLACK;
    } else{
        tree_pointer x = z->parent;                 // x è il padre di z

        if(x->color == RED){
            if(x == x->parent->left){               // se x è figlio sx
                tree_pointer y = x->parent->right;  // y è il fratello di x

                if(y != NULL) {

                    if (y->color == RED) {
                        x->color = BLACK;
                        y->color = BLACK;
                        x->parent->color = RED;
                        rbInsertFixup(T, x->parent);
                    } else if (z == x->right) {
                        z = x;
                        leftRotate(T, z);
                        x = z->parent;
                    }
                }

                x->color = BLACK;
                x->parent->color = RED;
                rightRotate(T, x->parent);
            } else{                                 // se x è figlio dx
                tree_pointer y = x->parent->left;   // y è il fratello di x

                if(y->color == RED){
                    x->color = BLACK;
                    y->color = BLACK;
                    x->parent->color = RED;
                    rbInsertFixup(T, x->parent);

                } else if(z == x->left){
                    z = x;
                    rightRotate(T, z);
                    x = z->parent;
                }

                x->color = BLACK;
                x->parent->color = RED;
                leftRotate(T, x->parent);
            }
        }
    }
}


