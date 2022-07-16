#ifndef _LVM_H
# define _LVM_H

# define _LVM_API

# include <stddef.h>
# include <stdint.h>
# include <stdlib.h>
# include <stdio.h>

# ifdef _DEBUG
void *myalloc(size_t n);
void *mycalloc(size_t n, size_t s);
void *myrealloc(void *p, size_t n);
void  mydealloc(void *p);
#   define lvm_alloc_v(n, ...)      myalloc(n)
#   define lvm_calloc_v(n, s, ...)  mycalloc(n, s)
#   define lvm_realloc_v(p, n, ...) myrealloc(p, n)
#   define lvm_dealloc_v(p, ...)    mydealloc(p)
# else
#   define lvm_alloc_v(n, ...)      malloc(n)
#   define lvm_calloc_v(n, s, ...)  calloc(n, s)
#   define lvm_realloc_v(p, n, ...) realloc(p, n)
#   define lvm_dealloc_v(p, ...)    free(p)
# endif

# define lvm_alloc(n)      lvm_alloc_v(n, 0)
# define lvm_calloc(n, s)  lvm_calloc_v(n, s, 0)
# define lvm_realloc(p, n) lvm_realloc_v(p, n, 0)
# define lvm_dealloc(p)    lvm_dealloc_v(p, 0)

typedef char     lvm_char_t     ;
typedef uint8_t  lvm_byte_t     ;
typedef uint32_t lvm_word_t     ;
typedef uint64_t lvm_mem_addr_t ;
typedef int8_t   lvm_seg_addr_t ;
typedef uint8_t  lvm_seg_perm_t ;
typedef int32_t  lvm_sig_t      ;

typedef union {
  uint64_t as_u64 ;
  int64_t  as_i64 ;
  double   as_f64 ;
  void    *as_ptr ;
} lvm_reg_t ;

# define LVM_SEG_INV_ADDR ((lvm_seg_addr_t)-1)
# define LVM_MEM_INV_ADDR ((lvm_mem_addr_t)-1)

# define LVM_SEG_NAME_LEN 31
# define LVM_SEG_PERM_X   1
# define LVM_SEG_PERM_R   2
# define LVM_SEG_PERM_W   4

typedef struct {
  lvm_char_t     name [LVM_SEG_NAME_LEN + 1] ;
  lvm_mem_addr_t ori  ;
  lvm_mem_addr_t end  ;
  lvm_mem_addr_t len  ;
  lvm_mem_addr_t alg  ;
  lvm_seg_perm_t perm ;
} lvm_seg_t ;

typedef struct {
  lvm_byte_t    *buf  ;
  lvm_mem_addr_t len  ;
  lvm_seg_t     *seg  ;
  lvm_seg_addr_t nseg ;
} lvm_mem_t ;

enum {
  LVM_REG_0 = 0 ,
  LVM_REG_1     ,
  LVM_REG_2     ,
  LVM_REG_3     ,
  LVM_REG_4     ,
  LVM_REG_5     ,
  LVM_REG_6     ,
  LVM_REG_7     ,
  LVM_REG_8     ,
  LVM_REG_9     ,
  LVM_REG_10    ,
  LVM_REG_11    ,
  LVM_REG_12    ,
  LVM_REG_13    ,
  LVM_REG_14    ,
  LVM_REG_15    ,
  LVM_REG_16    ,
  LVM_REG_17    ,
  LVM_REG_18    ,
  LVM_REG_19    ,
  LVM_REG_20    ,
  LVM_REG_21    ,
  LVM_REG_22    ,
  LVM_REG_23    ,
  LVM_REG_24    ,
  LVM_REG_25    ,
  LVM_REG_26    ,
  LVM_REG_27    ,
  LVM_REG_28    ,
  LVM_REG_29    ,
  LVM_REG_30    , LVM_REG_PC = LVM_REG_30 ,
  LVM_REG_31    , LVM_REG_FG = LVM_REG_31 ,
  
  LVM_N_REGS
};

typedef struct {
  lvm_reg_t  reg [LVM_N_REGS] ;
  lvm_word_t inst             ;
} lvm_ker_t ;

enum {
  LVM_SIG_UNSET   = 0 ,
  LVM_SIG_SUCCESS = 0 ,
  LVM_SIG_FAILURE     ,
  
  LVM_ERR_NO_MEM       ,
  LVM_ERR_OUT_OF_MEM   ,
  LVM_ERR_INV_MEM_ADDR ,
  LVM_ERR_INV_SEG_PERM ,
  LVM_ERR_INV_SEG_ADDR ,
  
  LVM_ALT_SEG_TRUNC_NAME ,
  LVM_ALT_SEG_GHOST      ,
};

typedef struct {
  lvm_ker_t ker ;
  lvm_mem_t mem ;
  lvm_sig_t sig ;
} lvm_t ;

// -----------------------------------------------------------------------------
// source: lvm.c

_LVM_API const lvm_char_t *lvm_get_addr_fmt (
  size_t addr
);

_LVM_API lvm_sig_t lvm_dump_chunk (
  FILE    *out       ,
  uint8_t *buf       ,
  size_t   len       ,
  size_t   chunk_ori ,
  size_t   chunk_len ,
  size_t   bpr         // bytes per row
);

_LVM_API lvm_sig_t lvm_read_file (
  const lvm_char_t *fn  ,
  lvm_byte_t      **src ,
  size_t           *len ,
  lvm_byte_t        txt
);

# define lvm_read_bin_file(fn, src, len) lvm_read_file(fn, src, len, 0)
# define lvm_read_txt_file(fn, src, len) lvm_read_file(fn, src, len, 1)

// -----------------------------------------------------------------------------
// source: seg.c

_LVM_API const lvm_char_t *lvm_seg_perm_as_str (
  lvm_seg_perm_t perm
);

_LVM_API lvm_sig_t lvm_seg_dump (
  lvm_t         *vm  ,
  FILE          *out ,
  lvm_seg_addr_t seg ,
  size_t         bpr
);

_LVM_API lvm_sig_t lvm_seg_make (
  lvm_seg_t        *seg  ,
  const lvm_char_t *name ,
  lvm_mem_addr_t    ori  ,
  lvm_mem_addr_t    len  ,
  lvm_mem_addr_t    alg  ,
  lvm_seg_perm_t    perm
);

// -----------------------------------------------------------------------------
// source: mem.c

_LVM_API lvm_mem_addr_t lvm_mem_align (
  lvm_mem_addr_t addr ,
  lvm_mem_addr_t alg
);

_LVM_API lvm_seg_addr_t lvm_mem_addr_to_seg (
  lvm_t         *vm   ,
  lvm_mem_addr_t addr ,
  lvm_mem_addr_t len
);

_LVM_API lvm_sig_t lvm_mem_dump (
  lvm_t         *vm   ,
  FILE          *out  ,
  lvm_mem_addr_t addr ,
  lvm_mem_addr_t len  ,
  size_t         bpr
);

_LVM_API lvm_mem_addr_t lvm_mem_load (
  lvm_t         *vm   ,
  lvm_mem_addr_t addr ,
  lvm_mem_addr_t len  ,
  lvm_byte_t    *data
);

_LVM_API lvm_mem_addr_t lvm_mem_store (
  lvm_t            *vm   ,
  lvm_mem_addr_t    addr ,
  lvm_mem_addr_t    len  ,
  const lvm_byte_t *data
);

// -----------------------------------------------------------------------------
// source: vm.c

_LVM_API lvm_sig_t lvm_vm_ctor (
  lvm_t         *vm   ,
  lvm_mem_addr_t len  ,
  lvm_seg_t     *seg  ,
  lvm_seg_addr_t nseg
);

_LVM_API void lvm_vm_dtor (
  lvm_t *vm
);

// -----------------------------------------------------------------------------

#endif
