#name: GOT dump (readelf -A) test 1
#source: got-dump-1.s
#as: -mips3
#ld: -Tgot-dump-1.ld -shared
#readelf: -A

Primary GOT:
 Canonical gp value: 0+068000

 Reserved entries:
 * Address * Access * Initial Purpose
  0+060010 -32752\(gp\) 0+000000 Lazy resolver
  0+06001[48] -3274[48]\(gp\) 8000000+ Module pointer \(GNU extension\)

 Local entries:
 * Address * Access * Initial
  0+0600(18|20) -327(44|36)\(gp\) 0+060000
  0+0600(1c|28) -327(40|28)\(gp\) 0+060004

 Global entries:
 * Address * Access * Initial * Sym.Val. Type * Ndx Name
  0+0600(2|3)0 -327(36|20)\(gp\) 0+050020 0+050020 FUNC    UND extern
  0+0600(24|38) -327(32|12)\(gp\) 0+050000 0+050000 FUNC      7 glob
  0+0600(28|40) -327(28|04)\(gp\) 0+000000 0+000000 NOTYPE  UND undef

