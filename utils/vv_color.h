#ifndef VV_COLOR_H_INCLUDED
#define VV_COLOR_H_INCLUDED

#include <stdlib.h>     // malloc, free
#include "vv_defs.h"	// Типы движка volvox

/*
	vv_color - файл поддержки цветов движка volvox.
	Описывает объединение цвета color_u и функции смешивания в пространстве RGB/RGBA
	Реализован с поддержкой little- и big-endian платформ.
*/

// = = = = Макросы = = = =
// todo

// = = = = Типы = = = =

// Константа прозрачного цвета (ссылка на vv_color.o)
extern const uint32_t vv_transparent_color;

// Константа полностью непрозрачного альфа канала цвета (ссылка на vv_color.o)
extern const uint8_t vv_alpha_opaque;

// Константа полностью прозрачного альфа канала цвета (ссылка на vv_color.o)
extern const uint8_t vv_alpha_full_transparent;

#pragma pack(push, 1)
// Тип структуры цвета формата R/G/B/A
typedef union {
    uint32_t word;  // Единое слово в формате R/G/B/A
    struct {
#ifdef VV_BYTE_ORDER_LE	// ABGR
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
#else					// RGBA
		uint8_t a;
        uint8_t b;
        uint8_t g;
        uint8_t r;
#endif
    };  // struct
} color_u;
#pragma pack(pop)

// = = = = Функции = = = =

#ifdef __cplusplus
extern "C" {
#endif

// Функция инициализации таблицы предумноженных компонент цветов.
// Используется в vv_cblend_alpha_pm.
void vv_cblend_pm_init( void );

// Функция смешивания цветов по альфа - каналу через таблицу предумноженных компонент цветов.
// Принимает цвет dst, цвет src.
// Возвращает цвет dst, смешанный по альфа - каналу с src.
// Перед использованием необходимо вызвать vv_cblend_pm_init.
color_u vv_cblend_alpha_pm( color_u, color_u );

// Функция смешивания цветов по альфа - каналу.
// Принимает цвет dst, цвет src.
// Возвращает цвет dst, смешанный по альфа - каналу с src.
color_u vv_cblend_alpha( color_u, color_u );

// Встроенная функция установки компонент цвета.
// Принимает указатель на цвет и компоненты цвета.
inline void __attribute__( ( always_inline ) ) vv_color_set( color_u *color_ptr, uint8_t r, uint8_t g, uint8_t b, uint8_t a ) {
#ifdef VV_BYTE_ORDER_LE
	color_ptr->word = ( ( uint32_t )( a ) << 24 | ( uint32_t )( b ) << 16 | ( uint32_t )( g ) << 8 | r );
#else
	color_ptr->word = ( ( uint32_t )( r ) << 24 | ( uint32_t )( g ) << 16 | ( uint32_t )( b ) << 8 | a );
#endif	// BYTE_ORDER
}	// vv_color_set

#ifdef __cplusplus
}
#endif

#endif // VV_COLOR_H_INCLUDED
