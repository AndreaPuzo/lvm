#include "lvm.h"
#include <stdlib.h>
#include <string.h>

_LVM_API lvm_sig_t lvm_vm_ctor (
  lvm_t         *vm   ,
  lvm_mem_addr_t len  ,
  lvm_seg_t     *seg  ,
  lvm_seg_addr_t nseg
)
{
  lvm_seg_addr_t i;

  // kernel
  
  for (i = 0; i < LVM_N_REGS; ++i)
    vm->ker.reg[i].as_u64 = 0;
  
  vm->ker.inst = 0;
  
  // memory
  
  vm->mem.buf = lvm_calloc(len, sizeof(lvm_byte_t));
  
  if (!vm->mem.buf) {
    vm->sig = LVM_ERR_NO_MEM;
    return vm->sig;
  }
  
  vm->mem.len = len;
  
  // segments
  
  vm->mem.seg = lvm_calloc(nseg, sizeof(lvm_seg_t));
  
  if (!vm->mem.seg) {
    vm->sig = LVM_ERR_NO_MEM;
    lvm_dealloc(vm->mem.buf) ;
    vm->mem.buf = NULL       ;
    vm->mem.len = len        ;
    return vm->sig;
  }
  
  for (i = 0; i < nseg; ++i) {
    strcpy(vm->mem.seg[i].name, seg[i].name) ;
    vm->mem.seg[i].ori  = seg[i].ori         ;
    vm->mem.seg[i].end  = seg[i].end         ;
    vm->mem.seg[i].len  = seg[i].len         ;
    vm->mem.seg[i].alg  = seg[i].alg         ;
    vm->mem.seg[i].perm = seg[i].perm        ;
  }
  
  vm->mem.nseg = nseg;
  
  vm->sig = LVM_SIG_SUCCESS;
  
  return vm->sig;
}

_LVM_API void lvm_vm_dtor (
  lvm_t *vm
)
{
  lvm_seg_addr_t i;

  // kernel
  
  for (i = 0; i < LVM_N_REGS; ++i)
    vm->ker.reg[i].as_u64 = 0;
  
  vm->ker.inst = 0;
  
  // memory
  
  if (vm->mem.buf)
    lvm_dealloc(vm->mem.buf);
  
  vm->mem.buf = NULL ;
  vm->mem.len = 0    ;
  
  // segments
  
  if (vm->mem.seg)
    lvm_dealloc(vm->mem.seg);
  
  vm->mem.seg  = NULL ;
  vm->mem.nseg = 0    ;
}
