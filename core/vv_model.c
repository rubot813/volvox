#include "vv_model.h"

cell_s* vv_model_get_cell( model_s *model_ptr, uint16_t x, uint16_t y ) {
	return &( model_ptr->cell[ ( y * model_ptr->size_y ) + x ] );
}	// vv_model_get_cell

uint16_t vv_model_get_cell_count( model_s *model_ptr ) {
	return ( model_ptr->size_x * model_ptr->size_y );
}	// vv_model_get_cell_count

void vv_model_free( model_s *model_ptr ) {
	// Если указатель валиден, очистка памяти, выделенной под ячейки
	if ( model_ptr )
		free( model_ptr->cell );
}	// vv_model_free

bool vv_model_set_height( model_s *model_ptr, uint16_t height ) {
	// Результат установки высоты модели
	bool result = true;

	// Проверка возможности изменения высоты модели
	uint32_t max_height = ( model_ptr->size_z + height );
	if ( ( bool )( max_height >> 8 ) ) {
		result = false;
		goto lb_set_height_end;
	}	// if max_height

	// Цикл по ячейкам модели
	for ( uint16_t x = 0; x < model_ptr->size_x; x++ )
		for ( uint16_t y = 0; y < model_ptr->size_y; y++ ) {
			// Получение указателя на ячейку модели
			cell_s *cell = vv_model_get_cell( model_ptr, x, y );

			// Получение общей высоты сегмента без нулевой
			uint8_t h_buf = ( cell->segment_height_total - cell->segment[ 0 ].height );

			// Установка высоты нулевого сегмента
			cell->segment[ 0 ].height = height;

			// Установка общей высоты сегмента
			cell->segment_height_total = ( h_buf + height );
		}	// for

lb_set_height_end:
	return result;
}	// vv_model_set_height

bool vv_model_merge( model_s *model_ptr, bool ow_flag, bool opt_flag, merge_mode_e mode ) {
	// Результат слияния модели с миром
	bool merge_result = true;

	// Цикл по ячейкам модели
	for ( uint16_t x = 0; x < model_ptr->size_x; x++ )
		for ( uint16_t y = 0; y < model_ptr->size_y; y++ ) {
			// Получение указателя на ячейку модели
			cell_s *model_cell = vv_model_get_cell( model_ptr, x, y );

			// Получение указателя на ячейку мира
			cell_s *world_cell = vv_world_get_cell( ( model_ptr->position_x + x ), ( model_ptr->position_y + y ) );

			// Слиянее ячейки мира и ячейки модели
			if ( !vv_cell_merge( world_cell, model_cell, ow_flag, mode ) );
				merge_result = false;

			// Оптимизация ячейки мира, если установлен флаг
			if ( opt_flag )
				vv_cell_optimize( world_cell );
		}	// for
	return merge_result;
}	// vv_model_merge

void vv_model_create_cube( model_s *model_ptr, color_s color, uint8_t size ) {
	// Инициализация координат модели
	model_ptr->position_x	= 0;
	model_ptr->position_y	= 0;
	model_ptr->_position_h	= 0;

	// Заполнение размеров модели
	model_ptr->size_x = size;
	model_ptr->size_y = size;
	model_ptr->size_z = size;

	// Получение количества ячеек модели
	uint16_t cell_count = vv_model_get_cell_count( model_ptr );

	// Выделение памяти под ячейки модели
	model_ptr->cell = malloc( cell_count * sizeof( cell_s ) );
	memset( model_ptr->cell, 0x00, cell_count * sizeof( cell_s ) );

	// Цикл по ячейкам модели
	for ( uint16_t i = 0; i < cell_count; i++ ) {
		// Инициализация сегментов модели
		model_ptr->cell[ i ].segment[ 0 ].color.word	= transparent_color;
		model_ptr->cell[ i ].segment[ 0 ].height		= 0;
		model_ptr->cell[ i ].segment[ 1 ].color			= color;
		model_ptr->cell[ i ].segment[ 1 ].height		= size;

		// Инициализация параметров сегментов
		model_ptr->cell[ i ].segment_count			= 2;
		model_ptr->cell[ i ].segment_height_total	= size;
	}	// for
}	// vv_model_create_cube
