#ifndef VV_CAMERA_H_INCLUDED
#define VV_CAMERA_H_INCLUDED

#include <math.h>
#include "vv_world.h"

/*
    vv_camera - ������ ������ � ������� � ������������ (������������)
    ���������� vv_world.
*/

// = = = = ������� = = = =

// ����������� �������� ����� ��
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// = = = = ���� = = = =

// ��������� �����������
typedef struct {
	uint16_t width, height;	// ������ (X) � ������ (Y) � ��������. ������ ���� ������ ������� ������!
	uint32_t *data;			// ��������� �� ������ �������� ��������� width * height
} framebuffer_s;

// ��������� ������
typedef struct {
	float x;				// ���������� X
	float y;				// ���������� Y
	float height;			// ������
	float angle;			// ���� ��������
	int16_t horizon;		// ����� ���������
	uint16_t distance;		// ��������� ���������
	float v_scale;			// ������������ ���������� �����������
} camera_s;

// = = = = ������� = = = =

#ifdef __cplusplus
extern "C" {
#endif

// ������� ��������� ��������� �� ������������ ������
// ��������� ��������� �� ��������� ������
void vv_set_camera( camera_s * );

// ������� �������� ������ ������.
// ��������� ����������� ��� ����������� ( >0.0f )
void vv_camera_forward( float );

// ������� �������� ������ �����.
// ��������� ���������� ��� ����������� ( >0.0f )
void vv_camera_backward( float );

// ������� �������� �����������.
// ��������� ������� ������ ( x/w/������ - y/h/������)
// ���������� ��������� �� ��������� ����������.
// ���� ��������� ����� NULL, ������ �� ����� �������� ������ ��������� ������, ����� �����.
framebuffer_s* vv_create_framebuffer( uint16_t, uint16_t );

// ������� ������� �����������.
// �������� ��������� ������, ��������� � vv_world
void vv_clear_framebuffer( void );

#ifdef __cplusplus
}
#endif

#endif // VV_CAMERA_H_INCLUDED
