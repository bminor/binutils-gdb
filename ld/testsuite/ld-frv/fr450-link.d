#source: fr450-linka.s -mcpu=fr400
#source: fr450-linkb.s -mcpu=fr405
#source: fr450-linkc.s -mcpu=fr450
#source: fr450-linkb.s -mcpu=fr405
#source: fr450-linka.s -mcpu=fr400
#ld: -r
#objdump: -p

.*:     file format elf32-frv
private flags = 0x8000000: -mcpu=fr450

