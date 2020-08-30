//
// Created by s on 19-5-14.
//

#include "str.h"
#include "common.h"

void str_trim_crlf(char *str) {
  char *p = str + (strlen(str) - 1);
  while (*p == '\r' || *p == '\n')
    *p-- = '\0';
}

void str_split(const char *str, char *left, char *right, char c) {
  char *p = strchr(str, c);
  if (p == NULL) {
    strcpy(left, str);
  } else {
    strncpy(left, str, p - str);
    strcpy(right, p + 1);
  }
}

int str_all_space(const char *str) {
  const char *p = str;
  while (*p) {
    if (isspace(*p) == 0) {
      return 0;
    }
    ++p;
  }
  return 1;
}

void str_upper(char *str) {
  char *p = str;
  while (*p) {
    *p = toupper(*p);
    ++p;
  }
}

long long str_to_longlong(const char *str) {
  // 有些可能不含有这个函数，需要自己重新编写
  //return atoll(str);
  if (str == NULL)
    return 0;
  long long result = 0;
  long long mult = 1;
  unsigned int len = strlen(str);
  int i;
  if (len > 15)
    return 0;
  for (i = len - 1; i >= 0; --i) {
    char ch = str[i];
    long long val;
    if (ch < '0' || ch > '9')
      return 0;
    val = ch - '0';
    val *= mult;
    mult *= 10;
    result += val;
  }
  return result;
}

unsigned int str_octal_to_uint(const char *str) {
  unsigned int result = 0;
  const char *p = str;
  int non_zero_digit = 0;
  while (*p) {
    if (*p < '0' || *p > '7')
      return 0;
    if (*p != 0)
      non_zero_digit = 1;
    if (non_zero_digit != 0) {
      unsigned int val = *p - '0';
      result = result << 3;
      result += val;
    }
    ++p;
  }
  return result;
}
