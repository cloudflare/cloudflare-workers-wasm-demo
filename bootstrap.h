// =======================================================================================
// Extremely minimal bootstrap library.
//
// This implements the bare minimum of libc needed in order to be able to compile our
// image resizer code. A real application would probably use Emscripten, which provides a
// full C library, but Emscripten requires more complicated support code on the JavaScript
// side. For this simple demo, we're working from scratch.
// =======================================================================================

// Basic types and decls.
typedef   signed char        int8_t;
typedef unsigned char       uint8_t;
typedef          short      int16_t;
typedef unsigned short     uint16_t;
typedef          int        int32_t;
typedef unsigned int       uint32_t;
typedef          long long  int64_t;
typedef unsigned long long uint64_t;

typedef unsigned long size_t;
typedef unsigned char byte;
typedef unsigned int uint;

#define NULL ((void*)0)
#define INT_MAX 0x7fffffff

int abs(int i) { return i < 0 ? -i : i; }

// stdarg.h is pretty simple to replace.
typedef __builtin_va_list va_list;
#define va_start(v,l)   __builtin_va_start(v,l)
#define va_end(v)       __builtin_va_end(v)
#define va_arg(v,l)     __builtin_va_arg(v,l)

// string.h. These implementations are poorly-optimized. Oh well.
void* memcpy(void* restrict dst, const void* restrict src, size_t n) {
  byte* bdst = (byte*)dst;
  byte* bsrc = (byte*)src;
  while (n-- > 0) {
    *bdst++ = *bsrc++;
  }
  return dst;
}

void* memmove(void* dst, const void* src, size_t n) {
  byte* bdst = (byte*)dst;
  byte* bsrc = (byte*)src;
  if (bdst < bsrc) {
    while (n-- > 0) {
      *bdst++ = *bsrc++;
    }
  } else {
    bdst += n;
    bsrc += n;
    while (n-- > 0) {
      *--bdst = *--bsrc;
    }
  }
  return dst;
}

void* memset(void* restrict ptr, int c, size_t n) {
  byte* cptr = (byte*)ptr;
  while (n-- > 0) {
    *cptr++ = c;
  }
  return ptr;
}

int memcmp(const void* s1, const void* s2, size_t n) {
  byte* b1 = (byte*)s1;
  byte* b2 = (byte*)s2;
  while (n-- > 0) {
    int d = *b1++ - *b2++;
    if (d != 0) return d;
  }
  return 0;
}

// Try extra-hard to make sure the compiler uses its built-in intrinsics rather than
// our crappy implementations.
#define memcpy __builtin_memcpy
#define memset __builtin_memset
#define memcmp __builtin_memcmp
#define memmove __builtin_memmove

// Really trivial malloc() implementation. We just allocate bytes sequentially from the start of
// the heap, and reset the whole heap to empty at the start of each request.
extern byte __heap_base;   // Start of heap -- symbol provided by compiler.

byte* heap = NULL;         // Current heap position.
void* last_malloc = NULL;  // Last value returned by malloc(), for trivial optimizations.

void* malloc(size_t n) {
  last_malloc = heap;
  heap += n;
  return last_malloc;
}

void free(void* ptr) {
  if (ptr == last_malloc) {
    heap = last_malloc;
  } else {
    // Fragmentation. Just leak the memory.
  }
}

void* realloc(void* ptr, size_t n) {
  if (ptr == last_malloc) {
    heap = (byte*)last_malloc + n;
    return ptr;
  } else {
    void* result = malloc(n);
    memmove(result, ptr, n);
    return result;
  }
}

// Math functions used by our image library, but not by parts of the image library that we
// actually call. We stub these out.
double pow(double x, double e) { return 0; }
double fabs(double x) { return 0; }
double ceil(double x) { return 0; }
double floor(double x) { return 0; }
double frexp(double x, int *exp) { return 0; }

// Our image library calls sprintf() in one place but we don't need that code to work.
int sprintf(char *str, const char *format, ...) { return 0; }
