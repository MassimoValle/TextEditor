from subprocess import run, PIPE
from json import load

with open('autotester_settings.json') as settings:
    count = 0
    json_file = load(settings)

    while True:
        count += 1

        compileProcess = run(
            ['gcc', *json_file['gccArgs'],
             '-o', 'executable', json_file['source']],
            text=True,
            stdout=PIPE)

        if compileProcess.stderr != None:
            print(compileProcess.stderr)

        generatorProcess = run(
            ['python', 'generatore.py'],
            input = ('\n'.join((json_file['generatorArgs'] + ['\n']))),
            text=True,
            stdout=PIPE)

        print(f'try number: {count}... ', end='')

        output = open('your_output.txt', 'w')

        runProcess = run(
            ['./executable'],
            input=open('test.txt', 'rb').read(),
            stdout=output)

        diffProcess = run(
            ['diff',  'your_output.txt', 'sol.txt', '--brief', '--ignore-trailing-space'],
            stdout=PIPE)

        if len(diffProcess.stdout) == 0:
            print('OK')
        else:
            print('KO')
            print(diffProcess.stdout.decode())
            print("Please run 'diff your_output.txt sol.txt' to find differences.")
            exit(1)
