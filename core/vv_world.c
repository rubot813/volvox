#include "vv_world.h"

world_s *_world_ptr = NULL;							// Указатель на используемый мир

world_s* vv_create_world( uint16_t size_x, uint16_t size_y, color_u back_color ) {
	// Указатель на структуру мира
	world_s *world_ptr = NULL;

	// Получение количества выставленных бит в size_x
	uint16_t bit_count;
	__asm__ volatile( "popcnt %1, %0" : "=r"( bit_count ) : "b" ( size_x ) );

	// Признак валидности фонового цвета. Альфа всегда должна быть 255 (полностью непрозрачный)
	bool back_color_valid = ( ( back_color.word != vv_transparent_color ) && ( back_color.a == 0xff ) );

	// Если размеры мира кратны степени двойки и фоновый цвет валиден
	if ( ( bit_count == 1 ) && ( size_x == size_y ) && back_color_valid ) {
		// Выделение памяти под структуру
		world_ptr = malloc( sizeof( world_s ) );

		// Выделение памяти под ячейки мира
		world_ptr->cell = malloc( size_x * size_y * sizeof( cell_s ) );

		// Установка параметров мира
		world_ptr->size_x = size_x;
		world_ptr->size_y = size_y;
		world_ptr->background_color = back_color;
	}	// if

	return world_ptr;
}	// vv_create_world

void vv_destroy_world( world_s *world_ptr ) {
	// Если указатель валиден
	if ( world_ptr ) {
		free( world_ptr->cell );	// Удаление массива ячеек
		free( world_ptr );			// Удаление структуры мира
	}	// if world_ptr
}	// vv_destroy_world

void vv_set_world( world_s *world_ptr ) {
	// Установка указателя
	_world_ptr = world_ptr;
}	// vv_set_world

cell_s* vv_world_get_cell( uint16_t x, uint16_t y ) {
	return &( _world_ptr->cell[ ( y * _world_ptr->size_x ) + x ] );
}	// vv_world_get_cell

void vv_cell_calc_height( cell_s *cell_ptr ) {
	// Обнуление общей высоты ячейки
	cell_ptr->segment_height_total = 0;

	// Указатель на сегменты ячейки
	segment_s *seg_ptr = cell_ptr->segment;

	// Проход по всем сегментам и инкремент общей высоты
	while ( seg_ptr < ( cell_ptr->segment + cell_ptr->segment_count ) ) {
		cell_ptr->segment_height_total += seg_ptr->height;
		seg_ptr++;
	} // while segment
}	// vv_cell_calc_height

bool vv_cell_optimize( cell_s *cell_ptr ) {
	// Флаг оптимизации
	bool opt_flag = false;

	// Признак получения непустого сегмента
	bool color_seg_flag = false;

	// Проход по сегментам в обратном направлении
	for ( int8_t seg_id = ( cell_ptr->segment_count - 1 ); seg_id >= 0; seg_id-- ) {
		// Если сегмент с цветом не получен
		if ( !color_seg_flag ) {
			// Если сегмент прозрачный
			if ( cell_ptr->segment[ seg_id ].color.word == vv_transparent_color ) {
				// Обновление общей высоты ячейки
				cell_ptr->segment_height_total -= cell_ptr->segment[ seg_id ].height;

				// Обнуление высоты текущего сегмента
				cell_ptr->segment[ seg_id ].height = 0;
			} else
				// Установка флага получения непустого сегмента
				color_seg_flag = true;

			// Сегмент с цветом получен. Сравнение цвета только если высота больше 0
		} else if ( seg_id > 0 ) {
			// Если цвета двух соседних сегментов совпадают
			if ( cell_ptr->segment[ seg_id ].color.word == cell_ptr->segment[ seg_id - 1 ].color.word ) {
				// Перенос высоты на следующий сегмент
				cell_ptr->segment[ seg_id - 1 ].height += cell_ptr->segment[ seg_id ].height;

				// Обнуление высоты текущего сегмента
				cell_ptr->segment[ seg_id ].height = 0;
			}	// if
		}	// if seg_id

		// Удаление сегмента, если его высота нулевая
		if ( !( cell_ptr->segment[ seg_id ].height ) ) {
			vv_cell_remove_segment( cell_ptr, seg_id );
			opt_flag = true;	// Была произведена оптимизация
		}	// if
	}	// for seg_id
	return opt_flag;
}	// vv_cell_optimize

void vv_cell_update_null( cell_s *cell_ptr ) {
	// Указатель на сегменты ячейки
	segment_s *seg_ptr = cell_ptr->segment;

	// Проход по всем сегментам
	while ( seg_ptr < ( cell_ptr->segment + cell_ptr->segment_count ) ) {
		// Замена цвета с нулевого на прозрачный
		if ( !( seg_ptr->color.word ) )
			seg_ptr->color.word = vv_transparent_color;
		seg_ptr++;
	} // while segment
}	// vv_cell_update_null

bool vv_cell_push_back( cell_s *cell_ptr, uint8_t height ) {
	// Результат выполнения операции
	bool result = true;

	// Если количество сегментов позволяет сделать вставку
	if ( cell_ptr->segment_count < CELL_SEGMENT_COUNT ) {
		// Получение указателя на сегмент и установка параметров
		segment_s *seg_ptr = &( cell_ptr->segment[ cell_ptr->segment_count++ ] );
		seg_ptr->color.word = vv_transparent_color;
		seg_ptr->height		= height;

		// Обновление общей высоты и количества сегментов
		cell_ptr->segment_count++;
		cell_ptr->segment_height_total += height;
	} else
		result = false;
	return result;
}	// vv_cell_push_back

bool vv_cell_push_front( cell_s *cell_ptr, uint8_t height ) {
	// Результат выполнения операции
	bool result = true;

	// Если количество сегментов позволяет сделать вставку
	if ( cell_ptr->segment_count < CELL_SEGMENT_COUNT ) {
		// Сдвиг всех сегментов вверх
		for ( int8_t seg_id = ( cell_ptr->segment_count - 1 ); seg_id >= 0; seg_id-- )
			cell_ptr->segment[ seg_id + 1 ] = cell_ptr->segment[ seg_id ];

		// Инициализация нулевого сегмента
		cell_ptr->segment[ 0 ].color.word	= vv_transparent_color;
		cell_ptr->segment[ 0 ].height		= height;

		// Обновление общей высоты и количества сегментов
		cell_ptr->segment_count++;
		cell_ptr->segment_height_total += height;
	} else
		result = false;
	return result;
}	// vv_cell_push_front

void vv_write_cell_to_vb( cell_s *cell_ptr, voxel_buffer_s *vb_ptr ) {
	uint8_t color_cnt = 0;
	segment_s *seg_ptr = cell_ptr->segment;
	while ( seg_ptr < ( cell_ptr->segment + cell_ptr->segment_count ) ) {
		for ( uint8_t h = 0; h < seg_ptr->height; h++ )
			vb_ptr->color[ color_cnt++ ] = seg_ptr->color;
		seg_ptr++;
	} // while segment
	vb_ptr->count = color_cnt;
}	// vv_write_cell_to_vb

bool vv_write_vb_to_cell( voxel_buffer_s *vb_ptr, cell_s *cell_ptr ) {
	bool result = true;						// Результат выполнения операции
	segment_s *seg_ptr = cell_ptr->segment;	// Указатель на сегмент

	// Обнуление высот ячейки мира
	cell_ptr->segment_count			= 1;
	cell_ptr->segment_height_total	= 0;

	// Проверка наличия элементов в буфере вокселей
	if ( !vb_ptr->count )
		goto lb_end;

	// Инициализация нулевого сегмента ячейки
	seg_ptr->color.word	= vb_ptr->color->word;
	seg_ptr->height		= 1;

	// Последние адреса буфера вокселей и массива сегментов
	color_u *end_color_addr = ( vb_ptr->color + vb_ptr->count );
	segment_s *end_seg_addr = ( cell_ptr->segment + CELL_SEGMENT_COUNT );

	// Цикл по буферу вокселей с первого элемента
	color_u *color_ptr = ( vb_ptr->color + 1 );
	while ( color_ptr < end_color_addr ) {
		if ( color_ptr->word ^ seg_ptr->color.word ) {
			// Обновление общей высоты ячейки мира
			cell_ptr->segment_height_total += seg_ptr->height;

			// Переход к следующему сегменту
			if ( ++seg_ptr < end_seg_addr ) {
				cell_ptr->segment_count++;
				seg_ptr->color.word	= color_ptr->word;
				seg_ptr->height	= 1;
			} else {
				// Количество цветов превышено
				result = false;
				goto lb_end;
			}	// if seg_ptr
		} else
			// Цвет не изменился, увеличение высоты
			seg_ptr->height++;
		// Переход к следующему элементу буфера
		color_ptr++;
	}	// while color ptr

	// Обновление общей высоты ячейки мира
	cell_ptr->segment_height_total += seg_ptr->height;

lb_end:
	return result;
}	// vv_write_cb_to_cell

// Внутренняя функция смешивания цветов.
// Принимает указатель на цвет dst, цвет src и признак смешивания непрозрачных цветов
// Используется в vv_cell_merge
static void _color_merge_add( color_u *dst_color, color_u *src_color, bool ow_flag ) {
	if ( ( ow_flag && ( src_color->word != vv_transparent_color ) ) || ( dst_color->word == vv_transparent_color ) )
		*dst_color = *src_color;
}	// _color_merge_add

// Внутренняя функция вырезания цветов.
// Принимает указатель на цвет dst, цвет src и признак смешивания непрозрачных цветов
// Используется в vv_cell_merge
static void _color_merge_sub( color_u *dst_color, color_u *src_color, bool ow_flag ) {
	// if ( ( ow_flag && ( src_color->word != transparent_color ) ) || ( dst_color->word == transparent_color ) )
	if ( dst_color->word == src_color->word )
		dst_color->word = vv_transparent_color;
}	// _color_merge_sub

// Внутренняя встроенная функция перехода к следующему сегменту. Пропускает сегменты с высотой 0.
// Принимает указатель на ячейку мира и id текущего сегмента.
// Возвращает id следующего непустого сегмента. Если таких сегментов нет, возвращает номер последнего или +1 сегмента.
// Используется в vv_cell_merge
static uint8_t _get_next_segment( cell_s *cell_ptr, int8_t id ) {
	do {
		if ( id++ >= cell_ptr->segment_count ) {
			id = cell_ptr->segment_count;
			break;
		}	// if seg count
		// Проверка высоты сегмента
	} while ( !( cell_ptr->segment[ id ].height ) );
	return ( uint8_t )( id );
}	// _get_next_segment

bool vv_cell_merge( cell_s *dst_cell, cell_s *src_cell, bool ow_flag, merge_mode_e mode ) {
	// Ленивая инициализация массива функций по режимам слияния
	static void ( *merge_func_array[ 2 ] )( color_u *, color_u *, bool ) = {
		_color_merge_add,	// mm_addition
		_color_merge_sub	// mm_substraction
	};	// merge_func_array

	// Результат выполнения операции
	bool merge_res = true;

	// Приведение высоты ячейки dst к высоте src
	int16_t height_diff = ( src_cell->segment_height_total - dst_cell->segment_height_total );
	if ( height_diff > 0 ) {
		if ( !vv_cell_push_back( dst_cell, height_diff ) ) {
			merge_res = false;
			goto lb_cell_merge_end;
		}	// if push_back
	}	// if height_total

	// Инициализация id сегмента dst
	uint8_t dst_seg_id = _get_next_segment( dst_cell, -1 );

	// Инициализация id сегмента src с проверкой номера сегмента
	uint8_t src_seg_id = _get_next_segment( src_cell, -1 );
	if ( src_seg_id == src_cell->segment_count )
		goto lb_cell_merge_end;

	// Остаток высоты в текущем сегменте src
	int16_t src_h_least	= src_cell->segment[ src_seg_id ].height;

	// Цикл по сегментам
	while ( dst_seg_id < dst_cell->segment_count ) {
		// Обновление остатка высоты
		src_h_least -= dst_cell->segment[ dst_seg_id ].height;

		// Выбор по остатку высоты в сегменте src
		if ( src_h_least > 0 )
			// Высота src > высоты dst
			// Смешивание цветов
			merge_func_array[ mode ]( &( dst_cell->segment[ dst_seg_id ].color ), &( src_cell->segment[ src_seg_id ].color ), ow_flag );
		else {
			// Высота src <= высоты dst
			// Если есть остаток высоты в src, сплит dst по разнице высот
			if ( src_h_least < 0 ) {
				if ( !vv_cell_split_segment( dst_cell, dst_seg_id, ( dst_cell->segment[ dst_seg_id ].height + src_h_least ) ) ) {
					merge_res = false;
					goto lb_cell_merge_end;
				}	// if split
			}	// if src_h_least < 0

			// Смешивание цветов
			merge_func_array[ mode ]( &( dst_cell->segment[ dst_seg_id ].color ), &( src_cell->segment[ src_seg_id ].color ), ow_flag );

			// Переход к следующему сегменту src
			src_seg_id = _get_next_segment( src_cell, src_seg_id );
			if ( src_seg_id == src_cell->segment_count )
				goto lb_cell_merge_end;

			// Обновление оставшейся высоты в сегменте src
			src_h_least	= src_cell->segment[ src_seg_id ].height;
		}	// if src_h_least

		// Переход к следующему сегменту dst
		dst_seg_id = _get_next_segment( dst_cell, dst_seg_id );
	}	// while dst_seg_id

lb_cell_merge_end:
	return merge_res;
}	// vv_merge_cells

int8_t vv_cell_read_segment( cell_s *cell_ptr, uint8_t height ) {
	int8_t segment_id = -1;	// Результирующий id сегмента
	uint8_t h_counter = 0;	// Счетчик высоты
	for ( uint8_t seg_id = 0; seg_id < cell_ptr->segment_count; seg_id++ ) {
		uint8_t seg_height = cell_ptr->segment[ seg_id ].height;

		// Проверка сегмента по высоте
		if ( ( h_counter + seg_height ) > height ) {
			segment_id = seg_id;	// Сохранение id сегмента
			break;
		}	// if height

		// Сохранение высоты сегмента
		h_counter += seg_height;
	}	// for segment
	return segment_id;
}	// vv_cell_read_segment

bool vv_cell_insert_segment( cell_s *cell_ptr, uint8_t id, segment_s segment ) {
	// Результат выполнения операции
	bool result = true;

	// Если количество сегментов позволяет сделать вставку
	if ( cell_ptr->segment_count < CELL_SEGMENT_COUNT ) {
		// Сдвиг всех старших сегментов вверх
		for ( int8_t seg_id = ( cell_ptr->segment_count - 1 ); seg_id >= id; seg_id-- )
			cell_ptr->segment[ seg_id + 1 ] = cell_ptr->segment[ seg_id ];

		// Копирование сегмента
		cell_ptr->segment[ id ] = segment;

		// Инкремент количества сегментов
		cell_ptr->segment_count++;

		// Увеличение количества общей высоты ячейки
		cell_ptr->segment_height_total += segment.height;
	} else
		result = false;
	return result;
}	// vv_cell_insert_segment

bool vv_cell_split_segment( cell_s *cell_ptr, uint8_t id, uint8_t height ) {
	// Результат выполнения операции
	bool result = true;

	// Если задано корректное значение высоты и количество сегментов позволяет сделать вставку
	if ( height && ( height < cell_ptr->segment[ id ].height ) && ( cell_ptr->segment_count < CELL_SEGMENT_COUNT ) ) {
		// Сдвиг всех старших сегментов вверх
		for ( int8_t seg_id = ( cell_ptr->segment_count - 1 ); seg_id >= id; seg_id-- )
			cell_ptr->segment[ seg_id + 1 ] = cell_ptr->segment[ seg_id ];

		// Установка высоты разделенных сегментов. Итоговая высота не меняется
		cell_ptr->segment[ id ].height		= height;
		cell_ptr->segment[ id + 1 ].height	-= height;

		// Инкремент количества сегментов ячейки
		cell_ptr->segment_count++;
	} else
		result = false;
	return result;
}	// vv_cell_split_segment

bool vv_cell_remove_segment( cell_s *cell_ptr, uint8_t id ) {
	// Результат выполнения операции
	bool result = true;

	// Если задан корректный номер сегмента
	if ( id < cell_ptr->segment_count ) {
		// Обновление общей высоты ячейки
		cell_ptr->segment_height_total -= cell_ptr->segment[ id ].height;

		// Сдвиг всех сегментов
		while ( id < ( cell_ptr->segment_count - 1 ) ) {
			// Сдвиг сегмента
			cell_ptr->segment[ id ] = cell_ptr->segment[ id + 1 ];
			id++;
		}	// while

		// Удаление крайнего сегмента
		cell_ptr->segment[ --cell_ptr->segment_count ] = ( segment_s ){ 0 };
	} else
		result = false;
	return result;
}	// vv_cell_remove_segment

color_u vv_cell_read_voxel( cell_s *cell_ptr, uint8_t height ) {
	// Результирующий цвет вокселя
	color_u color;
	color.word = vv_transparent_color;

	// Получение id сегмента
	int8_t segment_id = vv_cell_read_segment( cell_ptr, height );

	// Если сегмент валиден, сохранение цвета
	if ( segment_id != -1 )
		color = cell_ptr->segment[ segment_id ].color;
	return color;
}	// vv_cell_read_voxel

void vv_cell_print_info( cell_s *cell_ptr ) {
	printf( "= = = = vv_cell_print_info = = = =\n" );
	printf( "seg count = %u\nseg height = %u\n", cell_ptr->segment_count, cell_ptr->segment_height_total );
	for ( uint8_t i = 0; i < CELL_SEGMENT_COUNT; i++ )
		printf( "seg[%u] color = %u, height = %u\n", i, cell_ptr->segment[ i ].color, cell_ptr->segment[ i ].height );
}	// vv_cell_print_info

void vv_vb_print_info( voxel_buffer_s *vb_ptr ) {
	printf( "= = = = vv_vb_print_info = = = =\n" );
	printf( "count = %u\n", vb_ptr->count );
	for ( uint8_t i = 0; i < vb_ptr->count; i++ )
		printf( "col[%u] = %u\n", i, vb_ptr->color[ i ].word );
}	// void vv_vb_print_info

