#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct TreeNode* tree_pointer;

enum Color {
    RED, BLACK
};

struct HistoryNode {
    char value[1024];
    struct HistoryNode* next;
    struct HistoryNode* prev;
    struct HistoryNode* tail;

}; // used for commands history

struct TreeNode{

    long key;
    struct HistoryNode* text;
    enum Color color;

    tree_pointer prev, left, right;

};  // single node of the tree

struct HistoryNode* head;
struct HistoryNode* tail;

tree_pointer nil;





// RED BLACK TREE
void leftRotate(tree_pointer* root, tree_pointer z);
void rightRotate(tree_pointer* root, tree_pointer z);

void rbInsert(tree_pointer* root, tree_pointer z);
void rbInsertFixup(tree_pointer* root, tree_pointer w);

tree_pointer rbDelete(tree_pointer* T, tree_pointer z);
void rbDeleteFixup(tree_pointer* T, tree_pointer x);

tree_pointer treeSuccessor(tree_pointer x);
tree_pointer treeMinimum(tree_pointer x);

tree_pointer treeSearch(tree_pointer x, long k);

// HELPER
void getBounds(char row[], long *ind1, long *ind2);
struct TreeNode* createTreeNode(long key, struct HistoryNode* text);

// DOUBLE LINKED LIST
struct HistoryNode* createNode(char x[]);
void addInHistory(char *x);

/*
void change(int* x){
    *x = 5;
}

void change2(tree_pointer* y){
    printf("%ld\n", (*y)->key);
    (*y)->key = 10;
}*/


int main() {

    /*int x = 0;
    change(&x);
    printf("%d\n", x);

    tree_pointer y = malloc(sizeof(struct TreeNode));
    y->key = 5;
    change2(&y);
    printf("%ld\n", y->key);*/




    tree_pointer root;              // main tree to show
    tree_pointer root_removed;      // tree with removed nodes

    nil = malloc(sizeof(typeof(struct TreeNode)));
    root = nil;
    root_removed = nil;


    char row[1024];
    long undo = 0;                  // used to know how many undo I've made

    gets(row);

    while (strstr(row, "q") == NULL){


        if (strstr(row, "c") != NULL) {

            undo = 0;
            addInHistory(row);

            long ind1 = 0, ind2 = 0;

            unsigned long length = strlen(row);
            char copy[length-1];

            strncpy(copy, row, length - 1);

            getBounds(copy, &ind1, &ind2);

            long numRow = ind2-ind1+1;

            // MODO 1
            for (int i = 0; i < numRow; i++) {

                gets(row);

                long key = ind1+i;

                tree_pointer node = treeSearch(root, key);

                if(node != nil){

                    struct HistoryNode* n = createNode(row);

                    node->text->tail->next = n;
                    n->prev = node->text->tail;
                    node->text->tail = n;

                } else{

                    tree_pointer x = malloc(sizeof(typeof(struct TreeNode)));

                    x->key = key;

                    struct HistoryNode* n = createNode(row);
                    x->text = n;
                    x->text->tail = n;

                    x->left = nil;
                    x->right = nil;
                    x->prev = nil;

                    rbInsert(&root, x);
                }


            }

            // MODO 2
            /*gets(row);
            tree_pointer node = treeSearch(root, ind1);

            for (int i = 1; i < numRow; i++) {

                if(node != nil){

                    struct HistoryNode* n = createNode(row);

                    node->text->tail->next = n;
                    n->prev = node->text->tail;
                    node->text->tail = n;

                } else{

                    tree_pointer x = malloc(sizeof(typeof(struct TreeNode)));

                    x->key = ind1+i-1;

                    struct HistoryNode* n = createNode(row);
                    x->text = n;
                    x->text->tail = n;

                    x->left = nil;
                    x->right = nil;
                    x->prev = nil;

                    rbInsert(&root, x);
                }

                node = treeSuccessor(node);
                gets(row);

            }*/

            gets(row);

            if(strcmp(row, ".") != 0) {
                printf("Something went wrong\n");
            }

        }
        else if (strstr(row, "d") != NULL) {

            undo = 0;
            addInHistory(row);

            long ind1, ind2;
            getBounds(row, &ind1, &ind2);

            long numRow = ind2-ind1+1;

            for (int i = 0; i < numRow; i++) {

                long key = ind1+i;
                tree_pointer node = treeSearch(root, key);

                if(node != nil){

                    if(node->text == NULL){
                        rbDelete(&root, node);
                    } else{

                        /*tree_pointer add = malloc(sizeof(typeof(struct TreeNode)));
                        add->key = node->key;
                        add->text = node->text;
                        add->prev = nil;
                        add->left = nil;
                        add->right = nil;*/

                        tree_pointer add = createTreeNode(node->key, node->text);

                        rbDelete(&root, node);
                        rbInsert(&root_removed, add);
                    }


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
                tree_pointer node = treeSearch(root, key);

                if(node != nil){
                    printf("%s\n", node->text->tail->value);
                } else printf(".\n");
            }


            // MODO 2
            /*tree_pointer node = treeSearch(root, ind1);

            for (int i = 1; i < numRow; i++) {

                if(node != nil){
                    printf("%s\n", node->text->tail->value);
                } else printf(".\n");

                node = treeSuccessor(node);
            }*/

        }
        else if (strstr(row, "u") != NULL) {

            char* q;
            long ret = strtol(row, &q, 10);

            undo += ret;

            for (int i = 0; i < ret; ++i) {
                long ind1, ind2;

                getBounds(tail->value, &ind1, &ind2);

                long numRow = ind2-ind1+1;

                for (int j = 0; j < numRow; ++j) {

                    long key = ind1+j;
                    tree_pointer node = treeSearch(root, key);

                    if(node != nil){
                        node->text->tail = node->text->tail->prev;
                    } else{
                        node = treeSearch(root_removed, key);

                        if(node != nil){

                            /*tree_pointer add = malloc(sizeof(typeof(struct TreeNode)));
                            add->key = node->key;
                            add->text = node->text;
                            add->prev = nil;
                            add->left = nil;
                            add->right = nil;*/

                            tree_pointer add = createTreeNode(node->key, node->text);

                            rbDelete(&root_removed, node);
                            rbInsert(&root, add);
                        }

                    }
                }

                tail = tail->prev;
            }

        }
        else if (strstr(row, "r") != NULL) {

            if(undo > 0){

                char* q;
                long ret = strtol(row, &q, 10);

                for (int i = 0; i < ret; ++i) {
                    long ind1, ind2;

                    getBounds(tail->next->value, &ind1, &ind2);

                    long numRow = ind2-ind1+1;

                    for (int j = 0; j < numRow; ++j) {

                        long key = ind1+j;
                        tree_pointer node = treeSearch(root, key);

                        if(tail->next->value[3] == 'd'){

                            /*tree_pointer add = malloc(sizeof(typeof(struct TreeNode)));
                            add->key = node->key;
                            add->text = node->text;
                            add->prev = nil;
                            add->left = nil;
                            add->right = nil;*/

                            tree_pointer add = createTreeNode(node->key, node->text);

                            rbDelete(&root, node);
                            rbInsert(&root_removed, add);
                        }
                        else if(node->text->tail->next != NULL){
                            node->text->tail = node->text->tail->next;
                        }
                    }

                    tail = tail->next;
                }

                undo -= ret;

            }



        }


        gets(row);

    }

    return 0;
}





// RED BLACK TREE
void leftRotate(tree_pointer* root, tree_pointer z){

    tree_pointer x = z;
    tree_pointer y = x->right;
    x->right = y->left;
    if (y->left != nil){
        y->left->prev = x;
    }
    y->prev = x->prev;
    if (x->prev == nil){
        *root = y;
    }
    else if (x == x->prev->left){
        x->prev->left = y;
    }
    else {
        x->prev->right = y;
    }
    y->left = x;
    x->prev = y;

}
void rightRotate(tree_pointer* root, tree_pointer z){

    tree_pointer x = z;
    tree_pointer y = x->left;
    x->left = y->right;
    if (y->right != nil){
        y->right->prev = x;
    }
    y->prev = x->prev;
    if (x->prev == nil){
        *root = y;
    }
    else if (x == x->prev->left){
        x->prev->left = y;
    }
    else {
        x->prev->right = y;
    }
    y->right = x;
    x->prev = y;
}

void rbInsert(tree_pointer* root, tree_pointer z){

    tree_pointer y = nil;
    tree_pointer x = *root;

    while (x != nil){
        y = x;
        if(z->key < x->key){
            x = x->left;
        } else{
            x = x->right;
        }
    }

    z->prev = y;

    if(y == nil){
        *root = z;
    } else if(z->key < y->key){
        y->left = z;
    } else{
        y->right = z;
    }

    z->left = nil;
    z->right = nil;
    z->color = RED;

    rbInsertFixup(root, z);
}
void rbInsertFixup(tree_pointer* root, tree_pointer w){

    tree_pointer z = w;
    tree_pointer y, x;
    if (z==*root){
        (*root)->color = BLACK;
    }
    else {
        x = z->prev;
        if (x->color==RED){
            if (x==x->prev->left){
                y = x->prev->right;
                if (y->color==RED){
                    x->color = BLACK;
                    y->color = BLACK;
                    x->prev->color = RED;
                    rbInsertFixup(root, x->prev);
                }
                else {
                    if (z==x->right){
                        z = x;
                        leftRotate(root, z);
                        x = z->prev;
                    }
                    x->color = BLACK;
                    x->prev->color = RED;
                    rightRotate(root, x->prev);
                }
            }
            else {
                y = x->prev->left;
                if (y->color==RED){
                    x->color = BLACK;
                    y->color = BLACK;
                    x->prev->color = RED;
                    rbInsertFixup(root, x->prev);
                }
                else {
                    if (z==x->left){
                        z = x;
                        rightRotate(root, z);
                        x = z->prev;
                    }
                    x->color = BLACK;
                    x->prev->color = RED;
                    leftRotate(root, x->prev);
                }
            }
        }
    }
}

tree_pointer rbDelete(tree_pointer* root, tree_pointer z){

    tree_pointer x, y;

    if(z->left == nil || z->right == nil){
        y = z;
    } else{
        y = treeSuccessor(z);
    }

    if(y->left != nil){
        x = y->left;
    } else{
        x = y->right;
    }

    x->prev = y->prev;

    if(y->prev == nil){
        *root = x;
    } else if(y == y->prev->left){
        y->prev->left = x;
    } else{
        y->prev->right = x;
    }

    if(y != z){
        z->key = y->key;
        z->text = y->text;
    }

    if(y->color == BLACK){
        rbDeleteFixup(root, x);
    }

    return y;

}
void rbDeleteFixup(tree_pointer* root, tree_pointer x){

    if(x->color == RED || x->prev == nil){
        x->color = BLACK;
    } else if(x == x->prev->left){

        tree_pointer w = x->prev->right;

        if(w->color == RED){
            w->color = BLACK;
            x->prev->color = RED;
            leftRotate(root, x->prev);
            w = x->prev->right;
        }
        if(w->left->color == BLACK && w->right->color == BLACK){
            w->color = RED;
            rbDeleteFixup(root, x->prev);
        } else {
            if(w->right->color == BLACK) {
                w->left->color = BLACK;
                w->color = RED;
                rightRotate(root, w);
                w = x->prev->right;
            }

            w->color = x->prev->color;
            x->prev->color = BLACK;
            w->right->color = BLACK;
            leftRotate(root, x->prev);
        }
    } else{

        tree_pointer w = x->prev->left;

        if(w->color == RED){
            w->color = BLACK;
            x->prev->color = RED;
            rightRotate(root, x->prev);
            w = x->prev->left;
        }
        if(w->right->color == BLACK && w->left->color == BLACK){
            w->color = RED;
            rbDeleteFixup(root, x->prev);
        } else {
            if(w->left->color == BLACK) {
                w->right->color = BLACK;
                w->color = RED;
                leftRotate(root, w);
                w = x->prev->left;
            }

            w->color = x->prev->color;
            x->prev->color = BLACK;
            w->left->color = BLACK;
            rightRotate(root, x->prev);
        }

    }

}

tree_pointer treeSuccessor(tree_pointer x){
    if(x->right != nil){
        return treeMinimum(x->right);
    }

    tree_pointer y = x->prev;

    while (y != nil && x == y->right){
        x = y;
        y = y->prev;
    }

    return y;
}
tree_pointer treeMinimum(tree_pointer x){
    while (x->left != nil){
        x = x->left;
    }
    return x;
}

tree_pointer treeSearch(tree_pointer x, long k){
    if(x == nil || k == x->key){
        return x;
    }

    if(k < x->key){
        return treeSearch(x->left, k);
    } else return treeSearch(x->right, k);
}

// HELPER
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

        //printf("The number(unsigned long integer) is %ld\n", ret);

        if(i == 0) *ind1 = ret;
        else *ind2 = ret;

    }




}
struct TreeNode* createTreeNode(long key, struct HistoryNode* text){
    tree_pointer add = malloc(sizeof(typeof(struct TreeNode)));
    add->key = key;
    add->text = text;
    add->prev = nil;
    add->left = nil;
    add->right = nil;

    return add;
}

// DOUBLE LINKED LIST
struct HistoryNode* createNode(char x[]) {
    struct HistoryNode* newNode= (struct HistoryNode*)malloc(sizeof(struct HistoryNode));
    strcpy(newNode->value, x);
    newNode->prev = NULL;
    newNode->next = NULL;
    return newNode;
}
void addInHistory(char *x) {

    struct HistoryNode* newNode = createNode(x);

    if(head == NULL){
        head = newNode;
        tail = newNode;
        return;
    }

    tail->next = newNode;
    newNode->prev = tail;
    tail = newNode;
}