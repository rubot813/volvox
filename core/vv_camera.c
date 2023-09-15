#include "vv_camera.h"

camera_s			*_camera_ptr = NULL;		// Указатель на используемую камеру
framebuffer_s		_framebuffer;				// Фреймбуфер
static uint32_t		_framebuffer_pixel_count;	// Количество пикселей во фреймбуфере
extern world_s		*_world_ptr;				// Указатель на используемый мир ( vv_world )

void vv_set_camera( camera_s *camera_ptr ) {
	_camera_ptr = camera_ptr;
}   // vv_set_camera

void vv_camera_forward( float coeff ) {
	_camera_ptr->x += cos( _camera_ptr->angle ) * coeff;
	_camera_ptr->y += sin( _camera_ptr->angle ) * coeff;
}	// vv_camera_forward

void vv_camera_backward( float coeff ) {
	_camera_ptr->x -= cos( _camera_ptr->angle ) * coeff;
	_camera_ptr->y -= sin( _camera_ptr->angle ) * coeff;
}	// vv_camera_backward

framebuffer_s* vv_create_framebuffer( uint16_t width, uint16_t height ) {
	// Удаление старого фреймбуфера
	if ( _framebuffer.width && _framebuffer.height )
		free( _framebuffer.data );

	// Сохранение новых параметров и выделение памяти под буфер
	_framebuffer.width  = width;
	_framebuffer.height = height;
	_framebuffer_pixel_count = ( width * height );
	_framebuffer.data = malloc( _framebuffer_pixel_count * sizeof( uint32_t ) );

	return &( _framebuffer );
}   // vv_create_framebuffer

void vv_clear_framebuffer( void ) {
    uint32_t *pix_ptr = _framebuffer.data;
    while ( pix_ptr < ( _framebuffer.data + _framebuffer_pixel_count ) )
        *( pix_ptr++ ) = _world_ptr->background_color.word;
}   // vv_clear_framebuffer
