#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROW_LEN 1024
#define ALLOC_INCREMENT 300

typedef struct TextNode* text_pointer;
typedef struct TextNodeContainer* container_pointer;
typedef struct HistoryNode* history_pointer;


// HISTORY STRUCT
struct HistoryNode {
    char* value;
    long ind1;
    long ind2;
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

struct DynamicArray {
    long dim;
    container_pointer containers;
};


// GLOBAL VAR HISTORY
history_pointer head_history;
history_pointer tail_history;



struct DynamicArray document;
struct DynamicArray hell;
struct DynamicArray heaven;

long nodeInDocument = 0;
long nodeInHell = 0;
long nodeInHeaven = 0;
char* row;
char tmp[1024];
char* pendingCommand;

long undo = 0;      // indica di quante istruzioni si è fatta la undo
long redo = 0;

long possiblyUndo = 0;
long possiblyRedo = 0;
long doUndo = 0;
long doRedo = 0;
int undoOrRedoDone = 0;




// CREATE NODES
container_pointer createContainer();
text_pointer createTextNode(char *x);
history_pointer createHistoryNode(char *x, long ind1, long ind2);

// DB MANAGING
void addInHistory(long ind1, long ind2);
void addToDocument(char *x, long index);
void removeToDocument(long startIndex, long howMany, char* command);

void undoToDocument(long ret);
void redoToDocument(long ret);

int undoChange(long num);
int undoDelete(long numRow);
void redoChange(long num);
int redoDelete(long treeIndexToRemove, long howMany, char* historyCommand);

// HELPER
char* getRow();
void getBounds(char* line, long *ind1, long *ind2);
void reallocMem(struct DynamicArray* dynArray);


void freeDocument();
void freeHeaven();



int main() {

    document.containers = (container_pointer )calloc(ALLOC_INCREMENT, sizeof(struct TextNodeContainer));
    document.dim = ALLOC_INCREMENT;

    hell.containers = (container_pointer )calloc(ALLOC_INCREMENT, sizeof(struct TextNodeContainer));
    hell.dim = ALLOC_INCREMENT;

    heaven.containers = (container_pointer )calloc(ALLOC_INCREMENT, sizeof(struct TextNodeContainer));
    heaven.dim = ALLOC_INCREMENT;


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

                addInHistory(ind1, ind2);                     // aggiungo il comando alla cronologia

                long numRow = ind2 - ind1 + 1;


                for (int i = 0; i < numRow; i++) {

                    long key = ind1 + i;
                    getRow();

                    addToDocument(row, key);     // aggiungo all'albero al la stringa(row) all'indice key
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

                addInHistory(ind1, ind2);             // aggiungo il comando alla cronologia

                long numRow = ind2 - ind1 + 1;

                removeToDocument(ind1, numRow, tail_history->value);   // rimuovo dal documento il nodo(listNodeToDelete) e lo metto in hell
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

                    if (ind1 + i > document.dim) {

                        printf(".\n");
                    } else {

                        if(ind1+i > nodeInDocument){

                            printf(".\n");
                        } else{

                            container_pointer c = &document.containers[ind1 + i];
                            text_pointer text = c->tail_textNode;

                            if (text == NULL) printf(".\n");
                            else printf("%s\n", text->value);

                        }

                    }

                }
            }

        }
        else if (row[len-1] == 'u') {

            char* q;
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

                char* q;
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
                    row = (char*)calloc(21, sizeof(char));
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
                    row = (char*)calloc(22, sizeof(char));
                    snprintf(row, 21, "%ldr", -(undoSet));
                    len = strlen(row);

                } else if( undoSet == 0){

                    if(strstr(pendingCommand, "p") == NULL){    // se il comando non è una print
                        freeDocument();
                        freeHeaven();
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
                    freeDocument();
                    freeHeaven();
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
container_pointer createContainer(){

    //container_pointer ret = (container_pointer)malloc(sizeof(struct TextNodeContainer));
    container_pointer ret = (container_pointer)calloc(1, sizeof(struct TextNodeContainer));

    /*ret->index = 0;
    ret->command = NULL;
    ret->head_textNode = NULL;
    ret->tail_textNode = NULL;*/

    return ret;
}
text_pointer createTextNode(char *x){

    text_pointer newNode = (text_pointer)malloc(sizeof(struct TextNode));

    newNode->value = x;
    newNode->prev = NULL;
    newNode->next = NULL;

    return newNode;
}
history_pointer createHistoryNode(char *x, long ind1, long ind2) {

    history_pointer newNode = (history_pointer)malloc(sizeof(struct HistoryNode));

    newNode->value = x;
    newNode->ind1 = ind1;
    newNode->ind2 = ind2;
    newNode->prev = NULL;
    newNode->next = NULL;

    return newNode;
}

// DB MANAGING
void addInHistory(long ind1, long ind2) {

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

    possiblyUndo++;

}
void addToDocument(char *x, long index){

    if(index > document.dim-1 ){

        reallocMem(&document);

    }

    container_pointer element = &document.containers[index];
    text_pointer text = createTextNode(x);

    if(element->head_textNode == NULL){

        element->index = index;
        element->head_textNode = text;
        element->tail_textNode = text;

        nodeInDocument++;
    }
    else{

        element->tail_textNode->next = text;
        text->prev = element->tail_textNode;
        element->tail_textNode = text;

    }


}


void removeToDocument(long startIndex, long howMany, char* command){

    if(startIndex > document.dim){          // controllo l'elemento di partenza
        return;
    }

    if((&document.containers[startIndex])->head_textNode == NULL){          // controllo l'elemento di partenza
        return;
    }

    long actuallyRemoved = 0;


    for (int i = 0; i < howMany && (&document.containers[startIndex+i])->head_textNode != NULL; ++i) {       // aggiungo i nodi a hell prima di eliminarli da document

        if(nodeInHell > hell.dim-1 ){

            reallocMem(&hell);

        }

        (&hell.containers[nodeInHell])->index = startIndex;
        (&hell.containers[nodeInHell])->command = command;
        (&hell.containers[nodeInHell])->head_textNode = (&document.containers[startIndex+i])->head_textNode;
        (&hell.containers[nodeInHell])->tail_textNode = (&document.containers[startIndex+i])->tail_textNode;

        nodeInHell++;
        actuallyRemoved++;

    }

    // sposto tutti i nodi successivi in su di actuallyRemoved
    if(startIndex+actuallyRemoved <= nodeInDocument){

        container_pointer dest = &document.containers[startIndex];
        container_pointer src = &document.containers[startIndex+actuallyRemoved];
        long size = (nodeInDocument-(actuallyRemoved));

        memmove(dest, src, size*sizeof(struct TextNodeContainer));
    }

    for (int i = 0; i < actuallyRemoved; ++i) { // partendo dall'ultimo nodo, risalgo di actuallyRemoved per metterli a null dato che sono stati spostati in basso


        (&document.containers[nodeInDocument-i])->index = 0;
        (&document.containers[nodeInDocument-i])->command = NULL;
        (&document.containers[nodeInDocument-i])->head_textNode = NULL;
        (&document.containers[nodeInDocument-i])->tail_textNode = NULL;

    }

    nodeInDocument -= actuallyRemoved;

}

void undoToDocument(long ret){

    for (int i = 0; i < ret; ++i) {
        long ind1, ind2;

        if(tail_history == NULL){
            continue;
        }


        ind1 = tail_history->ind1;
        ind2 = tail_history->ind2;

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

void redoToDocument(long ret){

    for (int i = 0; i < ret; ++i) {
        long ind1, ind2;

        history_pointer historyCommand = NULL;

        if(tail_history == NULL){

            historyCommand = head_history;
        } else{

            if(tail_history->next != NULL){
                historyCommand = tail_history->next;
            } else{
                historyCommand = tail_history;
            }

        }


        ind1 = historyCommand->ind1;
        ind2 = historyCommand->ind2;


        long numRow = ind2-ind1+1;

        long val = 0;

        for (int j = 0; j < numRow; ++j) {

            if(historyCommand->value[3] == 'c'){

                redoChange(ind1+j);

            } else{

                if(val != 1){

                    val = redoDelete(ind1, numRow, historyCommand->value);

                }

            }

        }

        if(tail_history != NULL){
            tail_history = tail_history->next;
        } else tail_history = head_history;

    }

}

int undoChange(long num) {

    container_pointer c = &document.containers[num];    // document[num] deve esserci per forza per poter fare una undoChange

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

    if((&hell.containers[nodeInHell-1])->command != tail_history->value){
        return 0;
    }

    container_pointer r = &hell.containers[nodeInHell-1];   // hell[nodeInHell-1] deve esserci per forza per poter fare una undoDelete
    long r_index = r->index;


    if((&document.containers[r_index])->tail_textNode != NULL){   // prendo l'elemento al quale rimpiazzare r

        long size = nodeInDocument;

        if(r_index+1+size > document.dim){
            reallocMem(&document);
        }

        container_pointer dest = &document.containers[r_index+1];
        container_pointer src = &document.containers[r_index];


         memmove(dest, src, size*sizeof(struct TextNodeContainer));

        (&document.containers[r_index])->head_textNode = r->head_textNode;    // assegno le componenti di r all'elemento
        (&document.containers[r_index])->tail_textNode = r->tail_textNode;


        /*

        text_pointer tmp_head = (&document.containers[r_index])->head_textNode;   // mi salvo le sue componenti
        text_pointer tmp_tail = (&document.containers[r_index])->tail_textNode;

        (&document.containers[r_index])->head_textNode = r->head_textNode;    // assegno le componenti di r all'elemento
        (&document.containers[r_index])->tail_textNode = r->tail_textNode;


        long iterator = r_index+1;


        if(document[iterator] == NULL){                 // se non ci sono elementi dopo r_index nel documento
            document[iterator] = createContainer();
        }

        while (document[iterator]->head_textNode != NULL) { // in modo che questo non dia errore

            text_pointer tmp_head_2 = document[iterator]->head_textNode;
            text_pointer tmp_tail_2 = document[iterator]->tail_textNode;

            document[iterator]->head_textNode = tmp_head;
            document[iterator]->tail_textNode = tmp_tail;

            tmp_head = tmp_head_2;
            tmp_tail = tmp_tail_2;

            iterator++;


            if(document[iterator] == NULL){
                document[iterator] = createContainer();
            }

        }

        if(document[iterator] == NULL){
            document[iterator] = createContainer();
        }

        document[iterator]->head_textNode = tmp_head;
        document[iterator]->tail_textNode = tmp_tail;
        */

    }

    else{

        if((&document.containers[r_index])->head_textNode != NULL){   // se head==NULL significa che devo aggiungere un elemento ad heaven


            long actuallyRemoved = 0;


            for (int i = 0; i < numRow && (&document.containers[r_index+i])->head_textNode != NULL; ++i) {       // aggiungo i nodi a hell prima di eliminarli da document

                if( nodeInHeaven > heaven.dim-1 ){

                    reallocMem(&heaven);

                }

                (&heaven.containers[nodeInHeaven])->index = r_index;
                (&heaven.containers[nodeInHeaven])->head_textNode = (&document.containers[r_index+i])->head_textNode;
                (&heaven.containers[nodeInHeaven])->tail_textNode = (&document.containers[r_index+i])->tail_textNode;

                nodeInHeaven++;
                actuallyRemoved++;

            }

            nodeInDocument -= actuallyRemoved;

            /*
            int i = 0;
            //for (int i = 0; i < numRow; ++i) {
            while(document[r_index+i]->head_textNode != NULL && document[r_index+i]->tail_textNode == NULL){

                if(heaven[nodeInHeaven] == NULL){
                    heaven[nodeInHeaven] = createContainer();
                }

                heaven[nodeInHeaven]->index = document[r_index+i]->index-i;
                heaven[nodeInHeaven]->head_textNode = document[r_index+i]->head_textNode;
                heaven[nodeInHeaven]->tail_textNode = document[r_index+i]->tail_textNode;

                document[r_index+i]->head_textNode = NULL;
                document[r_index+i]->tail_textNode = NULL;

                nodeInHeaven++;
                i++;

                if(document[r_index+i] == NULL){
                    document[r_index+i] = createContainer();
                }

            }
            */

            ret = 2;

        }

        /*if(&document.containers[r_index] == NULL){
            document.containers[r_index] = createContainer();
        }*/

        (&document.containers[r_index])->head_textNode = r->head_textNode;
        (&document.containers[r_index])->tail_textNode = r->tail_textNode;

    }

    r->index = 0;
    r->command = NULL;
    r->head_textNode = NULL;
    r->tail_textNode = NULL;

    nodeInDocument++;
    nodeInHell--;

    return ret;

}

void redoChange(long num){

        container_pointer container = &document.containers[num];    // a logica dovrebbe essere stato inizializzato il container se provo a farci su una redoChange

        if(container->head_textNode != NULL){

            if(container->tail_textNode == NULL){

                container->tail_textNode = container->head_textNode;
            } else{

                container->tail_textNode = container->tail_textNode->next;
            }

        }
        else{

            container_pointer heavenNode = &heaven.containers[nodeInHeaven-1];
            long key = heavenNode->index;

            text_pointer tmp_head = heavenNode->head_textNode;
            text_pointer tmp_tail = heavenNode->tail_textNode;

            if(tmp_tail == NULL){
                tmp_tail = tmp_head;
            } else{
                tmp_tail = tmp_tail->next;
            }


            /*if(document.containers[key] == NULL){
                document[key] = createContainer();
            }*/

            container_pointer dest = &document.containers[key+1];
            container_pointer src = &document.containers[key];
            long size = nodeInDocument;

            memmove(dest, src, size*sizeof(struct TextNodeContainer));

            (&document.containers[key])->head_textNode = tmp_head;
            (&document.containers[key])->tail_textNode = tmp_tail;



            /*while (document[key]->head_textNode != NULL) {

                text_pointer tmp_head_2 = document[key]->head_textNode;
                text_pointer tmp_tail_2 = document[key]->tail_textNode;

                document[key]->head_textNode = tmp_head;
                document[key]->tail_textNode = tmp_tail;

                tmp_head = tmp_head_2;
                tmp_tail = tmp_tail_2;

                key++;

                if(document[key] == NULL){
                    document[key] = createContainer();
                }

            }

            if(document[key] == NULL){
                document[key] = createContainer();
            }

            (&document.containers[key])->head_textNode = tmp_head;
            (&document.containers[key])->tail_textNode = tmp_tail;
            */

            heavenNode->index = 0;
            heavenNode->head_textNode = NULL;
            heavenNode->tail_textNode = NULL;

            nodeInDocument++;
            nodeInHeaven--;

        }

}
int redoDelete(long treeIndexToRemove, long howMany, char* historyCommand){

    if((&document.containers[treeIndexToRemove])->tail_textNode != NULL){   // da controllare
        removeToDocument(treeIndexToRemove, howMany, historyCommand);
    }

    return 1;

}

// HELPER
char* getRow(){

    char* res = fgets(tmp, ROW_LEN, stdin);     // fgets gets '\n' at the end of the string

    if (res == NULL) {
        printf("Failed to read input.\n");
        return 0;
    }

    unsigned long len = strlen(tmp);

    row = (char *)malloc(sizeof(char) * len);

    strncpy(row, tmp, len);
    strtok(row, "\n");

    return row;
}
void getBounds(char* line, long *ind1, long *ind2){

    long ret;

    ret = strtol(line, &line, 10);
    line++;
    *ind1 = ret;
    ret = strtol(line, &line, 10);
    *ind2 = ret;

}
void reallocMem(struct DynamicArray* array){

    (*array).dim += ALLOC_INCREMENT;
    (*array).containers = (container_pointer)realloc((*array).containers, (*array).dim * sizeof(struct TextNodeContainer));

    for (long i = (*array).dim-ALLOC_INCREMENT; i < (*array).dim; ++i) {

        (*array).containers[i].index = 0;
        (*array).containers[i].command = NULL;
        (*array).containers[i].head_textNode = NULL;
        (*array).containers[i].tail_textNode = NULL;
    }

}

void freeDocument(){

    long iterator = 1;

    if(iterator > document.dim){
       reallocMem(&document);

    }

    while ((&document.containers[iterator])->head_textNode != NULL) {

        if( (&document.containers[iterator])->tail_textNode == NULL ){

            (&document.containers[iterator])->command = NULL;
            (&document.containers[iterator])->head_textNode = NULL;
            (&document.containers[iterator])->tail_textNode = NULL;

        }


        iterator++;

        if(iterator > document.dim){
           reallocMem(&document);

        }
    }

}
void freeHeaven(){

    long iterator = 0;//nodeInHeaven-1;

    if(iterator > heaven.dim){
        reallocMem(&heaven);
    }

    while ((&heaven.containers[iterator])->head_textNode != NULL) {

        (&heaven.containers[iterator])->head_textNode = NULL;
        (&heaven.containers[iterator])->tail_textNode = NULL;

        iterator++;

        if(iterator > heaven.dim){
            reallocMem(&heaven);
        }
    }

    nodeInHeaven = 0;

}