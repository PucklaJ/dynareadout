import dynareadout as dro

try:
    b = dro.Binout('test')
except RuntimeError as e:
    if str(e) != 'test: No files have been found':
        exit(1)

try:
    d = dro.D3plot('test')
except RuntimeError as e:
    if str(e) != 'No files with the name test do exist':
        print(e)
        exit(1)

try:
    k = dro.key_file_parse('test')
except RuntimeError as e:
    if not str(e).startswith('Failed to open key file'):
        print(e)
        exit(1)

print('Success')
