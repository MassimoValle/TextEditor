#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
};              // contains string
struct TextNodeContainer {
    text_pointer textNode;
    text_pointer tail_textNode;
    container_pointer next;
    container_pointer prev;
};     // contains text_pointer
struct RemoveContainer {
    char* command;
    container_pointer textContainer;
    remove_pointer next;
    remove_pointer prev;
};       // contains command string and container_pointer
struct Document {
    container_pointer head;
    container_pointer tail;
};              // contains container_pointer
struct DocumentRemoved {
    remove_pointer head;
    remove_pointer tail;
};       // contains remove_pointer

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

};               // contains key and container_pointer


// GLOBAL VAR HISTORY
history_pointer head_history;
history_pointer tail_history;


// GLOBAL VAR TREE
long nodes_in_tree = 0;
tree_pointer tree_root;
tree_pointer nil;



char* row;



// RED BLACK TREE
tree_pointer createTreeNode(long key, container_pointer* c);

void leftRotate(tree_pointer x);
void rightRotate(tree_pointer x);

void rbInsert(tree_pointer z);
void rbInsertFixup(tree_pointer z);

void rbDelete(tree_pointer z);
void rbDeleteFixup(tree_pointer t);

tree_pointer treeSuccessor(tree_pointer* x);
tree_pointer treePredecessor(tree_pointer* x);
tree_pointer treeMinimum(tree_pointer* x);
tree_pointer treeMaximum(tree_pointer* x);

tree_pointer treeSearch(tree_pointer* x, long k);

void cleanUpTree(tree_pointer* x);

void pushTreeNodes(long index);
void pullTreeNodes(long index);



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
container_pointer removeToDocument(document_pointer* document, documentRemoved_pointer* hell, remove_pointer* hellNode, long treeIndexToRemove);


// CLEAN NODES
void cleanUpDocumentFromPointer(container_pointer* head);
void cleanUpDocumentRemoved(documentRemoved_pointer* docRemoved);
void cleanUpRemoveContainers(remove_pointer* head);
void cleanUpTextFromHead(text_pointer* head);
void cleanUpHistoryFromHead(history_pointer* head);
void freeUnusedContainerNode(document_pointer* document);
void freeUnusedHellContainer(documentRemoved_pointer* hell);
void freeUnusedHeavenContainer(documentRemoved_pointer* heaven);
void freeUnusedHistoryNode();

// DB HELPER
container_pointer listSearch(document_pointer* document, container_pointer* k);

void addToHeaven(documentRemoved_pointer* heaven, container_pointer* c);
void addToHell(documentRemoved_pointer* hell, remove_pointer* hellNode);
void createHeadDocumentRemoved(documentRemoved_pointer* docRem, remove_pointer* c);
void appendNodeToDocumentRemoved(documentRemoved_pointer* docRem, remove_pointer* c);

int undoChange(long num);
int undoDelete(document_pointer* document, documentRemoved_pointer* oldNodes, documentRemoved_pointer* futureNodes, long index);
void redoChange(long num, document_pointer* document, documentRemoved_pointer* heaven);
int redoDelete(document_pointer* document, documentRemoved_pointer* hell, long treeIndexToRemove);


// HELPER
char* getRow();
void getBounds(char* line, long *ind1, long *ind2);




int main() {

    nil = (tree_pointer)malloc(sizeof(struct TreeNode));
    nil->color = BLACK;
    nil->prev = NULL;
    nil->left = NULL;
    nil->right = NULL;
    nil->key = 0;
    nil->value = NULL;

    tree_root = nil;

    document_pointer document = createDocument();

    documentRemoved_pointer hell = createDocumentRemoved();     // nodi rimossi dalla delete
    documentRemoved_pointer heaven = createDocumentRemoved();   // nodi rimossi dall'undo

    long undo = 0;      // indica di quante istruzioni si è fatta la undo

    row = getRow();

    while (strstr(row, "q") == NULL){


        if (strstr(row, "c") != NULL) {     // modifica o inserimento di righe

            if(undo > 0){       // mi serve per la undo/redo

                freeUnusedHistoryNode();
                freeUnusedContainerNode(&document);
                freeUnusedHellContainer(&hell);
                freeUnusedHeavenContainer(&heaven);

                /*if(hell->head == NULL || heaven->tail != NULL){
                    freeUnusedHistoryNode();
                    freeUnusedContainerNode(&document);
                } else{
                    freeUnusedHellContainer(&hell);
                    freeUnusedHeavenContainer(&heaven);
                }*/

                undo = 0;
            }

            addInHistory();                     // aggiungo il comando alla cronologia

            long ind1 = 0, ind2 = 0;

            getBounds(row, &ind1, &ind2);       // prendo gli indici del comando

            long numRow = ind2-ind1+1;

            // MODO 1
            for (int i = 0; i < numRow; i++) {

                long key = ind1+i;
                getRow();

                addToDocument(&document, row, key);     // aggiungo al documento al la stringa(row) all'indice key
            }

            getRow();

            if(strstr(row, ".") != NULL){
                free(row);
            } else printf("something went wrong\n");

        }
        else if (strstr(row, "d") != NULL) {         // elimina righe se presenti nel documento


            if(undo > 0){       // mi serve per la undo/redo

                if(hell->head == NULL || heaven->tail != NULL){
                    freeUnusedHistoryNode();
                    freeUnusedContainerNode(&document);
                } else{
                    freeUnusedHellContainer(&hell);
                    freeUnusedHeavenContainer(&heaven);
                }

                undo = 0;
            }


            addInHistory();             // aggiungo il comando alla cronologia

            long ind1, ind2;
            getBounds(row, &ind1, &ind2);

            long numRow = ind2-ind1+1;

            tree_pointer treeNode = treeSearch(&tree_root, ind1);       // cerco il primo nodo da eliminare

            /*long iterator = 0;
            while (treeNode == nil && iterator < nodes_in_tree){
                ind1++;
                iterator++;
                treeNode = treeSearch(&tree_root, ind1);
            }*/

            container_pointer listNodeToDelete;

            if(treeNode != nil){                // se esiste allora lo prendo da treeNode
                listNodeToDelete = treeNode->value;
            } else listNodeToDelete = NULL;


            for (int i = 0; i < numRow; i++) {

                if(listNodeToDelete == NULL){   // se non esiste il nodo
                    continue;
                }

                remove_pointer hellNode = createRemoveContainer();
                hellNode->command = tail_history->value;
                hellNode->textContainer = listNodeToDelete;

                container_pointer nextListNodeToDel = removeToDocument(&document, &hell, &hellNode, ind1);   // rimuovo dal documento il nodo(listNodeToDelete) e lo metto in hell

                listNodeToDelete = nextListNodeToDel;


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


            if(ind2 == 0){
                printf(".\n");
                getRow();
                continue;
            }
            long numRow = ind2-ind1+1;


            tree_pointer treeNode = treeSearch(&tree_root, ind1);
            /*long iterator = 0;
            while (treeNode == nil && iterator < nodes_in_tree){
                ind1++;
                iterator++;
                treeNode = treeSearch(&tree_root, ind1);
            }*/ // usando la pullTreeNode nella delete non serve

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

                        int val = undoChange(ind1+j);

                        if(val == 0){
                            continue;
                        }


                    } else{

                        int val = undoDelete(&document, &hell, &heaven, ind1);

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

                    char* historyCommand = NULL;
                    if(tail_history != NULL){
                        historyCommand = tail_history->next->value;
                    }
                    else historyCommand = head_history->value;


                    getBounds(historyCommand, &ind1, &ind2);


                    long numRow = ind2-ind1+1;

                    for (int j = 0; j < numRow; ++j) {

                        if(historyCommand[3] == 'c'){

                            redoChange(ind1+j, &document, &heaven);

                        } else{

                            redoDelete(&document, &hell, ind1);

                        }

                    }

                    if(tail_history != NULL){
                        tail_history = tail_history->next;
                    } else tail_history = head_history;
                }

                undo -= ret;

            }



        }
        

        getRow();

    }

    cleanUpDocumentRemoved(&hell);
    cleanUpHistoryFromHead(&head_history);
    cleanUpDocumentFromPointer(&document->head);

    cleanUpTree(&tree_root);



    free(row);
    free(nil);
    free(document);
    free(hell);
    free(heaven);

    //free(tree_root);  // le elimino già con cleanUpTree(&tree_root)


    return 0;
}




// RED BLACK TREE
tree_pointer createTreeNode(long key, container_pointer* c){

    tree_pointer x = (tree_pointer)malloc(sizeof(struct TreeNode));
    x->prev = nil;
    x->left = nil;
    x->right = nil;
    x->color = RED;

    x->key = key;
    x->value = *c;

    return x;
}

void leftRotate(tree_pointer x){

    tree_pointer y = x->right;
    x->right = y->left;

    if (y->left != nil){
        y->left->prev = x;
    }
    if (y != nil){
        y->prev = x->prev;
    }
    if (x->prev == nil){
        tree_root = y;
    }
    else if (x == x->prev->left){
        x->prev->left = y;
    }
    else {
        x->prev->right = y;
    }

    y->left = x;

    if (x != nil){
        x->prev = y;
    }

}
void rightRotate(tree_pointer x){

    tree_pointer y = x->left;
    x->left = y->right;

    if (y->right != nil){
        y->right->prev = x;
    }

    if(y != nil){
        y->prev = x->prev;
    }

    if (x->prev == nil){
        tree_root = y;
    }
    else if (x == x->prev->left){
        x->prev->left = y;
    }
    else {
        x->prev->right = y;
    }

    y->right = x;

    if (x != nil){
        x->prev = y;
    }
}

void rbInsert(tree_pointer z){

    tree_pointer y = nil;
    tree_pointer x = tree_root;

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
        tree_root = z;
    } else if(z->key < y->key){
        y->left = z;
    } else{
        y->right = z;
    }

    z->left = nil;
    z->right = nil;
    z->color = RED;

    //tree_insert_fixup(&tree_root, z);
    rbInsertFixup(z);

    nodes_in_tree++;

}
void rbInsertFixup(tree_pointer z){

    if (z==tree_root){
        tree_root->color = BLACK;
    }
    else {
        tree_pointer x = z->prev;

        if (x->color==RED){
            if (x==x->prev->left){
                tree_pointer y = x->prev->right;
                if (y->color==RED){
                    x->color = BLACK;
                    y->color = BLACK;
                    x->prev->color = RED;
                    rbInsertFixup(x->prev);
                }
                else {
                    if (z==x->right){
                        z = x;
                        leftRotate(z);
                        x = z->prev;
                    }
                    x->color = BLACK;
                    x->prev->color = RED;
                    rightRotate(x->prev);

                }
            }
            else {
                tree_pointer y = x->prev->left;
                if (y->color==RED){
                    x->color = BLACK;
                    y->color = BLACK;
                    x->prev->color = RED;
                    rbInsertFixup(x->prev);
                }
                else {
                    if (z==x->left){
                        z = x;
                        rightRotate(z);
                        x = z->prev;
                    }
                    x->color = BLACK;
                    x->prev->color = RED;
                    leftRotate(x->prev);

                }
            }
        }
    }


    /*
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
    */

}

void rbDelete(tree_pointer z){

    //!!! y è il nodo da eliminare, x è quello con cui lo sostituiamo

    tree_pointer x, y;

    if(z->left == nil || z->right == nil){
        y = z;
    } else{
        y = treeSuccessor(&z);
    }

    if(y->left != nil){
        x = y->left;
    } else{
        x = y->right;
    }

    x->prev = y->prev;

    if(y->prev == nil){
        tree_root = x;
    } else if(y == y->prev->left){
        y->prev->left = x;
    } else{
        y->prev->right = x;
    }

    if(y != z){
        z->key = y->key;
        z->value = y->value;
    }

    if(y->color == BLACK){
        //tree_delete_fixup(&tree_root, x);
        rbDeleteFixup(x);
    }

    free(y);

    nodes_in_tree--;

}
void rbDeleteFixup(tree_pointer t){

    tree_pointer x = t;
    tree_pointer w;
    if (x->color==RED || x->prev==nil){
        x->color = BLACK;
    }
    else if (x == x->prev->left){
        w = x->prev->right;
        if (w==nil){
            rbDeleteFixup(x->prev);
            return;
        }
        if (w->color==RED){
            w->color = BLACK;
            x->prev->color = RED;
            leftRotate(x->prev);
            w = x->prev->right;
        }
        if (w==nil){
            rbDeleteFixup(x->prev);
            return;
        }
        if (w->left->color==BLACK && w->right->color==BLACK){
            w->color = RED;
            rbDeleteFixup(x->prev);
        }
        else {
            if (w->right->color==BLACK) {
                w->left->color = BLACK;
                w->color = RED;
                rightRotate(w);
                w = x->prev->right;
            }
            w->color = x->prev->color;
            x->prev->color = BLACK;
            w->right->color = BLACK;
            leftRotate(x->prev);

        }
    }
    else {
        w = x->prev->left;
        if (w==nil){
            rbDeleteFixup(x->prev);
            return;
        }
        if (w->color==RED){
            w->color = BLACK;
            x->prev->color = RED;
            rightRotate(x->prev);
            w = x->prev->left;
        }
        if (w==nil){
            rbDeleteFixup(x->prev);
            return;
        }
        if (w->right->color==BLACK && w->left->color==BLACK){
            w->color = RED;
            rbDeleteFixup(x->prev);
        }
        else {
            if (w->left->color==BLACK){
                w->right->color = BLACK;
                w->color = RED;
                leftRotate(w);
                w = x->prev->left;
            }
            w->color = x->prev->color;
            x->prev->color = BLACK;
            w->left->color = BLACK;
            rightRotate(x->prev);

        }
    }

    /*
    if((*x)->color == RED || (*x)->prev == nil){
        (*x)->color = BLACK;
    } else if(*x == ((*x)->prev)->left){

        tree_pointer w = (*x)->prev->right;

        if(w->color == RED){
            w->color = BLACK;
            (*x)->prev->color = RED;
            leftRotate(&((*x)->prev));
            w = (*x)->prev->right;
        }
        if(w->left->color == BLACK && w->right->color == BLACK){
            w->color = RED;
            rbDeleteFixup(&((*x)->prev));
        } else {
            if(w->right->color == BLACK) {
                w->left->color = BLACK;
                w->color = RED;
                rightRotate(&w);
                w = (*x)->prev->right;
            }

            w->color = (*x)->prev->color;
            (*x)->prev->color = BLACK;
            w->right->color = BLACK;
            leftRotate(&((*x)->prev));
        }
    } else{

        tree_pointer w = (*x)->prev->left;

        if(w->color == RED){
            w->color = BLACK;
            (*x)->prev->color = RED;
            rightRotate(&((*x)->prev));
            w = (*x)->prev->left;
        }
        if(w->right->color == BLACK && w->left->color == BLACK){    // ECCEZIONE: W è nil
            w->color = RED;
            rbDeleteFixup(&((*x)->prev));
        } else {
            if(w->left->color == BLACK) {
                w->right->color = BLACK;
                w->color = RED;
                leftRotate(&w);
                w = (*x)->prev->left;
            }

            w->color = (*x)->prev->color;
            (*x)->prev->color = BLACK;
            w->left->color = BLACK;
            rightRotate(&((*x)->prev));
        }

    }
     */

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
tree_pointer treePredecessor(tree_pointer* x){

    if ((*x)->left != nil){
        return treeMaximum(&(*x)->left);
    }
    tree_pointer y = (*x)->prev;

    while (y!=nil && ((*x) == y->left)){
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
tree_pointer treeMaximum(tree_pointer* x){
    while ((*x)->right != nil){
        *x = (*x)->right;
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
tree_pointer treeSearchByContainer(tree_pointer* x, container_pointer* k){

    container_pointer container = *k;
    tree_pointer ret;

    if(*x == nil || container == (*x)->value){
        return *x;
    }

    ret = treeSearchByContainer(&(*x)->left, &container);

    if(ret != nil) return ret;
    else{
        return treeSearchByContainer(&(*x)->right, &container);
    }
}

void cleanUpTree(tree_pointer* x){

    if(*x != nil){
        cleanUpTree(&(*x)->left);
        cleanUpTree(&(*x)->right);

        free(*x);

    }
}

void pushTreeNodes(long index){

    tree_pointer searchNode = treeSearch(&tree_root, index);
    tree_pointer searchNode_next = searchNode;
    long iterator = 0;
    while(searchNode_next != nil && iterator < nodes_in_tree){
        searchNode_next = treeSearch(&tree_root, index+iterator+1);
        searchNode->key++;
        iterator++;
        searchNode = searchNode_next;
        //searchNode = treeSuccessor(&searchNode);
    }

}
void pullTreeNodes(long index){

    tree_pointer searchNode = treeSearch(&tree_root, index);
    tree_pointer searchNode_next = searchNode;
    long iterator = 0;
    while(searchNode_next != nil && iterator < nodes_in_tree){
        searchNode_next = treeSearch(&tree_root, index+iterator+1);
        searchNode->key--;
        iterator++;
        searchNode = searchNode_next;
        //searchNode = treeSuccessor(&searchNode);
    }

}



// CREATE NODES
document_pointer createDocument(){

    document_pointer doc = (document_pointer)malloc(sizeof(struct Document));
    doc->head = NULL;
    doc->tail = NULL;

    return doc;
}
documentRemoved_pointer createDocumentRemoved(){

    documentRemoved_pointer doc = (documentRemoved_pointer)malloc(sizeof(struct DocumentRemoved));
    doc->head = NULL;
    doc->tail = NULL;

    return doc;
}
container_pointer createContainer(){

    container_pointer newNode = (container_pointer)malloc(sizeof(struct TextNodeContainer));

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

    remove_pointer newNode = (remove_pointer)malloc(sizeof(struct RemoveContainer));

    newNode->textContainer = NULL;
    newNode->prev = NULL;
    newNode->next = NULL;

    return newNode;

}

// DB MANAGING
void addInHistory() {

    history_pointer newNode = createHistoryNode(row);

    if(tail_history == NULL){       // se è il primo nodo della cronologia lo aggingo in testa
        head_history = newNode;
        tail_history = newNode;
    }

    else{                                   // altrimento lo aggiungo in coda
        tail_history->next = newNode;
        newNode->prev = tail_history;
        tail_history = newNode;
    }

}
void addToDocument(document_pointer* document, char *x, long index){

    document_pointer doc = *document;

    if(doc->head == NULL || doc->tail == NULL){     // se il documento è vuoto

        text_pointer n = createTextNode(x);
        container_pointer c = createContainer();
        c->textNode = n;
        c->tail_textNode = n;


        doc->head = c;                                      // metto il nodo come primo e ultimo elemento della linked list
        doc->tail = c;

        tree_pointer node = createTreeNode(index, &c);
        rbInsert(node);                                     // aggiungo il nodo anche all'albero


    }
    else{

        tree_pointer  treeNode = treeSearch(&tree_root, index);     // cerco se esiste già una stringa a quella riga
        container_pointer y = treeNode->value;

        if(treeNode != nil && y != NULL){           // se c'è, aggiungo il valore in coda a textNode
            text_pointer n = createTextNode(x);
            y->tail_textNode->next = n;
            n->prev = y->tail_textNode;
            y->tail_textNode = n;
        }
        else{       // se non c'è, inserisci nuovo nodo in coda al documento

            text_pointer n = createTextNode(x);
            container_pointer c = createContainer();
            c->textNode = n;
            c->tail_textNode = n;

            doc->tail->next = c;
            c->prev = doc->tail;
            doc->tail = c;

            tree_pointer node = createTreeNode(index, &c);
            rbInsert(node);                                 // aggiungo il nodo anche all'albero

        }


    }
}
container_pointer removeToDocument(document_pointer* document, documentRemoved_pointer* hell, remove_pointer* hellNode, long treeIndexToRemove){

    document_pointer _document = *document;
    container_pointer del = (*hellNode)->textContainer;     // container da rimuovere


    if(del->prev != NULL){      // se non sto rimuovendo la testa del documento


        addToHell(hell, hellNode);

        // faccio il link tra i vicini del nodo rimosso
        del->prev->next = del->next;

        if(del->next == NULL) {     // se ho rimosso la coda del documento
            _document->tail = del->prev;
        } else{                             // altrimenti completo il link normalmente
            del->next->prev = del->prev;
        }


    } else{     // se sto rimuovendo la testa del documento


        addToHell(hell, hellNode);


        if(del->next == NULL){  // se ho cancellato l'unico nodo del documento

            _document->head = NULL;
            _document->tail = NULL;
        } else{

            del->next->prev = NULL;
            _document->head = del->next;
        }
    }


    // ora devo rimuovere il nodo anche dall'albero

    tree_pointer delNode = treeSearch(&tree_root, treeIndexToRemove);   // poichè è tipo una torre, cancello sempre lo stesso indice e gli altri cadono di 1 tramitr la pullTreeNodes
    rbDelete(delNode);

    pullTreeNodes(treeIndexToRemove+1);     // partendo dal nodo di indice successivo a quello cancellato, decremento le key di 1

    return del->next;
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

        rbDelete(treeSearchByContainer(&tree_root, &bulldozer));
        free(bulldozer);
        bulldozer = nextMiles;
    }
}
void cleanUpDocumentRemoved(documentRemoved_pointer* docRemoved){

    documentRemoved_pointer docRem = *docRemoved;
    remove_pointer rem = docRem->head;
    remove_pointer nextMiles = NULL;

    while (rem != NULL) {
        if (rem->next != NULL) {
            nextMiles = rem->next;
        } else nextMiles = NULL;

        ;
        //if(listSearch(document, &rem->textContainer) == NULL){  // se rem.textnode non è presente rimuovilo
        if(treeSearchByContainer(&tree_root, &rem->textContainer) == nil){
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

        if(c->tail_textNode == NULL){
            cleanUpDocumentFromPointer(&c);
            doc->head = NULL;
            doc->tail = NULL;
            return;
        }

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
void freeUnusedHellContainer(documentRemoved_pointer* hell){

    documentRemoved_pointer _hell = *hell;

    remove_pointer bulldozer = _hell->head;
    remove_pointer nextMiles = NULL;

    while (bulldozer != NULL) {
        if (bulldozer->next != NULL) {
            nextMiles = bulldozer->next;
        } else nextMiles = NULL;

        //free(bulldozer->command);
        cleanUpTextFromHead(&bulldozer->textContainer->textNode);
        free(bulldozer);
        bulldozer = nextMiles;
    }

    _hell->head = NULL;
    _hell->tail = NULL;

}
void freeUnusedHeavenContainer(documentRemoved_pointer* heaven){

    documentRemoved_pointer _heaven = *heaven;

    remove_pointer bulldozer = _heaven->tail;
    remove_pointer nextMiles = NULL;

    while (bulldozer != NULL) {
        if (bulldozer->prev != NULL) {
            nextMiles = bulldozer->prev;
        } else nextMiles = NULL;
        //free(bulldozer->command);
        cleanUpTextFromHead(&bulldozer->textContainer->textNode);
        //free(bulldozer->textContainer);
        free(bulldozer);
        bulldozer = nextMiles;
    }

    _heaven->head = NULL;
    _heaven->tail = NULL;

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

void addToHeaven(documentRemoved_pointer* heaven, container_pointer* c){

    documentRemoved_pointer fut = *heaven;
    container_pointer search = *c;

    if(search->tail_textNode == NULL){

        if(fut->head == NULL){
            remove_pointer newNode = createRemoveContainer();
            newNode->textContainer = search;

            fut->head = newNode;
            fut->tail = newNode;
        }
        else{
            remove_pointer newNode = createRemoveContainer();
            newNode->textContainer = search;

            fut->tail->next = newNode;
            newNode->prev = fut->tail;
            fut->tail = newNode;
        }

    }
}
void addToHell(documentRemoved_pointer* hell, remove_pointer* hellNode){

    documentRemoved_pointer _hell = *hell;

    if(_hell->head == NULL || _hell->tail == NULL){     // se è il primo nodo dei rimossi
        createHeadDocumentRemoved(hell, hellNode);

    } else{     // altrimenti lo aggiungo in coda

        appendNodeToDocumentRemoved(hell, hellNode);
    }

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

int undoChange(long num) {

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
int undoDelete(document_pointer* document, documentRemoved_pointer* oldNodes, documentRemoved_pointer* futureNodes, long index){

    // il nodo è stato eliminato, devo rimetterlo nel documento

    document_pointer doc = *document;
    documentRemoved_pointer hell = *oldNodes;

    remove_pointer r = hell->tail;


    if(r == NULL) return 0;

    while (r->command != tail_history->value){      // teoricamente è sempre la coda, ma confronta il comando con cui ho eliminato il nodo con quello della history

        if(r->prev == NULL){                    // voglio fare l'undo e ?
            return 0;
        }
        r = r->prev;
    }

    container_pointer c = r->textContainer;
    container_pointer search = doc->head;

    if(search == NULL || search->tail_textNode == NULL){     // se il documento è vuoto e quindi devo inserire la testa (compreso il caso in cui ci sono nodi oscurati)
        doc->head = c;
        doc->tail = c;

        while (search != NULL){     // vuol dire che ci sono solo nodi oscurati

            if(search->tail_textNode == NULL){                  // allora vanno cancellati tutti i vacchi nodi dall'albero

                documentRemoved_pointer fut = *futureNodes;

                addToHeaven(&fut, &search);                                 // aggiungo il nodo oscurato ad heaven
                rbDelete(treeSearchByContainer(&tree_root, &search));       // rimuovo il nodo oscurato dall'albero

                search = search->next;

            }
        }



    } else{                 // se il documento ha già dei nodi

        /*container_pointer search_prev = search;
        while (c->next != search){    // skippo avanti finchè non trovo il nodo a cui devo inserire in prev il nodo c,
                                            // quindi search è il nodo successivo. Per questo motivo mi serve search_prev nel caso che non ci sia il successivo
            if(search->next != NULL) {
                search_prev = search->next;
            }

            search = search->next;
        }*/
        /*
        tree_pointer searchTreeNode = treeSearch(&tree_root, index);
        tree_pointer searchTreeNode_prev = treePredecessor(&searchTreeNode);

        if(searchTreeNode != nil){
            search = searchTreeNode->value;

        } else{
            searchTreeNode_prev = treeSearch(&tree_root, index-1);
            search = NULL;
        }

        container_pointer search_prev;

        if(searchTreeNode_prev != nil){
            search_prev = searchTreeNode_prev->value;
        } else{
            search_prev = NULL;
        }
        */

        container_pointer search_prev = NULL;

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

    hell->tail = hell->tail->prev;  // porto indietro la coda poichè ho aggiunto quest'ultima al documento


    pushTreeNodes(index);       // incremento di 1 tutte le chiavi dei nodi da index in poi

    tree_pointer treeNode = createTreeNode(index, &c);
    rbInsert(treeNode);

    return 1;

}
void redoChange(long num, document_pointer* document, documentRemoved_pointer* heaven){

    // la restore di fut va messa qui

    if((*heaven)->tail != NULL){

        document_pointer _document = *document;
        documentRemoved_pointer _heaven = *heaven;

        if(_document->head == NULL){     // se il documento è vuoto

            container_pointer textNode = _heaven->head->textContainer;
            textNode->tail_textNode = textNode->textNode;

            _document->head = textNode;                                      // metto il nodo come primo e ultimo elemento della linked list
            _document->tail = textNode;

            tree_pointer node = createTreeNode(num, &textNode);
            rbInsert(node);                                     // aggiungo il nodo anche all'albero


        }
        else{

            tree_pointer treeNode = treeSearch(&tree_root, num);


            if(treeNode != nil){

                container_pointer container = treeNode->value;

                if(container->tail_textNode == NULL){
                    container->tail_textNode = container->textNode;
                } else{

                    container->tail_textNode = container->tail_textNode->next;
                }

                return;


            }
            else{
                //inserisci nuovo nodo in coda al documento

                container_pointer textNode = _heaven->head->textContainer;
                textNode->tail_textNode = textNode->textNode;

                _document->tail->next = textNode;
                textNode->prev = _document->tail;
                _document->tail = textNode;

                tree_pointer node = createTreeNode(num, &textNode);
                rbInsert(node);                                 // aggiungo il nodo anche all'albero

            }

        }
        remove_pointer prev = _heaven->head->next;
        free(_heaven->head);
        _heaven->head = prev;

    }

    else{

        tree_pointer treeNode = treeSearch(&tree_root, num);
        container_pointer c = treeNode->value;

        if(c->tail_textNode != NULL){
            c->tail_textNode = c->tail_textNode->next;
        } else c->tail_textNode = c->textNode;


    }




}
int redoDelete(document_pointer* document, documentRemoved_pointer* hell, long treeIndexToRemove){

    // devo tornare a cancellare i nodi che l'undo ha rimesso

    document_pointer _document = *document;
    documentRemoved_pointer _hell = *hell;

    remove_pointer r;


    r = _hell->tail;


    if(r == NULL){      // ci finisce solo se fut è vuota
        r = _hell->head;
    }

    while (r->command != tail_history->next->value){

        if(r->next == NULL){                    // voglio fare la redo e ?
            return 0;
        }
        r = r->next;
    }

    container_pointer c = r->textContainer;  // c è il nodo da rimuovere nel documento

    // teoricamente non serve fare sta cosa con il search perchè da c sò già chi lo procede e chi lo segue
    /*container_pointer search = _document->head; // search mi serve per sapere chi lo procede e chi lo segue

    container_pointer search_prev = search;
    while (c->next != search){    // skippo avanti finchè non trovo il nodo a cui devo rimuovere in prev il nodo c,
        // quindi search è il nodo successivo. Per questo motivo mi serve search_prev nel caso che non ci sia il successivo
        if(search != NULL) {
            search_prev = search;
        }

        search = search->next;
    }*/


    container_pointer search = c->next;
    container_pointer search_prev = c->next->prev;


    if(search->next == NULL && search->prev == NULL){       // se il documento contiene un solo nodo
        _document->head = NULL;
        _document->tail = NULL;
    }
    else{

        if(search_prev->prev == NULL){                   // se sto eliminando la testa del documento
            search_prev->next->prev = NULL;
            _document->head = search_prev->next;
        }
        else{

            if(search_prev->next == NULL){               // se sto eliminando la coda del documento
                search_prev->prev->next = NULL;
                _document->tail = search_prev->prev;
            }
            else{                                   // se sto eliminando un nodo nel mezzo
                search_prev->next->prev = search_prev->prev;
                search_prev->prev->next = search_prev->next;
            }

        }

    }

    if(_hell->tail == NULL){
        _hell->tail = _hell->head;
    } else{
        _hell->tail = _hell->tail->next;  // porto avanti la coda poichè ho rimosso quest'ultima al documento
    }


    /*tree_pointer searchNode = treeSearch(&tree_root, treeIndexToRemove);
    while(searchNode == nil){
        treeIndexToRemove++;
        searchNode = treeSearch(&tree_root, treeIndexToRemove);
    }

    tree_pointer treeNode = treeSearch(&tree_root, treeIndexToRemove);
    rbDelete(treeNode);*/

    tree_pointer delNode = treeSearch(&tree_root, treeIndexToRemove);   // poichè è tipo una torre, cancello sempre lo stesso indice e gli altri cadono di 1 tramitr la pullTreeNodes
    rbDelete(delNode);

    pullTreeNodes(treeIndexToRemove + 1);     // partendo dal nodo di indice successivo a quello cancellato, decremento le key di 1


    return 1;

}


// HELPER
char* getRow(){
    
    row = NULL;

    char* tmp = (char *)malloc(sizeof(char) * ROW_LEN);

    char* res = fgets(tmp, ROW_LEN, stdin);     // fgets gets '\n' at the end of the string

    if (res == NULL) {
        printf("Failed to read input.\n");
        return 0;
    }

    unsigned long len = strlen(tmp);

    row = (char *)malloc(sizeof(char) * len);

    strncpy(row, tmp, len);
    strtok(row, "\n");

    free(tmp);


    return row;
}
void getBounds(char* line, long *ind1, long *ind2){

    long *ret = (long *)malloc(sizeof(long));

    *ret = strtol(line, &line, 10);
    line++;
    *ind1 = *ret;
    *ret = strtol(line, &line, 10);
    *ind2 = *ret;

    free(ret);

}

