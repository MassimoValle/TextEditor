#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROW_LEN 1024

typedef struct TextNode* text_pointer;
typedef struct TextNodeContainer* container_pointer;
typedef struct RemoveContainer* remove_pointer;
typedef struct HistoryNode* history_pointer;
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
};     // contains text_pointer
struct RemoveContainer {
    char* command;
    container_pointer textContainer;
    remove_pointer next;
    remove_pointer prev;
};       // contains command string and container_pointer
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
tree_pointer treeMinimum(tree_pointer* x);

tree_pointer treeSearch(tree_pointer* x, long k);

void cleanUpTree(tree_pointer* x);

void pushTreeNodes(long index);
void pullTreeNodes(long index);



// CREATE NODES
documentRemoved_pointer createDocumentRemoved();
container_pointer createContainer();
text_pointer createTextNode(char *x);
history_pointer createHistoryNode(char *x);
remove_pointer createRemoveContainer();

// DB MANAGING
void addInHistory();
void addToDocument(char *x, long index);
void removeToDocument(documentRemoved_pointer* hell, remove_pointer* hellNode, long treeIndexToRemove);
int undoChange(long num);
int undoDelete(documentRemoved_pointer* hell, documentRemoved_pointer* heaven, long index, long j);
void redoChange(long num, documentRemoved_pointer* heaven);
int redoDelete(documentRemoved_pointer* hell, long treeIndexToRemove);

// CLEAN NODES
void cleanUpHell(documentRemoved_pointer* hell);
void cleanUpHeaven(documentRemoved_pointer* heaven);
void cleanUpTextFromHead(text_pointer* head);
void cleanUpHistoryFromHead(history_pointer* head);
void freeUnusedHellContainer(documentRemoved_pointer* hell);
void freeUnusedHeavenContainer(documentRemoved_pointer* heaven);
void freeUnusedHistoryNode();

// DB HELPER
void addToHeaven(documentRemoved_pointer* heaven, container_pointer* c);
void addToHell(documentRemoved_pointer* hell, remove_pointer* hellNode);
void addFormHeaven(documentRemoved_pointer* heaven, long num);
void createHeadDocumentRemoved(documentRemoved_pointer* docRem, remove_pointer* c);
void appendNodeToDocumentRemoved(documentRemoved_pointer* docRem, remove_pointer* c);

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

    documentRemoved_pointer hell = createDocumentRemoved();     // nodi rimossi dalla delete
    documentRemoved_pointer heaven = createDocumentRemoved();   // nodi rimossi dall'undo

    long undo = 0;      // indica di quante istruzioni si è fatta la undo

    row = getRow();

    while (strstr(row, "q") == NULL){


        if (strstr(row, "c") != NULL) {     // modifica o inserimento di righe

            if(undo > 0){       // mi serve per la undo/redo

                freeUnusedHistoryNode();
                freeUnusedHellContainer(&hell);
                freeUnusedHeavenContainer(&heaven);

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

                addToDocument(row, key);     // aggiungo all'albero al la stringa(row) all'indice key
            }

            getRow();

            if(strstr(row, ".") != NULL){
                free(row);
            } else printf("something went wrong\n");

        }
        else if (strstr(row, "d") != NULL) {         // elimina righe se presenti nel documento

            if(undo > 0){       // mi serve per la undo/redo

                freeUnusedHistoryNode();
                freeUnusedHellContainer(&hell);
                freeUnusedHeavenContainer(&heaven);

                undo = 0;
            }


            addInHistory();             // aggiungo il comando alla cronologia

            long ind1, ind2;
            getBounds(row, &ind1, &ind2);

            long numRow = ind2-ind1+1;

            for (int i = 0; i < numRow; i++) {

                tree_pointer treeNode = treeSearch(&tree_root, ind1);       // cerco il primo nodo da eliminare

                container_pointer listNodeToDelete;

                if(treeNode != nil){                // se esiste allora lo prendo da treeNode
                    listNodeToDelete = treeNode->value;
                } else continue;                    // se non esiste il nodo


                remove_pointer hellNode = createRemoveContainer();
                hellNode->command = tail_history->value;
                hellNode->textContainer = listNodeToDelete;

                removeToDocument(&hell, &hellNode, ind1);   // rimuovo dal documento il nodo(listNodeToDelete) e lo metto in hell

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
            tree_pointer treeNode_next = treeNode;

            container_pointer headPrint;

            for (int i = 0; i < numRow; i++) {

                if(treeNode != nil){
                    headPrint = treeNode->value;
                    treeNode_next = treeSuccessor(&treeNode);
                } else headPrint = NULL;


                if(headPrint != NULL && headPrint->tail_textNode != NULL){
                    printf("%s\n", headPrint->tail_textNode->value);
                } else{
                    printf(".\n");
                }

                treeNode = treeNode_next;

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
                    undo--;
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

                        int val = undoDelete(&hell, &heaven, ind1, j);

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

                            redoChange(ind1+j, &heaven);

                        } else{

                            redoDelete(&hell, ind1);

                        }

                    }

                    if(tail_history != NULL){
                        tail_history = tail_history->next;
                    } else tail_history = head_history;
                }

                undo -= ret;

            }
            else{
                free(row);  // è una redo a vuoto (senza che prima ci sia un'undo), quindi la ignoro
            }



        }
        

        getRow();

    }


    cleanUpHell(&hell);
    cleanUpHeaven(&heaven);

    cleanUpTree(&tree_root);
    cleanUpHistoryFromHead(&head_history);



    free(row);
    free(nil);
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

    tree_pointer tmp = (*x);
    while (tmp->left != nil){
        tmp = tmp->left;
    }
    return tmp;
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

void pushTreeNodes(long index){

    tree_pointer searchNode = treeSearch(&tree_root, index);
    tree_pointer searchNode_next = searchNode;

    while(searchNode != nil){

        searchNode_next = treeSuccessor(&searchNode_next);

        searchNode->key++;

        searchNode = searchNode_next;

    }


}
void pullTreeNodes(long index){

    tree_pointer searchNode = treeSearch(&tree_root, index);
    tree_pointer searchNode_next = searchNode;

    while(searchNode != nil){

        searchNode_next = treeSuccessor(&searchNode_next);

        searchNode->key--;

        searchNode = searchNode_next;

    }

}



// CREATE NODES
documentRemoved_pointer createDocumentRemoved(){

    documentRemoved_pointer doc = (documentRemoved_pointer)malloc(sizeof(struct DocumentRemoved));
    doc->head = NULL;
    doc->tail = NULL;

    return doc;
}
container_pointer createContainer(){

    container_pointer newNode = (container_pointer)malloc(sizeof(struct TextNodeContainer));

    newNode->textNode = NULL;

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
void addToDocument(char *x, long index){

    tree_pointer  treeNode = treeSearch(&tree_root, index);     // cerco se esiste già una stringa a quella riga
    container_pointer y = treeNode->value;

    if(treeNode != nil && y != NULL){           // se c'è, aggiungo il valore in coda a textNode
        text_pointer n = createTextNode(x);
        y->tail_textNode->next = n;
        n->prev = y->tail_textNode;
        y->tail_textNode = n;
    }
    else{                                       // se non c'è, aggiungo il nodo all'albero

        text_pointer n = createTextNode(x);
        container_pointer c = createContainer();
        c->textNode = n;
        c->tail_textNode = n;

        tree_pointer node = createTreeNode(index, &c);
        rbInsert(node);

    }


}
void removeToDocument(documentRemoved_pointer* hell, remove_pointer* hellNode, long treeIndexToRemove){

    addToHell(hell, hellNode);



    // ora devo rimuovere il nodo anche dall'albero

    tree_pointer delNode = treeSearch(&tree_root, treeIndexToRemove);   // poichè è tipo una torre, cancello sempre lo stesso indice e gli altri cadono di 1 tramitr la pullTreeNodes
    rbDelete(delNode);

    pullTreeNodes(treeIndexToRemove+1);     // partendo dal nodo di indice successivo a quello cancellato, decremento le key di 1

}
int undoChange(long num) {

    tree_pointer treeNode = treeSearch(&tree_root, num);
    container_pointer c = treeNode->value;

    if (c->tail_textNode == NULL) {     // se annullo l'istruzione che ha creato quel nodo
        return 0;
    } else {
        c->tail_textNode = c->tail_textNode->prev;
        return 1;
    }
}
int undoDelete(documentRemoved_pointer* hell, documentRemoved_pointer* heaven, long index, long j){

    // il nodo è stato eliminato, devo rimetterlo nel documento

    documentRemoved_pointer _hell = *hell;

    remove_pointer r;


    if(_hell->tail == NULL) {
        r = _hell->head;
    } else{
        r = _hell->tail;
    }


    if( r->command != tail_history->value ){
        return 0;
    }



    container_pointer c = r->textContainer;
    tree_pointer treeNodeSearch = treeSearch(&tree_root, index+j);

    if(treeNodeSearch != nil){

        container_pointer search = treeNodeSearch->value;

        if(search->tail_textNode == NULL){     // se il documento è vuoto e quindi devo inserire la testa (compreso il caso in cui ci sono nodi oscurati)
                                                // allora vanno cancellati tutti i vacchi nodi dall'albero
            documentRemoved_pointer _heaven = *heaven;

            addToHeaven(&_heaven, &search);                                 // aggiungo il nodo oscurato ad heaven
            rbDelete(treeSearchByContainer(&tree_root, &search));       // rimuovo il nodo oscurato dall'albero

        }

    }


    if(_hell->tail != NULL){
        _hell->tail = _hell->tail->prev;  // porto indietro la coda poichè ho aggiunto quest'ultima al documento
    } else _hell->tail = _hell->head;


    pushTreeNodes(index);       // incremento di 1 tutte le chiavi dei nodi da index in poi

    tree_pointer treeNode = createTreeNode(index, &c);
    rbInsert(treeNode);

    return 1;

}
void redoChange(long num, documentRemoved_pointer* heaven){

    if(tree_root->value == NULL){     // se il documento è vuoto

        addFormHeaven(heaven, num);

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

            addFormHeaven(heaven, num);
        }

    }

}
int redoDelete(documentRemoved_pointer* hell, long treeIndexToRemove){

    // devo tornare a cancellare i nodi che l'undo ha rimesso

    documentRemoved_pointer _hell = *hell;

    remove_pointer r;

    if(_hell->tail == NULL){      // ci finisce solo se tail è null
        r = _hell->head;
    } else{
        r = _hell->tail->next;
    }

    if( r->command != tail_history->next->value ){
        return 0;
    }


    tree_pointer delNode = treeSearch(&tree_root, treeIndexToRemove);   // poichè è tipo una torre, cancello sempre lo stesso indice e gli altri cadono di 1 tramitr la pullTreeNodes

    if(delNode != nil){     // caso in cui rifaccio la delete di un nodo che non esiste

        // porto avanti la coda poichè ho rimosso quest'ultima al documento
        if(_hell->tail == NULL){
            _hell->tail = _hell->head;
        } else{
            _hell->tail = _hell->tail->next;
        }

        rbDelete(delNode);
        pullTreeNodes(treeIndexToRemove + 1);     // partendo dal nodo di indice successivo a quello cancellato, decremento le key di 1

    }

    return 1;

}

// CLEAN NODES
void cleanUpTree(tree_pointer* x){

    if(*x != nil){
        cleanUpTree(&(*x)->left);
        cleanUpTree(&(*x)->right);

        if((*x)->value->textNode != NULL){
            cleanUpTextFromHead(&(*x)->value->textNode);
            (*x)->value->textNode = NULL;
        }

        if((*x)->value != NULL){

            free((*x)->value);
            (*x)->value = NULL;

        }


        free(*x);


    }
}
void cleanUpHell(documentRemoved_pointer* hell){

    documentRemoved_pointer _hell = *hell;
    remove_pointer rem = _hell->head;
    remove_pointer nextMiles = NULL;

    while (rem != NULL) {
        if (rem->next != NULL) {
            nextMiles = rem->next;
        } else nextMiles = NULL;


        if(rem->textContainer != NULL){

            if(rem->textContainer->textNode != NULL){

                cleanUpTextFromHead(&rem->textContainer->textNode);
                rem->textContainer->textNode = NULL;

            }

            if( treeSearchByContainer(&tree_root, &rem->textContainer) == nil ){

                free(rem->textContainer);
                rem->textContainer = NULL;
            }


        }

        free(rem);
        rem = nextMiles;
    }

    _hell->head = NULL;
    _hell->tail = NULL;

}
void cleanUpHeaven(documentRemoved_pointer* heaven){

    documentRemoved_pointer _heaven = *heaven;
    remove_pointer rem = _heaven->tail;
    remove_pointer nextMiles = NULL;

    while (rem != NULL) {

        if(rem->textContainer != NULL)
        {
            if (rem->prev != NULL) {
                nextMiles = rem->prev;
            } else nextMiles = NULL;


            if(rem->textContainer != NULL){

                /*if(rem->textContainer->textNode != NULL){

                    cleanUpTextFromHead(&rem->textContainer->textNode);
                    rem->textContainer->textNode = NULL;

                }*/

                //free(rem->textContainer);
                rem->textContainer = NULL;

            }


            free(rem);
            rem = nextMiles;
        }

    }

    _heaven->head = NULL;
    _heaven->tail = NULL;

}
void cleanUpTextFromHead(text_pointer* head){

    text_pointer bulldozer = *head;
    text_pointer nextMiles = NULL;

    while (bulldozer != NULL) {
        if (bulldozer->next != NULL) {
            nextMiles = bulldozer->next;
        } else nextMiles = NULL;

        if(bulldozer->value != NULL){

            free(bulldozer->value);
            bulldozer->value = NULL;

        }

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

        if(bulldozer->value != NULL){

            free(bulldozer->value);
            bulldozer->value = NULL;

        }

        free(bulldozer);
        bulldozer = nextMiles;
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
        if(bulldozer->textContainer->textNode != NULL){

            cleanUpTextFromHead(&bulldozer->textContainer->textNode);
            bulldozer->textContainer->textNode = NULL;

        }


        //free(bulldozer->textContainer);
        bulldozer->textContainer = NULL;

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
        if(bulldozer->textContainer->textNode != NULL) {

            cleanUpTextFromHead(&bulldozer->textContainer->textNode);
            bulldozer->textContainer->textNode = NULL;

        }

        //free(bulldozer->textContainer);
        bulldozer->textContainer = NULL;

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
void addFormHeaven(documentRemoved_pointer* heaven, long num){

    documentRemoved_pointer _heaven = *heaven;

    container_pointer textNode = _heaven->head->textContainer;
    textNode->tail_textNode = textNode->textNode;

    tree_pointer node = createTreeNode(num, &textNode);
    rbInsert(node);                                     // aggiungo il nodo anche all'albero

    remove_pointer next = _heaven->head->next;
    //free(_heaven->head);
    _heaven->head = next;

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