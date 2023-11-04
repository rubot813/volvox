#ifndef VV_PERLIN_H_INCLUDED
#define VV_PERLIN_H_INCLUDED

#include <math.h>
#include "vv_defs.h"

/*
	vv_perlin - модуль генерации шума Перлина.
	Используется для генерации эффектов/текстур/карт высот.

*/

// = = = = Макросы = = = =

// Базовое смещение по таблице для шума Перлина. Аналогично сиду.
// Значение по умолчанию: 0.
#define PERLIN_OFFSET 0

// = = = = Типы = = = =

// = = = = Функции = = = =

#ifdef __cplusplus
extern "C" {
#endif

// Функция получения значения шума Перлина в двухмерном пространстве.
// Принимает координаты X, Y, частоту и глубину(больше нуля)
// Возвращает значение шума по указанным параметрам.
vv_fp_math_type vv_perlin_2d( vv_fp_math_type, vv_fp_math_type, vv_fp_math_type, int32_t );

#ifdef __cplusplus
}
#endif

#endif // VV_PERLIN_H_INCLUDED
