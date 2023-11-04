#ifndef VV_DEFS_H_INCLUDED
#define VV_DEFS_H_INCLUDED

#include <stdint.h>     // uint, int
#include <stdbool.h>    // bool

/*
	vv_defs - заголовочный файл с типами и определениями, общими для всех файлов движка volvox.
*/

// = = = = Макросы = = = =
// Макрос порядка байт движка volvox
#if !defined(VV_BYTE_ORDER_LE) && !defined(VV_BYTE_ORDER_BE)
#if ( __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ )
#define VV_BYTE_ORDER_LE	// little endian
#else
#define VV_BYTE_ORDER_BE	// big endian
#endif	// __BYTE_ORDER__
#endif // VV_BYTE_ORDER

// = = = = Типы = = = =

// Тип математики с плавающей точкой, оптимальный для платформы ( float, double, etc )
typedef float vv_fp_math_type;

#endif // VV_DEFS_H_INCLUDED
