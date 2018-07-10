#source: pr17550-2.s
#source: pr17550-1.s
#ld: -r
#readelf: -s --wide
#xfail: alpha-*-* cr16-*-* crx-*-*
# Disabled on alpha because alpha has a different .set directive.
# cr16 and crx use non-standard scripts with memory regions, which don't
# play well with comdat group sections under ld -r.

#failif
#...
 +[0-9]+: +[0-9a-f]+ +0 +OBJECT +GLOBAL +DEFAULT +UND x_alias
#...
