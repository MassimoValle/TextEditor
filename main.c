#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define ROW_LEN 1024

typedef struct TextNode* text_pointer;
typedef struct TextNodeContainer* container_pointer;
typedef struct RemoveContainer* remove_pointer;
typedef struct HistoryNode* history_pointer;
typedef struct Document* document_pointer;
typedef struct DocumentRemoved* documentRemoved_pointer;

typedef struct TreeNode* tree_pointer;


// HISTORY STRUCT
struct HistoryNode {
    char* value;
    history_pointer next;
    history_pointer prev;

};  // used for commands history

// DOCUMENT STRUCT
struct TextNode {
    char* value;
    text_pointer next;
    text_pointer prev;
};
struct TextNodeContainer {
    text_pointer textNode;
    text_pointer tail_textNode;
    container_pointer next;
    container_pointer prev;
};
struct RemoveContainer {
    char* command;
    container_pointer textContainer;
    remove_pointer next;
    remove_pointer prev;
};
struct Document {
    container_pointer head;
    container_pointer tail;
};
struct DocumentRemoved {
    remove_pointer head;
    remove_pointer tail;
};

// TREE STRUCT
enum Color{
    RED, BLACK
};
struct TreeNode{
    long key;
    container_pointer value;
    enum Color color;
    tree_pointer prev;
    tree_pointer left;
    tree_pointer right;

};


history_pointer head_history;
history_pointer tail_history;

long nodes_in_tree = 0;
tree_pointer tree_root;
tree_pointer nil;



char* row;



// RED BLACK TREE
tree_pointer createTreeNode(long key, container_pointer* c);

void leftRotate(tree_pointer* root, tree_pointer* z);
void rightRotate(tree_pointer* root, tree_pointer* z);

void rbInsert(tree_pointer* root, tree_pointer* z);
void rbInsertFixup(tree_pointer* root, tree_pointer* w);

void rbDelete(tree_pointer* root, tree_pointer* z);
void rbDeleteFixup(tree_pointer* root, tree_pointer* x);

tree_pointer treeSuccessor(tree_pointer* x);
tree_pointer treeMinimum(tree_pointer* x);

tree_pointer treeSearch(tree_pointer* x, long k);



// CREATE NODES
document_pointer createDocument();
documentRemoved_pointer createDocumentRemoved();
container_pointer createContainer();
text_pointer createTextNode(char *x);
history_pointer createHistoryNode(char *x);
remove_pointer createRemoveContainer();

// DB MANAGING
void addInHistory();
void addToDocument(document_pointer* document, char *x, long index);
int removeToDocument(document_pointer* document, documentRemoved_pointer* rowRemoved, container_pointer* headDel, char* command);


// CLEAN NODES
void cleanUpDocumentFromPointer(container_pointer* head);
void cleanUpDocumentRemoved(document_pointer* document, documentRemoved_pointer* docRemoved);
void cleanUpRemoveContainers(remove_pointer* head);
void cleanUpTextFromHead(text_pointer* head);
void cleanUpHistoryFromHead(history_pointer* head);
void freeUnusedContainerNode(document_pointer* document);
void freeUnusedRemoveContainer(documentRemoved_pointer* docRem);
void freeUnusedHistoryNode();

// DB HELPER
container_pointer listSearch(document_pointer* document, container_pointer* k);
void createHeadDocumentRemoved(documentRemoved_pointer* docRem, remove_pointer* c);
void appendNodeToDocumentRemoved(documentRemoved_pointer* docRem, remove_pointer* c);
int undoChange(document_pointer* document, long num);
int undoDelete(document_pointer* document, documentRemoved_pointer* docRem, long index);
void redoChange(document_pointer* document, long num);
int redoDelete(document_pointer* doc, documentRemoved_pointer* docRem, long index);


// HELPER
char* getRow();
void getBounds(char* line, long *ind1, long *ind2);




int main() {

    nil = malloc(sizeof(struct TreeNode));
    nil->color = BLACK;

    tree_root = nil;

    document_pointer document = createDocument();
    documentRemoved_pointer rowRemoved = createDocumentRemoved();

    long undo = 0;

    row = getRow();

    while (strstr(row, "q") == NULL){


        if (strstr(row, "c") != NULL) {

            if(undo > 0){

                if(rowRemoved->head == NULL){
                    freeUnusedHistoryNode();
                    freeUnusedContainerNode(&document);
                } else{
                    freeUnusedRemoveContainer(&rowRemoved);
                }
            }

            undo = 0;
            addInHistory(row);

            long ind1 = 0, ind2 = 0;

            getBounds(row, &ind1, &ind2);

            long numRow = ind2-ind1+1;

            // MODO 1
            for (int i = 0; i < numRow; i++) {

                long key = ind1+i;
                getRow();

                addToDocument(&document, row, key);
            }

            getRow();

            if(strstr(row, ".") != NULL){
                free(row);
            } else printf("something went wrong\n");

        }
        else if (strstr(row, "d") != NULL) {


            if(undo > 0){

                if(rowRemoved->head == NULL){
                    freeUnusedHistoryNode();
                    freeUnusedContainerNode(&document);
                } else{
                    freeUnusedRemoveContainer(&rowRemoved);
                }
            }

            undo = 0;
            addInHistory(row);

            long ind1, ind2;
            getBounds(row, &ind1, &ind2);

            long numRow = ind2-ind1+1;

            //container_pointer headDel = skip(&document, ind1);
            tree_pointer treeNode = treeSearch(&tree_root, ind1);

            long iterator = 0;
            while (treeNode == nil && iterator < nodes_in_tree){
                ind1++;
                iterator++;
                treeNode = treeSearch(&tree_root, ind1);
            }

            container_pointer headDel;
            if(treeNode != nil){
                headDel = treeNode->value;
            } else headDel = NULL;

            for (int i = 0; i < numRow; i++) {

                int res = removeToDocument(&document, &rowRemoved, &headDel, tail_history->value);

                if(res > 0){
                    tree_pointer delNode = treeSearch(&tree_root, ind1 + i);
                    rbDelete(&tree_root, &delNode);
                }


                /*if(ret == 0){

                    if(i == 0){     // se è proprio tutto il comando invalido
                        history_pointer clean = tail_history;
                        tail_history = tail_history->prev;

                        free(clean->value);
                        free(clean);

                        break;
                    }

                }*/

            }

        }
        else if (strstr(row, "p") != NULL) {

            long ind1, ind2;
            getBounds(row, &ind1, &ind2);

            free(row);


            if(ind1 == 0){
                printf(".\n");
                getRow();
                continue;
            }
            long numRow = ind2-ind1+1;


            //container_pointer headPrint = skip(&document, ind1);
            tree_pointer treeNode = treeSearch(&tree_root, ind1);
            long iterator = 0;
            while (treeNode == nil && iterator < nodes_in_tree){
                ind1++;
                iterator++;
                treeNode = treeSearch(&tree_root, ind1);
            }

            container_pointer headPrint;
            if(treeNode != nil){
                headPrint = treeNode->value;
            } else headPrint = NULL;

            for (int i = 0; i < numRow; i++) {

                if(headPrint != NULL && headPrint->tail_textNode != NULL){
                    printf("%s\n", headPrint->tail_textNode->value);
                    headPrint = headPrint->next;
                } else{
                    printf(".\n");
                }

            }

        }
        else if (strstr(row, "u") != NULL) {

            char* q;
            long ret = strtol(row, &q, 10);

            free(row);

            undo += ret;

            for (int i = 0; i < ret; ++i) {
                long ind1, ind2;

                if(tail_history == NULL){
                    continue;
                }

                getBounds(tail_history->value, &ind1, &ind2);

                long numRow = ind2-ind1+1;

                for (int j = 0; j < numRow; ++j) {

                    if(tail_history->value[3] == 'c'){

                        int val = undoChange(&document, ind1+j);

                        if(val == 0){
                            continue;
                        }


                    } else{

                        int val = undoDelete(&document, &rowRemoved, ind1+j);

                        if(val == 0){
                            continue;
                        }

                    }
                }

                tail_history = tail_history->prev;
            }

        }
        else if (strstr(row, "r") != NULL) {

            if(undo > 0){

                char* q;
                long ret = strtol(row, &q, 10);

                free(row);

                if(ret > undo){
                    ret = undo;
                }

                for (int i = 0; i < ret; ++i) {
                    long ind1, ind2;

                    getBounds(tail_history->next->value, &ind1, &ind2);

                    long numRow = ind2-ind1+1;

                    for (int j = 0; j < numRow; ++j) {

                        if(tail_history->next->value[3] == 'c'){

                            redoChange(&document, ind1+j);

                        } else{

                            redoDelete(&document, &rowRemoved, ind1+j);

                        }

                    }

                    tail_history = tail_history->next;
                }

                undo -= ret;

            }



        }
        

        getRow();

    }

    cleanUpDocumentRemoved(&document, &rowRemoved);
    cleanUpHistoryFromHead(&head_history);
    cleanUpDocumentFromPointer(&document->head);




    free(row);
    free(document);
    free(rowRemoved);


    return 0;
}




// RED BLACK TREE
tree_pointer createTreeNode(long key, container_pointer* c){

    tree_pointer x = malloc(sizeof(struct TreeNode));
    x->prev = NULL;
    x->left = NULL;
    x->right = NULL;
    x->color = RED;

    x->key = key;
    x->value = *c;

    return x;
}

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

    nodes_in_tree++;

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
        (*z)->value = y->value;
    }

    if(y->color == BLACK){
        rbDeleteFixup(root, &x);
    }

    nodes_in_tree--;
    //free(*z);

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



// CREATE NODES
document_pointer createDocument(){

    document_pointer doc = malloc(sizeof(struct Document));
    doc->head = NULL;
    doc->tail = NULL;

    return doc;
}
documentRemoved_pointer createDocumentRemoved(){

    documentRemoved_pointer doc = malloc(sizeof(struct DocumentRemoved));
    doc->head = NULL;
    doc->tail = NULL;

    return doc;
}
container_pointer createContainer(){

    container_pointer newNode = malloc(sizeof(struct TextNodeContainer));

    newNode->textNode = NULL;
    newNode->prev = NULL;
    newNode->next = NULL;

    return newNode;
}
text_pointer createTextNode(char *x){

    text_pointer newNode = (text_pointer)malloc(sizeof(struct TextNode));

    newNode->value = x;
    newNode->prev = NULL;
    newNode->next = NULL;

    return newNode;
}
history_pointer createHistoryNode(char *x) {

    history_pointer newNode = (history_pointer)malloc(sizeof(struct HistoryNode));

    //strcpy(newNode->textNode, x);
    newNode->value = x;
    newNode->prev = NULL;
    newNode->next = NULL;

    return newNode;
}
remove_pointer createRemoveContainer(){

    remove_pointer newNode = malloc(sizeof(struct RemoveContainer));

    newNode->textContainer = NULL;
    newNode->prev = NULL;
    newNode->next = NULL;

    return newNode;

}

// DB MANAGING
void addInHistory() {

    history_pointer newNode = createHistoryNode(row);

    if(tail_history == NULL){
        head_history = newNode;
        tail_history = newNode;
        return;
    }

    tail_history->next = newNode;
    newNode->prev = tail_history;
    tail_history = newNode;
}
void addToDocument(document_pointer* document, char *x, long index){

    document_pointer doc = *document;

    if(doc->head == NULL || doc->tail == NULL){     // se è il primo nodo

        text_pointer n = createTextNode(x);
        container_pointer c = createContainer();
        c->textNode = n;
        c->tail_textNode = n;


        doc->head = c;
        doc->tail = c;

        tree_pointer node = createTreeNode(index, &c);
        rbInsert(&tree_root, &node);


    }
    else{

        //container_pointer y = skip(document, index);
        tree_pointer  treeNode = treeSearch(&tree_root, index);
        container_pointer y = treeNode->value;

        if(treeNode != nil && y != NULL){  // cambia il valore in coda a textNode
            text_pointer n = createTextNode(x);
            y->tail_textNode->next = n;
            n->prev = y->tail_textNode;
            y->tail_textNode = n;
        }
        else{
            // inserisci nuovo nodo in coda al documento

            text_pointer n = createTextNode(x);
            container_pointer c = createContainer();
            c->textNode = n;
            c->tail_textNode = n;

            doc->tail->next = c;
            c->prev = doc->tail;
            doc->tail = c;

            tree_pointer node = createTreeNode(index, &c);
            rbInsert(&tree_root, &node);

        }


    }
}
int removeToDocument(document_pointer* document, documentRemoved_pointer* rowRemoved, container_pointer* headDel, char* command){

    if(*headDel == NULL){   // se non esiste il nodo
        return 0;
    }

    document_pointer doc = *document;
    documentRemoved_pointer docRem = *rowRemoved;
    container_pointer del = *headDel;


    if(del->prev != NULL){      // se non sto rimuovendo la testa del documento

        remove_pointer c = createRemoveContainer();
        c->command = command;
        c->textContainer = del;

        if(docRem->head == NULL || docRem->tail==NULL){     // se è il primo nodo dei rimossi
           createHeadDocumentRemoved(rowRemoved, &c);

        } else{     // altrimenti lo aggiungo in coda

            appendNodeToDocumentRemoved(rowRemoved, &c);
        }

        // faccio il link tra i vicini del nodo rimosso
        del->prev->next = del->next;

        if(del->next == NULL) {     // se ho rimosso la coda del documento
            doc->tail = del->prev;
        } else{                             // altrimenti completo il link normalmente
            del->next->prev = del->prev;
        }


    } else{     // se sto rimuovendo la testa del documento

        remove_pointer c = createRemoveContainer();
        c->command = command;
        c->textContainer = del;

        if(docRem->head == NULL || docRem->tail==NULL){   // quando cazzo ci capito qui???

            createHeadDocumentRemoved(rowRemoved, &c);
        } else{

            appendNodeToDocumentRemoved(rowRemoved, &c);
        }


        if(del->next == NULL){  // se ho cancellato l'unico nodo del documento

            doc->head = NULL;
            doc->tail = NULL;
        } else{

            del->next->prev = NULL;
            doc->head = del->next;
        }
    }


    del = del->next;
    *headDel = del;

    return 1;
}


// CLEAN NODES
void cleanUpDocumentFromPointer(container_pointer* head){

    container_pointer bulldozer = *head;
    container_pointer nextMiles = NULL;

    while (bulldozer != NULL) {
        if (bulldozer->next != NULL) {
            nextMiles = bulldozer->next;
        } else nextMiles = NULL;

        cleanUpTextFromHead(&(bulldozer->textNode));
        free(bulldozer);
        bulldozer = nextMiles;
    }
}
void cleanUpDocumentRemoved(document_pointer* document, documentRemoved_pointer* docRemoved){

    documentRemoved_pointer docRem = *docRemoved;
    remove_pointer rem = docRem->head;
    remove_pointer nextMiles = NULL;

    while (rem != NULL) {
        if (rem->next != NULL) {
            nextMiles = rem->next;
        } else nextMiles = NULL;


        if(listSearch(document, &rem->textContainer) == NULL){  // se rem.textnode non è presente rimuovilo
            cleanUpTextFromHead(&rem->textContainer->textNode);
            free(rem->textContainer);
        }
        free(rem);
        rem = nextMiles;
    }

    docRem->head = NULL;
    docRem->tail = NULL;

}
void cleanUpRemoveContainers(remove_pointer* head){

    remove_pointer bulldozer = *head;
    remove_pointer nextMiles = NULL;

    while (bulldozer != NULL) {
        if (bulldozer->next != NULL) {
            nextMiles = bulldozer->next;
        } else nextMiles = NULL;
        free(bulldozer->command);
        //cleanUpTextFromHead(&bulldozer->textContainer->textNode);
        //free(bulldozer->textContainer);
        free(bulldozer);
        bulldozer = nextMiles;
    }

}
void cleanUpTextFromHead(text_pointer* head){

    text_pointer bulldozer = *head;
    text_pointer nextMiles = NULL;

    while (bulldozer != NULL) {
        if (bulldozer->next != NULL) {
            nextMiles = bulldozer->next;
        } else nextMiles = NULL;
        free(bulldozer->value);
        free(bulldozer);
        bulldozer = nextMiles;
    }
}
void cleanUpHistoryFromHead(history_pointer* head){

    history_pointer bulldozer = *head;
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
void freeUnusedContainerNode(document_pointer* document){

    document_pointer doc = *document;
    if(doc->tail == NULL){
        cleanUpDocumentFromPointer(&doc->head);
        doc->head = NULL;
    } else{

        container_pointer c = (*document)->head;
        container_pointer c_next = NULL;

        while (c != NULL){

            if(c->next != NULL){
                c_next = c->next;
            } else c_next = NULL;

            if(c->tail_textNode->next != NULL){

                cleanUpTextFromHead(&c->tail_textNode->next);
                c->tail_textNode->next = NULL;

                if(c->textNode == NULL){
                    doc->head = c->next;
                    free(c);
                }

            }

            c = c_next;


        }
    }
}
void freeUnusedRemoveContainer(documentRemoved_pointer* docRem){

    documentRemoved_pointer rowRem = *docRem;

    if(rowRem->tail == NULL){
        cleanUpRemoveContainers(&rowRem->head);
        rowRem->head = NULL;
    }
    else cleanUpRemoveContainers(&rowRem->tail);

}
void freeUnusedHistoryNode(){

    if(tail_history != NULL && tail_history->next != NULL){

        cleanUpHistoryFromHead(&tail_history->next);
        tail_history->next = NULL;

    } else cleanUpHistoryFromHead(&head_history);

}

// DB HELPER
container_pointer listSearch(document_pointer* document, container_pointer* k){

    container_pointer x = (*document)->head;

    while (x != NULL && x != *k){
        x = x->next;
    }

    return x;
}
void createHeadDocumentRemoved(documentRemoved_pointer* docRem, remove_pointer* c){
    (*docRem)->head = *c;
    (*docRem)->tail = *c;
}
void appendNodeToDocumentRemoved(documentRemoved_pointer* docRem, remove_pointer* c){
    (*docRem)->tail->next = *c;
    (*c)->prev = (*docRem)->tail;
    (*docRem)->tail = *c;
}
int undoChange(document_pointer* document, long num) {

    //container_pointer c = skip(document, num);
    tree_pointer treeNode = treeSearch(&tree_root, num);
    container_pointer c = treeNode->value;

    if (c->tail_textNode == NULL) {     // se annullo l'istruzione che ha creato quel nodo
        return 0;
    } else {
        c->tail_textNode = c->tail_textNode->prev;
        return 1;
    }
}
int undoDelete(document_pointer* document, documentRemoved_pointer* docRem, long index){

    // il nodo è stato eliminato, devo rimetterlo nel documento

    document_pointer doc = *document;
    documentRemoved_pointer rowRemoved = *docRem;

    remove_pointer r = rowRemoved->tail;


    if(r == NULL) return 0;

    while (r->command != tail_history->value){

        if(r->prev == NULL){                    // voglio fare l'undo e ?
            return 0;
        }
        r = r->prev;
    }

    container_pointer c = r->textContainer;
    container_pointer search = doc->head;

    if(search == NULL || search->tail_textNode == NULL){     // se il documento è vuoto e quindi devo inserire la testa
        doc->head = c;
        doc->tail = c;

        if(search->tail_textNode == NULL){                  // allora vanno cancellati tutti i vacchi nodi dall'albero

            long index2 = index;
            long iterator = 0;
            while (iterator < nodes_in_tree){
                tree_pointer searchNode = treeSearch(&tree_root, index2);

                while(searchNode == nil){
                    index2++;
                    searchNode = treeSearch(&tree_root, index2);
                }

                tree_pointer treeNode = treeSearch(&tree_root, index2);
                rbDelete(&tree_root, &treeNode);

                iterator++;
            }

        }

    } else{                 // se il documento ha già dei nodi

        container_pointer search_prev = search;
        while (c->next != search){    // skippo avanti finchè non trovo il nodo a cui devo inserire in prev il nodo c

            if(search->next != NULL) {
                search_prev = search->next;
            }

            search = search->next;
        }

        if(search == NULL){             // se devo inserire il nodo in coda
            search_prev->next = c;
            doc->tail = c;
        } else{

            if(search->prev == NULL){   // se devo inserire il mio nodo prima di search
                c->next = search;
                search->prev = c;
                doc->head = c;
            }
            else{                       // se devo inserirlo nel mezzo
                search->prev->next = c;
                search->prev = c;
            }

        }


    }

    rowRemoved->tail = rowRemoved->tail->prev;  // porto indietro la coda poichè ho aggiunto quest'ultima al documento


    tree_pointer searchNode = treeSearch(&tree_root, index);
    while(searchNode != nil){
        searchNode->key++;
        searchNode = treeSearch(&tree_root, index);
    }

    tree_pointer treeNode = createTreeNode(index, &c);
    rbInsert(&tree_root, &treeNode);


    return 1;

}
void redoChange(document_pointer* document, long num){

    //container_pointer c = skip(document, num);
    tree_pointer treeNode = treeSearch(&tree_root, num);
    container_pointer c = treeNode->value;

    if(c->tail_textNode != NULL){
        c->tail_textNode = c->tail_textNode->next;
    } else c->tail_textNode = c->textNode;


}
int redoDelete(document_pointer* doc, documentRemoved_pointer* docRem, long index){

    // devo tornare a cancellare i nodi che l'undo ha rimesso

    document_pointer document = *doc;
    documentRemoved_pointer rowRemoved = *docRem;

    remove_pointer r = rowRemoved->tail;

    if(r == NULL){
        r = rowRemoved->head;
    }

    while (r->command != tail_history->next->value){

        if(r->next == NULL){                    // voglio fare la redo e ?
            return 0;
        }
        r = r->next;
    }

    container_pointer c = r->textContainer;  // c è il nodo da rimuovere nel documento

    // teoricamente non serve fare sta cosa con il search perchè da c sò già chi lo procede e chi lo segue
    container_pointer search = document->head; // search mi serve per sapere chi lo procede e chi lo segue

    while (c->next != search->next){
        search = search->next;
    }


    if(search->next == NULL && search->prev == NULL){       // se il documento contiene un solo nodo
        document->head = NULL;
        document->tail = NULL;
    }
    else{

        if(search->prev == NULL){                   // se sto eliminando la testa del documento
            search->next->prev = NULL;
            document->head = search->next;
        }
        else{

            if(search->next == NULL){               // se sto eliminando la coda del documento
                search->prev->next = NULL;
                document->tail = search->prev;
            }
            else{                                   // se sto eliminando un nodo nel mezzo
                search->next->prev = search->prev;
                search->prev->next = search->next;
            }

        }

    }

    if(rowRemoved->tail == NULL){
        rowRemoved->tail = rowRemoved->head;
    } else{
        rowRemoved->tail = rowRemoved->tail->next;  // porto avanti la coda poichè ho rimosso quest'ultima al documento
    }


    tree_pointer searchNode = treeSearch(&tree_root, index);
    while(searchNode == nil){
        index++;
        searchNode = treeSearch(&tree_root, index);
    }

    tree_pointer treeNode = treeSearch(&tree_root, index);
    rbDelete(&tree_root, &treeNode);

    return 1;

}


// HELPER
char* getRow(){
    
    row = NULL;

    char* tmp = malloc(sizeof(char) * ROW_LEN);

    fgets(tmp, ROW_LEN, stdin);     // fgets gets '\n' at the end of the string

    unsigned long len = strlen(tmp);

    row = malloc(sizeof(char) * len);

    strncpy(row, tmp, len);
    strtok(row, "\n");

    free(tmp);


    return row;
}
void getBounds(char* line, long *ind1, long *ind2){

    long *ret = malloc(sizeof(long));

    *ret = strtol(line, &line, 10);
    line++;
    *ind1 = *ret;
    *ret = strtol(line, &line, 10);
    *ind2 = *ret;

    free(ret);

}

