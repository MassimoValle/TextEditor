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

    array_string_ptr savedState;   // array salvato in caso di modifica di una o più stringhe, o righe aggiunte
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


string_ptr pendingCommand = NULL;   // poichè le undo/redo aspettano un comando c, d o p per essere eseguite,
                                    // quando le eseguo mi serve salvare temporaneamente il comando che le ha triggerate


// CREATE NODES
void allocMem(struct DynamicArray* array, long cellNeeded);
history_pointer createHistoryNode(string_ptr x, long ind1, long ind2);

// DB MANAGING
void addInHistory(long ind1, long ind2, bool zero);

void addToDocument(long ind1, long numRow);
void removeToDocument(long startIndex, long howMany);

void undoToDocument(long ret);
void redoToDocument(long ret);

void restorePrevSavedState();

// HELPER
string_ptr getRow();
void getBounds(string_ptr line, long *ind1, long *ind2);



int main() {

    // SALVO LO STATO IN CASO DI CHANGE CHE MODIFICA ALMENO UN NODO O DI DELETE, SEMPRE DOPO AVER FATTO LA MODIFICA
    // NEL CASO DI CHANGE CHE AGGIUNGE SOLO NODI INVECE MI SALVO LE RIGHE AGGIUNTE

    document.dim = 0;
    allocMem(&document, 1);


    // init 0 state
    addInHistory(0, 0, true);       // nodo inziale della history, corrisponde al documento vuoto iniziale

    tail_history->value = (string_ptr)"0,0c";
    tail_history->numRow = nodeInDocument;
    tail_history->cellAllocated = document.dim;
    tail_history->savedState = (array_string_ptr ) calloc(document.dim, sizeof(string_ptr));

    tail_history->copy = true;


    // start program

    bool exit = false;      // indica se devo eseguire veramente il comando o se prima devo fare le undo/redo
                            // exit == true ==> svolgi prima le undo/redo

    row = getRow();
    unsigned long len = strlen(row);

    while (row[len-1] != 'q'){


        if (row[len-1] == 'c') {     // modifica o inserimento di righe

            if(undo > 0 || redo > 0 || possiblyRedo > 0){       // mi serve per la undo/redo

                pendingCommand = row;
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


                        // teoricamente dovrebbe entrare sempre nell'else perchè il numero di elementi validi è gestito da nodeInDocument
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

            if(doUndo == true){     // allora eseguo le undo

                undoToDocument(ret);
                possiblyUndo -= ret;
                undoOrRedoDone = 1;

            } else{     // altrimenti le colleziono

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

                if(doRedo == true){    // allora eseguo le redo

                    redoToDocument(ret);
                    undoOrRedoDone = 1;
                    redo -= ret;
                    possiblyUndo += ret;
                    possiblyRedo -= ret;

                } else{         // altrimenti le colleziono

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

                if(redo > 0) {          // solo nel caso in cui sono state fatte delle redo
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

                if(undoSet > 0){            // ho fatto più undo che redo
                    row = NULL;
                    row = (string_ptr)calloc(21, sizeof(char));
                    snprintf(row, 20, "%ldu", undoSet);     // creo il comando di undo
                    len = strlen(row);

                    if(redo < 0){
                        possiblyRedo -= redo;
                    } else{
                        possiblyRedo += undo;
                    }

                }
                else if(undoSet < 0){       // allora ci sono più redo che undo

                    row = NULL;
                    row = (string_ptr)calloc(22, sizeof(char));
                    snprintf(row, 21, "%ldr", -(undoSet));      // creo il comando di redo
                    len = strlen(row);

                } else if( undoSet == 0){   // se si bilanciano (#undo == #redo)

                    row = NULL;
                    row = pendingCommand;
                    len = strlen(row);

                    if(strstr(pendingCommand, "p") == NULL){    // se il comando non è una print (non contiene la lettera p)
                        possiblyRedo = 0;
                        pendingCommand = NULL;
                    }

                    undo = 0;
                    redo = 0;

                    doUndo = false;
                    doRedo = false;
                    undoOrRedoDone = 0;

                    exit = false;

                }
            }
            else{   // qui ci entra dopo aver effettuato la undo/redo e prima di eseguire il pending state

                row = NULL;
                row = pendingCommand;
                len = strlen(row);

                if(strstr(pendingCommand, "p") == NULL){    // se il comando non è una print (non contiene la lettera p)
                    possiblyRedo = 0;
                    pendingCommand = NULL;
                }

                undo = 0;
                redo = 0;

                doUndo = false;
                doRedo = false;
                undoOrRedoDone = 0;

                exit = false;
            }

        }
        else{   // caso base in cui non devo effettuare nessuna undo/redo

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

    while (increment < cellNeeded){     // incremento di ALLOC_INCREMENT alla volta fino a che non supero le quantità richiesta
        increment += ALLOC_INCREMENT;
    }

    (*array).dim += increment;
    (*array).containers = (array_string_ptr)realloc((*array).containers, (*array).dim * sizeof(string_ptr));

}
history_pointer createHistoryNode(string_ptr x, long ind1, long ind2) {

    history_pointer newNode = (history_pointer)malloc(sizeof(struct HistoryNode));

    newNode->value = x;
    newNode->ind1 = ind1;
    newNode->ind2 = ind2;
    newNode->numRow = 0;
    newNode->cellAllocated = 0;
    newNode->savedState = NULL;
    newNode->copy = false;

    newNode->prev = NULL;
    newNode->next = NULL;

    return newNode;
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

    if(!zero)   // se è il primo nodo (quello di inizio main) non lo devo contare come possibile undo
                                // poichè indica solo lo stato iniziale (quindi vuoto) del documento
        possiblyUndo++;

}


void addToDocument(long ind1, long numRow){

    bool modify = false;    // indica se è una change che modifica o se solo aggiunge

    if (ind1 <= nodeInDocument) {
        // se la change modifica alcune righe allora lo stato va salvato dopo la modifica
        modify = true;
    }
    else {
        // crea spazio per salvare le nuove righe aggiunte
        tail_history->numRow = numRow;
        tail_history->cellAllocated = numRow;

        tail_history->savedState = (array_string_ptr ) calloc(numRow, sizeof(string_ptr));

        tail_history->copy = false;
    }

    // ind1 è incluso in numRow, per questo faccio (numRow-1)
    if(ind1+(numRow-1) > document.dim-1){     // se voglio scrivere in un nodo che non è ancora stato allocato
        allocMem(&document, numRow);
    }

    for (int i = 0; i < numRow; i++) {

        long key = ind1 + i;


        getRow();

        if (key > nodeInDocument) {     // se creo un nuovo nodo (ci entra sia nelle change che aggiungono solo che
            nodeInDocument++;           // nelle change che modificano almeno un nodo e ne aggiungono altri


            if(modify == false){    // se sono solo nodi nuovi allora li salvo nella history per poterli poi ripristinare

                tail_history->savedState[i] = row;
            }

        }

        document.containers[key] = row;     // assegno row all'indice di document

    }


    if (modify) {
        // se la change modifica delle righe allora salva l'intero stato dopo la change
        tail_history->numRow = nodeInDocument;
        tail_history->cellAllocated = document.dim;
        tail_history->savedState = (array_string_ptr ) calloc(document.dim, sizeof(string_ptr));

        memcpy(tail_history->savedState, document.containers, document.dim * sizeof(string_ptr));

        tail_history->copy = true;
    }

}
void removeToDocument(long startIndex, long howMany){

    if (startIndex <= nodeInDocument) {
                    // (howMany-1) perchè startIndex è compreso in howMany
        if(startIndex+(howMany-1) > nodeInDocument){       // se provo ad eliminare più nodi di quanti ce ne sono
            howMany = (nodeInDocument-startIndex+1);      // calcolo quanti nodi effettivamente posso cancellare
        }



        for (int i = 0; i < nodeInDocument-howMany; ++i) {  // traslo tutto in giù di howMany

            document.containers[startIndex+i] = document.containers[startIndex+howMany+i];

        }

        nodeInDocument -= howMany;
    }

    // !!! non metto a NULL le ultime componenti per il semplice fatto che verranno sovrascritte in caso dai comandi successivi


    // salva lo stato dopo aver effetuato la remove
    tail_history->numRow = nodeInDocument;
    tail_history->cellAllocated = document.dim;
    tail_history->savedState = (array_string_ptr ) calloc(document.dim, sizeof(string_ptr));

    memcpy(tail_history->savedState, document.containers, document.dim * sizeof(string_ptr));

    tail_history->copy = true;

}


void undoToDocument(long ret){

    bool stop = false;
    for (int i = 0; i < ret && !stop; ++i) {         // svolgo le undo, torno indietro nella history di ret volte

        if (tail_history->prev == NULL) {       // per debug, non ci dovrebbe mai entrare
            printf("HISTORY ERROR\n");
            stop = true;
        }

        tail_history = tail_history->prev;

    }

    if (tail_history->copy) {
        // in questo punto della hitstory è salvata una copia intera del document
        // quindi posso ripristinare subito lo stato

        //document.containers = tail_history->savedState;    // ripristino la versione precendente
        document.dim = tail_history->cellAllocated;
        nodeInDocument = tail_history->numRow;

        // modo giusto di ripristinare la versione precendente
        document.containers = (array_string_ptr ) calloc(tail_history->cellAllocated, sizeof(string_ptr));
        memcpy(document.containers, tail_history->savedState, tail_history->cellAllocated * sizeof(string_ptr));

    }
    else
    {
        // in questo punto della history non è stata salvata una copia ma solo le righe aggiunte

        restorePrevSavedState();
    }

}
void redoToDocument(long ret){

    bool stop = false;
    for (int i = 0; i < ret && !stop; ++i) {         // svolgo le redo, vado avanti nella history di ret volte

        if (tail_history->next == NULL) {       // per debug, non ci dovrebbe mai entrare
            printf("HISTORY ERROR\n");
            stop = true;
        }

        tail_history = tail_history->next;

    }

    if (tail_history->copy) {
        // in questo punto della hitstory è salvata una copia intera del document
        // quindi posso ripristinare subito lo stato

        //document.containers = tail_history->savedState;    // ripristino la versione successiva
        document.dim = tail_history->cellAllocated;
        nodeInDocument = tail_history->numRow;

        // modo giusto di ripristinare la versione successiva
        document.containers = (array_string_ptr ) calloc(tail_history->cellAllocated, sizeof(string_ptr));
        memcpy(document.containers, tail_history->savedState, tail_history->cellAllocated * sizeof(string_ptr));

    }
    else
    {
        // in questo punto della history non è stata salvata una copia ma solo le righe aggiunte

        restorePrevSavedState();
    }

}

void restorePrevSavedState() {

    // torna indietro fino ad uno stato completo
    history_pointer iterator = tail_history->prev;

    while (!iterator->copy) {
        iterator = iterator->prev;
    }

    // copia lo stato completo
    document.containers = (array_string_ptr ) calloc(iterator->cellAllocated, sizeof(string_ptr));
    memcpy(document.containers, iterator->savedState, iterator->cellAllocated * sizeof(string_ptr));

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

            document.containers[startIndex+i] = iterator->savedState[i];     // applico le modifiche
        }

        nodeInDocument += numRow;       // incremento nodeInDocument del numero di nodi che sono andato ad aggiungere al documento

        iterator = iterator->next;
    }


}


// HELPER
string_ptr getRow(){

    string_ptr res = fgets(tmp, ROW_LEN, stdin);     // !!! fgets mette '\n' alla fine della stringa

    if (res == NULL) {
        printf("Failed to read input.\n");
        return 0;
    }

    unsigned long len = strlen(tmp);                // calcolo la reale lunghezza della stringa per poter allocare row

    row = (string_ptr)malloc(sizeof(char) * len);   // alloco row della lunghezza che mi serve

    strncpy(row, tmp, len);     // copio tmp in row
    strtok(row, "\n");     // taglio \n da row

    return row;
}
void getBounds(string_ptr line, long *ind1, long *ind2){

    long ret;

    ret = strtol(line, &line, 10);      // restituisce il primo numero che trova e lo rimuove dalla stringa
    line++;                                   // mi sposto dopo la virgola
    *ind1 = ret;
    ret = strtol(line, &line, 10);
    *ind2 = ret;

}