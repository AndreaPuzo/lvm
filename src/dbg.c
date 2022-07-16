#include "lvm.h"
#include <time.h>

struct ptr_t {
  void   *p ;
  size_t  n ;
  clock_t t ;
  clock_t e ;
};

struct ptr_t myptrs[128] = {
  (struct ptr_t){ NULL,0,0 }
};

struct ptr_t *getptr(void *p)
{
  for (int i = 0; i < 128; ++i) {
    if (myptrs[i].p == p)
      return myptrs + i;
  }
  
  return NULL;
}

void *myalloc(size_t n)
{
  struct ptr_t *ptr = getptr(NULL);
  
  ptr->p = malloc(n);
  
  if (!ptr->p)
    printf("(myalloc) error: cannot allocate %zu bytes\n", n);
  else {
    ptr->n = n;
    ptr->t = clock();
    printf("(myalloc:%u) %zu bytes ALLOCATED at %zu\n", ptr->t, ptr->n, (size_t)ptr->p);
  }
  
  return ptr->p;
}

void *mycalloc(size_t n, size_t s)
{
  struct ptr_t *ptr = getptr(NULL);
  
  ptr->p = calloc(n, s);
  
  if (!ptr->p)
    printf("(mycalloc) error: cannot allocate %zu bytes\n", n);
  else {
    ptr->n = n * s;
    ptr->t = clock();
    printf("(mycalloc:%u) %zu data of %zu bytes long ALLOCATED at %zu\n", ptr->t, n, s, (size_t)ptr->p);
  }
  
  return ptr->p;
}

void *myrealloc(void *p, size_t n)
{
  struct ptr_t *ptr = getptr(p);
  
  if (!ptr)
    printf("(myrealloc) error: no pointer found for %zu\n", (size_t)p);
  else {
    if (!ptr->p)
      return myalloc(n);
    
    ptr->p = realloc(ptr->p, n);
    ptr->e = clock();
    printf("(myrealloc:%u) (%zu,%zu) bytes REALLOCATED at %zu\n", ptr->e, ptr->n, n, (size_t)ptr->p);
    ptr->n = n;
    printf("(myrealloc) -- lifetime: %u\n", ptr->e - ptr->t);
    ptr->t = ptr->e;
    ptr->e = 0;
    
    return ptr->p;
  }
  
  return NULL;
}

void mydealloc(void *p)
{
  struct ptr_t *ptr = getptr(p);
  
  if (!ptr)
    printf("(mydealloc) error: no pointer found for %zu\n", (size_t)p);
  else {
    if (!ptr->p)
      printf("(mydealloc) error: no pointer found for %zu\n", (size_t)p);
    else {
      free(ptr->p);
      ptr->e = clock();
      printf("(mydealloc:%u) %zu bytes DEALLOCATED at %zu\n", ptr->e, ptr->n, (size_t)ptr->p);
      ptr->p = NULL;
      ptr->n = 0;
      printf("(mydealloc) -- lifetime: %u\n", ptr->e - ptr->t);
      ptr->e = ptr->t = 0;
    }
  }
}
