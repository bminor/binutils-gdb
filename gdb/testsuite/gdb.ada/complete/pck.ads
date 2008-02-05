package Pck is

   My_Global_Variable : Integer := 1;

   Exported_Capitalized : Integer := 2;
   pragma Export (C, Exported_Capitalized, "Exported_Capitalized");

   Local_Identical_One : Integer := 4;
   Local_Identical_Two : Integer := 8;

   External_Identical_One : Integer := 19;

   package Inner is
      Inside_Variable : Integer := 3;
   end Inner;

   procedure Proc (I : Integer);

end Pck;
