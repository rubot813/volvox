#include "vv_render.h"

static voxel_buffer_s	_voxel_buffer;		// Буфер вокселей для рендеринга
extern camera_s 		*_camera_ptr;		// Указатель на используемую камеру ( vv_camera )
extern framebuffer_s	_framebuffer;		// Фреймбуфер ( vv_camera )
extern world_s			*_world_ptr;		// Указатель на мир ( vv_world )
static render_param_s   _render_param = {	// Параметры рендеринга
	.lod = {	1, 1, 1, 1,				// 0..1, 2..3, 4..7, 8..15
				1, 1, 1, 2,				// 16..31, 32..63, 64..127, 128..255
				2, 4, 8, 16,			// 256..511, 512..1023, 1024..2047, 2048..4095
				32, 64, 128, 128 }		// 4096..8191, 8192..16383, 16384..32767, 32768..65534
};	// render_param_s

void vv_set_render_param( render_param_s *render_param ) {
    _render_param = *render_param;
}   // vv_set_render_param

void vv_render_base( void ) {
	// Расчет синуса и косинуса угла поворота камеры
	vv_fp_math_type cam_angle_sin = sin( _camera_ptr->angle );
	vv_fp_math_type cam_angle_cos = cos( _camera_ptr->angle );

	// Получение левой дальней точки FOV
	vv_fp_math_type plx = ( cam_angle_cos * _camera_ptr->distance ) + ( cam_angle_sin * _camera_ptr->distance );
	vv_fp_math_type ply = ( cam_angle_sin * _camera_ptr->distance ) - ( cam_angle_cos * _camera_ptr->distance );

	// Получение правой дальней точки FOV
	vv_fp_math_type prx = ( cam_angle_cos * _camera_ptr->distance ) - ( cam_angle_sin * _camera_ptr->distance );
	vv_fp_math_type pry = ( cam_angle_sin * _camera_ptr->distance ) + ( cam_angle_cos * _camera_ptr->distance );

	// Цикл по лучам слева направо
	for ( uint16_t ray = 0; ray < _framebuffer.width; ray++ ) {
		// Расчет смещения x/y для бросания луча
		vv_fp_math_type ray_delta_x = ( plx + ( prx - plx ) / _framebuffer.width * ray ) / _camera_ptr->distance;
		vv_fp_math_type ray_delta_y = ( ply + ( pry - ply ) / _framebuffer.width * ray ) / _camera_ptr->distance;

		// Установка начальной позиции луча
		vv_fp_math_type ray_x = _camera_ptr->x;
		vv_fp_math_type ray_y = _camera_ptr->y;

		// Бросание луча в направлении от камеры
		for ( uint16_t z = 1; z < _camera_ptr->distance; z++ ) {
			// Смещение координаты точки на луче
			ray_x += ray_delta_x;
			ray_y += ray_delta_y;

			// Преобразование координаты точки в целое
			int32_t rx = ( int32_t )( ray_x );
			int32_t ry = ( int32_t )( ray_y );

			// Проверка на выход точки на луче за пределы мира
			if ( ( rx >= _world_ptr->size_x ) || ( rx < 0 ) )
				continue;
			if ( ( ry >= _world_ptr->size_y ) || ( ry < 0 ) )
				continue;

			// Получение текущей ячейки мира
			cell_s *cell_ptr = vv_world_get_cell( rx, ry );

			// Получение высоты верхней точки линии
			int16_t h_up = ( ( vv_fp_math_type )( _camera_ptr->height - cell_ptr->segment_height_total ) / z * _camera_ptr->v_scale + _camera_ptr->horizon );

			// Получение высоты нижней точки линии
			int16_t h_down = ( ( vv_fp_math_type )( _camera_ptr->height ) / z * _camera_ptr->v_scale + _camera_ptr->horizon );

			// Проверка на попадание линии в экран
			if ( ( h_up > _framebuffer.height ) || ( h_down < 0 ) )
				continue;

            // Запись буфера вокселей
            vv_write_cell_to_vb( cell_ptr, &( _voxel_buffer ) );

            // Получение соотношения пикселя экрана к вокселю в мире
			vv_fp_math_type pix_coeff = ( ( vv_fp_math_type )( cell_ptr->segment_height_total ) / ( h_down - h_up ) );

            // Расчет начального цвета вокселя в линии
			vv_fp_math_type vbuf_pos = .0f;
			if ( h_down > _framebuffer.height )
				vbuf_pos = ( h_down - _framebuffer.height ) * pix_coeff;

			// Конечная y координата отрисовки линии
			int16_t end_y = ( h_up >= 0 ) ? h_up : -1;

			// Начальная координата отрисовки и счетчик
			uint16_t fb_pix = (_framebuffer.height - 1 );
			int16_t pos_y = ( h_down < fb_pix ) ? h_down : fb_pix;

			// Отрисовка линии
			while ( pos_y > end_y ) {
				// Получение id пикселя фреймбуфера
				uint32_t id = ( pos_y-- * _framebuffer.width ) + ray;

				// Получение цвета вокселя
				uint32_t color = _voxel_buffer.color[ ( uint8_t )( vbuf_pos ) ].word;

				// Отрисовка, если цвет вокселя не прозрачный и цвет во фреймбуфере фоновый
				if ( ( color ^ vv_transparent_color ) && ( _framebuffer.data[ id ] == _world_ptr->background_color.word ) )
					_framebuffer.data[ id ] = color;

				// Смещение цвета вокселя
				vbuf_pos += pix_coeff;
            }   // while
        }   // for z
    }   // for screen size width (x)
}	// vv_render_base

void vv_render_opti( void ) {
	// Предварительный расчет старшего выставленного бита размера бита
	// Для последующей проверки точки на луче на выход за пределы мира.
	static uint16_t rng_bit;
	__asm__ volatile( "bsr %1, %0" : "=r"( rng_bit ) : "b" ( _world_ptr->size_x ) );

	// Расчет синуса и косинуса угла поворота камеры
	vv_fp_math_type cam_angle_sin = sin( _camera_ptr->angle );
	vv_fp_math_type cam_angle_cos = cos( _camera_ptr->angle );

	// Получение левой дальней точки FOV
	vv_fp_math_type plx = ( cam_angle_cos * _camera_ptr->distance ) + ( cam_angle_sin * _camera_ptr->distance );
	vv_fp_math_type ply = ( cam_angle_sin * _camera_ptr->distance ) - ( cam_angle_cos * _camera_ptr->distance );

	// Получение правой дальней точки FOV
	vv_fp_math_type prx = ( cam_angle_cos * _camera_ptr->distance ) - ( cam_angle_sin * _camera_ptr->distance );
	vv_fp_math_type pry = ( cam_angle_sin * _camera_ptr->distance ) + ( cam_angle_cos * _camera_ptr->distance );

	// Цикл по лучам слева направо
	for ( uint16_t ray = 0; ray < _framebuffer.width; ray++ ) {
		// Расчет смещения x/y для бросания луча
		vv_fp_math_type ray_delta_x = ( plx + ( prx - plx ) / _framebuffer.width * ray ) / _camera_ptr->distance;
		vv_fp_math_type ray_delta_y = ( ply + ( pry - ply ) / _framebuffer.width * ray ) / _camera_ptr->distance;

		// Установка начальной позиции луча
		vv_fp_math_type ray_x = _camera_ptr->x;
		vv_fp_math_type ray_y = _camera_ptr->y;

		// Параметры Z - буфера
		int16_t h_up_zbuf	= INT16_MAX;
		int16_t h_down_zbuf	= INT16_MIN;

		// Бросание луча в направлении от камеры
		uint16_t z = 1;	// u16
		while ( z < _camera_ptr->distance ) {
			// Обработка LOD
			// Получение старшего выставленного бита z
			uint16_t lod_id;	// ebx, id в массиве LOD
			__asm__ volatile( "bsr %1, %0" : "=r"( lod_id ) : "b" ( z ) );

			// Получение коэффициента уровня деатилизации
			uint8_t lod_coeff = _render_param.lod[ lod_id ];

			// Смещение координаты точки на луче
			ray_x += ( ray_delta_x * lod_coeff );
			ray_y += ( ray_delta_y * lod_coeff );

			// Преобразование координаты точки в целое
			int32_t rx = ( int32_t )( ray_x );
			int32_t ry = ( int32_t )( ray_y );

			// Проверка на выход точки на луче за пределы мира через старший выставленный бит в rx и ry
			if ( ( rx >> rng_bit ) || ( ry >> rng_bit ) )
				goto lb_ray_end;

			// Получение текущей ячейки мира
			cell_s *cell_ptr = vv_world_get_cell( rx, ry );

			// Получение высоты верхней точки линии
			int16_t h_up = ( ( vv_fp_math_type )( _camera_ptr->height - cell_ptr->segment_height_total ) / z * _camera_ptr->v_scale + _camera_ptr->horizon );

			// Получение высоты нижней точки линии
			int16_t h_down = ( ( vv_fp_math_type )( _camera_ptr->height ) / z * _camera_ptr->v_scale + _camera_ptr->horizon );

			// Проверка на попадание линии в экран
			if ( ( h_up > _framebuffer.height ) || ( h_down < 0 ) )
				goto lb_ray_end;

			// Отсечение по Z - буферу
			if ( ( h_up >= h_up_zbuf ) && ( h_down <= h_down_zbuf ) )
				goto lb_ray_end;

			// Обновление Z - буфера
			h_up_zbuf	= h_up;
			h_down_zbuf	= h_down;

			// Запись буфера вокселей
			vv_write_cell_to_vb( cell_ptr, &( _voxel_buffer ) );

			// Получение соотношения пикселя экрана к вокселю в мире
			vv_fp_math_type pix_coeff = ( ( vv_fp_math_type )( cell_ptr->segment_height_total ) / ( h_down - h_up ) );

			// Расчет начальной позиции вокселя в линии
			vv_fp_math_type vbuf_pos = .0;
			if ( h_down > _framebuffer.height )
				vbuf_pos = ( h_down - _framebuffer.height ) * pix_coeff;

			// Конечная y координата отрисовки линии
			int16_t end_y = ( h_up >= 0 ) ? h_up : -1;

			// Начальная координата отрисовки и счетчик
			uint16_t fb_pix = ( _framebuffer.height - 1 );
			int16_t pos_y = ( h_down < fb_pix ) ? h_down : fb_pix;

			// Отрисовка линии
			while ( pos_y > end_y ) {
				// Получение id пикселя фреймбуфера
				uint32_t id = ( pos_y-- * _framebuffer.width ) + ray;

				// Получение цвета вокселя
				uint32_t color = _voxel_buffer.color[ ( uint8_t )( vbuf_pos ) ].word;

				// Если цвет во фреймбуфере фоновый
				if ( _framebuffer.data[ id ] == _world_ptr->background_color.word ) {
					// Если цвет вокселя не фоновый, то отрисовка, иначе сброс Z-буфера
					if ( color ^ vv_transparent_color )
						_framebuffer.data[ id ] = color;
					else {
						// Сброс параметров Z - буфера
						h_up_zbuf	= INT16_MAX;
						h_down_zbuf	= INT16_MIN;
					}	// if color
				}	// if

				// Смещение цвета вокселя
				vbuf_pos += pix_coeff;
            }   // while
        // Перемещение по лучу
lb_ray_end:
			z += lod_coeff;
		}	// while z
    }   // for screen size width (x)
}	// vv_render_opti
