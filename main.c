#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROW_LEN 1024

typedef struct TextNode* text_pointer;
typedef struct HistoryNode* history_pointer;
typedef struct Document* document_pointer;


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

struct Document {
    text_pointer head;
    text_pointer tail;
};

history_pointer head_history;
history_pointer tail_history;


char* row;

document_pointer createDocument();
void cleanUpDocument(document_pointer* document);

void addToDocument(document_pointer* document,  document_pointer* rowRemoved, char *x, long index);
void removeToDocument(document_pointer* document, text_pointer* headDel);
text_pointer skip(document_pointer* document, long skip);
text_pointer createTextNode(char *x);

void addToRemoved(document_pointer* rowRemoved, char *x);

// HELPER
char* getRow();
void getBounds(long *ind1, long *ind2);

// DOUBLE LINKED LIST
history_pointer createHistoryNode(char *x);
void addInHistory();
void freeUnusedHistoryNode();
void cleanUpHistoryFromHead(history_pointer head);




int main() {

    document_pointer document = createDocument();
    document_pointer rowRemoved = createDocument();

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

                addToDocument(&document, &rowRemoved, row, key);
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

            text_pointer headDel = skip(&document, ind1);

            for (int i = 0; i < numRow; i++) {

                removeToDocument(&document, &headDel);

            }

        }
        else if (strstr(row, "p") != NULL) {

            long ind1, ind2;
            getBounds(&ind1, &ind2);

            free(row);

            long numRow = ind2-ind1+1;


            text_pointer headPrint = skip(&document, ind1);


            for (int i = 0; i < numRow; i++) {

                if(headPrint != NULL){
                    printf("%s\n", headPrint->value);
                    headPrint = headPrint->next;
                } else{
                    printf(".\n");
                }

            }

        }
        

        getRow();

    }

    cleanUpDocument(&document);
    cleanUpDocument(&rowRemoved);
    cleanUpHistoryFromHead(head_history);


    return 0;
}


document_pointer createDocument(){

    document_pointer doc = malloc(sizeof(struct Document));
    doc->head = NULL;
    doc->tail = NULL;

    return doc;
}
void cleanUpDocument(document_pointer* document){

    text_pointer bulldozer = (*document)->head;
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

void addToDocument(document_pointer* document, document_pointer* rowRemoved, char *x, long index){

    document_pointer doc = *document;

    if(doc->head == NULL){
        text_pointer n = createTextNode(x);
        doc->head = n;
        doc->tail = n;
    } else{

        text_pointer y = skip(document, index);

        if(y != NULL){
            addToRemoved(rowRemoved, y->value);
            y->value = x;
        }
        else{
            text_pointer n = createTextNode(x);

            doc->tail->next = n;
            n->prev = doc->tail;
            doc->tail = n;
        }


    }
}
void removeToDocument(document_pointer* document, text_pointer* headDel){

    if(*headDel == NULL){
        return;
    }

    document_pointer doc = *document;
    text_pointer del = *headDel;


    if(del->prev != NULL){
        del->prev->next = del->next;

        if(del->next != NULL) {
            del->next->prev = del->prev;
        } else{
            doc->tail = del->prev;
        }

        del = del->next;


    } else{

        doc->head = del->next;
        del->next->prev = doc->head;
        doc->head->prev = NULL;

        del = del->next;
    }

    *headDel = del;
}

text_pointer skip(document_pointer* document, long skip){

    document_pointer doc = *document;

    text_pointer ret = doc->head;

    for (int i = 1; i < skip; ++i) {

        if(ret != NULL){
            ret = ret->next;
        }

    }

    return ret;
}
text_pointer createTextNode(char *x){

    text_pointer newNode = (text_pointer)malloc(sizeof(struct TextNode));

    newNode->value = x;
    newNode->prev = NULL;
    newNode->next = NULL;

    return newNode;
}

void addToRemoved(document_pointer* rowRemoved, char *x){

    document_pointer doc = *rowRemoved;

    if(doc->head == NULL){
        text_pointer n = createTextNode(x);
        doc->head = n;
        doc->tail = n;
    } else{

        text_pointer n = createTextNode(x);

        doc->tail->next = n;
        n->prev = doc->tail;
        doc->tail = n;

    }
}


// HELPER
char* getRow(){
    
    row = NULL;

    char* tmp = malloc(sizeof(char) * ROW_LEN);

    fgets(tmp, ROW_LEN, stdin);     // fgets gets '\n' at the end of the string
    strtok(tmp, "\n");

    unsigned long len = strlen(tmp);

    row = malloc(sizeof(char) * (len+1));

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
