#!/usr/bin/env python3
from random import choice

'''
Index viene spostato da Undo e Redo, e serve come indice per
capire quante istruzioni ci sono da disfare/rifare
'''
index = 0

'''
File è l'insieme delle linee di testo che l'editor produce, è
un array di stringhe
'''
file = []

'''
PUndo è un array che tiene traccia delle istruzioni, è un array
di classi Undo
'''
PUndo = []


'''
INPUT
'''

# Lunghezza massima del file
maxlen = int(input("Massima lughezza del file? [default 1024] ") or "1024")

# Numero di comandi del test
length = int(input("Quanti comandi? [default 1000] ") or "1000")

# Categoria di test
category = int(input("Che modalità di test?\n\
        0. Test con tutti i comandi possibili, in ordine casuale\n\
        1. Test simile a RollingBack, c, d e p all'inizio, u, r e p alla fine (ultimo 10%)\n\
        Scegliere il numero corrispondente [default 0] ") or "0")


'''
UNDO
'''

# Questa classe contiene le informazioni necessarie per fare gli undo
class Undo():
    def __init__(self, op, strings, pos):
        # che operazione è (change o delete)
        self.Op = op
        # che stringhe vengono cambiate
        self.Strings = strings
        # in che posizione vengono cambiate
        self.Pos = pos

# Undo di un'istruzione Change
def ChangeUndo(elem):
    n0 = elem.Pos[0]
    n1 = elem.Pos[1]
    j = 0
    num = n1 - n0 + 1
    for i in range(num):
        if elem.Strings[i] == "":
            elem.Strings[i] = file.pop(n0 - 1 - i + j)
        else:
            tmp = elem.Strings[i]
            elem.Strings[i] = file[n0 - 1]
            file[n0 - 1] = tmp
            j += 1
        n0 += 1

# Undo di un'istruzione Delete
def DeleteUndo(elem):
    n0 = elem.Pos[0]
    n1 = elem.Pos[1]

    if elem.Strings == []:
        return

    save0 = n0 - 1
    for i in range(n1 - n0 + 1):
        try:
            file.insert(save0, elem.Strings[i])
        except IndexError:
            break;
        save0 += 1

# Redo di un'istruzione Change
def ChangeRedo(elem):
    n0 = elem.Pos[0]
    n1 = elem.Pos[1]
    save0 = n0 - 1
    for i in range(n1 - n0 + 1):
        if (len(file) != 0 and len(file) > save0):
            tmp = elem.Strings[i]
            elem.Strings[i] = file[save0]
            file[save0] = tmp
        else:
            file.insert(save0, elem.Strings[i])
            elem.Strings[i] = ""
        save0 += 1

# Redo di un'istruzione Delete
def DeleteRedo(elem):
    n0 = elem.Pos[0]
    n1 = elem.Pos[1]
    num = 0
    if n1 != 0:
        num = n1 - n0 + 1
        if n1 > len(file):
            num = num - (n1 - len(file))
    else:
        num = 0
    for i in range(num):
        file.pop(n0 - 1 - i)
        n0 += 1

# Flush della pila
def Flush():
    if index != len(PUndo):
        del PUndo[index:]


'''
FILE
'''

# Crea un array con le frasi da usare nelle Change
quotes = []
f = open("quotes.txt", "r")
for line in f:
    quotes.append(line)
f.close()

# Crea il test e la sua soluzione
f = open("test.txt", "w")
f1 = open("sol.txt", "w")

letters = ['c', 'p', 'd', 'u', 'r']
letters1 = ['c', 'p', 'd']
letters2 = ['u', 'r', 'p']

'''
MAIN
'''
# Ciclo principale
tenpercent = length - (length * (1/10))
for i in range(length - 1):
    # sceglie a caso una lettera tra le 5
    if category == 0:
        letter = choice(letters)
    else:
        # se è un test simil-RollingBack, mette le
        # Undo e Redo solo nell'ultimo 10%
        if i < tenpercent:
            letter = choice(letters1)
        else:
            letter = choice(letters2)

    # Undo o Redo
    if letter == 'u' or letter == 'r':
        # sceglie un numero a caso tra 0 e maxlen
        num = choice(range(maxlen))
        # crea il comando e lo mette nella stringa s
        s = '{}{}'.format(num, letter)

        # tiene traccia dell'indice: per le Undo si sposta
        # indietro, per le Redo avanti
        if letter == 'u':
            for j in range(num):
                if index == 0:
                    continue

                index -= 1
                elem = PUndo[index]
                if elem.Op == 'c':
                    ChangeUndo(elem)
                else:
                    DeleteUndo(elem)
                if index < 0:
                    index = 0
                    break;
                
        # Redo
        else:
            for j in range(num):
                if len(PUndo) == index:
                    continue

                index += 1
                elem = PUndo[index - 1]
                if elem.Op == 'c':
                    ChangeRedo(elem)
                else:
                    DeleteRedo(elem)

    # Delete o Print
    elif letter == 'd' or letter == 'p':
        # sceglie due numeri a caso tra 0 e maxlen, dove il secondo
        # è maggiore del primo
        num0 = choice(range(maxlen))
        num1 = choice(range(num0, maxlen))
        # crea il comando e lo mette nella stringa s
        s = '{},{}{}'.format(num0, num1, letter)

        # Delete
        if letter == 'd':
            Flush()

            strs = []
            if num0 == 0:
                num0 = 1
            nums = [num0, num1]
            num = num1 - num0 + 1

            for j in range(num):
                if len(file) > 0:
                    try:
                        if num0 != 0:
                            strs.append(file.pop(num0 - 1))
                    except IndexError:
                        pass

            # aggiunge il comando alla pila degli undo
            PUndo.append(Undo('d', strs, nums))

            index += 1

        # Print
        else:
            save0 = num0
            for j in range(num1 - num0 + 1):
                try:
                    if save0 == 0:
                        f1.write(".\n")
                    else:
                        f1.write(file[save0 - 1])
                except IndexError:
                    f1.write(".\n")
                save0 += 1

    # Change
    else:
        # se la lunghezza attuale del file è 0, il comando deve per forza
        # iniziare da 1
        if len(file) == 0:
            num0 = 1
        # altrimenti, numero casuale tra 1 e la lunghezza attuale + 1
        else:
            num0 = choice(range(1, len(file) + 1))
        num1 = choice(range(num0, maxlen))
        nums = [num0, num1]

        Flush()
        # crea il comando e lo mette nella stringa s
        s = '{},{}{}\n'.format(num0, num1, letter)
        # aggiunge alla stringa il numero stabilito di frasi, e il . finale
        save0 = num0
        strs = []
        for j in range(num1 - num0 + 1):
            randstr = choice(quotes)
            s += randstr
            # Add
            if save0 > len(file):
                strs.append('')
                file.append(randstr)
            # Change
            else:
                strs.append(file[save0 - 1])
                file[save0 - 1] = randstr
            save0 += 1
        s += '.'

        index += 1

        PUndo.append(Undo('c', strs, nums))

    # scrive il comando s nel test
    f.write(s + '\n')

# Scrive il q finale nel test e chiude i file
f.write('q\n')
f.close()
f1.close()
