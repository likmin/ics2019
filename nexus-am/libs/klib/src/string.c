#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t i = 0;
  while(s[i] && s[i] != '\0') i++;
  return i;
}

char *strcpy(char* dst,const char* src) {
  size_t dstSize = strlen(dst);
  strncpy(dst, src, dstSize);
}

char* strncpy(char* dst, const char* src, size_t n) {
  size_t srcSize = strlen(src) + 1; // for '\0'
  if(n > srcSize) n = srcSize;

  uint32_t i;
  for (i = 0; i < n; i++) {
	dst[i] = src[i];
  }

  dst[i] = '0';
  return dst;
}

char* strcat(char* dst, const char* src) {
  size_t dstSize = strlen(dst);
  size_t srcSize = strlen(src);
  
  dst = (char *)realloc(dst, dstSize + srcSize + 1);
  
  uint32_t i = 0;
  for (i = 0; i < srcSize; i++) {
	dst[dstSize+i] = src[i];
  }
  dst[i] = '\0';
  return NULL;
}

int strcmp(const char* s1, const char* s2) {
  size_t s1_size = strlen(s1);
  size_t s2_size = strlen(s2);
  
  size_t n = s1_size < s2_size ? s1_size : s2_size;
  
  strncmp(s1, s2, n)
  return 0;
}

int strncmp(const char* s1, const char* s2, size_t n) {
  size_t s1_size = strlen(s1);
  size_t s2_size = strlen(s2);
  
  if (s1_size < n) n = s1_size;
  if (s2_size < n) n = s2_size;
  
  uint32_t i = 0;
  for (i = 0; i < n; i++) {
	int D_value = s1[i] - s2[i];
	if (D_value) return D_value;
  }

  return 0;
}

void* memset(void* v,int c,size_t n) {
  const unsigned char uc = c; /* only cat the lowest c */
  unsigned char *vu;
  for (vu = v; 0 < n; ++vu, --n)
	  *vu = uc;
  return v;
}

void* memcpy(void* out, const void* in, size_t n) {
  const char *src = in;
  char *dst = out;

  while (n--) *dst++ = *src++;

  return dst;
}

int memcmp(const void* s1, const void* s2, size_t n){
  
  return 0;
}

#endif
