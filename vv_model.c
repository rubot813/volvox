#include "vv_model.h"

cell_s* vv_model_get_cell( model_s *model_ptr, uint16_t x, uint16_t y ) {
	return &( model_ptr->cell[ ( y * model_ptr->size_y ) + x ] );
}	// vv_model_get_cell

uint16_t vv_model_get_cell_count( model_s *model_ptr ) {
	return ( model_ptr->size_x * model_ptr->size_y );
}	// vv_model_get_cell_count

void vv_model_free( model_s *model_ptr ) {
	// ���� ��������� �������, ������� ������, ���������� ��� ������
	if ( model_ptr )
		free( model_ptr->cell );
}	// vv_model_free

bool vv_model_set_height( model_s *model_ptr, uint16_t height ) {
	// ��������� ��������� ������ ������
	bool result = true;

	// �������� ����������� ��������� ������ ������
	uint32_t max_height = ( model_ptr->size_z + height );
	if ( ( bool )( max_height >> 8 ) ) {
		result = false;
		goto lb_set_height_end;
	}	// if max_height

	// ���� �� ������� ������
	for ( uint16_t x = 0; x < model_ptr->size_x; x++ )
		for ( uint16_t y = 0; y < model_ptr->size_y; y++ ) {
			// ��������� ��������� �� ������ ������
			cell_s *cell = vv_model_get_cell( model_ptr, x, y );

			// ��������� ����� ������ �������� ��� �������
			uint8_t h_buf = ( cell->segment_height_total - cell->segment[ 0 ].height );

			// ��������� ������ �������� ��������
			cell->segment[ 0 ].height = height;

			// ��������� ����� ������ ��������
			cell->segment_height_total = ( h_buf + height );
		}	// for

lb_set_height_end:
	return result;
}	// vv_model_set_height

bool vv_model_merge( model_s *model_ptr, bool ow_flag, bool opt_flag, merge_mode_e mode ) {
	// ��������� ������� ������ � �����
	bool merge_result = true;

	// ���� �� ������� ������
	for ( uint16_t x = 0; x < model_ptr->size_x; x++ )
		for ( uint16_t y = 0; y < model_ptr->size_y; y++ ) {
			// ��������� ��������� �� ������ ������
			cell_s *model_cell = vv_model_get_cell( model_ptr, x, y );

			// ��������� ��������� �� ������ ����
			cell_s *world_cell = vv_world_get_cell( ( model_ptr->position_x + x ), ( model_ptr->position_y + y ) );

			// ������� ������ ���� � ������ ������
			if ( !vv_cell_merge( world_cell, model_cell, ow_flag, mode ) );
				merge_result = false;

			// ����������� ������ ����, ���� ���������� ����
			if ( opt_flag )
				vv_cell_optimize( world_cell );
		}	// for
	return merge_result;
}	// vv_model_merge

void vv_model_create_cube( model_s *model_ptr, color_s color, uint8_t size ) {
	// ������������� ��������� ������
	model_ptr->position_x	= 0;
	model_ptr->position_y	= 0;
	model_ptr->_position_h	= 0;

	// ���������� �������� ������
	model_ptr->size_x = size;
	model_ptr->size_y = size;
	model_ptr->size_z = size;

	// ��������� ���������� ����� ������
	uint16_t cell_count = vv_model_get_cell_count( model_ptr );

	// ��������� ������ ��� ������ ������
	model_ptr->cell = malloc( cell_count * sizeof( cell_s ) );
	memset( model_ptr->cell, 0x00, cell_count * sizeof( cell_s ) );

	// ���� �� ������� ������
	for ( uint16_t i = 0; i < cell_count; i++ ) {
		// ������������� ��������� ������
		model_ptr->cell[ i ].segment[ 0 ].color.word	= transparent_color;
		model_ptr->cell[ i ].segment[ 0 ].height		= 0;
		model_ptr->cell[ i ].segment[ 1 ].color			= color;
		model_ptr->cell[ i ].segment[ 1 ].height		= size;

		// ������������� ���������� ���������
		model_ptr->cell[ i ].segment_count			= 2;
		model_ptr->cell[ i ].segment_height_total	= size;
	}	// for
}	// vv_model_create_cube
