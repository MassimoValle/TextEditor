# Generatore Test API 2020

Generatore Test e Autotester in Python3 per il progetto di Algoritmi e Progettazione dell'Informatica del Politecnico di Milano, Anno Accademico 2019/2020

## Come utilizzare il generatore:

Il Generatore genera un file di test casuale, rispettoso delle specifiche del progetto, con annessa soluzione.

In particolare genera, nella cartella in cui viene eseguito:

* ```test.txt```, file di test

* ```sol.txt```, file con la soluzione

Per generare questi file, da un terminale o shell, lanciare il comando ```python3 generatore.py```

## Come utilizzare l'autotester:

* Controllare i settings in ```autotester_settings.json``` per poter personalizzare la creazione dei test del generatore

In particolare: ```generatorArgs``` contiene una lista con le scelte inviate al generatore per creare i test della simulazione

* controllare di avere il proprio file sorgente nella directory corrente, con nome ```main.c```
(e' possibile rinominare il file in maniera diversa e specificare un differente valore nel json al valore ```source```)

* lanciare il comando
```shell
python3 autotester.py
```

### Esempio di output:
```
try number: 1...OK
try number: 2...OK
try number: 3...OK
try number: 4...OK
try number: 5...OK
try number: 6...OK
try number: 7...OK
try number: 8...OK
try number: 9...OK
try number: 10... KO
Files your_output.txt and sol.txt differ

Please run 'diff your_output.txt sol.txt' to find differences.
```

Nel caso di programma funzionante il generatore andra' avanti all'infinito senza mai segnalare un **KO**.

E' possibile semplicemente interrompere il programma con **ctrl+c**
