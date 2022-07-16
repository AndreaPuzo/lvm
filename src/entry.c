#include "lvm.h"
#include <stdlib.h>
#include <stdarg.h>

void print_addr(int n, ...)
{
  va_list ap;
  va_start(ap, n);
  
  while (n > 0) {
    printf("%s: ", va_arg(ap, char *));
    lvm_mem_addr_t addr = va_arg(ap, lvm_mem_addr_t);
    
    if (addr == LVM_MEM_INV_ADDR)
      printf("MEM_INV_ADDR");
    else
      printf(lvm_get_addr_fmt(addr), addr);
    printf("\n");
    
    --n;
  }
  
  va_end(ap);
}

void print_sig(int n, ...)
{
  va_list ap;
  va_start(ap, n);
  
  while (n > 0) {
    printf("%s: ", va_arg(ap, char *));
  
    switch (va_arg(ap, lvm_sig_t)) {
    case LVM_SIG_SUCCESS        : printf("SUCCESS\n"); break;
    case LVM_SIG_FAILURE        : printf("FAILURE\n"); break;
    case LVM_ERR_NO_MEM         : printf("(ERROR) NO_MEM\n"); break;
    case LVM_ERR_OUT_OF_MEM     : printf("(ERROR) OUT_OF_MEM\n"); break;
    case LVM_ERR_INV_MEM_ADDR   : printf("(ERROR) INV_MEM_ADDR\n"); break;
    case LVM_ERR_INV_SEG_PERM   : printf("(ERROR) INV_SEG_PERM\n"); break;
    case LVM_ERR_INV_SEG_ADDR   : printf("(ERROR) INV_SEG_ADDR\n"); break;
    case LVM_ALT_SEG_TRUNC_NAME : printf("(ALERT) SEG_TRUNC_NAME\n"); break;
    case LVM_ALT_SEG_GHOST      : printf("(ALERT) SEG_GHOST\n"); break;
    };
    
    --n;
  }
  
  va_end(ap);
}

int main(int argc, const char **argv)
{
  printf("Entry!!\n");
  
  lvm_t vm;
  
  lvm_seg_t segs[5];
  
  print_sig(5,
    ".text"  , lvm_seg_make(segs + 0, ".text"  ,    0 , 128 , 4 , LVM_SEG_PERM_X | LVM_SEG_PERM_R) ,
    ".data"  , lvm_seg_make(segs + 1, ".data"  ,  256 , 256 , 1 , LVM_SEG_PERM_R)                  ,
    ".bss"   , lvm_seg_make(segs + 2, ".bss"   ,  512 , 512 , 1 , LVM_SEG_PERM_R | LVM_SEG_PERM_W) ,
    ".stack" , lvm_seg_make(segs + 3, ".stack" , 1024 , 512 , 0 , LVM_SEG_PERM_R | LVM_SEG_PERM_W) ,
    "_Ghost" , lvm_seg_make(segs + 4, "_Ghost" , 1536 , 128 , 1 , 0)
  );
  
  printf("\n\nVM Creation\n\n");
  
  print_sig(1, "ctor", lvm_vm_ctor(&vm, 2048, segs, 5));
  
  const char msg[] = "Hello World!\n";
  char out_msg[256];
  
  printf("\n\nStore\n\n");
  
  print_addr(7,
    ".text"  , lvm_mem_store (&vm,    0, sizeof(msg), msg) ,
    ".data"  , lvm_mem_store (&vm,  256, sizeof(msg), msg) ,
    ".bss"   , lvm_mem_store (&vm,  512, sizeof(msg), msg) ,
    ".bss"   , lvm_mem_store (&vm,  526, sizeof(msg), msg) ,
    ".stack" , lvm_mem_store (&vm, 1024, sizeof(msg), msg) ,
    ".stack" , lvm_mem_store (&vm, 1038, sizeof(msg), msg) ,
    "_Ghost" , lvm_mem_store (&vm, 1536, sizeof(msg), msg)
  );
  
  print_sig(1, 
    "dump"   , lvm_mem_dump  (&vm, stdout, 0, 2048, 16)
  );
  
  printf("\n\nLoad\n\n");
  
  print_addr(5,
    ".text"  , lvm_mem_load  (&vm,    0, sizeof(msg), out_msg) ,
    ".data"  , lvm_mem_load  (&vm,  256, sizeof(msg), out_msg) ,
    ".bss"   , lvm_mem_load  (&vm,  512, sizeof(msg), out_msg) ,
    ".stack" , lvm_mem_load  (&vm, 1024, sizeof(msg), out_msg) ,
    "_Ghost" , lvm_mem_load  (&vm, 1536, sizeof(msg), out_msg)
  );
  
  print_sig(1,
    "dump"   , lvm_mem_dump  (&vm, stdout, 0, 2048, 16)
  );
  
  printf("\n\nVM Destruction\n\n");
  
  lvm_vm_dtor(&vm);
  
  return EXIT_SUCCESS;
}
