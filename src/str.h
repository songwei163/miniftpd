//
// Created by s on 19-5-14.
//

#ifndef __STR_H__
#define __STR_H__

// 以下都为字符串工具函数

/**
 * str_trim_crlf - 去除字符串中的\r\n
 * @str - 传入的字符串
 *
 */
void str_trim_crlf(char *str);

/**
 * str_split - 按照c字符分割字符串
 * @str - 需要分割的字符串
 * @left - 分割后左边的字符串
 * @right - 分割后右边的字符串
 * @c - 分割字符
 */
void str_split(const char *str, char *left, char *right, char c);

/**
 * str_all_space - 判断字符串是否全为空字符
 * @str - 传入的字符串
 * return value - 字符串全为空字符返回1,否则为0
 */
int str_all_space(const char *str);

/**
 * str_upper - 将字符串转化为大写
 * @str - 传入的字符串
 */
void str_upper(char *str);

/**
 * str_to_longlong - 将字符串转化为长长整型
 * @str - 需要转化的字符串
 * return value - 转化后的长长整型
 */
long long str_to_longlong(const char *str);

/**
 * str_octal_to_uint - 将字符串(八进制)转化为无符号整型
 * @str - 需要转化的字符串
 * return value - 成功转化的无符号整型
 */
unsigned int str_octal_to_uint(const char *str);

#endif /* __STR_H__ */
