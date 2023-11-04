#include "vv_color.h"

// Размер таблицы предумноженных байт (в одном измерении)
#ifndef ALPHA_PM_TABLE_SIZE
#define ALPHA_PM_TABLE_SIZE 0x100
#endif // ALPHA_PM_TABLE_SIZE

// Инициализация констант (vv_color.h)
const uint32_t vv_transparent_color		= 0xff000000;	// Альфа канал = 255 (полностью непрозрачный)
const uint8_t vv_alpha_opaque			= 0xff;
const uint8_t vv_alpha_full_transparent	= 0x00;

// Таблица числе двух умноженных байт, используется для premultipled alphablend.
static uint8_t alpha_pm_table[ ALPHA_PM_TABLE_SIZE ][ ALPHA_PM_TABLE_SIZE ];

void vv_cblend_pm_init( void ) {
	for ( uint16_t i = 0; i < ALPHA_PM_TABLE_SIZE; i++ )
		for ( uint16_t j = 0; j < ALPHA_PM_TABLE_SIZE; j++ )
			alpha_pm_table[ i ][ j ] = ( uint8_t )( ( i * j ) / 0xff );
}	// vv_cblend_pm_init

color_u vv_cblend_alpha_pm( color_u dst_color, color_u src_color ) {
	// Итоговый цвет после смешивания
	color_u res_color;

	// Если src полностью прозрачный, возвращается dst
	if ( src_color.a == vv_alpha_full_transparent ) {
		res_color = dst_color;
		goto lb_cblend_alpha_end;
	}	// if

	// Получение инверсии альфа - канала src
	uint32_t not_a = ( 0xff - src_color.a );

	// Смешивание компонент RGBA
	res_color.word = src_color.word +
					( alpha_pm_table[ not_a ][ dst_color.r ] |
					( ( uint32_t )( alpha_pm_table[ not_a ][ dst_color.g ] ) << 8 ) |
					( ( uint32_t )( alpha_pm_table[ not_a ][ dst_color.b ] ) << 16 ) |
					( ( uint32_t )( alpha_pm_table[ not_a ][ dst_color.a ] ) << 24 ) );

	// Возврат смешанного цвета
lb_cblend_alpha_end:
	return res_color;
}	// vv_cblend_alpha_pm

color_u vv_cblend_alpha( color_u dst_color, color_u src_color ) {
	// Итоговый цвет после смешивания
	color_u res_color;

	// Если src полностью прозрачный, возвращается dst
	if ( src_color.a == vv_alpha_full_transparent ) {
		res_color = dst_color;
		goto lb_cblend_alpha_end;
	}	// if

	// Получение инверсии альфа - канала src
	uint32_t not_a = ( 0xff - src_color.a );

	// Смешивание компонент RGBA
	res_color.word = src_color.word +
					( ( ( not_a * ( dst_color.r * 0xff ) ) >> 16 ) |
					( ( ( not_a * ( dst_color.g * 0xff ) ) >> 8 ) & 0xff00 ) |
					( ( ( not_a * ( dst_color.b * 0xff ) ) ) & 0xff0000 ) |
					( ( ( not_a * ( dst_color.a * 0xff ) ) << 8 ) & 0xff000000 ) );

	// Возврат смешанного цвета
lb_cblend_alpha_end:
	return res_color;
}	// vv_cblend_alpha
