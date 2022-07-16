#include "lvm.h"
#include <string.h>

_LVM_API const lvm_char_t *lvm_seg_perm_as_str (
  lvm_seg_perm_t perm
)
{
  static lvm_char_t str[4];
  
  str[0] = str[1] = str[2] = '-';
  str[3] = 0;
  
  if (perm & LVM_SEG_PERM_X)
    str[0] = 'x';
  
  if (perm & LVM_SEG_PERM_R)
    str[1] = 'r';
  
  if (perm & LVM_SEG_PERM_W)
    str[2] = 'w';
  
  return str;
}

_LVM_API lvm_sig_t lvm_seg_dump (
  lvm_t         *vm  ,
  FILE          *out ,
  lvm_seg_addr_t seg ,
  size_t         bpr
)
{
  fprintf(out, "Segment %s [ ", vm->mem.seg[seg].name);
  fprintf(out, lvm_get_addr_fmt(vm->mem.seg[seg].ori), vm->mem.seg[seg].ori);
  fprintf(out, " ; ");
  fprintf(out, lvm_get_addr_fmt(vm->mem.seg[seg].end), vm->mem.seg[seg].end);
  fprintf(out, " ) - %zu bytes, %s\n", vm->mem.seg[seg].len, lvm_seg_perm_as_str(vm->mem.seg[seg].perm));

  vm->sig = lvm_dump_chunk(
    out                  ,
    vm->mem.buf          ,
    vm->mem.len          ,
    vm->mem.seg[seg].ori ,
    vm->mem.seg[seg].len ,
    bpr
  );
  
  return vm->sig;
}

_LVM_API lvm_sig_t lvm_seg_make (
  lvm_seg_t        *seg  ,
  const lvm_char_t *name ,
  lvm_mem_addr_t    ori  ,
  lvm_mem_addr_t    len  ,
  lvm_mem_addr_t    alg  ,
  lvm_seg_perm_t    perm
)
{
  lvm_sig_t sig = LVM_SIG_SUCCESS;
  
  size_t name_len = strnlen(name, LVM_SEG_NAME_LEN);
  
  if (name[name_len])
    sig = LVM_ALT_SEG_TRUNC_NAME;
  
  if (!alg)
    alg = sizeof(lvm_reg_t);
  
  memcpy(seg->name, name, name_len + 1) ;
  seg->ori  = ori                       ;
  seg->end  = ori + len                 ;
  seg->len  = len                       ;
  seg->alg  = alg                       ;
  seg->perm = perm                      ;
  
  if (!(perm & (LVM_SEG_PERM_X | LVM_SEG_PERM_R | LVM_SEG_PERM_W)))
    sig = LVM_ALT_SEG_GHOST;
  
  return sig;
}
