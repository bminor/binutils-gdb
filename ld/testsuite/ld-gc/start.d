#name: --gc-sections with __start_
#ld: --gc-sections -e _start
#nm: -n
#target: *-*-linux*
#notarget: *-*-*aout *-*-*oldld

#...
[0-9a-f]+ A +__start__foo
#...
