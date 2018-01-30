#name: --gc-sections removing __stop_
#ld: --gc-sections -e _start
#nm: -n
#target: *-*-linux* *-*-gnu*
#notarget: *-*-*aout *-*-*oldld frv-*-linux* metag-*-linux*

#failif
#...
[0-9a-f]+ D +__stop__foo
#...
