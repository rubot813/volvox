#ifndef VV_VOX_H_INCLUDED
#define VV_VOX_H_INCLUDED

#include <string.h>		// memset
#include "../core/vv_model.h"

/*
	vv_vox - модуль поддержки файлов .vox
	Использует типы и функции vv_model, vv_world
	Ограничение: высота модели должна быть не более 255 вокселей.
*/

// = = = = Макросы = = = =

// Поддерживаемая версия файла
#define VOX_VERSION 150

// Макрос вывода отладочных сообщений модуля в консоль
#define VOX_PRINT_DEBUG

// = = = = Типы = = = =

// Палитра цветов по умолчанию (ссылка на vox.o) - формат RGBA
extern const uint32_t vox_default_palette[ 256 ];

// Перечисление результатов открытия файлов vox
typedef enum {
	vo_ok = 0,			// Успешно
	vo_wrong_format,	// Некорректный формат файла
	vo_memory_error,	// Ошибка памяти
	vo_read_error,		// Ошибка чтения файла
	vo_open_error,		// Ошибка открытия файла (отсутствует, занят либо нет прав)
	vo_wrong_version,	// Некорректная версия файла (не равна VOX_VERSION)
} vox_open_res_e;

#pragma pack(push, 1)
// Тип структуры заголовка файла .vox
typedef struct {
	char header[ 4 ];	// Заголовок
	uint32_t version;	// Версия файла
} vox_header_s;

// Тип структуры заголовка чанка файла .vox
typedef struct {
	char header[ 4 ];			// Заголовок
	uint32_t chunk_size;		// Размер чанка
	uint32_t children_size;		// Размер дочерних чанков
} vox_chunk_header_u;

// Тип структуры вокселя файла.vox
typedef union {
    uint32_t word;  // Единое слово в формате X/Y/Z/C
    struct {
        uint8_t x;	// Координаты вокселя в системе модели
        uint8_t y;
        uint8_t z;
        uint8_t color_index;	// Индекс цвета вокселя
    };  // struct
} vox_voxel_u;
#pragma pack(pop)

// Тип структуры модели .vox файла
typedef struct {
	uint32_t size_x;			// Размер модели по осям x, y, z
	uint32_t size_y;
	uint32_t size_z;
	uint32_t voxel_count;		// Количество вокселей в модели
	vox_voxel_u *voxel_ptr;		// Указатель на буфер вокселей
	uint32_t	*color_palette;	// Указатель на палитру цветов (формат RGBA). NULL - палитра по умолчанию (vox_default_palette)
} vox_model_s;

// Тип буфера .vox файла
typedef struct {
	uint32_t	file_size;		// Размер файла в байтах
	uint8_t		model_count;	// Количество моделей
	vox_model_s	*model_ptr;		// Указатель на массив моделей количеством model_count
} vox_buffer_s;

// = = = = Функции = = = =

#ifdef __cplusplus
extern "C" {
#endif

// Функция создания пустого буфера .vox
// Производит корректную инициализацию всех параметров структуры.
// Возвращает указатель на созданный буфер. Если указатель равен NULL, значит произошла ошибка (нехватка памяти)
vox_buffer_s* vv_vox_create_buffer( void );

// Функция очистки буфера .vox . Производит очистку памяти и удаление указателя.
// Принимает указатель на буфер
void vv_vox_free_buffer( vox_buffer_s * );

// Функция чтения модели из файла .vox
// Принимает строку, содержащую путь к файлу, указатель на структуру буфер .vox для записи.
// Возвращает результат чтения из файла
vox_open_res_e vv_vox_read_file( const char *, vox_buffer_s * );

// Функция создания модели volvox из модели .vox
// Принимает указатель на модель .vox и указатель на модель volvox для записи.
// Возвращает признак успеха создания модели:
// true - успешно, false - некорректная высота модели.
bool vv_vox_create_model( vox_model_s *, model_s * );

#ifdef __cplusplus
}
#endif

#endif // VV_VOX_H_INCLUDED
