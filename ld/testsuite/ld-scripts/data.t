SECTIONS
{
  .text 0x1000 :
   {
     LONG (label)
     label = .;
     LONG (ADDR (.other))
   }
   .other 0x2000 : {}
}
