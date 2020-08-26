#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROW_LEN 1024
#define ARRAY_LEN 500000

typedef struct TextNode* text_pointer;
typedef struct TextNodeContainer* container_pointer;
typedef struct HistoryNode* history_pointer;


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
    long index;
    char* command;
    text_pointer head_textNode;
    text_pointer tail_textNode;
};     // contains text_pointer


// GLOBAL VAR HISTORY
history_pointer head_history;
history_pointer tail_history;



container_pointer document[ARRAY_LEN];
container_pointer hell[ARRAY_LEN];
container_pointer heaven[ARRAY_LEN];

long nodeInHell = 0;
long nodeInHeaven = 0;
char* row;





// CREATE NODES
container_pointer createContainer();
text_pointer createTextNode(char *x);
history_pointer createHistoryNode(char *x);

// DB MANAGING
void addInHistory();
void addToDocument(char *x, long index);
void removeToDocument(long startIndex, long howMany, char* command);
int undoChange(long num);
int undoDelete(long numRow);
void redoChange(long num);
int redoDelete(long treeIndexToRemove, long howMany);

// HELPER
char* getRow();
void getBounds(char* line, long *ind1, long *ind2);

void freeDocument();
void freeHell();
void freeHeaven();


void cleanUpDocumentTextNode();
void cleanUpDocumentContainer();
void cleanUpTextFromHead(text_pointer* head);
void cleanUpHistoryFromHead(history_pointer* head);


int main() {

    for (int i = 0; i < ARRAY_LEN; ++i) {
        document[i] = createContainer();
        hell[i] = createContainer();
        heaven[i] = createContainer();
    }

    long undo = 0;      // indica di quante istruzioni si è fatta la undo

    row = getRow();

    while (strstr(row, "q") == NULL){


        if (strstr(row, "c") != NULL) {     // modifica o inserimento di righe

            if(undo > 0){       // mi serve per la undo/redo

                freeDocument();
                //freeHell();
                freeHeaven();

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

                freeDocument();
                //freeHell();
                freeHeaven();

                undo = 0;
            }


            addInHistory();             // aggiungo il comando alla cronologia

            long ind1, ind2;
            getBounds(row, &ind1, &ind2);

            long numRow = ind2-ind1+1;

            removeToDocument(ind1, numRow, tail_history->value);   // rimuovo dal documento il nodo(listNodeToDelete) e lo metto in hell


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



            for (int i = 0; i < numRow; i++) {

                text_pointer text = (document[ind1+i])->tail_textNode;

                if(text == NULL) printf(".\n");
                else printf("%s\n", text->value);

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

                int val = 0;

                for (int j = 0; j < numRow; ++j) {

                    if(tail_history->value[3] == 'c'){

                        val = undoChange(ind1+j);

                        if(val == 0){
                            continue;
                        }


                    } else{

                        if(val == 2){
                            val = undoDelete(0);
                        } else{
                            val = undoDelete(numRow);
                        }

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

                    long val = 0;

                    for (int j = 0; j < numRow; ++j) {

                        if(historyCommand[3] == 'c'){

                            redoChange(ind1+j);

                        } else{

                            if(val != 1){

                                val = redoDelete(ind1, numRow);

                            }

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

    /*
    cleanUpDocumentTextNode();
    cleanUpDocumentContainer();
    cleanUpHistoryFromHead(&head_history);
    */


    free(row);


    return 0;
}



// CREATE NODES
container_pointer createContainer(){

    container_pointer ret = (container_pointer)malloc(sizeof(struct TextNodeContainer));

    ret->index = 0;
    ret->command = NULL;
    ret->head_textNode = NULL;
    ret->tail_textNode = NULL;

    return ret;
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

    container_pointer element = document[index];

    if(element->head_textNode == NULL){

        text_pointer text = createTextNode(x);
        element->index = index;
        element->head_textNode = text;
        element->tail_textNode = text;
    }
    else{

        text_pointer text = createTextNode(x);

        element->tail_textNode->next = text;
        text->prev = element->tail_textNode;
        element->tail_textNode = text;

    }


}
void removeToDocument(long startIndex, long howMany, char* command){

    long iterator = 0;

    while (document[startIndex+iterator]->head_textNode != NULL){

        if(iterator < howMany){

            hell[nodeInHell]->index = document[startIndex+iterator]->index-iterator;
            hell[nodeInHell]->command = command;
            hell[nodeInHell]->head_textNode = document[startIndex+iterator]->head_textNode;
            hell[nodeInHell]->tail_textNode = document[startIndex+iterator]->tail_textNode;
            /*cleanUpTextFromHead(&document[startIndex+iterator]->head_textNode);
            document[startIndex+iterator]->head_textNode = NULL;
            document[startIndex+iterator]->tail_textNode = NULL;*/

            nodeInHell++;


        }

        document[startIndex+iterator]->head_textNode = document[startIndex+howMany+iterator]->head_textNode;
        document[startIndex+iterator]->tail_textNode = document[startIndex+howMany+iterator]->tail_textNode;

        iterator++;

    }

}

int undoChange(long num) {

    container_pointer c = document[num];

    if (c->tail_textNode == NULL) {     // se annullo l'istruzione che ha creato quel nodo
        return 0;
    } else {
        c->tail_textNode = c->tail_textNode->prev;
        return 1;
    }
}
int undoDelete(long numRow){

    // il nodo è stato eliminato, devo rimetterlo nel documento

    int ret = 1;


    if(nodeInHell-1 < 0){
        return 0;
    }

    if(hell[nodeInHell-1]->command != tail_history->value){
        return 0;
    }

    container_pointer r = hell[nodeInHell-1];
    long r_index = r->index;

    if(document[r_index]->tail_textNode != NULL){

        text_pointer tmp_head = document[r_index]->head_textNode;
        text_pointer tmp_tail = document[r_index]->tail_textNode;

        document[r_index]->head_textNode = r->head_textNode;
        document[r_index]->tail_textNode = r->tail_textNode;


        long iterator = r_index+1;


        while (document[iterator]->head_textNode != NULL) {

            text_pointer tmp_head_2 = document[iterator]->head_textNode;
            text_pointer tmp_tail_2 = document[iterator]->tail_textNode;

            document[iterator]->head_textNode = tmp_head;
            document[iterator]->tail_textNode = tmp_tail;

            tmp_head = tmp_head_2;
            tmp_tail = tmp_tail_2;

            iterator++;

        }


        document[iterator]->head_textNode = tmp_head;
        document[iterator]->tail_textNode = tmp_tail;


    }

    else{

        if(document[r_index]->head_textNode != NULL){

            for (int i = 0; i < numRow; ++i) {

                heaven[nodeInHeaven]->index = document[r_index+i]->index-i;
                heaven[nodeInHeaven]->head_textNode = document[r_index+i]->head_textNode;
                heaven[nodeInHeaven]->tail_textNode = document[r_index+i]->tail_textNode;

                document[r_index+i]->head_textNode = NULL;
                document[r_index+i]->tail_textNode = NULL;

                nodeInHeaven++;

            }

            ret = 2;

        }

        document[r_index]->head_textNode = r->head_textNode;
        document[r_index]->tail_textNode = r->tail_textNode;

    }

    r->index = 0;
    r->command = NULL;
    r->head_textNode = NULL;
    r->tail_textNode = NULL;

    nodeInHell--;

    return ret;

}
void redoChange(long num){

        container_pointer container = document[num];

        if(container->head_textNode != NULL){

            if(container->tail_textNode == NULL){

                container->tail_textNode = container->head_textNode;
            } else{

                container->tail_textNode = container->tail_textNode->next;
            }

        }
        else{

            container_pointer heavenNode = heaven[nodeInHeaven-1];
            long key = heavenNode->index;

            text_pointer tmp_head = heavenNode->head_textNode;
            text_pointer tmp_tail = heavenNode->tail_textNode;

            if(tmp_tail == NULL){
                tmp_tail = tmp_head;
            } else{
                tmp_tail = tmp_tail->next;
            }



            while (document[key]->head_textNode != NULL) {

                text_pointer tmp_head_2 = document[key]->head_textNode;
                text_pointer tmp_tail_2 = document[key]->tail_textNode;

                document[key]->head_textNode = tmp_head;
                document[key]->tail_textNode = tmp_tail;

                tmp_head = tmp_head_2;
                tmp_tail = tmp_tail_2;

                key++;

            }


            document[key]->head_textNode = tmp_head;
            document[key]->tail_textNode = tmp_tail;

            heaven[nodeInHeaven-1]->index = 0;
            heaven[nodeInHeaven-1]->head_textNode = NULL;
            heaven[nodeInHeaven-1]->tail_textNode = NULL;

            nodeInHeaven--;

        }

}

int redoDelete(long treeIndexToRemove, long howMany){

    char* command = NULL;

    if(tail_history == NULL){

        command = head_history->value;
    } else{

        if(tail_history->next != NULL){
            command = tail_history->next->value;
        } else{
            command = tail_history->value;
        }

    }

    removeToDocument(treeIndexToRemove, howMany, command);

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

void freeDocument(){

    long iterator = 1;

    while (document[iterator]->head_textNode != NULL) {

        if( document[iterator]->tail_textNode == NULL ){

            document[iterator]->command = NULL;
            document[iterator]->head_textNode = NULL;
            document[iterator]->tail_textNode = NULL;

        }


        iterator++;
    }

}
void freeHell(){

    long iterator = nodeInHell-1;

    while (hell[iterator]->head_textNode != NULL) {

        hell[iterator]->head_textNode = NULL;
        hell[iterator]->tail_textNode = NULL;

        iterator++;
    }

}
void freeHeaven(){

    long iterator = nodeInHeaven-1;

    while (heaven[iterator]->head_textNode != NULL) {

        heaven[iterator]->head_textNode = NULL;
        heaven[iterator]->tail_textNode = NULL;

        iterator++;
    }

}

void cleanUpDocumentTextNode(){

    long iterator = 1;
    while (document[iterator]->head_textNode != NULL){

        cleanUpTextFromHead(&document[iterator]->head_textNode);
        document[iterator]->head_textNode = NULL;
        document[iterator]->tail_textNode = NULL;

        iterator++;

    }

}
void cleanUpDocumentContainer(){

    long iterator = 0;
    while (document[iterator] != NULL){

        free(document[iterator]);
        document[iterator] = NULL;

        iterator++;

    }

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
        bulldozer = NULL;
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
        bulldozer = NULL;

        bulldozer = nextMiles;
    }
}