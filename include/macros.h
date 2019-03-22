
#ifndef __GLOG_CONFIG_MACROS_H_
#define __GLOG_CONFIG_MACROS_H_

//--debug
#include <stdio.h>

/*
__STDC_LIMIT_MACROS 和 __STDC_CONSTANT_MACROS 是一种解决方法，允许 C++ 程序使用标准中指定的stdint.h 宏，而不在 C++ 标准中。宏( 如 UINT8_MAX 。INT64_MIN 和 INT32_C() ) 可能已经在 C++ 应用程序中定义为其他方式。为了允许用户決定是否需要为 C 定义宏，许多实现需要在 stdint.h 包含之前定义 __STDC_LIMIT_MACROS 和 __STDC_CONSTANT_MACROS 。
这不是 C++ 标准的一部分，但它已经被多个实现所采用。
*/
#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif

#define BIT_ENABLED(WORD,BITS) (((WORD)&(BITS)) != 0)
#define BIT_DISABLED(WORD,BITS) (((WORD)&(BITS)) == 0)
#define BIT_CMP_MASK(WORD,BITS,MASK) (((WORD)&(BITS)) == MASK)
#define SET_BITS(WORD,BITS) ((WORD) |= (BITS))
#define CLR_BITS(WORD,BITS) ((WORD) &= (~(BITS)))

#define MAXLEN_PATH 1024

#ifdef WIN
/*
__thread是GCC内置的线程局部存储设施，存取效率可以和全局变量相比。
__thread变量每一个线程有一份独立实体，各个线程的值互不干扰。
可以用来修饰那些带有全局性且值可能变，但是又不值得用全局变量保护的变量。
*/
#define __thread 

/*
builtin_expect这个宏定义是gcc引入的，作用是"允许程序员将最有可能执行的分支告诉编译器"。
这个指令的写法为：__builtin_expect(EXP, N)。意思是：EXP==N的概率很大。
*/
#define __builtin_expect

#define pid_t int

#define pthread_t int

#endif

#ifndef LIKELY
#define LIKELY(EXP) __builtin_expect((EXP), 1)
#endif

#ifndef UNLIKELY
#define UNLIKELY(EXP) __builtin_expect((EXP), 0)
#endif

#endif
