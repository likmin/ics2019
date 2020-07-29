#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

/* dhrystone原来一直死循环
 * 参考：https://github.com/CX-514/ics2019.git
 */
size_t strlen(const char *s) {
  size_t i = 0;
  while(s[i] != '\0') i++;
  return i;
}

char *strcpy(char* dst,const char* src) {

  size_t i;
  for(i=0;src[i]!='\0';i++) {
    dst[i]=src[i];
  }
  dst[i]='\0';
  return dst;
  //return strncpy(dst, src, strlen(dst));
}

char* strncpy(char* dst, const char* src, size_t n) {
  
  uint32_t i;
  for (i = 0; i < n && src[i]!='\0'; i++) 
	  dst[i] = src[i];
  
  for (;i<n;i++)
    dst[i] = '\0';

  return dst;
}

char* strcat(char* dst, const char* src) {
  // strcpy (dst + strlen(src), src);
  // return dst;
  char *str=dst;
  assert(dst!=NULL && src!=NULL);
  while(*dst!='\0') {
    dst++;
  }
  while((*dst++ = *src++) != '\0');
  return str;
}

int strcmp(const char* s1, const char* s2) {
  
  uint32_t i = 0;
  for (i = 0; s1[i] == s2[i]; i++) if(s1[i] == '\0') return 0;

  return ((signed char *)s1)[i] - ((signed char *)s2)[i];
  
}

int strncmp(const char* s1, const char* s2, size_t n) {
  
  int D_value = 0;
  uint32_t  i = 0;
  
  for (i = 0; *s1 && *s2 && i < n && D_value != 0; i++) {
    s1++; s2++;
  	D_value = *s1 - *s2;
  }

  return D_value;
  // int res=0;
  // while(n-- && *s1 && *s2 && (res=(*s1-*s2))==0) {
  //   s1++,s2++;
  // }
  // return res;
}

void* memset(void* v,int c,size_t n) {
  const unsigned char uc = c; /* only cat the lowest c */
  unsigned char *vu = v;
  for (; n > 0; ++vu, --n)
	  *vu = uc;
  return v;
}

void* memcpy(void* out, const void* in, size_t n) {
  const char *src = (char *)in;
  char *dst = (char *)out;

  while (n--) *dst++ = *src++;

  return out;
}

int memcmp(const void* s1, const void* s2, size_t n){
  if(!n) return 0;
  while(--n && *(char *)s1 == *(char *)s2) {
	  s1 = (char *)s1 + 1;
	  s2 = (char *)s2 + 1;
  }
  return (*((unsigned char *)s1) - *((unsigned char *)s2));
}

#endif
