ss: MODULE  /* This is Cynus PR chill/5696. */

DCL s20 CHARS(20) VARYING;

DCL s10 CHARS(10);


s20 := "Moser Wilfried";
S10 := "1234567890";

WRITETEXT (stdout, "s20 := ""%C"", s10 := ""%C""%/", s20, s10);

END ss;
