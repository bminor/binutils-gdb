#source: pr26936a.s
#source: pr26936b.s
#source: pr26936c.s
#as: -g
#ld: -z noseparate-code -Ttext-segment 0x10000 -z max-page-size=0x1000
#readelf: -wL

#...
CU: .*/pr26936c.s:
File name +Line number +Starting address +View +Stmt
pr26936c.s +6 +0x10[0-9a-f][0-9a-f][0-9a-f] +x
#pass
