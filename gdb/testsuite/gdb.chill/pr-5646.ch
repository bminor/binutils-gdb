y: MODULE

<> USE_SEIZE_FILE "x.grt" <>
SEIZE a_ps;

p: PROC ();

  DCL xx a_ps;

  xx := [a, b];
END p;

p();

END y;
