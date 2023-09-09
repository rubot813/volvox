#ifndef VV_CAMERA_H_INCLUDED
#define VV_CAMERA_H_INCLUDED

#include <math.h>
#include "vv_world.h"

/*
    vv_camera - модуль работы с камерой и отображением (фреймбуфером)
    Использует vv_world.
*/

// = = = = Макросы = = = =

// Определение значения числа ПИ
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// = = = = Типы = = = =

// Структура фреймбуфера
typedef struct {
	uint16_t width, height;	// Ширина (X) и высота (Y) в пикселях. Должны быть кратны степени двойки!
	uint32_t *data;			// Указатель на массив пикселей размерами width * height
} framebuffer_s;

// Структура камеры
typedef struct {
	float x;				// Координата X
	float y;				// Координата Y
	float height;			// Высота
	float angle;			// Угол поворота
	int16_t horizon;		// Линия горизонта
	uint16_t distance;		// Дальность отрисовки
	float v_scale;			// Вертикальное растяжение изображения
} camera_s;

// = = = = Функции = = = =

#ifdef __cplusplus
extern "C" {
#endif

// Функция установки указателя на используемую камеру
// Принимает указатель на структуру камеры
void vv_set_camera( camera_s * );

// Функция движения камеры вперед.
// Принимает расстрояние для перемещения ( >0.0f )
void vv_camera_forward( float );

// Функция движения камеры назад.
// Принимает расстояние для перемещения ( >0.0f )
void vv_camera_backward( float );

// Функция создания фреймбуфера.
// Принимает размеры буфера ( x/w/ширина - y/h/высота)
// Возвращает указатель на созданный фреймбуфер.
// Если указатель равен NULL, значит во время создания буфера произошла ошибка, иначе успех.
framebuffer_s* vv_create_framebuffer( uint16_t, uint16_t );

// Функция очистки фреймбуфера.
// Заливает фреймбуре цветом, указанным в vv_world
void vv_clear_framebuffer( void );

#ifdef __cplusplus
}
#endif

#endif // VV_CAMERA_H_INCLUDED
