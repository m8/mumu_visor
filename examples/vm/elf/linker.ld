ENTRY(main)

SECTIONS
{
  . = 0x1000;
  .text : { *(.text) }
  . = 0x2000;
  .data : { *(.data) }
}
