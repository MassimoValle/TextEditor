#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define ROW_LEN 1024
#define ALLOC_INCREMENT 1000


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
    bool copy;
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
long nodeInDocument = 0;    // nodi validi nel documento (indice 0 escluso)

string_ptr row;
char tmp[1024];

long undo = 0;
long redo = 0;

long possiblyUndo = 0;
long possiblyRedo = 0;
long doUndo = 0;
long doRedo = 0;
int undoOrRedoDone = 0;

bool savePendingState = true;
string_ptr pendingCommand;
history_pointer pendingState;


// CREATE NODES
history_pointer createHistoryNode(string_ptr x, long ind1, long ind2);

// DB MANAGING
void addInHistory(long ind1, long ind2, bool zero);
void addToDocument(long ind1, long numRow, bool save);
void removeToDocument(long startIndex, long howMany, bool save);

void undoToDocument(long ret);
void redoToDocument(long ret);

void undoChange();
void undoDelete();
void redoChange();
void redoDelete(long startIndex, long numRow);

// HELPER
string_ptr getRow();
void getBounds(string_ptr line, long *ind1, long *ind2);


void allocMem(struct DynamicArray* array);



int main() {

    allocMem(&document);


    // init 0 state
    addInHistory(0, 0, true);

    tail_history->value = (string_ptr)"0,0c";
    tail_history->numRow = nodeInDocument;
    tail_history->rowModified = (array_string_ptr ) calloc(1+nodeInDocument, sizeof(string_ptr));

    tail_history->rowModified = document.containers;
    tail_history->copy = true;

    // start program

    int exit = 0;
    row = getRow();
    unsigned long len = strlen(row);

    while (row[len-1] != 'q'){


        if (row[len-1] == 'c') {     // modifica o inserimento di righe

            if(undo > 0 || redo > 0 || possiblyRedo > 0){       // mi serve per la undo/redo

                pendingCommand = row;
                savePendingState = true;
                doUndo = 1;
                doRedo = 1;
                exit = 1;

            }

            if(exit != 1) {

                long ind1 = 0, ind2 = 0;

                getBounds(row, &ind1, &ind2);       // prendo gli indici del comando

                long numRow = ind2 - ind1 + 1;
                addInHistory(ind1, ind2, false);                     // aggiungo il comando alla cronologia

                addToDocument(ind1, numRow, true);     // aggiungo all'albero al la stringa(row) all'indice key

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
                savePendingState = true;
                doUndo = 1;
                doRedo = 1;
                exit = 1;

            }

            if(exit != 1) {

                long ind1, ind2;
                getBounds(row, &ind1, &ind2);

                long numRow = ind2 - ind1 + 1;
                addInHistory(ind1, ind2, false);             // aggiungo il comando alla cronologia

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

                    if(ind1+i > nodeInDocument){
                        printf(".\n");
                    } else{
                        string_ptr string = document.containers[ind1+i];

                        if(string == NULL || strcmp(string, "") == 0){
                            printf(".\n");
                        } else printf("%s\n", string);
                    }



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
history_pointer createHistoryNode(string_ptr x, long ind1, long ind2) {

    history_pointer newNode = (history_pointer)malloc(sizeof(struct HistoryNode));

    newNode->value = x;
    newNode->ind1 = ind1;
    newNode->ind2 = ind2;
    newNode->numRow = 0;
    newNode->rowModified = NULL;
    newNode->copy = false;
    newNode->prev = NULL;
    newNode->next = NULL;

    return newNode;
}
void initPendingState(){    // salva l'ultimo stato raggiunto dal programma

    // creo lastState
    pendingState = createHistoryNode(tail_history->value, tail_history->ind1, tail_history->ind2);

    // assegno memoria a rowModified
    pendingState->numRow = nodeInDocument;
    pendingState->rowModified = (array_string_ptr ) calloc(1+nodeInDocument, sizeof(string_ptr));

    // copio lo stato attuale
    memcpy(pendingState->rowModified, document.containers, (1 + nodeInDocument) * sizeof(string_ptr));
    pendingState->copy = true;

}

// DB MANAGING
void addInHistory(long ind1, long ind2, bool zero) {

    history_pointer newNode = createHistoryNode(row, ind1, ind2);

    if(tail_history == NULL){       // se è il primo nodo della cronologia lo aggingo in testa
        head_history = newNode;
        tail_history = newNode;
    }

    else{                                   // altrimento lo aggiungo in coda
        tail_history->next = newNode;
        newNode->prev = tail_history;
        tail_history = newNode;
    }

    if(!zero)
        possiblyUndo++;

}


void addToDocument(long ind1, long numRow, bool save){

    bool modify = false;    // indica se è una change che modifica o se solo aggiunge

    if(ind1 <= nodeInDocument){// se sto andando a modificare qualcosa allora devo salvare lo salvo
        if (save) {

            tail_history->numRow = nodeInDocument;
            tail_history->rowModified = (array_string_ptr ) calloc(1+nodeInDocument, sizeof(string_ptr));

            memcpy(tail_history->rowModified, document.containers, (1 + nodeInDocument) * sizeof(string_ptr));

            tail_history->copy = true;
            modify = true;

        }

    } else{ // se vado ad aggiungere dei nodi solamente

        tail_history->numRow = numRow;                          // ind1 comprende la riga di indice 0
        tail_history->rowModified = (array_string_ptr ) calloc(numRow, sizeof(string_ptr));    // ind1 è incluso in numRow
        tail_history->copy = false;
    }


    for (int i = 0; i < numRow; i++) {

        long key = ind1 + i;


        getRow();

        if (key > document.dim-1) {     // se l'indice a cui voglio inserire la riga va oltre lo spazio allocato per document

            allocMem(&document);

        }

        if (document.containers[key] == NULL) { // se è un nodo nuovo...
            nodeInDocument++;

            if(modify == false){    // se sono solo nodi nuovi allora li salvo nella history per poterli poi ripristinare
                tail_history->rowModified[i] = row;
                tail_history->numRow = numRow;
            }

        }


        document.containers[key] = row;     // assegno row all'indice di document

    }

}
void removeToDocument(long startIndex, long howMany, _Bool save){

    if(startIndex > nodeInDocument){    // se provo ad eliminare un nodo che non esiste
        return;
    }

                    // (howMany-1) perchè startIndex è compreso in howMany
    if(startIndex+(howMany-1) > nodeInDocument){       // se provo ad eliminare più nodi di quanti ce ne sono
        howMany = (nodeInDocument-startIndex+1);      // calcolo quanti nodi effettivamente posso cancellare
    }

    if(save){       // se devo salvare lo stato (delete semplice)
        tail_history->numRow = nodeInDocument;
        tail_history->rowModified = (array_string_ptr ) calloc(1+nodeInDocument, sizeof(string_ptr));

        memcpy(tail_history->rowModified, document.containers, (1+nodeInDocument)*sizeof(string_ptr));

        tail_history->copy = true;
    }


    for (int i = 0; i < nodeInDocument-howMany; ++i) {  // traslo tutto in giù di howMany

        document.containers[startIndex+i] = document.containers[startIndex+howMany+i];

    }

    for (int i = 0; i < howMany; ++i) {     // metto a null gli ultimi howMany nodi in fondo

        document.containers[nodeInDocument-i] = NULL;
    }

    string_ptr debug0 = document.containers[0];
    string_ptr debug1 = document.containers[1];
    string_ptr debug2 = document.containers[2];

    string_ptr tdebug0 = tail_history->rowModified[0];
    string_ptr tdebug1 = tail_history->rowModified[1];
    string_ptr tdebug2 = tail_history->rowModified[2];

    nodeInDocument -= howMany;

}


void undoToDocument(long ret){

    if(savePendingState == true){
        initPendingState();
        savePendingState = false;
    }


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

        if(tail_history == NULL){
            tail_history = head_history;
            //continue;
        }

        if(tail_history->next == NULL){   // se lo stato successivo è pendingState (ho rifatto tante redo quante undo fatte prima)

            document.containers = pendingState->rowModified;
            nodeInDocument = pendingState->numRow;

            string_ptr debug0 = document.containers[0];
            string_ptr debug1 = document.containers[1];
            string_ptr debug2 = document.containers[2];

        }

        if(tail_history->next->value[3] == 'c'){

            redoChange();

        } else{

            long ind1 = tail_history->next->ind1;
            long ind2 = tail_history->next->ind2;

            long numRow = ind2-ind1+1;

            redoDelete(ind1, numRow);

        }

        if(tail_history->next != NULL){
            tail_history = tail_history->next;
        }

    }

}


void undoChange(){

    string_ptr debug0 = document.containers[0];
    string_ptr debug1 = document.containers[1];
    string_ptr debug2 = document.containers[2];

    string_ptr tdebug0 = tail_history->rowModified[0];
    string_ptr tdebug1 = tail_history->rowModified[1];
    string_ptr tdebug2 = tail_history->rowModified[2];


    if(tail_history->copy){     // se avevo modificato dei nodi

        document.containers = tail_history->rowModified; // ripristino la modifica prima della modifica dei nodi
        nodeInDocument = tail_history->numRow;

    } else{     // se avevo aggiunto solo dei nodi allora li metto semplicemente a null

        long startIndex = tail_history->ind1;
        for (int i = 0; i < tail_history->numRow; ++i) {

            document.containers[startIndex+i] = NULL;
            nodeInDocument--;

        }

    }

}
void undoDelete() {

    if(nodeInDocument > 0){
        string_ptr debug0 = document.containers[0];
        string_ptr debug1 = document.containers[1];
        string_ptr debug2 = document.containers[2];
    }


    document.containers = tail_history->rowModified;    // ripristino la versione precendente alla delete

    nodeInDocument = tail_history->numRow;

    if(nodeInDocument > 0)
    {
        string_ptr debug0 = document.containers[0];
        string_ptr debug1 = document.containers[1];
        string_ptr debug2 = document.containers[2];
    }


}

void redoChange(){

    if(tail_history->next->copy) {    // se avevo modificato dei nodi devo andare allo stato successivo

        document.containers = tail_history->next->next->rowModified;
        nodeInDocument = tail_history->next->next->numRow;

        string_ptr debug0 = document.containers[0];
        string_ptr debug1 = document.containers[1];
        string_ptr debug2 = document.containers[2];

    } else{     // se avevo aggiunto dei nodi li ripristino prendendoli dalla rowModified

        long startIndex = tail_history->next->ind1;
        for (int i = 0; i < tail_history->next->numRow; ++i) {

            document.containers[startIndex+i] = tail_history->next->rowModified[i];

        }

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


void allocMem(struct DynamicArray* array){  // alloco nuova memoria ad array

    (*array).dim += ALLOC_INCREMENT;
    (*array).containers = (array_string_ptr)realloc((*array).containers, (*array).dim * sizeof(string_ptr));

    for (long i = (*array).dim-ALLOC_INCREMENT; i < (*array).dim; ++i) {

        (*array).containers[i] = NULL;
    }

}