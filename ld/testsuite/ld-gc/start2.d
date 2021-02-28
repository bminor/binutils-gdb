#name: --gc-sections with -z start-stop-gc
#ld: --gc-sections -e _start -z start-stop-gc
#nm: -n
#target: *-*-linux* *-*-gnu* arm*-*-uclinuxfdpiceabi
#xfail: bfin-*-*linux* frv-*-*

#failif
#...
[0-9a-f]+ D +__start__foo
#...
