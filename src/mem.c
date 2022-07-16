#include "lvm.h"

_LVM_API lvm_mem_addr_t lvm_mem_align (
  lvm_mem_addr_t addr ,
  lvm_mem_addr_t alg
)
{
  return ((addr + alg - 1) / alg) * alg;
}

_LVM_API lvm_seg_addr_t lvm_mem_addr_to_seg (
  lvm_t         *vm   ,
  lvm_mem_addr_t addr ,
  lvm_mem_addr_t len
)
{
  lvm_mem_addr_t end = addr + len;
  
  if (end > vm->mem.len) {
    vm->sig = LVM_ERR_OUT_OF_MEM;
    return LVM_SEG_INV_ADDR;
  }
  
  for (lvm_seg_addr_t i = 0; i < vm->mem.nseg; ++i) {
    if (vm->mem.seg[i].ori <= addr && end <= vm->mem.seg[i].end)
      return i;
  }
  
  return LVM_SEG_INV_ADDR;
}

_LVM_API lvm_sig_t lvm_mem_dump (
  lvm_t         *vm   ,
  FILE          *out  ,
  lvm_mem_addr_t addr ,
  lvm_mem_addr_t len  ,
  size_t         bpr
)
{
  lvm_mem_addr_t end = addr + len;
  
  if (end > vm->mem.len)
    vm->sig = LVM_ERR_OUT_OF_MEM;
  else {
    int entire = 0;
    
    if (!addr && (len == vm->mem.len)) {
      entire = 1;
      
      fprintf(out, "Memory [ ");
      fprintf(out, lvm_get_addr_fmt(addr), addr);
      fprintf(out, " ; ");
      fprintf(out, lvm_get_addr_fmt(end), end);
      fprintf(out, " ) - %zu bytes\n", len);
    } else {
      vm->sig = LVM_SIG_UNSET;
      
      lvm_seg_addr_t seg = lvm_mem_addr_to_seg(vm, addr, len);
      
      if (seg == LVM_SEG_INV_ADDR) {
        if (vm->sig != LVM_SIG_UNSET)
          return vm->sig;
        
        // generic introduction
        
        fprintf(out, "In Memory [ ");
        fprintf(out, lvm_get_addr_fmt(addr), addr);
        fprintf(out, " ; ");
        fprintf(out, lvm_get_addr_fmt(end), end);
        fprintf(out, " ) - %zu bytes\n", len);
      } else {
        if ((addr == vm->mem.seg[seg].ori) && (len == vm->mem.seg[seg].len)) {
          
          // entire segment
          
          lvm_seg_dump(vm, out, seg, bpr);
        } else {
          
          // segment chunk
          
          fprintf(out, "In Segment %s [ ", vm->mem.seg[seg].name);
          fprintf(out, lvm_get_addr_fmt(addr), addr);
          fprintf(out, " ; ");
          fprintf(out, lvm_get_addr_fmt(end), end);
          fprintf(out, " ) - %zu bytes\n", len);
        }
      }
    }
    
    vm->sig = lvm_dump_chunk(
      out         ,
      vm->mem.buf ,
      vm->mem.len ,
      addr        ,
      len         ,
      bpr
    );
    
    if (entire) {
      for (lvm_seg_addr_t i = 0; i < vm->mem.nseg; ++i) {
        fputc('\n', out);
        lvm_seg_dump(vm, out, i, bpr);
      }
      
      fputc('\n', out);
    }
  }
  
  return vm->sig;
}

_LVM_API lvm_mem_addr_t lvm_mem_load (
  lvm_t         *vm   ,
  lvm_mem_addr_t addr ,
  lvm_mem_addr_t len  ,
  lvm_byte_t    *data
)
{
  vm->sig = LVM_SIG_UNSET; // unset the signal

  // get the alignment

  lvm_seg_addr_t seg = lvm_mem_addr_to_seg(vm, addr, len);
  lvm_mem_addr_t alg = sizeof(lvm_byte_t); // default alignment
  
  if (seg == LVM_SEG_INV_ADDR) {
    if (vm->sig != LVM_SIG_UNSET)
      return LVM_MEM_INV_ADDR;
  } else {
    if (!(vm->mem.seg[seg].perm & LVM_SEG_PERM_R)) {
      vm->sig = LVM_ERR_INV_SEG_PERM;
      return LVM_MEM_INV_ADDR;
    }
    
    alg = vm->mem.seg[seg].alg;
  }
  
  // align the address
  
  lvm_mem_addr_t alg_addr = lvm_mem_align(addr, alg);
  
  if (alg_addr != addr) {
    lvm_seg_addr_t alg_seg  = lvm_mem_addr_to_seg(vm, alg_addr, len);
  
    if (alg_seg != seg) {
      vm->sig = LVM_ERR_INV_SEG_ADDR;
      return LVM_MEM_INV_ADDR;
    }
  }
  
  // load from memory
  
  lvm_mem_addr_t end = alg_addr + len;
  
  for (addr = 0; addr < len; ++addr)
    data[addr] = vm->mem.buf[alg_addr + addr];
  
  return end;
}

_LVM_API lvm_mem_addr_t lvm_mem_store (
  lvm_t            *vm   ,
  lvm_mem_addr_t    addr ,
  lvm_mem_addr_t    len  ,
  const lvm_byte_t *data
)
{
  vm->sig = LVM_SIG_UNSET; // unset the signal

  // get the alignment

  lvm_seg_addr_t seg = lvm_mem_addr_to_seg(vm, addr, len);
  lvm_mem_addr_t alg = sizeof(lvm_byte_t); // default alignment
  
  if (seg == LVM_SEG_INV_ADDR) {
    if (vm->sig != LVM_SIG_UNSET)
      return LVM_MEM_INV_ADDR;
  } else {
    if (!(vm->mem.seg[seg].perm & LVM_SEG_PERM_W)) {
      vm->sig = LVM_ERR_INV_SEG_PERM;
      return LVM_MEM_INV_ADDR;
    }
    
    alg = vm->mem.seg[seg].alg;
  }
  
  // align the address
  
  lvm_mem_addr_t alg_addr = lvm_mem_align(addr, alg);
  
  if (alg_addr != addr) {
    lvm_seg_addr_t alg_seg  = lvm_mem_addr_to_seg(vm, alg_addr, len);
  
    if (alg_seg != seg) {
      vm->sig = LVM_ERR_INV_SEG_ADDR;
      return LVM_MEM_INV_ADDR;
    }
  }
  
  // load from memory
  
  lvm_mem_addr_t end = alg_addr + len;
  
  for (addr = 0; addr < len; ++addr)
    vm->mem.buf[alg_addr + addr] = data[addr];
  
  return end;
}
