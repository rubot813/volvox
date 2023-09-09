#include "vv_world.h"

world_s *_world_ptr = NULL;						// ��������� �� ������������ ���
const uint32_t transparent_color = 0x00000000;	// ������������� ����������� �����

world_s* vv_create_world( uint16_t size_x, uint16_t size_y, color_s back_color ) {
	// ��������� ������ ��� ���������
	world_s *world_ptr = malloc( sizeof( world_s ) );

	// ��������� ������ ��� ������ ����
	world_ptr->cell = malloc( size_x * size_y * sizeof( cell_s ) );

	// ��������� ���������� ����
	world_ptr->size_x = size_x;
	world_ptr->size_y = size_y;
	world_ptr->background_color = back_color;

	return world_ptr;
}	// vv_create_world

void vv_destroy_world( world_s *world_ptr ) {
	// ���� ��������� �������
	if ( world_ptr ) {
		free( world_ptr->cell );	// �������� ������� �����
		free( world_ptr );			// �������� ��������� ����
	}	// if world_ptr
}	// vv_destroy_world

void vv_set_world( world_s *world_ptr ) {
	// ��������� ���������
	_world_ptr = world_ptr;
}	// vv_set_world

cell_s* vv_world_get_cell( uint16_t x, uint16_t y ) {
	return &( _world_ptr->cell[ ( y * _world_ptr->size_y ) + x ] );
}	// vv_world_get_cell

void vv_cell_calc_height( cell_s *cell_ptr ) {
	cell_ptr->segment_height_total = 0;
	segment_s *seg_ptr = cell_ptr->segment;
	while ( seg_ptr < ( cell_ptr->segment + cell_ptr->segment_count ) ) {
		cell_ptr->segment_height_total += seg_ptr->height;
		seg_ptr++;
	} // while segment
}	// vv_cell_calc_height

bool vv_cell_optimize( cell_s *cell_ptr ) {
	// ���� �����������
	bool opt_flag = false;

	// ������ �� ��������� � �������� �����������
	for ( int8_t seg_id = ( cell_ptr->segment_count - 1 ); seg_id >= 0; seg_id-- ) {
		// ��������� ����� ������� ���� ������ ������ 0
		if ( seg_id > 0 ) {
			// ���� ����� ���� �������� ��������� ���������
			if ( cell_ptr->segment[ seg_id ].color.word == cell_ptr->segment[ seg_id - 1 ].color.word ) {
				// ������� ������ �� ��������� �������
				cell_ptr->segment[ seg_id - 1 ].height += cell_ptr->segment[ seg_id ].height;

				// ��������� ������ �������� ��������
				cell_ptr->segment[ seg_id ].height = 0;
			}	// if
		}	// if seg_id

		// �������� ��������, ���� ��� ������ �������
		if ( !( cell_ptr->segment[ seg_id ].height ) ) {
			vv_cell_remove_segment( cell_ptr, seg_id );
			opt_flag = true;	// ���� ����������� �����������
		}	// if
	}	// for seg_id
	return opt_flag;
}	// vv_cell_optimize

bool vv_cell_push_back( cell_s *cell_ptr, uint8_t height ) {
	// ��������� ���������� ��������
	bool result = true;

	// ���� ���������� ��������� ��������� ������� �������
	if ( cell_ptr->segment_count < CELL_SEGMENT_COUNT ) {
		// ��������� ��������� �� ������� � ��������� ����������
		segment_s *seg_ptr = &( cell_ptr->segment[ cell_ptr->segment_count++ ] );
		seg_ptr->color.word = transparent_color;
		seg_ptr->height		= height;

		// ���������� ����� ������ � ���������� ���������
		cell_ptr->segment_count++;
		cell_ptr->segment_height_total += height;
	} else
		result = false;
	return result;
}	// vv_cell_push_back

bool vv_cell_push_front( cell_s *cell_ptr, uint8_t height ) {
	// ��������� ���������� ��������
	bool result = true;

	// ���� ���������� ��������� ��������� ������� �������
	if ( cell_ptr->segment_count < CELL_SEGMENT_COUNT ) {
		// ����� ���� ��������� �����
		for ( int8_t seg_id = ( cell_ptr->segment_count - 1 ); seg_id >= 0; seg_id-- )
			cell_ptr->segment[ seg_id + 1 ] = cell_ptr->segment[ seg_id ];

		// ������������� �������� ��������
		cell_ptr->segment[ 0 ].color.word	= transparent_color;
		cell_ptr->segment[ 0 ].height		= height;

		// ���������� ����� ������ � ���������� ���������
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
	bool result = true;						// ��������� ���������� ��������
	segment_s *seg_ptr = cell_ptr->segment;	// ��������� �� �������

	// ��������� ����� ������ ����
	cell_ptr->segment_count			= 1;
	cell_ptr->segment_height_total	= 0;

	// �������� ������� ��������� � ������ ��������
	if ( !vb_ptr->count )
		goto lb_end;

	// ������������� �������� �������� ������
	seg_ptr->color.word	= vb_ptr->color->word;
	seg_ptr->height		= 1;

	// ��������� ������ ������ �������� � ������� ���������
	color_s *end_color_addr = ( vb_ptr->color + vb_ptr->count );
	segment_s *end_seg_addr = ( cell_ptr->segment + CELL_SEGMENT_COUNT );

	// ���� �� ������ �������� � ������� ��������
	color_s *color_ptr = ( vb_ptr->color + 1 );
	while ( color_ptr < end_color_addr ) {
		if ( color_ptr->word ^ seg_ptr->color.word ) {
			// ���������� ����� ������ ������ ����
			cell_ptr->segment_height_total += seg_ptr->height;

			// ������� � ���������� ��������
			if ( ++seg_ptr < end_seg_addr ) {
				cell_ptr->segment_count++;
				seg_ptr->color.word	= color_ptr->word;
				seg_ptr->height	= 1;
			} else {
				// ���������� ������ ���������
				result = false;
				goto lb_end;
			}	// if seg_ptr
		} else
			// ���� �� ���������, ���������� ������
			seg_ptr->height++;
		// ������� � ���������� �������� ������
		color_ptr++;
	}	// while color ptr

	// ���������� ����� ������ ������ ����
	cell_ptr->segment_height_total += seg_ptr->height;

lb_end:
	return result;
}	// vv_write_cb_to_cell

// ���������� ������� ���������� ������.
// ��������� ��������� �� ���� dst, ���� src � ������� ���������� ������������ ������
// ������������ � vv_cell_merge
void _color_merge_add( color_s *dst_color, color_s *src_color, bool ow_flag ) {
	if ( ( ow_flag && ( src_color->word != transparent_color ) ) || ( dst_color->word == transparent_color ) )
		*dst_color = *src_color;
}	// _color_merge_add

// ���������� ������� ��������� ������.
// ��������� ��������� �� ���� dst, ���� src � ������� ���������� ������������ ������
// ������������ � vv_cell_merge
void _color_merge_sub( color_s *dst_color, color_s *src_color, bool ow_flag ) {
	// if ( ( ow_flag && ( src_color->word != transparent_color ) ) || ( dst_color->word == transparent_color ) )
	if ( dst_color->word == src_color->word )
		dst_color->word = transparent_color;
}	// _color_merge_sub

// ���������� ���������� ������� �������� � ���������� ��������. ���������� �������� � ������� 0.
// ��������� ��������� �� ������ ���� � id �������� ��������.
// ���������� id ���������� ��������� ��������. ���� ����� ��������� ���, ���������� ����� ���������� ��� +1 ��������.
// ������������ � vv_cell_merge
uint8_t _get_next_segment( cell_s *cell_ptr, int8_t id ) {
	do {
		if ( id++ >= cell_ptr->segment_count ) {
			id = cell_ptr->segment_count;
			break;
		}	// if seg count
		// �������� ������ ��������
	} while ( !( cell_ptr->segment[ id ].height ) );
	return ( uint8_t )( id );
}	// _get_next_segment

bool vv_cell_merge( cell_s *dst_cell, cell_s *src_cell, bool ow_flag, merge_mode_e mode ) {
	// ������� ������������� ������� ������� �� ������� �������
	static void ( *merge_func_array[ 2 ] )( color_s *, color_s *, bool ) = {
		_color_merge_add,	// mm_addition
		_color_merge_sub	// mm_substraction
	};	// merge_func_array

	// ��������� ���������� ��������
	bool merge_res = true;

	// ���������� ������ ������ dst � ������ src
	int16_t height_diff = ( src_cell->segment_height_total - dst_cell->segment_height_total );
	if ( height_diff > 0 ) {
		if ( !vv_cell_push_back( dst_cell, height_diff ) ) {
			merge_res = false;
			goto lb_cell_merge_end;
		}	// if push_back
	}	// if height_total

	// ������������� id �������� dst
	uint8_t dst_seg_id = _get_next_segment( dst_cell, -1 );

	// ������������� id �������� src � ��������� ������ ��������
	uint8_t src_seg_id = _get_next_segment( src_cell, -1 );
	if ( src_seg_id == src_cell->segment_count )
		goto lb_cell_merge_end;

	// ������� ������ � ������� �������� src
	int16_t src_h_least	= src_cell->segment[ src_seg_id ].height;

	// ���� �� ���������
	while ( dst_seg_id < dst_cell->segment_count ) {
		// ���������� ������� ������
		src_h_least -= dst_cell->segment[ dst_seg_id ].height;

		// ����� �� ������� ������ � �������� src
		if ( src_h_least > 0 )
			// ������ src > ������ dst
			// ���������� ������
			merge_func_array[ mode ]( &( dst_cell->segment[ dst_seg_id ].color ), &( src_cell->segment[ src_seg_id ].color ), ow_flag );
		else {
			// ������ src <= ������ dst
			// ���� ���� ������� ������ � src, ����� dst �� ������� �����
			if ( src_h_least < 0 ) {
				if ( !vv_cell_split_segment( dst_cell, dst_seg_id, ( dst_cell->segment[ dst_seg_id ].height + src_h_least ) ) ) {
					merge_res = false;
					goto lb_cell_merge_end;
				}	// if split
			}	// if src_h_least < 0

			// ���������� ������
			merge_func_array[ mode ]( &( dst_cell->segment[ dst_seg_id ].color ), &( src_cell->segment[ src_seg_id ].color ), ow_flag );

			// ������� � ���������� �������� src
			src_seg_id = _get_next_segment( src_cell, src_seg_id );
			if ( src_seg_id == src_cell->segment_count )
				goto lb_cell_merge_end;

			// ���������� ���������� ������ � �������� src
			src_h_least	= src_cell->segment[ src_seg_id ].height;
		}	// if src_h_least

		// ������� � ���������� �������� dst
		dst_seg_id = _get_next_segment( dst_cell, dst_seg_id );
	}	// while dst_seg_id

lb_cell_merge_end:
	return merge_res;
}	// vv_merge_cells

int8_t vv_cell_read_segment( cell_s *cell_ptr, uint8_t height ) {
	int8_t segment_id = -1;	// �������������� id ��������
	uint8_t h_counter = 0;	// ������� ������
	for ( uint8_t seg_id = 0; seg_id < cell_ptr->segment_count; seg_id++ ) {
		uint8_t seg_height = cell_ptr->segment[ seg_id ].height;

		// �������� �������� �� ������
		if ( ( h_counter + seg_height ) > height ) {
			segment_id = seg_id;	// ���������� id ��������
			break;
		}	// if height

		// ���������� ������ ��������
		h_counter += seg_height;
	}	// for segment
	return segment_id;
}	// vv_cell_read_segment

bool vv_cell_insert_segment( cell_s *cell_ptr, uint8_t id, segment_s segment ) {
	// ��������� ���������� ��������
	bool result = true;

	// ���� ���������� ��������� ��������� ������� �������
	if ( cell_ptr->segment_count < CELL_SEGMENT_COUNT ) {
		// ����� ���� ������� ��������� �����
		for ( int8_t seg_id = ( cell_ptr->segment_count - 1 ); seg_id >= id; seg_id-- )
			cell_ptr->segment[ seg_id + 1 ] = cell_ptr->segment[ seg_id ];

		// ����������� ��������
		cell_ptr->segment[ id ] = segment;

		// ��������� ���������� ���������
		cell_ptr->segment_count++;

		// ���������� ���������� ����� ������ ������
		cell_ptr->segment_height_total += segment.height;
	} else
		result = false;
	return result;
}	// vv_cell_insert_segment

bool vv_cell_split_segment( cell_s *cell_ptr, uint8_t id, uint8_t height ) {
	// ��������� ���������� ��������
	bool result = true;

	// ���� ������ ���������� �������� ������ � ���������� ��������� ��������� ������� �������
	if ( height && ( height < cell_ptr->segment[ id ].height ) && ( cell_ptr->segment_count < CELL_SEGMENT_COUNT ) ) {
		// ����� ���� ������� ��������� �����
		for ( int8_t seg_id = ( cell_ptr->segment_count - 1 ); seg_id >= id; seg_id-- )
			cell_ptr->segment[ seg_id + 1 ] = cell_ptr->segment[ seg_id ];

		// ��������� ������ ����������� ���������. �������� ������ �� ��������
		cell_ptr->segment[ id ].height		= height;
		cell_ptr->segment[ id + 1 ].height	-= height;

		// ��������� ���������� ��������� ������
		cell_ptr->segment_count++;
	} else
		result = false;
	return result;
}	// vv_cell_split_segment

bool vv_cell_remove_segment( cell_s *cell_ptr, uint8_t id ) {
	// ��������� ���������� ��������
	bool result = true;

	// ���� ����� ���������� ����� ��������
	if ( id < cell_ptr->segment_count ) {
		// ���������� ����� ������ ������
		cell_ptr->segment_height_total -= cell_ptr->segment[ id ].height;

		// ����� ���� ���������
		while ( id < ( cell_ptr->segment_count - 1 ) ) {
			// ����� ��������
			cell_ptr->segment[ id ] = cell_ptr->segment[ id + 1 ];
			id++;
		}	// while

		// �������� �������� ��������
		cell_ptr->segment[ --cell_ptr->segment_count ] = ( segment_s ){ 0 };
	} else
		result = false;
	return result;
}	// vv_cell_remove_segment

color_s vv_cell_read_voxel( cell_s *cell_ptr, uint8_t height ) {
	// �������������� ���� �������
	color_s color;
	color.word = transparent_color;

	// ��������� id ��������
	int8_t segment_id = vv_cell_read_segment( cell_ptr, height );

	// ���� ������� �������, ���������� �����
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

