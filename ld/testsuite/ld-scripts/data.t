SECTIONS
{
  .text 0x1000 :
   {
     LONG (label)
     label = .;
     LONG (ADDR (.other))
   }
   .other : {}
}
