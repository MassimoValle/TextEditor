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
    history_pointer tail;

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

document_pointer createDocument();
documentRemoved_pointer createDocumentRemoved();
container_pointer createContainer();
text_pointer createTextNode(char *x);
history_pointer createHistoryNode(char *x);
remove_pointer createRemoveContainer();

void addInHistory();
void addToDocument(document_pointer* document, char *x, long index);
void removeToDocument(document_pointer* document, documentRemoved_pointer* rowRemoved, container_pointer* headDel);

container_pointer skip(document_pointer* document, long skip);

void cleanUpDocument(document_pointer* document);
void cleanUpDocumentRemoved(documentRemoved_pointer* document);
void cleanUpTextFromHead(text_pointer* head);
void freeUnusedHistoryNode();
void cleanUpHistoryFromHead(history_pointer* head);


// HELPER
char* getRow();
void getBounds(long *ind1, long *ind2);




int main() {

    document_pointer document = createDocument();
    documentRemoved_pointer rowRemoved = createDocumentRemoved();

    row = getRow();

    while (strstr(row, "q") == NULL){


        if (strstr(row, "c") != NULL) {

            freeUnusedHistoryNode();
            addInHistory(row);

            long ind1 = 0, ind2 = 0;

            getBounds(&ind1, &ind2);

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

            freeUnusedHistoryNode();
            addInHistory(row);

            long ind1, ind2;
            getBounds(&ind1, &ind2);

            long numRow = ind2-ind1+1;

            container_pointer headDel = skip(&document, ind1);

            for (int i = 0; i < numRow; i++) {

                removeToDocument(&document, &rowRemoved, &headDel);

            }

        }
        else if (strstr(row, "p") != NULL) {

            long ind1, ind2;
            getBounds(&ind1, &ind2);


            long numRow = ind2-ind1+1;


            container_pointer headPrint = skip(&document, ind1);


            for (int i = 0; i < numRow; i++) {

                if(headPrint != NULL){
                    printf("%s\n", headPrint->tail_textNode->value);
                    headPrint = headPrint->next;
                } else{
                    printf(".\n");
                }

            }

        }
        

        getRow();

    }

    cleanUpDocument(&document);
    cleanUpDocumentRemoved(&rowRemoved);
    cleanUpHistoryFromHead(&head_history);


    return 0;
}


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
    newNode->tail = NULL;

    return newNode;
}
remove_pointer createRemoveContainer(){

    remove_pointer newNode = malloc(sizeof(struct RemoveContainer));

    newNode->textContainer = NULL;
    newNode->prev = NULL;
    newNode->next = NULL;

    return newNode;

}

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

    if(doc->head == NULL){
        text_pointer n = createTextNode(x);
        container_pointer c = createContainer();
        c->textNode = n;
        c->tail_textNode = n;
        doc->head = c;
        doc->tail = c;
    } else{

        container_pointer y = skip(document, index);

        if(y != NULL){
            text_pointer n = createTextNode(x);
            y->tail_textNode->next = n;
            n->prev = y->tail_textNode;
            y->tail_textNode = n;
        }
        else{

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
void removeToDocument(document_pointer* document, documentRemoved_pointer* rowRemoved, container_pointer* headDel){

    if(*headDel == NULL){
        return;
    }

    document_pointer doc = *document;
    documentRemoved_pointer docRem = *rowRemoved;
    container_pointer del = *headDel;


    if(del->prev != NULL){

        remove_pointer c = createRemoveContainer();
        c->textContainer = del;

        if(docRem->head == NULL){
            docRem->head = c;
            docRem->tail = c;
        } else{
            docRem->tail->next = c;
            c->prev = docRem->tail;
            docRem->tail = c;
        }


        del->prev->next = del->next;

        if(del->next != NULL) {
            del->next->prev = del->prev;
        } else{
            doc->tail = del->prev;
        }

        del = del->next;


    } else{

        remove_pointer c = createRemoveContainer();
        c->textContainer = del;

        if(docRem->head == NULL){
            docRem->head = c;
            docRem->tail = c;
        } else{
            docRem->tail->next = c;
            c->prev = docRem->tail;
            docRem->tail = c;
        }


        doc->head = del->next;

        if(doc->head != NULL){
            doc->head->prev = NULL;
        } else{
            doc->tail = NULL;
        }

        del = del->next;
    }

    *headDel = del;
}

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

void cleanUpDocument(document_pointer* document){

    container_pointer bulldozer = (*document)->head;
    container_pointer nextMiles = NULL;

    while (bulldozer != NULL) {
        if (bulldozer->next != NULL) {
            nextMiles = bulldozer->next;
        } else nextMiles = NULL;

        cleanUpTextFromHead(bulldozer->textNode);
        free(bulldozer);
        bulldozer = nextMiles;
    }
}
void cleanUpDocumentRemoved(documentRemoved_pointer* document){

    remove_pointer r = (*document)->head;
    remove_pointer nextCont = NULL;
    container_pointer bulldozer = r->textContainer;
    container_pointer nextMiles = NULL;

    while (r != NULL){

        while (bulldozer != NULL) {
            if (bulldozer->next != NULL) {
                nextMiles = bulldozer->next;
            } else nextMiles = NULL;

            cleanUpTextFromHead(bulldozer->textNode);
            free(bulldozer);
            bulldozer = nextMiles;
        }

        cleanUpTextFromHead(bulldozer->textNode);
        free(bulldozer);
        bulldozer = nextMiles;


        //------

        if (r->next != NULL) {
            nextCont = r->next;
        } else nextCont = NULL;

        r = nextCont;

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
void freeUnusedHistoryNode(){

    if(tail_history != NULL && tail_history->next != NULL){

        history_pointer bulldozer = tail_history->next;
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


// HELPER
char* getRow(){
    
    row = NULL;

    char* tmp = malloc(sizeof(char) * ROW_LEN);

    fgets(tmp, ROW_LEN, stdin);     // fgets gets '\n' at the end of the string
    strtok(tmp, "\n");

    unsigned long len = strlen(tmp);

    row = malloc(sizeof(char) * len);

    strncpy(row, tmp, len);

    free(tmp);


    return row;
}
void getBounds(long *ind1, long *ind2){

    long *ret = malloc(sizeof(long));

    *ret = strtol(row, &row, 10);
    row++;
    *ind1 = *ret;
    *ret = strtol(row, &row, 10);
    *ind2 = *ret;

    free(ret);

}

