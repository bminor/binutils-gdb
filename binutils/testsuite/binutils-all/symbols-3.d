#name: weaken 'fo*' but not 'foo', localize foo.
#PROG: objcopy
#objcopy: -w -W !foo -W fo* -L foo
#source: symbols.s
#DUMPPROG: nm
#nm: -n

0+ D bar
0+ W foa
0+ W fob
0+ d foo
0+ W foo1
0+ W foo2

