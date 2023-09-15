#ifndef VV_VOX_H_INCLUDED
#define VV_VOX_H_INCLUDED

#include <string.h>		// memset
#include "../core/vv_model.h"

/*
	vv_vox - ������ ��������� ������ .vox
	���������� ���� � ������� vv_model, vv_world
	�����������: ������ ������ ������ ���� �� ����� 255 ��������.
*/

// = = = = ������� = = = =

// �������������� ������ �����
#define VOX_VERSION		150

// ������ ������ ���������� ��������� ������ � �������
#define VOX_PRINT_DEBUG

// = = = = ���� = = = =

// ������� ������ �� ��������� (������ �� vox.o) - ������ RGBA
extern const uint32_t vox_default_palette[ 256 ];

// ������������ ����������� �������� ������ vox
typedef enum {
	vo_ok = 0,			// �������
	vo_open_error,		// ������ �������� ����� (�����������, ����� ���� ��� ����)
	vo_wrong_version,	// ������������ ������ ����� (�� ����� VOX_VERSION)
	vo_wrong_format,	// ������������ ������ �����
	vo_memory_error,	// ������ ������
} vox_open_res_e;

#pragma pack(push, 1)
// ��� ��������� ������� �����.vox
typedef union {
    uint32_t word;  // ������ ����� � ������� X/Y/Z/C
    struct {
        uint8_t x;	// ���������� ������� � ������� ������
        uint8_t y;
        uint8_t z;
        uint8_t color_index;	// ������ ����� �������
    };  // struct
} vox_voxel_s;
#pragma pack(pop)

// ��� ��������� ������ .vox �����
typedef struct {
	uint32_t size_x;			// ������ ������ �� ���� x, y, z
	uint32_t size_y;
	uint32_t size_z;
	uint32_t voxel_count;		// ���������� �������� � ������
	vox_voxel_s *voxel_ptr;		// ��������� �� ����� ��������
	uint32_t	*color_palette;	// ��������� �� ������� ������ (������ RGBA). NULL - ������� �� ��������� (vox_default_palette)
} vox_model_s;

// ��� ������ .vox �����
typedef struct {
	uint8_t		model_count;	// ���������� �������
	vox_model_s	*model_ptr;		// ��������� �� ������ ������� ����������� model_count
} vox_buffer_s;

// = = = = ������� = = = =

#ifdef __cplusplus
extern "C" {
#endif

// ������� ������ ������ �� ����� .vox
// ��������� ������, ���������� ���� � �����, ��������� �� ��������� ����� .vox ��� ������.
// ���������� ��������� ������ �� �����
vox_open_res_e vv_vox_read_file( const char *, vox_buffer_s * );

// ������� �������� ������ volvox �� ������ .vox
// ��������� ��������� �� ������ .vox � ��������� �� ������ volvox ��� ������.
// ���������� ������� ������ �������� ������:
// true - �������, false - ������������ ������ ������.
bool vv_vox_create_model( vox_model_s *, model_s * );

#ifdef __cplusplus
}
#endif

#endif // VV_MODEL_H_INCLUDED
