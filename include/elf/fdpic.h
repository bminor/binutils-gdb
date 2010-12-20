#ifndef _ELF_FDPIC_H
#define _ELF_FDPIC_H

struct elf32_fdpic_loadseg
{
  /* Core address to which the segment is mapped.  */
  Elf32_Addr addr;
  /* VMA recorded in the program header.  */
  Elf32_Addr p_vaddr;
  /* Size of this segment in memory.  */
  Elf32_Word p_memsz;
};

struct elf32_fdpic_loadmap
{
  /* Protocol version number, must be zero.  */
  Elf32_Half version;
  /* Number of segments in this map.  */
  Elf32_Half nsegs;
  /* The actual memory map.  */
  struct elf32_fdpic_loadseg segs[/*nsegs*/];
};

#endif /* _ELF_FDPIC_H */
