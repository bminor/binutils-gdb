.include "t-macros.i"

	start

	loadpsw2 PSW_SM
	checkpsw2 1 PSW_SM

	loadpsw2 PSW_01
	checkpsw2 2 0 ;; PSW_01

	loadpsw2 PSW_EA
	checkpsw2 3 PSW_EA

	loadpsw2 PSW_DB
	checkpsw2 4 PSW_DB

	loadpsw2 PSW_DM
	checkpsw2 5 PSW_DM

	loadpsw2 PSW_IE
	checkpsw2 6 PSW_IE

	loadpsw2 PSW_RP
	checkpsw2 7 PSW_RP

	loadpsw2 PSW_MD
	checkpsw2 8 PSW_MD

	loadpsw2 PSW_FX|PSW_ST
	checkpsw2 9 PSW_FX|PSW_ST

	;; loadpsw2 PSW_ST
	;; checkpsw2 10 

	loadpsw2 PSW_10
	checkpsw2 11 0 ;; PSW_10

	loadpsw2 PSW_11
	checkpsw2 12 0 ;; PSW_11

	loadpsw2 PSW_F0
	checkpsw2 13 PSW_F0

	loadpsw2 PSW_F1
	checkpsw2 14 PSW_F1

	loadpsw2 PSW_14
	checkpsw2 15 0 ;; PSW_14

	loadpsw2 PSW_C
	checkpsw2 16 PSW_C

	exit0
