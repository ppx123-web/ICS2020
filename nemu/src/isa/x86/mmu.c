#include <isa.h>
#include <memory/vaddr.h>
#include <memory/paddr.h>

#define PT_P 0x1

paddr_t isa_mmu_translate(vaddr_t vaddr, int type, int len)
{
  uint32_t va_dir_idx = ((uint32_t)vaddr >> 22) & 0x3ff;
  uint32_t va_page_table_idx = ((uint32_t)vaddr >> 12) & 0x3ff;
  uint32_t offset = vaddr & 0xfff;
  uint32_t page_dir_base = cpu.CR3&(~0xfff);
  uint32_t page_table_entry = paddr_read(page_dir_base + 4 * va_dir_idx, 4);
  if ((page_table_entry & PT_P) == 0)
  {
    printf("MMU pte fail:pte = %x pdx = %x base = %x\n",page_table_entry,va_dir_idx,cpu.CR3);
    return MEM_RET_FAIL;
  }
  else
  {
    uint32_t page_table_value = paddr_read((page_table_entry & (~0xfff)) + 4 * va_page_table_idx, 4);
    if ((page_table_value & PT_P) == 0) 
    {
      printf("MMU:page table value: %x PTE:%x base = %x pc = %x\n",page_table_value,page_table_entry&(~0xfff),cpu.CR3,cpu.pc);
      return MEM_RET_FAIL;
    }
    else
    {
      if (offset + len > PAGE_SIZE)
      {
        //printf("%d %d\n",len,offset);
        return MEM_RET_CROSS_PAGE;
      }
      else return MEM_RET_OK;
    }
  }
}

paddr_t page_table_walk(vaddr_t vaddr)
{
  uint32_t va_dir_idx = (vaddr >> 22) & 0x3ff;
  uint32_t va_page_table_idx = (vaddr >> 12) & 0x3ff;
  uint32_t offset = vaddr & 0xfff;
  uint32_t page_dir_base = cpu.CR3;
  uint32_t page_table_entry = paddr_read(page_dir_base + 4 * va_dir_idx, 4);
  assert((page_table_entry & 0x1) == 0x1);
  uint32_t page_table_value = paddr_read((page_table_entry & (~0xfff)) + 4 * va_page_table_idx, 4);
  assert((page_table_value & 0x1) == 0x1);
  paddr_t paddr = (page_table_value & (~0xfff)) | offset;
  return paddr;
}