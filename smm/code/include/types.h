/**
 * @file types.h
 * @author zhao.wei (hw)
 * @brief 数据类型重定向
 * @version 0.1
 * @date 2020-07-17
 * 
 * @copyright Copyright (c) 2020
 * 
 */


#ifndef __TYPES_H__
#define __TYPES_H__


//机器位数
#define _CPU_WORD_SIZE 64

#if (_CPU_WORD_SIZE == 16)
typedef unsigned short int  u6;
typedef signed   short int  s16;

typedef unsigned long       u32;
typedef signed   long       s32;

#endif


#if (_CPU_WORD_SIZE == 32)
typedef unsigned short int  u16;
typedef signed short int    s16;

typedef unsigned int        u32;
typedef signed   int        s32;

typedef unsigned long long  u64;
typedef signed long long    s64;

#endif


#if (_CPU_WORD_SIZE == 64)
typedef unsigned short int  u16;
typedef signed short int    s16;

typedef unsigned int        u32;
typedef signed   int        s32;

typedef unsigned long       u64;
typedef signed   long       s64;

#endif
typedef unsigned char       u8;
typedef signed char         s8;

#endif
