#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define ROW_LEN 1024
#define ALLOC_INCREMENT 80000


typedef struct HistoryNode* history_pointer;
typedef char* string_ptr;
typedef char** array_string_ptr;

// HISTORY STRUCT
struct HistoryNode {
    string_ptr value;
    long ind1;
    long ind2;
    array_string_ptr rowModified;
    long numRow;
    history_pointer next;
    history_pointer prev;

};  // used for commands history

struct DynamicArray {
    long dim;
    array_string_ptr containers;
};


// GLOBAL VAR HISTORY
history_pointer head_history;
history_pointer tail_history;



struct DynamicArray document;
long nodeInDocument = 0;

string_ptr row;
char tmp[1024];
string_ptr pendingCommand;

long undo = 0;      // indica di quante istruzioni si è fatta la undo
long redo = 0;

long possiblyUndo = 0;
long possiblyRedo = 0;
long doUndo = 0;
long doRedo = 0;
int undoOrRedoDone = 0;

long setToNull = 0;


// CREATE NODES
history_pointer createHistoryNode(string_ptr x, long ind1, long ind2, long numRow);

// DB MANAGING
void addInHistory(long ind1, long ind2, long numRow);
void addToDocument(string_ptr x, long index);
void removeToDocument(long startIndex, long howMany, bool save);

void undoToDocument(long ret);
void redoToDocument(long ret);

void undoChange();
void undoDelete();
void redoChange(long startIndex, long numRow);
void redoDelete(long startIndex, long numRow);

// HELPER
string_ptr getRow();
void getBounds(string_ptr line, long *ind1, long *ind2);


void allocMem(struct DynamicArray* array);



int main() {

    allocMem(&document);


    int exit = 0;
    row = getRow();
    unsigned long len = strlen(row);

    while (row[len-1] != 'q'){


        if (row[len-1] == 'c') {     // modifica o inserimento di righe

            if(undo > 0 || redo > 0 || possiblyRedo > 0){       // mi serve per la undo/redo

                pendingCommand = row;
                doUndo = 1;
                doRedo = 1;
                exit = 1;

            }

            if(exit != 1) {

                long ind1 = 0, ind2 = 0;

                getBounds(row, &ind1, &ind2);       // prendo gli indici del comando

                long numRow = ind2 - ind1 + 1;
                addInHistory(ind1, ind2, numRow);                     // aggiungo il comando alla cronologia


                for (int i = 0; i < numRow; i++) {

                    long key = ind1 + i;
                    getRow();

                    addToDocument(row, key);     // aggiungo all'albero al la stringa(row) all'indice key
                    tail_history->rowModified[i] = row;
                }

                getRow();
                len = strlen(row);

                if (strstr(row, ".") != NULL) {
                    free(row);
                } else printf("something went wrong\n");
            }

        }
        else if (row[len-1] == 'd') {         // elimina righe se presenti nel documento

            if(undo > 0 || redo > 0 || possiblyRedo > 0){       // mi serve per la undo/redo

                pendingCommand = row;
                doUndo = 1;
                doRedo = 1;
                exit = 1;

            }

            if(exit != 1) {

                long ind1, ind2;
                getBounds(row, &ind1, &ind2);

                long numRow = ind2 - ind1 + 1;
                addInHistory(ind1, ind2, document.dim);             // aggiungo il comando alla cronologia

                removeToDocument(ind1, numRow, true);
            }

        }
        else if (row[len-1] == 'p') {

            if(undo > 0 || redo > 0){       // mi serve per la undo/redo

                pendingCommand = row;
                doUndo = 1;
                doRedo = 1;
                exit = 1;

            }

            if(exit != 1) {

                long ind1, ind2;
                getBounds(row, &ind1, &ind2);

                free(row);


                if (ind2 == 0) {
                    printf(".\n");
                    getRow();
                    len = strlen(row);
                    continue;
                }
                long numRow = ind2 - ind1 + 1;


                for (int i = 0; i < numRow; i++) {

                    string_ptr string = document.containers[ind1+i];

                    if(string == NULL || strcmp(string, "") == 0){
                        printf(".\n");
                    } else printf("%s\n", string);

                }
            }

        }
        else if (row[len-1] == 'u') {

            string_ptr q;
            long ret = strtol(row, &q, 10);

            free(row);

            if(doUndo == 1){

                undoToDocument(ret);
                possiblyUndo -= ret;
                undoOrRedoDone = 1;

            } else{

                undo += ret;

                if(undo > possiblyUndo + redo){
                    undo = possiblyUndo + redo;
                }
            }

        }

        else if (row[len-1] == 'r') {

            if(undo > 0 || possiblyRedo > 0){

                string_ptr q;
                long ret = strtol(row, &q, 10);

                free(row);

                if(doRedo == 1){

                    redoToDocument(ret);
                    undoOrRedoDone = 1;
                    redo -= ret;
                    possiblyUndo += ret;
                    possiblyRedo -= ret;

                } else{

                    redo += ret;

                    if(possiblyRedo == 0){

                        if(redo > undo){ redo = undo; }
                    } else{

                        if(redo > possiblyRedo + undo){
                            redo = possiblyRedo + undo;
                        }
                    }



                }

            }
            else{
                free(row);  // è una redo a vuoto (senza che prima ci sia un'undo), quindi la ignoro
            }

        }

        if(doUndo == 1){

            if(undoOrRedoDone == 0){

                if(redo > 0) {
                    redo -= undo;
                    undo = 0;
                }



                long undoSet;

                if(redo > possiblyRedo) {   // ci sono più redo di undo

                    redo = possiblyRedo;

                    undoSet = -redo;

                } else{     // ci sopo più undo delle redo oppure si bilanciano
                    undoSet = undo - redo;
                }

                if(undoSet > 0){
                    row = NULL;
                    row = (string_ptr)calloc(21, sizeof(char));
                    snprintf(row, 20, "%ldu", undoSet);
                    len = strlen(row);

                    if(redo < 0){
                        possiblyRedo -= redo;
                    } else{
                        possiblyRedo += undo;
                    }

                }
                else if(undoSet < 0){

                    row = NULL;
                    row = (string_ptr)calloc(22, sizeof(char));
                    snprintf(row, 21, "%ldr", -(undoSet));
                    len = strlen(row);

                } else if( undoSet == 0){

                    if(strstr(pendingCommand, "p") == NULL){    // se il comando non è una print
                        //freeDocument();
                        //freeHeaven();
                        possiblyRedo = 0;
                    }

                    row = NULL;
                    row = pendingCommand;
                    len = strlen(row);

                    undo = 0;
                    redo = 0;

                    doUndo = 0;
                    doRedo = 0;
                    undoOrRedoDone = 0;

                    exit = 0;

                }
            }
            else{

                if(strstr(pendingCommand, "p") == NULL){    // se il comando non è una print
                    //freeDocument();
                    //freeHeaven();
                    possiblyRedo = 0;
                }

                row = NULL;
                row = pendingCommand;
                len = strlen(row);

                undo = 0;
                redo = 0;

                doUndo = 0;
                doRedo = 0;
                undoOrRedoDone = 0;

                exit = 0;
            }

        }
        else{

            getRow();
            len = strlen(row);
        }

    }


    free(row);


    return 0;
}



// CREATE NODES
history_pointer createHistoryNode(string_ptr x, long ind1, long ind2, long numRow) {

    history_pointer newNode = (history_pointer)malloc(sizeof(struct HistoryNode));

    newNode->value = x;
    newNode->ind1 = ind1;
    newNode->ind2 = ind2;
    newNode->numRow = numRow;
    newNode->rowModified = (array_string_ptr ) calloc(numRow, sizeof(string_ptr));
    newNode->prev = NULL;
    newNode->next = NULL;

    return newNode;
}

// DB MANAGING
void addInHistory(long ind1, long ind2, long numRow) {

    history_pointer newNode = createHistoryNode(row, ind1, ind2, numRow);

    if(tail_history == NULL){       // se è il primo nodo della cronologia lo aggingo in testa
        head_history = newNode;
        tail_history = newNode;
    }

    else{                                   // altrimento lo aggiungo in coda
        tail_history->next = newNode;
        newNode->prev = tail_history;
        tail_history = newNode;
    }

    possiblyUndo++;

}


void addToDocument(string_ptr x, long index){

    if(index > document.dim-1 ){

        allocMem(&document);

    }

    if(document.containers[index] == NULL){
        nodeInDocument++;
    }


    document.containers[index] = x;


}
void removeToDocument(long startIndex, long howMany, _Bool save){

    if(startIndex > nodeInDocument){
        return;
    }

    if(howMany > nodeInDocument){
        howMany = nodeInDocument;
    }

    if(save){
        memcpy(tail_history->rowModified, document.containers, (nodeInDocument+1)*sizeof(string_ptr));
        tail_history->numRow = nodeInDocument;
    }


    for (int i = 0; i < nodeInDocument-howMany; ++i) {

        document.containers[startIndex+i] = document.containers[startIndex+howMany+i];

    }

    for (int i = 0; i < howMany; ++i) {

        document.containers[nodeInDocument-i] = NULL;
    }

    nodeInDocument -= howMany;

}


void undoToDocument(long ret){

    for (int i = 0; i < ret; ++i) {

        if(tail_history == NULL){
            continue;
        }

        if(tail_history->value[3] == 'c'){

            undoChange();


        } else{

            undoDelete();

        }

        tail_history = tail_history->prev;
    }

}
void redoToDocument(long ret){

    for (int i = 0; i < ret; ++i) {
        long ind1, ind2;

        if(tail_history == NULL){

            tail_history = head_history;
        } else{

            if(tail_history->next != NULL){
                tail_history = tail_history->next;
            }

        }


        ind1 = tail_history->ind1;
        ind2 = tail_history->ind2;


        long numRow = ind2-ind1+1;



        if(tail_history->value[3] == 'c'){

            redoChange(ind1, numRow);

        } else{

            redoDelete(ind1, numRow);


        }

    }

}


void undoChange(){      // caso più tricky tricky

    long ind1 = tail_history->ind1;
    long ind2 = tail_history->ind2;


    long numRow = tail_history->numRow;

    if(setToNull > 0 && tail_history->prev != NULL){

        if(tail_history->prev->numRow > setToNull){

            numRow += setToNull;
            setToNull = 0;

        }

    }

    for (int i = 0; i < numRow; ++i) {

        if(tail_history->prev == NULL){
            document.containers[ind1 + i] = NULL;
        } else{

            string_ptr debug = document.containers[ind1 + i];

            if(strstr(tail_history->prev->value, "d") == NULL && nodeInDocument <= tail_history->prev->numRow){
                // se il comando prima è una change allora vado a rimettere i valori vecchi


                if(ind1+i > tail_history->prev->numRow) {
                    setToNull++;
                    nodeInDocument--;
                    document.containers[ind1 + i] = NULL;
                } else{
                    document.containers[ind1 + i] = tail_history->prev->rowModified[i];
                }

            } else{
                // se però è una delete devo mettere il nodo a null, altrimenti farei una undoDelete al posto di una undoChange
                string_ptr debug2 = document.containers[ind1 + (numRow-1) - i];
                document.containers[ind1 + (numRow-1) - i] = NULL;
                nodeInDocument--;
            }
        }


    }

}
void undoDelete() {

    document.containers = tail_history->rowModified;    // ripristino la versione prima della delete

    nodeInDocument = tail_history->numRow;

    string_ptr debug = document.containers[1];
    string_ptr debug2 = document.containers[2];
}

void redoChange(long startIndex, long numRow){

    for (int j = 0; j < numRow; ++j) {

        if(document.containers[startIndex+j] == NULL){
            nodeInDocument++;
        }

        document.containers[startIndex+j] = tail_history->rowModified[j];   // riapplico al documento le stringhe di quando ho fatto quel comando
    }

}
void redoDelete(long startIndex, long numRow){

    removeToDocument(startIndex, numRow, false);    // rifaccio la delete senza però salvare nella history il document

}

// HELPER
string_ptr getRow(){

    string_ptr res = fgets(tmp, ROW_LEN, stdin);     // fgets gets '\n' at the end of the string

    if (res == NULL) {
        printf("Failed to read input.\n");
        return 0;
    }

    unsigned long len = strlen(tmp);

    row = (string_ptr)malloc(sizeof(char) * len);

    strncpy(row, tmp, len);
    strtok(row, "\n");

    return row;
}
void getBounds(string_ptr line, long *ind1, long *ind2){

    long ret;

    ret = strtol(line, &line, 10);
    line++;
    *ind1 = ret;
    ret = strtol(line, &line, 10);
    *ind2 = ret;

}


void allocMem(struct DynamicArray* array){

    (*array).dim += ALLOC_INCREMENT;
    (*array).containers = (array_string_ptr)realloc((*array).containers, (*array).dim * sizeof(string_ptr));

    for (long i = (*array).dim-ALLOC_INCREMENT; i < (*array).dim; ++i) {

        (*array).containers[i] = NULL;
    }

}