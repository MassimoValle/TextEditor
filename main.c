#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROW_LEN 1024

typedef struct TreeNode* tree_pointer;
typedef struct HistoryNode* history_pointer;

enum Color {
    RED, BLACK
};

struct HistoryNode {
    char* value;
    history_pointer next;
    history_pointer prev;
    history_pointer tail;

};  // used for commands history

struct TreeNode{

    long key;
    history_pointer text;
    enum Color color;

    tree_pointer prev, left, right;

};      // single node of the tree


tree_pointer nil;
history_pointer head;
history_pointer tail;

char* row;





// RED BLACK TREE
void leftRotate(tree_pointer* root, tree_pointer* z);
void rightRotate(tree_pointer* root, tree_pointer* z);

void rbInsert(tree_pointer* root, tree_pointer* z);
void rbInsertFixup(tree_pointer* root, tree_pointer* w);

void rbDelete(tree_pointer* T, tree_pointer* z);
void rbDeleteFixup(tree_pointer* T, tree_pointer* x);

tree_pointer treeSuccessor(tree_pointer* x);
tree_pointer treeMinimum(tree_pointer* x);

tree_pointer treeSearch(tree_pointer* x, long k);

void cleanUpTree(tree_pointer* x);

// HELPER
char* getRow();
void getBounds(char row[], long *ind1, long *ind2);
tree_pointer createTreeNode(long key, history_pointer text);

// DOUBLE LINKED LIST
history_pointer createHistoryNode(char *x);
void addInHistory(char row[]);
void freeUnusedHistoryNode();
void cleanUpHistoryFromHead(history_pointer head);




int main() {

    tree_pointer root;              // main tree to show
    tree_pointer root_removed;      // tree with removed nodes

    nil = malloc(sizeof(typeof(struct TreeNode)));
    root = nil;
    root_removed = nil;


    row;
    //char* row = calloc(sizeof(char), ROW_LEN);
    long undo = 0;                  // used to know how many undo I've made

    row = getRow();

    while (strstr(row, "q") == NULL){


        if (strstr(row, "c") != NULL) {

            undo = 0;
            freeUnusedHistoryNode();
            addInHistory(row);

            long ind1 = 0, ind2 = 0;

            getBounds(row, &ind1, &ind2);

            long numRow = ind2-ind1+1;

            // MODO 1
            for (int i = 0; i < numRow; i++) {

                getRow();

                long key = ind1+i;

                tree_pointer node = treeSearch(&root, key);

                if(node != nil){

                    history_pointer n = createHistoryNode(row);

                    node->text->tail->next = n;
                    n->prev = node->text->tail;
                    node->text->tail = n;

                } else{

                    tree_pointer x = malloc(sizeof(typeof(struct TreeNode)));

                    x->key = key;

                    history_pointer n = createHistoryNode(row);
                    x->text = n;
                    x->text->tail = n;

                    x->left = nil;
                    x->right = nil;
                    x->prev = nil;

                    rbInsert(&root, &x);
                }


            }

            // MODO 2
            /*getRow();
            tree_pointer node = treeSearch(root, ind1);

            for (int i = 1; i < numRow; i++) {

                if(node != nil){

                    struct history_pointer n = createNode(row);

                    node->text->tail->next = n;
                    n->prev = node->text->tail;
                    node->text->tail = n;

                } else{

                    tree_pointer x = malloc(sizeof(typeof(struct TreeNode)));

                    x->key = ind1+i-1;

                    struct history_pointer n = createHistoryNode(row);
                    x->text = n;
                    x->text->tail = n;

                    x->left = nil;
                    x->right = nil;
                    x->prev = nil;

                    rbInsert(&root, x);
                }

                node = treeSuccessor(node);
                getRow();

            }*/

            getRow();

            if(strstr(row, ".") != NULL){
                free(row);
            } else printf("something went wrong");

        }
        else if (strstr(row, "d") != NULL) {

            undo = 0;
            freeUnusedHistoryNode();
            addInHistory(row);

            long ind1, ind2;
            getBounds(row, &ind1, &ind2);

            long numRow = ind2-ind1+1;

            for (int i = 0; i < numRow; i++) {

                long key = ind1+i;
                tree_pointer node = treeSearch(&root, key);

                if(node != nil){

                    if(node->text == NULL){
                        rbDelete(&root, &node);
                    } else{

                        /*tree_pointer add = malloc(sizeof(typeof(struct TreeNode)));
                        add->key = node->key;
                        add->text = node->text;
                        add->prev = nil;
                        add->left = nil;
                        add->right = nil;*/

                        tree_pointer node2 = treeSearch(&root, key+numRow);

                        if(node2 != nil){

                            node->text->tail->next = node2->text;
                            node2->text->prev = node->text->tail;
                            node->text->tail = node2->text->tail;

                            rbDelete(&root, &node2);

                        } else{

                            node2 = treeSearch(&root_removed, key);

                            if(node2 != nil){

                                node2->text->tail->next = node->text;
                                node->text->prev = node2->text->tail;
                                node2->text->tail = node->text;

                                rbDelete(&root, &node);


                            } else{

                                tree_pointer add = createTreeNode(node->key, node->text);

                                rbDelete(&root, &node);
                                rbInsert(&root_removed, &add);

                            }

                        }

                    }


                }
            }

        }
        else if (strstr(row, "p") != NULL) {

            long ind1, ind2;
            getBounds(row, &ind1, &ind2);

            free(row);

            long numRow = ind2-ind1+1;

            int dotReserve = 0;

            // MODO 1
            for (int i = 0; i < numRow; i++) {

                long key = ind1+i;
                tree_pointer node = treeSearch(&root, key);

                if(node == nil){
                    node = treeSearch(&root_removed, key);
                    if(node == nil){
                        printf(".\n");
                    } else dotReserve++;
                } else printf("%s\n", node->text->tail->value);
            }

            for (int i = 0; i < dotReserve; ++i) {
                printf(".\n");
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

            free(row);

            undo += ret;

            for (int i = 0; i < ret; ++i) {
                long ind1, ind2;

                getBounds(tail->value, &ind1, &ind2);

                long numRow = ind2-ind1+1;

                for (int j = 0; j < numRow; ++j) {

                    long key = ind1+j;
                    tree_pointer node = treeSearch(&root, key);

                    if(node != nil){
                        node->text->tail = node->text->tail->prev;
                    } else{
                        node = treeSearch(&root_removed, key);

                        if(node != nil){

                            /*tree_pointer add = malloc(sizeof(typeof(struct TreeNode)));
                            add->key = node->key;
                            add->text = node->text;
                            add->prev = nil;
                            add->left = nil;
                            add->right = nil;*/

                            tree_pointer add = createTreeNode(node->key, node->text);

                            rbDelete(&root_removed, &node);
                            rbInsert(&root, &add);
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

                free(row);

                for (int i = 0; i < ret; ++i) {
                    long ind1, ind2;

                    getBounds(tail->next->value, &ind1, &ind2);

                    long numRow = ind2-ind1+1;

                    for (int j = 0; j < numRow; ++j) {

                        long key = ind1+j;
                        tree_pointer node = treeSearch(&root, key);

                        if(tail->next->value[3] == 'd'){

                            /*tree_pointer add = malloc(sizeof(typeof(struct TreeNode)));
                            add->key = node->key;
                            add->text = node->text;
                            add->prev = nil;
                            add->left = nil;
                            add->right = nil;*/

                            tree_pointer add = createTreeNode(node->key, node->text);

                            rbDelete(&root, &node);
                            rbInsert(&root_removed, &add);
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

        getRow();

    }


    cleanUpTree(&root);
    cleanUpTree(&root_removed);
    free(nil);
    free(row);
    cleanUpHistoryFromHead(head);

    return 0;
}




// RED BLACK TREE
void leftRotate(tree_pointer* root, tree_pointer* z){

    tree_pointer x = *z;
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
void rightRotate(tree_pointer* root, tree_pointer* z){

    tree_pointer x = *z;
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

void rbInsert(tree_pointer* root, tree_pointer* z){

    tree_pointer y = nil;
    tree_pointer x = *root;

    while (x != nil){
        y = x;
        if((*z)->key < x->key){
            x = x->left;
        } else{
            x = x->right;
        }
    }

    (*z)->prev = y;

    if(y == nil){
        *root = *z;
    } else if((*z)->key < y->key){
        y->left = *z;
    } else{
        y->right = *z;
    }

    (*z)->left = nil;
    (*z)->right = nil;
    (*z)->color = RED;

    rbInsertFixup(root, z);

}
void rbInsertFixup(tree_pointer* root, tree_pointer* w){

    tree_pointer z = *w;
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
                    rbInsertFixup(root, &x->prev);
                }
                else {
                    if (z==x->right){
                        z = x;
                        leftRotate(root, &z);
                        x = z->prev;
                    }
                    x->color = BLACK;
                    x->prev->color = RED;
                    rightRotate(root, &x->prev);
                }
            }
            else {
                y = x->prev->left;
                if (y->color==RED){
                    x->color = BLACK;
                    y->color = BLACK;
                    x->prev->color = RED;
                    rbInsertFixup(root, &x->prev);
                }
                else {
                    if (z==x->left){
                        z = x;
                        rightRotate(root, &z);
                        x = z->prev;
                    }
                    x->color = BLACK;
                    x->prev->color = RED;
                    leftRotate(root, &x->prev);
                }
            }
        }
    }


}

void rbDelete(tree_pointer* root, tree_pointer* z){

    tree_pointer x, y;

    if((*z)->left == nil || (*z)->right == nil){
        y = *z;
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

    if(y != *z){
        (*z)->key = y->key;
        (*z)->text = y->text;
    }

    if(y->color == BLACK){
        rbDeleteFixup(root, &x);
    }

    free(*z);

}
void rbDeleteFixup(tree_pointer* root, tree_pointer* x){

    if((*x)->color == RED || (*x)->prev == nil){
        (*x)->color = BLACK;
    } else if(x == &((*x)->prev)->left){

        tree_pointer w = (*x)->prev->right;

        if(w->color == RED){
            w->color = BLACK;
            (*x)->prev->color = RED;
            leftRotate(root, &((*x)->prev));
            w = (*x)->prev->right;
        }
        if(w->left->color == BLACK && w->right->color == BLACK){
            w->color = RED;
            rbDeleteFixup(root, &((*x)->prev));
        } else {
            if(w->right->color == BLACK) {
                w->left->color = BLACK;
                w->color = RED;
                rightRotate(root, &w);
                w = (*x)->prev->right;
            }

            w->color = (*x)->prev->color;
            (*x)->prev->color = BLACK;
            w->right->color = BLACK;
            leftRotate(root, &((*x)->prev));
        }
    } else{

        tree_pointer w = (*x)->prev->left;

        if(w->color == RED){
            w->color = BLACK;
            (*x)->prev->color = RED;
            rightRotate(root, &((*x)->prev));
            w = (*x)->prev->left;
        }
        if(w->right->color == BLACK && w->left->color == BLACK){
            w->color = RED;
            rbDeleteFixup(root, &((*x)->prev));
        } else {
            if(w->left->color == BLACK) {
                w->right->color = BLACK;
                w->color = RED;
                leftRotate(root, &w);
                w = (*x)->prev->left;
            }

            w->color = (*x)->prev->color;
            (*x)->prev->color = BLACK;
            w->left->color = BLACK;
            rightRotate(root, &((*x)->prev));
        }

    }

}

tree_pointer treeSuccessor(tree_pointer* x){
    if((*x)->right != nil){
        return treeMinimum(&(*x)->right);
    }

    tree_pointer y = (*x)->prev;

    while (y != nil && *x == y->right){
        *x = y;
        y = y->prev;
    }

    return y;
}
tree_pointer treeMinimum(tree_pointer* x){
    while ((*x)->left != nil){
        *x = (*x)->left;
    }
    return *x;
}

tree_pointer treeSearch(tree_pointer* x, long k){
    if(*x == nil || k == (*x)->key){
        return *x;
    }

    if(k < (*x)->key){
        return treeSearch(&(*x)->left, k);
    } else return treeSearch(&(*x)->right, k);
}

void cleanUpTree(tree_pointer* x){
    if(*x != nil){
        cleanUpTree(&(*x)->left);
        cleanUpTree(&(*x)->right);

        cleanUpHistoryFromHead((*x)->text);
        free(*x);

    }
}

// HELPER
char* getRow(){
    
    row = NULL;

    char tmp[1024];

    fgets(tmp, ROW_LEN, stdin);     // fgets gets '\n' at the end of the string
    strtok(tmp, "\n");

    unsigned long len = strlen(tmp);

    row = malloc(sizeof(char) * (len+1));

    strncpy(row, tmp, len);


    return row;
}
void getBounds(char row[], long *ind1, long *ind2){

    long *ret = malloc(sizeof(long));

    *ret = strtol(row, &row, 10);
    row++;
    *ind1 = *ret;
    *ret = strtol(row, &row, 10);
    *ind2 = *ret;

    free(ret);

}
tree_pointer createTreeNode(long key, history_pointer text){

    tree_pointer add = malloc(sizeof(typeof(struct TreeNode)));
    add->key = key;
    add->text = text;
    add->prev = nil;
    add->left = nil;
    add->right = nil;

    return add;
}

// DOUBLE LINKED LIST
history_pointer createHistoryNode(char *x) {

    history_pointer newNode = (history_pointer)malloc(sizeof(struct HistoryNode));

    //strcpy(newNode->value, x);
    newNode->value = x;
    newNode->prev = NULL;
    newNode->next = NULL;
    newNode->tail = NULL;

    return newNode;
}
void addInHistory(char row[]) {

    history_pointer newNode = createHistoryNode(row);

    if(tail == NULL){
        head = newNode;
        tail = newNode;
        return;
    }

    tail->next = newNode;
    newNode->prev = tail;
    tail = newNode;
}
void freeUnusedHistoryNode(){

    if(tail != NULL && tail->next != NULL){

        history_pointer bulldozer = tail->next;
        history_pointer nextMiles = NULL;

        while (bulldozer != NULL) {
            if (bulldozer->next != NULL) {
                nextMiles = bulldozer->next;
            } else nextMiles = NULL;
            free(bulldozer);
            bulldozer = nextMiles;
        }


        /*while (bulldozer != NULL){

            if(bulldozer->next != NULL) {
                bulldozer = bulldozer->next;
                free(bulldozer->prev);
            } else free(bulldozer);
        }*/

    }

}
void cleanUpHistoryFromHead(history_pointer head){

    history_pointer bulldozer = head;
    history_pointer nextMiles = NULL;

    while (bulldozer != NULL) {
        if (bulldozer->next != NULL) {
            nextMiles = bulldozer->next;
        } else nextMiles = NULL;
        free(bulldozer->value);
        free(bulldozer);
        bulldozer = nextMiles;
    }
}
