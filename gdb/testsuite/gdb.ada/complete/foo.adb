with Pck; use Pck;

procedure Foo is
   Some_Local_Variable : Integer := 1;
   External_Identical_Two : Integer := 74;
begin
   My_Global_Variable := Some_Local_Variable + 1; -- START
   Proc (External_Identical_Two);
end Foo;

