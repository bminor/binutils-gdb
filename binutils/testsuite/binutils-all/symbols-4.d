#name: weaken '*' but not 'foo' or 'bar'
#PROG: objcopy
#objcopy: -w -W !foo -W !bar -W *
#source: symbols.s
#DUMPPROG: nm
#nm: -n

0+ D bar
0+ W foa
0+ W fob
0+ D foo
0+ W foo1
0+ W foo2

