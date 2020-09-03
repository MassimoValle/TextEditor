#!/usr/bin/env python3
from random import choice

'''
Precisazione: Nei commenti, viene chiamato "file" l'insieme delle
linee di testo che l'editor produce, a prescindere dalla struttura
dati utilizzata
'''

# Costanti
MAXLEN = 1024
MAXNUM = 25
MAXUNDO = 5
letters = ['c', 'd', 'p', 'u']

'''
Maxfilelen è una lista che tiene traccia continuamente della
lunghezza del file, in modo da avere un riferimento da usare
nelle Change
'''
maxfilelen = [0]

'''
Index viene spostato da Undo e Redo, e serve come indice di
maxfilelen per sapere qual è la dimensione del file attuale
'''
index = 0

# Numero di comandi del test
length = int(input("Quanti comandi? "))

# Crea un array con le frasi da usare nelle Change
quotes = []
f = open("./q.txt", "r")
for line in f:
    quotes.append(line)
f.close()

# Crea il test
f = open("./test.txt", "w")
f2 = open("./test_without_rows.txt", "w")

letter = ''
last_letter = ''

# Ciclo principale
for i in range(length - 1):
    # sceglie a caso una lettera tra le 5
    last_last_letter = last_letter
    last_letter = letter
    letter = choice(letters)

    if last_letter == 'u' :
        redoer = choice(range(1,10))
        if redoer <= 2 :
            letter = 'r'
    if last_letter == 'p' and last_last_letter == 'u' :
        redoer = choice(range(1,10))
        if redoer <= 9 :
            letter = 'r'

    # Undo o Redo
    if letter == 'u' or letter == 'r':
        # sceglie un numero a caso tra 0 e 1024
        num = choice(range(1, MAXUNDO))
        # crea il comando e lo mette nella stringa s
        s = '{}{}'.format(num, letter)
        s2 = s

        # tiene traccia dell'indice: per le Undo si sposta
        # indietro, per le Redo avanti
        if letter == 'u':
            index -= num
            if index < 0:
                index = 0
        else:
            index += num
            if index > len(maxfilelen) - 1:
                index = len(maxfilelen) - 1

    # Delete o Print
    elif letter == 'd' or letter == 'p':
        # sceglie due numeri a caso tra 0 e 1024, dove il secondo
        # è maggiore del primo
        num0 = choice(range(MAXNUM))
        num1 = choice(range(num0, MAXNUM))
        # crea il comando e lo mette nella stringa s
        s = '{},{}{}'.format(num0, num1, letter)
        s2 = s

        # se è una Delete, tiene traccia della nuova lunghezza del file
        if letter == 'd':
            # se non è alla fine della lista maxfilelen, vuol dire che
            # c'è stata un'Undo precedentemente, ma ora elimina tutto
            # quello che viene dopo l'indice attuale
            if index != len(maxfilelen) - 1:
                maxfilelen = maxfilelen[:index + 1]

            # se non viene eliminato niente (il file era di lunghezza
            # 0 precedentemente o sta cercando di eliminare qualcosa che
            # viene dopo il file), la lunghezza rimane invariata
            if (maxfilelen[index] == 0 or num0 > maxfilelen[index]):
                maxfilelen.append(maxfilelen[index])
            # se sta eliminando tutto il file, la lunghezza è 0
            elif maxfilelen[index] <= num1 and (num0 == 1 or num0 == 0):
                maxfilelen.append(0)
            # altrimenti, scrive la lunghezza, quella precedente meno
            # il numero di righe cancellate
            else:
                num1 = min(maxfilelen[index], num1)
                maxfilelen.append(maxfilelen[index] - (num1 - num0 + 1))
            index += 1

    # Change
    else:
        # se la lunghezza attuale del file è 0, il comando deve per forza
        # iniziare da 1
        if maxfilelen[index] == 0:
            num0 = 1
        # altrimenti, numero casuale tra 1 e la lunghezza attuale + 1
        else:
            num0 = choice(range(1, maxfilelen[index] + 1))
        num1 = choice(range(num0, MAXNUM))

        # crea il comando e lo mette nella stringa s
        s = '{},{}{}\n'.format(num0, num1, letter)
        s2 = '{},{}{}'.format(num0, num1, letter)
        # aggiunge alla stringa il numero stabilito di frasi, e il . finale
        for j in range(num1 - num0 + 1):
            s += choice(quotes)
        s += '.'

        # se non è alla fine della lista maxfilelen, vuol dire che
        # c'è stata un'Undo precedentemente, ma ora elimina tutto
        # quello che viene dopo l'indice attuale e aggiunge il nuovo
        # valore di lunghezza del file
        if index != len(maxfilelen) - 1:
            maxfilelen = maxfilelen[:index + 1]
        maxfilelen.append(max(num1, maxfilelen[index]))
        index += 1

    # scrive il comando s nel test
    f.write(s + '\n')
    f2.write(s2 + '\n')

# Scrive il q finale nel test e chiude il file di test
f.write('q\n')
f2.write('q\n')
f.close()
f2.close()
