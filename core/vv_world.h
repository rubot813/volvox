#ifndef VV_WORLD_H_INCLUDED
#define VV_WORLD_H_INCLUDED

#include <stdio.h>		// printf
#include <stdint.h>     // uint, int
#include <stdbool.h>    // bool
#include <stdlib.h>     // malloc, free

/*
	vv_world - модуль работы с миром, ячейками, сегментами, вокселями и цветами.
*/

// = = = = Макросы = = = =

// Количество сегментов в ячейке мира
#define CELL_SEGMENT_COUNT 16

// Количество элементов в буфере вокселей
#define VOXEL_BUFFER_ELEM_COUNT 256

// = = = = Типы = = = =

// Константа прозрачного цвета (ссылка на vv_world.o)
extern const uint32_t vv_transparent_color;

// Тип математики с плавающей точкой, оптимальный для платформы ( float, double, etc )
typedef float vv_fp_math_type;

#pragma pack(push, 1)
// Тип структуры цвета формата R/G/B/A
typedef union {
    uint32_t word;  // Единое слово в формате R/G/B/A
    struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };  // struct
} color_s;

// Структура сегмента мира
typedef struct {
    uint8_t height; // Высота сегмента
    color_s color;  // Цвет сегмента
} segment_s;

// Структура ячейки мира
typedef struct {
    uint8_t segment_count;                  // Количество использованных сегментов ( 0 - CELL_SEGMENT_COUNT )
    uint8_t segment_height_total;           // Общая высота всех сегментов
    segment_s segment[ CELL_SEGMENT_COUNT ];// Сегменты ячейки
} cell_s;
#pragma pack(pop)

// Структура мира
typedef struct {
    // Массив (матрица) ячеек мира
    cell_s *cell;

    // Размеры мира по осям X и Y
	uint16_t size_x, size_y;

    // Цвет фона
    color_s background_color;
} world_s;

// Структура буфера вокселей
typedef struct {
    color_s color[ VOXEL_BUFFER_ELEM_COUNT ];   // Массив цветов вокселей
    uint8_t count;                              // Количество цветов в color
} voxel_buffer_s;

// Перечисление режимов смешивания ячеек
typedef enum {
	mm_addition = 0,	// Добавление src к dst
	mm_substraction		// Вырезание src из dst
} merge_mode_e;

// = = = = Функции = = = =

#ifdef __cplusplus
extern "C" {
#endif

// Функция создания мира.
// Принимает размеры мира по осям X и Y и цвет фона.
// Возвращает указатель на структуру созданного мира.
// Функция не устанавливает указатель на мир. Для использования вызвать vv_set_world
world_s* vv_create_world( uint16_t, uint16_t, color_s );

// Функция удаления мира и высвобождения памяти.
// Принимает указатель на структуру мира для удаления.
void vv_destroy_world( world_s * );

// Функция установки указателя на используемый мир
// Принимает указатель на структуру мира
void vv_set_world( world_s * );

// Функция получния указателя на ячейку мира.
// Принимает координаты X и Y
cell_s* vv_world_get_cell( uint16_t, uint16_t );

// Функция расчета общей высоты сегментов ячейки мира.
// Принимает структуру ячейки мира
void vv_cell_calc_height( cell_s * );

// Функция оптимизации ячейки. Совмещает одинаковые сегменты, исключает сегменты с высотой 0, удаляет все крайние пустые сегменты.
// Принимает структуру ячейки мира
// Возвращает признак оптимизация (1 - производилась, 0 - ячейка оптимальна)
bool vv_cell_optimize( cell_s * );

// Функция добавления пустого сегмента указанного размера в конец ячейки мира.
// Принимает указатель на ячейку мира и размер сегмента (высоту) для добавления.
// Возвращает признак успеха: true - успешно, false - не хватает сегментов.
bool vv_cell_push_back( cell_s *, uint8_t );

// Функция добавления пустого сегмента указанного размера в начало ячейки мира.
// Принимает указатель на ячейку мира и размер сегмента (высоту) для добавления.
// Возвращает признак успеха: true - успешно, false - не хватает сегментов.
bool vv_cell_push_front( cell_s *, uint8_t );

// Функция записи структуры ячейки мира в буфер вокселей.
// Принимает указатель на ячейку мира и указатель на буфер вокселей.
void vv_write_cell_to_vb( cell_s *, voxel_buffer_s * );

// Функция записи буфера цвета в структуру ячейки мира.
// Принимает указатель на буфер вокселей и указатель на ячейку мира.
// Возвращает признак успеха: true - успешно, false - не хватает сегментов
bool vv_write_vb_to_cell( voxel_buffer_s *, cell_s * );

// Функция совмещения двух ячеек мира.
// Принимает указатель на ячейку мира для записи, указатель на ячейку мира для совмещения,
// признак перезаписи непрозрачных вокселей и режим смешивания.
// Возвращает признак успеха: true - успешно, false - не хватает сегментов.
bool vv_cell_merge( cell_s *, cell_s *, bool, merge_mode_e );

// Функция получения id сегмента по высоте из структуры ячейки мира.
// Принимает указатель на структуру ячейки мира и высоту вокселя.
// Возвращает id сегмента (0 .. CELL_SEGMENT_COUNT - 1), либо -1 если сегмент не найден.
int8_t vv_cell_read_segment( cell_s *, uint8_t );

// Функция вставки сегмента в структуру ячейки мира. Все старшие сегменты сдвигаются вверх.
// Принимает указатель на структуру ячейки мира, индекс сегмента и структуру сегмента.
// Возвращает признак успеха: true - успешно, false - не хватает сегментов
bool vv_cell_insert_segment( cell_s *, uint8_t, segment_s );

// Функция разделения сегмента на два.
// Принимает указатель на структуру ячейки мира, id сегмента и высоту разделения.
// Высота разделения h должна быть 0 < h < h_seg. (h_seg - высота сегмента)
// Возвращает признак успеха: true - успешно, false - не хватает сегментов либо некорректное значение высоты.
bool vv_cell_split_segment( cell_s *, uint8_t, uint8_t );

// Функция удаления сегмента.
// Принимает указатель на структуру ячейки мира и id сегмента.
// Исключает сегмент и обновляет значение общей высоты.
// Возвращает признак успеха: true - успешно, false - некорректый id сегмента.
bool vv_cell_remove_segment( cell_s *, uint8_t );

// Функция получения вокселя по высоте из структуры ячейки мира.
// Принимает указатель на структуру ячейки мира и высоту вокселя.
// Возвращает цвет вокселя, либо vv_transparent_color если сегмент не найден.
color_s vv_cell_read_voxel( cell_s *, uint8_t );

// Фунцкия вывода в консоль информации о структуре ячейки мира.
// Выводит информацию через printf.
void vv_cell_print_info( cell_s * );

// Функция вывода в консоль информации о буфере вокселей.
// Выводит информацию через printf
void vv_vb_print_info( voxel_buffer_s * );


#ifdef __cplusplus
}
#endif

#endif // VV_WORLD_H_INCLUDED
