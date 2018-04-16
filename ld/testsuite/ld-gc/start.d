#name: --gc-sections with __start_
#ld: --gc-sections -e _start
#nm: -n
#target: *-*-linux* *-*-gnu*
#notarget: frv-*-linux* metag-*-linux*

#...
[0-9a-f]+ D +__start__foo
#...
