package body Pck is

   procedure Proc (I : Integer) is
      Not_In_Scope : Integer := 77;
   begin
      Inner.Inside_Variable := Not_In_Scope + I;
   end Proc;

end Pck;
