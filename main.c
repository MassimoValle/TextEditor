#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define ROW_LEN 1024
#define ALLOC_INCREMENT 50


typedef struct HistoryNode* history_pointer;
typedef char* string_ptr;
typedef char** array_string_ptr;

// HISTORY STRUCT
struct HistoryNode {
    string_ptr value;   // comando
    long ind1;
    long ind2;

    //array_string_ptr newValues;     // usato nel caso di una copia per salvarmi i nuovi valori che do alla riga
    //long numNewValues;              // numero di righe a cui ho cambiato valore

    array_string_ptr prevState;   // array salvato in caso di modifica di una o più stringhe, o righe aggiunte
    long cellAllocated;       // spazio di memoria allocato
    long numRow;    // numero di righe se cui ha avuto effetto la modifica (se è una copia sono nodeInDocument)

    bool copy;      // true se ho salvato una copia dell'array (caso in cui ho modificato almeno un nodo)

    history_pointer next;
    history_pointer prev;

};  // used for commands history

struct DynamicArray {

    long dim;                       // dimensione allocata in memoria
    array_string_ptr containers;    // array di stringhe

};


// GLOBAL VAR HISTORY
history_pointer head_history;       // punta il primo comando della cronologia
history_pointer tail_history;       // punta il primo comando effettivo della cronologia



struct DynamicArray document;
long nodeInDocument = 0;            // nodi validi nel documento (indice 0 escluso)

string_ptr row;                     // riga attuale presa da stdin
char tmp[1024];                     // helper per tagliare la stringa


//#### VARIABILI USATE PER RAGGRUPPARE LE UNDO E REDO ####//
long undo = 0;                      // indica quanti comandi di undo si ha inserito
long redo = 0;                      // indica quanti comandi di redo si ha inserito

long possiblyUndo = 0;              // indica quante undo è possibile effettuare
long possiblyRedo = 0;              // indica quante redo è possibile effettuare
bool doUndo = 0;
bool doRedo = 0;
int undoOrRedoDone = 0;
//#########################################################//

//bool savePendingState = true;
string_ptr pendingCommand = NULL;
//history_pointer pendingState = NULL;


// CREATE NODES
void allocMem(struct DynamicArray* array, long cellNeeded);
history_pointer createHistoryNode(string_ptr x, long ind1, long ind2);
//void initPendingState();

// DB MANAGING
void addInHistory(long ind1, long ind2, bool zero);
void addToDocument(long ind1, long numRow);
void removeToDocument(long startIndex, long howMany);

void undoToDocument(long ret);
void redoToDocument(long ret);

void restorePrevSavedState();

//void undoChange();
//void undoDelete();
//void redoChange();
//void redoDelete();

// HELPER
string_ptr getRow();
void getBounds(string_ptr line, long *ind1, long *ind2);



int main() {

    document.dim = 0;
    allocMem(&document, 1);


    // init 0 state
    addInHistory(0, 0, true);

    tail_history->value = (string_ptr)"0,0c";
    tail_history->numRow = nodeInDocument;
    tail_history->cellAllocated = document.dim;
    tail_history->prevState = (array_string_ptr ) calloc(document.dim, sizeof(string_ptr));

    //tail_history->prevState = document.containers;
    tail_history->copy = true;


    // start program

    bool exit = false;

    row = getRow();
    unsigned long len = strlen(row);

    while (row[len-1] != 'q'){


        if (row[len-1] == 'c') {     // modifica o inserimento di righe

            if(undo > 0 || redo > 0 || possiblyRedo > 0){       // mi serve per la undo/redo

                pendingCommand = row;
                //savePendingState = true;
                doUndo = true;
                doRedo = true;
                exit = true;

            }

            if(exit == false) {

                long ind1 = 0, ind2 = 0;

                getBounds(row, &ind1, &ind2);       // prendo gli indici del comando

                long numRow = ind2 - ind1 + 1;
                addInHistory(ind1, ind2, false);                     // aggiungo il comando alla cronologia

                addToDocument(ind1, numRow);     // aggiungo all'albero al la stringa(row) all'indice key

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
                //savePendingState = true;
                doUndo = true;
                doRedo = true;
                exit = true;

            }

            if(exit == false) {

                long ind1, ind2;
                getBounds(row, &ind1, &ind2);

                long numRow = ind2 - ind1 + 1;
                addInHistory(ind1, ind2, false);             // aggiungo il comando alla cronologia

                removeToDocument(ind1, numRow);
            }

        }
        else if (row[len-1] == 'p') {

            if(undo > 0 || redo > 0){       // mi serve per la undo/redo

                //if(pendingCommand == NULL) savePendingState = true;

                pendingCommand = row;
                doUndo = true;
                doRedo = true;
                exit = true;

            }

            if(exit == false) {

                long ind1, ind2;
                getBounds(row, &ind1, &ind2);

                free(row);      // non mi serve salvarmi il comando di print


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

            free(row);      // non mi serve salvarmi il comando di undo

            if(doUndo == true){

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

                free(row);      // non mi serve salvarmi il comando di redo

                if(doRedo == true){

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

        if(doUndo == true){

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

                    row = NULL;
                    row = pendingCommand;
                    len = strlen(row);

                    if(strstr(pendingCommand, "p") == NULL){    // se il comando non è una print
                        possiblyRedo = 0;
                        pendingCommand = NULL;
                        //pendingState = NULL;
                    }

                    undo = 0;
                    redo = 0;

                    doUndo = false;
                    doRedo = false;
                    undoOrRedoDone = 0;

                    exit = false;

                }
            }
            else{

                row = NULL;
                row = pendingCommand;
                len = strlen(row);

                if(strstr(pendingCommand, "p") == NULL){    // se il comando non è una print (non contiene la lettera p)
                    possiblyRedo = 0;
                    pendingCommand = NULL;
                    //pendingState = NULL;
                }

                undo = 0;
                redo = 0;

                doUndo = false;
                doRedo = false;
                undoOrRedoDone = 0;

                exit = false;
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
void allocMem(struct DynamicArray* array, long cellNeeded){  // alloco nuova memoria ad array

    long increment = 0;
    long oldDim = document.dim;

    while (increment < cellNeeded){
        increment += ALLOC_INCREMENT;
    }

    (*array).dim += increment;
    (*array).containers = (array_string_ptr)realloc((*array).containers, (*array).dim * sizeof(string_ptr));

    /*for (long i = oldDim; i < (*array).dim; ++i) {

        (*array).containers[i] = NULL;

    }*/

}
history_pointer createHistoryNode(string_ptr x, long ind1, long ind2) {

    history_pointer newNode = (history_pointer)malloc(sizeof(struct HistoryNode));

    newNode->value = x;
    newNode->ind1 = ind1;
    newNode->ind2 = ind2;
    newNode->numRow = 0;
    newNode->cellAllocated = 0;
    //newNode->newValues = NULL;
    //newNode->numNewValues = 0;
    newNode->prevState = NULL;
    newNode->copy = false;

    newNode->prev = NULL;
    newNode->next = NULL;

    return newNode;
}
/*void initPendingState(){    // salva l'ultimo stato raggiunto dal programma

    // creo lastState
    pendingState = createHistoryNode(tail_history->value, tail_history->ind1, tail_history->ind2);

    // assegno memoria a rowModified
    pendingState->numRow = nodeInDocument;
    pendingState->cellAllocated = document.dim;

    // in teoria basta salvare solo il riferimento allo stato corrente perché non dovrebbe mai essere modificato dalle undo/redo
    pendingState->prevState = document.containers;

    //pendingState->prevState = (array_string_ptr ) calloc(document.dim, sizeof(string_ptr));

    // copio lo stato attuale

    //memcpy(pendingState->prevState, document.containers, document.dim * sizeof(string_ptr));

    pendingState->copy = true;

}*/


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


void addToDocument(long ind1, long numRow){

    bool modify = false;    // indica se è una change che modifica o se solo aggiunge

    // TODO: PER ORA SALVA SEMPRE LO STATO PER RISOLVERE IL PROBLEMA, POI IL COMMENTO VA TOLTO
    if (ind1 <= nodeInDocument) {
        // se la change modifica alcune righe allora lo stato va salvato dopo la modifica
        modify = true;
    }
    else {
        // crea spazio per salvare le nuove righe aggiunte
        tail_history->numRow = numRow;
        tail_history->cellAllocated = numRow;

        tail_history->prevState = (array_string_ptr ) calloc(numRow, sizeof(string_ptr));    // ind1 è incluso in numRow

        tail_history->copy = false;
    }


    if(ind1+(numRow-1) > document.dim-1){     // se voglio scrivere in un nodo che non è ancora stato allocato
        allocMem(&document, numRow);
    }

    for (int i = 0; i < numRow; i++) {

        long key = ind1 + i;


        getRow();

        //if (document.containers[key] == NULL) { // se è un nodo nuovo...
        if (key > nodeInDocument) {
            nodeInDocument++;


            if(modify == false){    // se sono solo nodi nuovi allora li salvo nella history per poterli poi ripristinare
                // usato quando faccio una change che modifica e aggiunge nuovi nodi
                tail_history->prevState[i] = row;
            }

        }

        document.containers[key] = row;     // assegno row all'indice di document

    }


    if (modify) {
        // se la change modifica delle righe allora salva l'intero stato dopo la change
        tail_history->numRow = nodeInDocument;
        tail_history->cellAllocated = document.dim;
        tail_history->prevState = (array_string_ptr ) calloc(document.dim, sizeof(string_ptr));

        memcpy(tail_history->prevState, document.containers, document.dim * sizeof(string_ptr));

        tail_history->copy = true;
    }


    /*bool modify = false;    // indica se è una change che modifica o se solo aggiunge

    if(ind1 <= nodeInDocument){// se sto andando a modificare qualcosa allora devo salvare lo salvo prima della modifica

            tail_history->numRow = nodeInDocument;
            tail_history->cellAllocated = document.dim;
            //tail_history->numNewValues = numRow;
            //tail_history->newValues = (array_string_ptr ) calloc(numRow, sizeof(string_ptr));
            tail_history->prevState = (array_string_ptr ) calloc(document.dim, sizeof(string_ptr));

            memcpy(tail_history->prevState, document.containers, document.dim * sizeof(string_ptr));

            tail_history->copy = true;
            modify = true;

    }

    if(ind1+(numRow-1) > document.dim-1){     // se voglio scrivere in un nodo che non è ancora stato allocato
        allocMem(&document, numRow);
    }

    if(modify == false){

        tail_history->numRow = numRow;
        tail_history->cellAllocated = numRow;
        tail_history->prevState = (array_string_ptr ) calloc(numRow, sizeof(string_ptr));    // ind1 è incluso in numRow
        tail_history->copy = false;

    }

    for (int i = 0; i < numRow; i++) {

        long key = ind1 + i;


        getRow();

        if (document.containers[key] == NULL) { // se è un nodo nuovo...
            nodeInDocument++;


            if(modify == false){    // se sono solo nodi nuovi allora li salvo nella history per poterli poi ripristinare
            // usato quando faccio una change che modifica e aggiunge nuovi nodi
                tail_history->prevState[i] = row;
            }

        }


        //if(modify){
        //    tail_history->newValues[i] = row;
        //}

        document.containers[key] = row;     // assegno row all'indice di document

    }*/

}
void removeToDocument(long startIndex, long howMany){

    /*if(startIndex > nodeInDocument){    // se provo ad eliminare un nodo che non esiste
        return;
    }*/

    if (startIndex <= nodeInDocument) {
        // (howMany-1) perchè startIndex è compreso in howMany
        if(startIndex+(howMany-1) > nodeInDocument){       // se provo ad eliminare più nodi di quanti ce ne sono
            howMany = (nodeInDocument-startIndex+1);      // calcolo quanti nodi effettivamente posso cancellare
        }



        for (int i = 0; i < nodeInDocument-howMany; ++i) {  // traslo tutto in giù di howMany

            document.containers[startIndex+i] = document.containers[startIndex+howMany+i];

        }

        /*for (int i = 0; i < howMany; ++i) {     // metto a null gli ultimi howMany nodi in fondo

            document.containers[nodeInDocument-i] = NULL;
        }*/

        nodeInDocument -= howMany;
    }

    // salva lo stato dopo aver effetuato la remove
    tail_history->numRow = nodeInDocument;
    tail_history->cellAllocated = document.dim;
    tail_history->prevState = (array_string_ptr ) calloc(document.dim, sizeof(string_ptr));

    memcpy(tail_history->prevState, document.containers, document.dim * sizeof(string_ptr));

    tail_history->copy = true;

}


void undoToDocument(long ret){

    /*if(savePendingState == true){       // salvo lo stato in tutti i casi tranne quando faccio undo tramite il comandi print
        initPendingState();             // e poi faccio ancora delle undo tramite un'altro comando di print
        savePendingState = false;
    }*/

    bool stop = false;
    for (int i = 0; i < ret && !stop; ++i) {         // svolgo le undo

        // TODO: CONTROLLO SE ARRIVATO IN CIMA ALLA HISTORY
        if (tail_history->prev == NULL) {
            printf("HISTORY ERROR\n"); // TODO: DA TOGLIERE
            stop = true;
        }

        tail_history = tail_history->prev;

    }

    if (tail_history->copy) {
        // in questo punto della hitstory è salvata una copia intera del document
        // quindi posso ripristinare subito lo stato

        //document.containers = tail_history->prevState;    // ripristino la versione precendente alla delete
        document.dim = tail_history->cellAllocated;
        nodeInDocument = tail_history->numRow;

        document.containers = (array_string_ptr ) calloc(tail_history->cellAllocated, sizeof(string_ptr));
        memcpy(document.containers, tail_history->prevState, tail_history->cellAllocated * sizeof(string_ptr));

    }
    else
    {
        // in questo punto della history non è stata salvata una copia ma solo le righe aggiunte
        // basta tenere lo stato attuale e decrementare il numero di righe presenti in document

        // TODO: CONTROLLA perché non ci credo che funziona così semplicemente, dai fratm è assurdo
        //nodeInDocument -= tail_history->numRow;

        restorePrevSavedState();
    }

}
void redoToDocument(long ret){

    bool stop = false;
    for (int i = 0; i < ret && !stop; ++i) {

        // TODO: CONTROLLO SE ARRIVATO IN FONDO ALLA HISTORY
        if (tail_history->next == NULL) {
            printf("HISTORY ERROR\n"); // TODO: DA TOGLIERE
            stop = true;
        }

        tail_history = tail_history->next;

    }

    if (tail_history->copy) {
        // in questo punto della hitstory è salvata una copia intera del document
        // quindi posso ripristinare subito lo stato

        //document.containers = tail_history->prevState;    // ripristino la versione precendente alla delete
        document.dim = tail_history->cellAllocated;
        nodeInDocument = tail_history->numRow;

        document.containers = (array_string_ptr ) calloc(tail_history->cellAllocated, sizeof(string_ptr));
        memcpy(document.containers, tail_history->prevState, tail_history->cellAllocated * sizeof(string_ptr));

    }
    else
    {
        // in questo punto della history non è stata salvata una copia ma solo le righe aggiunte
        // devo cercare lo stato successivo in cui è stata fatta una copia completa e ritornare allo stato richiesto

        restorePrevSavedState();
    }

}

void restorePrevSavedState() {
    //printf("cane\n");
    /*int exceedingRows = 0;

    history_pointer iterator = tail_history->next;

    // il pendingState serve se l'ultimo comando (o una serie di ultimi comandi) sono change che aggiungono
    // perché in questo caso non ci sarebbe uno stato completo dopo le change che aggiungo => crasha tutto eheh
    //if (iterator == NULL) iterator = pendingState;

    while (!iterator->copy) {
        // se copy è false vuol dire che questo è un comando che aggiunge solo righe, salvo quante ne ha aggiunte
        exceedingRows += iterator->numRow;

        iterator = iterator->next;

        //if (iterator == NULL) iterator = pendingState;
    }


    document.containers = iterator->prevState;    // ripristino la versione precendente alla delete
    document.dim = iterator->cellAllocated;
    nodeInDocument = iterator->numRow;

    // se sono state effetuate più change che aggiungono
    // tolgo da nodeInDocument il numero di righe che sono state aggiunte nelle change successive alla prossima
    nodeInDocument -= exceedingRows;*/


    // torna indietro fino ad uno stato completo
    history_pointer iterator = tail_history->prev;
    //long rowCount = tail_history->numRow;

    while (!iterator->copy) {
        //rowCount += iterator->numRow;
        iterator = iterator->prev;
    }

    /*document.dim = iterator->cellAllocated;
    nodeInDocument = iterator->numRow;

    if(nodeInDocument+rowCount > document.dim){

    }*/

    // copia lo stato completo
    document.containers = (array_string_ptr ) calloc(iterator->cellAllocated, sizeof(string_ptr));
    memcpy(document.containers, iterator->prevState, iterator->cellAllocated * sizeof(string_ptr));

    document.dim = iterator->cellAllocated;
    nodeInDocument = iterator->numRow;



    // applica le modifiche successive fino allo stato corrente richiesto
    iterator = iterator->next;

    while (iterator != tail_history->next) {

        long startIndex = iterator->ind1;
        long numRow = iterator->numRow;

        if(startIndex+(numRow-1) > document.dim-1){     // se voglio scrivere in un nodo che non è ancora stato allocato
            allocMem(&document, numRow);
        }

        for (int i = 0; i < numRow; ++i) {

            document.containers[startIndex+i] = iterator->prevState[i];
        }

        nodeInDocument += numRow;

        iterator = iterator->next;
    }


}

/*void undoToDocument(long ret){

    //if(savePendingState == true){       // salvo lo stato in tutti i casi tranne quando faccio undo tramite il comandi print
    //    initPendingState();             // e poi faccio ancora delle undo tramite un'altro comando di print
    //    savePendingState = false;
    //}


    for (int i = 0; i < ret; ++i) {         // svolgo le undo

        tail_history = tail_history->prev;

        if(tail_history->value[3] == 'c'){

            undoChange();

        } else{

            undoDelete();

        }

        //tail_history = tail_history->prev;
    }

}*/
/*void redoToDocument(long ret){

    for (int i = 0; i < ret; ++i) {

        //if(tail_history == NULL){
        //    tail_history = head_history;    // non dovrebbe mai verificarsi se ho gestito bene i contatori di undo/redo nel main

        //} else{
        //    tail_history = tail_history->next;      // porto subito avanti la coda perchè altrimenti andrei a fare la stessa cosa che ha fatto l'ultima undo

            /*if(tail_history->next == NULL){   // se lo stato successivo è pendingState (ho rifatto tante redo quante undo fatte prima)

                if(tail_history->value[3] == 'c' && tail_history->copy == false){

                    redoChange();

                } else{

                    document.containers = pendingState->prevState;
                    nodeInDocument = pendingState->numRow;
                    document.dim = pendingState->cellAllocated;

                    continue;   // skippo perchè non devo gestirlo

                }


            }*

        //}


        if(tail_history->value[3] == 'c'){

            redoChange();

        } else{

            redoDelete();

        }

        if(tail_history == NULL){
            tail_history = head_history;    // non dovrebbe mai verificarsi se ho gestito bene i contatori di undo/redo nel main

        } else{
            tail_history = tail_history->next;      // porto subito avanti la coda perchè altrimenti andrei a fare la stessa cosa che ha fatto l'ultima undo

        }

    }

}*/


/*void undoChange(){
    if (tail_history->copy) {
        document.containers = tail_history->prevState; // ripristino la modifica prima della modifica dei nodi
        document.dim = tail_history->cellAllocated;
        nodeInDocument = tail_history->numRow;
    }
    else {
        history_pointer curr = tail_history->prev;

        while (!curr->copy) {
            curr = curr->prev;
        }

        document.containers = curr->prevState;
        document.dim = curr->cellAllocated;
        nodeInDocument = curr->numRow;

        while (curr != tail_history) {
            long startIndex = curr->ind1;
            long numRow = curr->numRow;

            for (int i = 0; i < numRow; i++) {
                document.containers[startIndex+i] = curr->prevState[i];
            }

            nodeInDocument += numRow;

            curr = curr->next;
        }

        /*long startIndex = tail_history->ind1;
        long numRow = tail_history->numRow;

        long iterator = startIndex+numRow;
        //string_ptr ptr = document.containers[iterator];

        while (document.containers[iterator] != NULL){
            document.containers[iterator] = NULL;
            iterator++;
        }

        for (int i = 0; i < numRow; ++i) {

            document.containers[startIndex+i] = tail_history->prevState[i];

        }*
    }


    /*if(tail_history->copy){     // se avevo modificato dei nodi

        document.containers = tail_history->prevState; // ripristino la modifica prima della modifica dei nodi
        document.dim = tail_history->cellAllocated;
        nodeInDocument = tail_history->numRow;

    } else{     // se avevo aggiunto solo dei nodi allora li metto semplicemente a null

        long startIndex = tail_history->ind1;
        long numRow = tail_history->numRow;

        /*if(startIndex+(numRow-1) > document.dim-1){     // se voglio scrivere in un nodo che non è ancora stato allocato
            allocMem(&document, numRow);
        }*

        for (int i = 0; i < numRow; ++i) {

            document.containers[startIndex+i] = NULL;

        }

        //document.dim = tail_history->cellAllocated;
        nodeInDocument-= tail_history->numRow;

    }*

}*/
/*void undoDelete() {
    document.containers = tail_history->prevState;    // ripristino la versione precendente alla delete
    document.dim = tail_history->cellAllocated;
    nodeInDocument = tail_history->numRow;

    //if(tail_history->copy){

        /*document.containers = tail_history->prevState;    // ripristino la versione precendente alla delete

        document.dim = tail_history->cellAllocated;
        nodeInDocument = tail_history->numRow;*

    //}
}*/


/*void redoChange(){

    document.containers = tail_history->prevState;
    document.dim = tail_history->cellAllocated;
    nodeInDocument = tail_history->numRow;

    /*if(tail_history->copy) {    // se avevo modificato dei nodi devo andare allo stato successivo

        if(tail_history->next->copy){   // se lo stato successivo è una copia allora la applico

            document.containers = tail_history->next->prevState;
            document.dim = tail_history->next->cellAllocated;
            nodeInDocument = tail_history->next->numRow;

        } /*else{    // altrimenti nel caso l'ilstruzione successiva aggiunge solo nodi non posso far puntare rowModified
                    // devo rieffettuare la modifica assegnando di nuovo i valori modificati
            long startIndex = tail_history->ind1;
            long numRow = tail_history->numNewValues;

            for (int i = 0; i < numRow; ++i) {

                document.containers[startIndex+i] = tail_history->newValues[i];
            }

        }*

    } else{     // se avevo aggiunto dei nodi li ripristino prendendoli dalla rowModified

        /*long startIndex = tail_history->ind1;
        long numRow = tail_history->numRow;

        for (int i = 0; i < numRow; ++i) {

            document.containers[startIndex+i] = tail_history->prevState[i];

        }

        nodeInDocument += tail_history->numRow;*

        history_pointer curr = tail_history->next;

        /*if(curr == NULL) curr = pendingState;

        while (!curr->copy) {
            curr = curr->next;
            if(curr == NULL) curr = pendingState;
        }*

        document.containers = curr->prevState;
        document.dim = curr->cellAllocated;
        nodeInDocument = curr->numRow;

        long startIndex = tail_history->ind1;
        long numRow = tail_history->numRow;

        long iterator = startIndex+numRow;
        //string_ptr ptr = document.containers[iterator];

        while (document.containers[iterator] != NULL){
            document.containers[iterator] = NULL;
            iterator++;
        }

        for (int i = 0; i < numRow; ++i) {

            document.containers[startIndex+i] = tail_history->prevState[i];

        }

    }*

}*/
/*void redoDelete(){

    /*if(tail_history->next->copy){

        document.containers = tail_history->next->prevState;    // ripristino la versione successiva alla delete

        document.dim = tail_history->next->cellAllocated;
        nodeInDocument = tail_history->next->numRow;

    } else{     // nel caso l'ilstruzione successiva aggiunge solo nodi non posso far puntare rowModified

        long numRow = tail_history->numRow;
        long startIndex = tail_history->ind1;

        for (int i = 0; i < numRow; ++i) {

            document.containers[startIndex+i] = NULL;

        }
    }*

    document.containers = tail_history->prevState;
    document.dim = tail_history->cellAllocated;
    nodeInDocument = tail_history->numRow;

}*/



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