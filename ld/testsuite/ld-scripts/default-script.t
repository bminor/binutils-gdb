_START = DEFINED(_START) ? _START : 0x900;
SECTIONS
{
  . = _START;
  .text : {*(.text)}
  /DISCARD/ : {*(*)}
}
