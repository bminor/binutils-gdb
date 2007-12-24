with Pck; use Pck;

procedure Foo is
   C : Character := 'a';
   WC : Wide_Character := 'b';
   WWC : Wide_Wide_Character := 'c';
begin
   Do_Nothing (C'Address);  -- START
   Do_Nothing (WC'Address);
   Do_Nothing (WWC'Address);
end Foo;
