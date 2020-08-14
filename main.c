#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Tree* tree_pointer;

enum Color {
    RED, BLACK
};

struct Tree{

    long key;
    char string[1024];
    enum Color color;

    tree_pointer root, prev, left, right;

};

struct Node {
    char command[1024];
    __unused struct Node* next;
    struct Node* prev;
};

struct Node* head;
struct Node* tail;



// CLASSIC TREE
void printTree(tree_pointer T);

// RED BLACK TREE
void leftRotate(tree_pointer T, tree_pointer x);
void rightRotate(tree_pointer T, tree_pointer z);

void rbInsert(tree_pointer T, tree_pointer z);
void rbInsertFixup(tree_pointer T, tree_pointer z);

tree_pointer rbDelete(tree_pointer T, tree_pointer z);
void rbDeleteFixup(tree_pointer T, tree_pointer x);

tree_pointer treeMinimum(tree_pointer x);

__unused tree_pointer treeMaximum(tree_pointer x);
tree_pointer successorNode(tree_pointer x);

tree_pointer treeSearch(tree_pointer x, long k);

// HELPER
void getBounds(char row[], long *ind1, long *ind2);

// DOUBLE LINKED LIST
struct Node* createNode(char x[]);
void insertTail(char x[]);
void comeBackTail();
void goNextTail();






int main() {

    long undo = 0;
    tree_pointer temp_root = NULL;

    tree_pointer tree = malloc(sizeof(typeof(struct Tree)));
    tree->left = NULL;
    tree->right = NULL;
    tree->prev = NULL;
    tree->root = NULL;

    /*for (int i = 10; i > 0; i--){
        tree_pointer x = malloc(sizeof(typeof(struct Tree)));
        x->key = i;
        x->left = NULL;
        x->right = NULL;
        x->prev = NULL;
        x->root = NULL;

        rbInsert(tree, x);
    }   // add some nodes for debug

    printf("Inoder Traversal of Created Tree\n");
    printTree(tree->root);*/


    while (1){
        char row[1024];
        gets(row);
        //scanf("%s", row);


        if (strstr(row, "c") != NULL) {

            insertTail(row);
            undo = 0;

            long ind1 = 0, ind2 = 0;

            unsigned long length = strlen(row);
            char copy[length-1];

            strncpy(copy, row, length - 1);

            getBounds(copy, &ind1, &ind2);

            long numRow = ind2-ind1+1;

            // MODO 1
            for (int i = 0; i < numRow; i++) {

                //scanf("%s", row);
                gets(row);

                long key = ind1+i;

                tree_pointer node = treeSearch(tree->root, key);

                if(node != NULL){

                    strcpy(node->string, row);

                } else{

                    tree_pointer x = malloc(sizeof(typeof(struct Tree)));
                    x->key = key;
                    strcpy(x->string, row);
                    x->left = NULL;
                    x->right = NULL;
                    x->prev = NULL;
                    x->root = NULL;

                    rbInsert(tree, x);
                }


            }

            // MODO 2
            /*//scanf("%s", row);
            gets(row);
            tree_pointer node = treeSearch(tree->root, ind1);
            node->string = row;

            for (int i = 0; i < numRow; i++) {

                node = successorNode(node);
                //scanf("%s", row);
                gets(row);
                node->string = row;

            }*/

            gets(row);
            if(strcmp(row, ".") == 0){
                printf("command executed\n");
            }

        }
        else if (strstr(row, "d") != NULL) {

            insertTail(row);
            undo = 0;

            long ind1, ind2;
            getBounds(row, &ind1, &ind2);

            long numRow = ind2-ind1+1;

            for (int i = 0; i < numRow; i++) {

                long key = ind1+i;
                tree_pointer node = treeSearch(tree->root, key);

                if(node != NULL){
                    rbDelete(tree->root, node);
                }
            }

        }
        else if (strstr(row, "p") != NULL) {

            long ind1, ind2;
            getBounds(row, &ind1, &ind2);

            long numRow = ind2-ind1+1;

            // MODO 1
            for (int i = 0; i < numRow; i++) {

                long key = ind1+i;
                tree_pointer node = treeSearch(tree->root, key);

                if(node != NULL){
                    printf("%s\n", node->string);
                }
            }


            // MODO 2
            /*tree_pointer node = treeSearch(tree, ind1);
            printf("%s", node->string);

            for (int i = 1; i < numRow; i++) {

                node = successorNode(node);
                printf("%s", node->string);
            }*/

        }
        else if (strstr(row, "u") != NULL) {

            if(undo == 0) temp_root = tree->root;

            long ret = strtol(row, (char **) row, 10);

            undo += ret;

            for (int i = 0; i < ret; ++i) {
                comeBackTail();
            }

        }
        else if (strstr(row, "r") != NULL) {

            if(undo > 0){

                long ret = strtol(row, (char **) row, 10);


                if(undo < ret){
                    tree->root = temp_root;
                    temp_root = NULL;
                }

                for (int i = 0; i < ret; ++i) {
                    goNextTail();
                }

            }

        }
        else if (strstr(row, "q") != NULL) {

            return 0;

        }


        //printf("Inoder Traversal of Created Tree\n");
        //printTree(tree->root);
    }


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
        printf("%ld ", T->key);
        printf("%s\n", T->string);
        printTree(T->right);
    }
}

void leftRotate(tree_pointer T, tree_pointer x){
    tree_pointer y = x->right;
    x->right = y->left;

    if(y->left != NULL){
        y->left->prev = x;
    }

    y->prev = x->prev;

    if(x->prev == NULL){
        T->root = y;
    } else if(x == x->prev->left){
        x->prev->left = y;
    } else{
        x->prev->right = y;
    }

    y->left = x;
    x->prev = y;
}
void rightRotate(tree_pointer T, tree_pointer z){

    tree_pointer x = z;
    tree_pointer y = x->left;
    x->left = y->right;
    if (y->right != NULL){
        y->right->prev = x;
    }
    y->prev = x->prev;
    if (x->prev == NULL){
        T->root = y;
    }
    else if (x == x->prev->left){
        x->prev->left = y;
    }
    else {
        x->prev->right = y;
    }
    y->right = x;
    x->prev = y;

    /*
    tree_pointer y = x->left;
    x->left = y->right;

    if(y->right != NULL){
        y->right->prev = x;
    }

    y->prev = x->prev;

    if(x->prev == NULL){
        T->root = y;
    } else if(x == x->prev->left){
        x->prev->left = y;
    } else{
        x->prev->right = y;
    }

    y->right = x;
    x->prev = y;*/
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

    z->prev = y;

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

    tree_pointer x, y;

    if (z == T->root){
        T->root->color = BLACK;
    }
    else {
        x = z->prev;
        if (x->color == RED){
            if (x == x->prev->left){
                y = x->prev->right;

                if(y != NULL) {
                    if (y->color == RED) {
                        x->color = BLACK;
                        y->color = BLACK;
                        x->prev->color = RED;
                        rbInsertFixup(T, x->prev);
                    }
                }

                else {
                    if (z == x->right){
                        z = x;
                        leftRotate(T, z);
                        x = z->prev;
                    }
                    x->color = BLACK;
                    x->prev->color = RED;
                    rightRotate(T, x->prev);
                }

            }
            else {
                y = x->prev->left;

                if(y != NULL) {
                    if (y->color == RED) {
                        x->color = BLACK;
                        y->color = BLACK;
                        x->prev->color = RED;
                        rbInsertFixup(T, x->prev);
                    }
                }

                else {
                    if (z == x->left){
                        z = x;
                        rightRotate(T, z);
                        x = z->prev;
                    }
                    x->color = BLACK;
                    x->prev->color = RED;
                    leftRotate(T, x->prev);
                }

            }
        }
    }

    /*
    if(z == T->root){
        T->root->color = BLACK;
    } else{
        tree_pointer x = z->prev;                 // x è il padre di z

        if(x->color == RED){
            if(x == x->prev->left){               // se x è figlio sx
                tree_pointer y = x->prev->right;  // y è il fratello di x

                if(y != NULL){
                    if (y->color == RED) {
                        x->color = BLACK;
                        y->color = BLACK;
                        x->prev->color = RED;
                        rbInsertFixup(T, x->prev);
                    }
                } else {

                    if (z == x->right) {
                        z = x;
                        leftRotate(T, z);
                        x = z->prev;
                    }

                    x->color = BLACK;
                    x->prev->color = RED;
                    rightRotate(T, x->prev);
                }
            } else {                                 // se x è figlio dx
                tree_pointer y = x->prev->left;   // y è il fratello di x

                if(y != NULL){
                    if (y->color == RED) {
                        x->color = BLACK;
                        y->color = BLACK;
                        x->prev->color = RED;
                        rbInsertFixup(T, x->prev);
                    }
                } else {
                    if (z == x->left) {
                        z = x;
                        rightRotate(T, z);
                        x = z->prev;
                    }

                    x->color = BLACK;
                    x->prev->color = RED;
                    leftRotate(T, x->prev);
                }
            }
        }
    }*/
}


tree_pointer rbDelete(tree_pointer T, tree_pointer z){

    tree_pointer x, y;

    if(z->left == NULL || z->right == NULL){
        y = z;
    } else{
        y = successorNode(z);
    }

    if(y->left != NULL){
        x = y->left;
    } else{
        x = y->right;
    }

    x->prev = y->prev;

    if(y->prev == NULL){
        T->root = x;
    } else if(y == y->prev->left){
        y->prev->left = x;
    } else{
        y->prev->root = x;
    }

    if(y != z){
        z->key = y->key;
    }

    if(y->color == BLACK){
        rbDeleteFixup(T, x);
    }

    return y;

}
void rbDeleteFixup(tree_pointer T, tree_pointer x){

    if(x->color == RED || x->prev == NULL){
        x->color = BLACK;
    } else if(x == x->prev->left){

        tree_pointer w = x->prev->right;

        if(w->color == RED){
            w->color = BLACK;
            x->prev->color = RED;
            leftRotate(T, x->prev);
            w = x->prev->right;
        }
        if(w->left->color == BLACK && w->right->color == BLACK){
            w->color = RED;
            rbDeleteFixup(T, x->prev);
        } else {
            if(w->right->color == BLACK) {
                w->left->color = BLACK;
                w->color = RED;
                rightRotate(T, w);
                w = x->prev->right;
            }

            w->color = x->prev->color;
            x->prev->color = BLACK;
            w->right->color = BLACK;
            leftRotate(T, x->prev);
        }
    } else{

        tree_pointer w = x->prev->left;

        if(w->color == RED){
            w->color = BLACK;
            x->prev->color = RED;
            rightRotate(T, x->prev);
            w = x->prev->left;
        }
        if(w->right->color == BLACK && w->left->color == BLACK){
            w->color = RED;
            rbDeleteFixup(T, x->prev);
        } else {
            if(w->left->color == BLACK) {
                w->right->color = BLACK;
                w->color = RED;
                leftRotate(T, w);
                w = x->prev->left;
            }

            w->color = x->prev->color;
            x->prev->color = BLACK;
            w->left->color = BLACK;
            rightRotate(T, x->prev);
        }

    }

}

tree_pointer treeMinimum(tree_pointer x){
    while (x->left != NULL){
        x = x->left;
    }
    return x;
}

__unused tree_pointer treeMaximum(tree_pointer x){
    while (x->right != NULL){
        x = x->right;
    }
    return x;
}
tree_pointer successorNode(tree_pointer x){
    if(x->right == NULL){
        return treeMinimum(x->right);
    }

    tree_pointer y = x->prev;

    while (y != NULL && x == y->right){
        x = y;
        y = y->prev;
    }

    return y;
}

tree_pointer treeSearch(tree_pointer x, long k){
    if(x == NULL || k == x->key){
        return x;
    }

    if(k < x->key){
        return treeSearch(x->left, k);
    } else return treeSearch(x->right, k);
}

void getBounds(char row[], long *ind1, long *ind2){

    /*char splitChar = ',';

    char *token;
    token = strtok(row, splitChar);

    while (token != NULL) {

         if(token == ','){
            ind1 = atoi(token);
            splitChar = ' ';
        } else {
            ind2 = atoi(token);
        }

        token = strtok(NULL, splitChar);
    }*/

    long ret;

    for (int i = 0; i < 2; ++i) {


        ret = strtol(row, &row, 10);
        char *ps = row;
        ps++;
        row = ps;

        printf("The number(unsigned long integer) is %ld\n", ret);

        if(i == 0) *ind1 = ret;
        else *ind2 = ret;

    }




}



// DOUBLE LINKED LIST
struct Node* createNode(char x[]) {
    struct Node* newNode= (struct Node*)malloc(sizeof(struct Node));
    strcpy(newNode->command, x);
    newNode->prev = NULL;
    newNode->next = NULL;
    return newNode;
}
void insertTail(char x[]) {

    struct Node* newNode = createNode(x);

    if(head == NULL){
        head = newNode;
        tail = newNode;
        return;
    }

    tail->next = newNode;
    newNode->prev = tail;
    tail = newNode;
}

void comeBackTail(){
    tail = tail->prev;
}
void goNextTail(){
    tail = tail->next;
}