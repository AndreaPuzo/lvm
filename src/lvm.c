#include "lvm.h"
#include <ctype.h>
#include <stdlib.h>
#include <inttypes.h>

_LVM_API const lvm_char_t *lvm_get_addr_fmt (
  size_t addr
)
{
  static const lvm_char_t *addr_fmts[4] = {
    "%02"  PRIX8  ,
    "%04"  PRIX16 ,
    "%08"  PRIX32 ,
    "%016" PRIX64
  };
  
  const lvm_char_t *addr_fmt = addr_fmts[3];
  
  if (addr <= 0xFF)
    addr_fmt = addr_fmts[0];
  else if (addr <= 0xFFFF)
    addr_fmt = addr_fmts[1];
  else if (addr <= 0xFFFFFFFF)
    addr_fmt = addr_fmts[2];
  
  return addr_fmt;
}

_LVM_API lvm_sig_t lvm_dump_chunk (
  FILE    *out       ,
  uint8_t *buf       ,
  size_t   len       ,
  size_t   chunk_ori ,
  size_t   chunk_len ,
  size_t   bpr
)
{
  size_t end = chunk_ori + chunk_len;
  
  if (end > len || bpr > 0xFF)
    return LVM_SIG_FAILURE;
  
  // get the format
  
  const lvm_char_t *addr_fmt = lvm_get_addr_fmt(end - 1);
  
  // create the header
  
  size_t j, pad = 16;
  
  if (addr_fmt[2] == '2')
    pad = 2;
  else if (addr_fmt[2] == '4')
    pad = 4;
  else if (addr_fmt[2] == '8')
    pad = 8;
  
  pad += 1;
  
  for (j = 0; j < pad + 1; ++j)
    fputc(' ', out);
  
  for (j = 0; j < bpr; ++j)
    fprintf(out, " %02X", j);
  
  for (j = 0; j < 4; ++j)
    fputc(' ', out);
  
  fprintf(out, "ASCII text\n");
  
  for (j = 0; j < pad; ++j)
    fputc(' ', out);
  
  fputc('.', out);
  
  pad = 4 * bpr + 4;
  
  for (j = 0; j < pad; ++j)
    fputc('-', out);
  
  fputc('\n', out);
  
  // dump the chunk
  
  size_t i, lim;
  
  for (i = chunk_ori; i < end; i += lim) {
    
    // print address
    
    fprintf(out, addr_fmt, i);
    fprintf(out, " |");
    
    // print chunk as hex
    
    lim = end - i;
    
    if (lim > bpr)
      lim = bpr;
    
    for (j = 0; j < lim; ++j)
      fprintf(out, " %02X", buf[i + j]);
    
    // print padding
    
    pad = 4 + 3 * (bpr - lim);
    
    for (j = 0; j < pad; ++j)
      fputc(' ', out);
    
    // print chunk as ASCII
    
    for (j = 0; j < lim; ++j) {
      if (isprint(buf[i + j]))
        fputc(buf[i + j], out);
      else
        fputc('.', out);
    }
    
    fputc('\n', out);
  }
  
  
  return LVM_SIG_SUCCESS;
}

_LVM_API lvm_sig_t lvm_read_file (
  const lvm_char_t *fn  ,
  lvm_byte_t      **src ,
  size_t           *len ,
  lvm_byte_t        txt
)
{
  FILE *fp = fopen(fn, "rb");
  
  if (!fp)
    return LVM_SIG_FAILURE;
  
  fseek(fp, 0, SEEK_END);
  *len = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  
  *src = lvm_alloc(*len + txt);
  
  if (!*src) {
    fclose(fp) ;
    *len = 0   ;
    return LVM_SIG_FAILURE;
  }
  
  if (fread(*src, *len, sizeof(lvm_byte_t), fp) == EOF) {
    fclose(fp)        ;
    lvm_dealloc(*src) ;
    *src = NULL       ;
    *len = 0          ;
    return LVM_SIG_FAILURE;
  }
  
  fclose(fp);
  
  if (txt)
    (*src)[*len + txt] = 0;
  
  return LVM_SIG_SUCCESS;
}
