x: MODULE

SYNMODE m_arri = ARRAY(1:5) INT;
DCL v_arri m_arri := [ -1, -2, -3, -4, -5 ];

SYNMODE m_arrui = ARRAY(1:5) UINT;
DCL v_arrui m_arrui := [ 1, 2, 3, 4, 5 ];

SYNMODE r1 = RANGE (1:5);
SYNMODE m_arrb = ARRAY(r1) BYTE;
DCL v_arrb m_arrb := [ -3, -4, -5, -6, -7 ];

SYNMODE m_arrub = ARRAY(r1) UBYTE;
DCL v_arrub m_arrub := [ 3, 4, 5, 6, 7 ];

SYNMODE m_arrc = ARRAY (1:5) CHAR;
DCL v_arrc m_arrc := [ '1', '2', '3', '4', '5' ];

SYNMODE m_ps = POWERSET r1;
DCL v_ps m_ps := [ 1, 3, 5 ];

DCL v_cv CHARS(20) VARYING := "foo";

SYNMODE m_arrbool = ARRAY(r1) BOOL;
DCL v_arrbool m_arrbool := [ true, false, true, false, true ];

DCL j r1 := 4;

DCL i INT;

i := 0;

END x;
