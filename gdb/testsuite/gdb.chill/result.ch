test_result:  MODULE

  DCL i INT := 5;

  simple_func: PROC () RETURNS (INT);
    DCL j INT := i;
    RESULT 10;
    i + := 2;
    RESULT j + 2;
    i + := 2;
  END simple_func;

  i := simple_func ();
  i := simple_func ();
  i * := 10;
END test_result;
