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


struct HistoryNode {
    char* value;
    history_pointer next;
    history_pointer prev;

};  // used for commands history

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

history_pointer head_history;
history_pointer tail_history;


char* row;

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
void cleanUpTextFromHead(text_pointer* head);
void cleanUpHistoryFromHead(history_pointer* head);
void freeUnusedContainerNode(document_pointer* document);
void freeUnusedHistoryNode();

// DB HELPER
container_pointer skip(document_pointer* document, long skip);
void createHeadDocumentRemoved(documentRemoved_pointer* docRem, remove_pointer* c);
void appendNodeToDocumentRemoved(documentRemoved_pointer* docRem, remove_pointer* c);
int undoChange(document_pointer* document, long num);
int undoDelete(document_pointer* document, documentRemoved_pointer* docRem);
void redoChange(document_pointer* document, long num);
int redoDelete(document_pointer* doc, documentRemoved_pointer* docRem);


// HELPER
char* getRow();
void getBounds(char* line, long *ind1, long *ind2);




int main() {

    document_pointer document = createDocument();
    documentRemoved_pointer rowRemoved = createDocumentRemoved();

    long undo = 0;

    row = getRow();

    while (strstr(row, "q") == NULL){


        if (strstr(row, "c") != NULL) {

            if(undo > 0){
                freeUnusedHistoryNode();
                freeUnusedContainerNode(&document);
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
                freeUnusedHistoryNode();
                freeUnusedContainerNode(&document);
            }

            undo = 0;
            addInHistory(row);

            long ind1, ind2;
            getBounds(row, &ind1, &ind2);

            long numRow = ind2-ind1+1;

            container_pointer headDel = skip(&document, ind1);

            for (int i = 0; i < numRow; i++) {

                removeToDocument(&document, &rowRemoved, &headDel, tail_history->value);

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


            if(ind1 == 0){
                printf(".\n");
                getRow();
                continue;
            }
            long numRow = ind2-ind1+1;


            container_pointer headPrint = skip(&document, ind1);


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

                        int val = undoDelete(&document, &rowRemoved);

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

                for (int i = 0; i < ret; ++i) {
                    long ind1, ind2;

                    getBounds(tail_history->next->value, &ind1, &ind2);

                    long numRow = ind2-ind1+1;

                    for (int j = 0; j < numRow; ++j) {

                        if(tail_history->next->value[3] == 'c'){

                            redoChange(&document, ind1+j);

                        } else{

                            redoDelete(&document, &rowRemoved);

                        }

                    }

                    tail_history = tail_history->next;
                }

                undo -= ret;

            }



        }
        

        getRow();

    }

    cleanUpDocumentFromPointer(&document->head);
    cleanUpHistoryFromHead(&head_history);

    free(document);
    free(rowRemoved);


    return 0;
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
    }
    else{

        container_pointer y = skip(document, index);

        if(y != NULL){  // cambia il valore in coda a textNode
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

            if(c->tail_textNode == NULL){
                cleanUpTextFromHead(&c->textNode);
                doc->head = c->next;

                if(c->next == NULL){
                    doc->tail = NULL;
                }

                free(c);
            }

            c = c_next;


        }
    }
}
void freeUnusedHistoryNode(){

    if(tail_history != NULL && tail_history->next != NULL){

        cleanUpHistoryFromHead(&tail_history->next);

    } else cleanUpHistoryFromHead(&head_history);

}

// DB HELPER
container_pointer skip(document_pointer* document, long skip){

    document_pointer doc = *document;

    container_pointer ret = doc->head;

    for (int i = 1; i < skip; ++i) {

        if(ret != NULL){
            ret = ret->next;
        } else{
            ret = NULL;
        }

    }

    return ret;
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

    container_pointer c = skip(document, num);

    if (c->tail_textNode == NULL) {     // se annullo l'istruzione che ha creato quel nodo
        return 0;
    } else {
        c->tail_textNode = c->tail_textNode->prev;
        return 1;
    }
}
int undoDelete(document_pointer* document, documentRemoved_pointer* docRem){

    // il nodo è stato eliminato, devo rimetterlo nel documento

    document_pointer doc = *document;
    documentRemoved_pointer rowRemoved = *docRem;

    remove_pointer r = rowRemoved->tail;



    while (r->command != tail_history->value){

        if(r->prev == NULL){                    // voglio fare l'undo e ?
            return 0;
        }
        r = r->prev;
    }

    container_pointer c = r->textContainer;
    container_pointer search = doc->head;

    if(search == NULL){     // se il documento è vuoto e quindi devo inserire la testa
        doc->head = c;
        doc->tail = c;

    } else{                 // se il documento ha già dei nodi

        container_pointer search_prev = NULL;
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
    return 1;

}
void redoChange(document_pointer* document, long num){

    container_pointer c = skip(document, num);

    c->tail_textNode = c->tail_textNode->next;

}
int redoDelete(document_pointer* doc, documentRemoved_pointer* docRem){

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

