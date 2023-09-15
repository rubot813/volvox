#include "vv_vox.h"

// Инициализация палитры цветов файла .vox по умолчанию
const uint32_t vox_default_palette[ 256 ] = {
   0x00000000, 0xffffffff, 0xffccffff, 0xff99ffff, 0xff66ffff, 0xff33ffff, 0xff00ffff, 0xffffccff,
   0xffccccff, 0xff99ccff, 0xff66ccff, 0xff33ccff, 0xff00ccff, 0xffff99ff, 0xffcc99ff, 0xff9999ff,
   0xff6699ff, 0xff3399ff, 0xff0099ff, 0xffff66ff, 0xffcc66ff, 0xff9966ff, 0xff6666ff, 0xff3366ff,
   0xff0066ff, 0xffff33ff, 0xffcc33ff, 0xff9933ff, 0xff6633ff, 0xff3333ff, 0xff0033ff, 0xffff00ff,
   0xffcc00ff, 0xff9900ff, 0xff6600ff, 0xff3300ff, 0xff0000ff, 0xffffffcc, 0xffccffcc, 0xff99ffcc,
   0xff66ffcc, 0xff33ffcc, 0xff00ffcc, 0xffffcccc, 0xffcccccc, 0xff99cccc, 0xff66cccc, 0xff33cccc,
   0xff00cccc, 0xffff99cc, 0xffcc99cc, 0xff9999cc, 0xff6699cc, 0xff3399cc, 0xff0099cc, 0xffff66cc,
   0xffcc66cc, 0xff9966cc, 0xff6666cc, 0xff3366cc, 0xff0066cc, 0xffff33cc, 0xffcc33cc, 0xff9933cc,
   0xff6633cc, 0xff3333cc, 0xff0033cc, 0xffff00cc, 0xffcc00cc, 0xff9900cc, 0xff6600cc, 0xff3300cc,
   0xff0000cc, 0xffffff99, 0xffccff99, 0xff99ff99, 0xff66ff99, 0xff33ff99, 0xff00ff99, 0xffffcc99,
   0xffcccc99, 0xff99cc99, 0xff66cc99, 0xff33cc99, 0xff00cc99, 0xffff9999, 0xffcc9999, 0xff999999,
   0xff669999, 0xff339999, 0xff009999, 0xffff6699, 0xffcc6699, 0xff996699, 0xff666699, 0xff336699,
   0xff006699, 0xffff3399, 0xffcc3399, 0xff993399, 0xff663399, 0xff333399, 0xff003399, 0xffff0099,
   0xffcc0099, 0xff990099, 0xff660099, 0xff330099, 0xff000099, 0xffffff66, 0xffccff66, 0xff99ff66,
   0xff66ff66, 0xff33ff66, 0xff00ff66, 0xffffcc66, 0xffcccc66, 0xff99cc66, 0xff66cc66, 0xff33cc66,
   0xff00cc66, 0xffff9966, 0xffcc9966, 0xff999966, 0xff669966, 0xff339966, 0xff009966, 0xffff6666,
   0xffcc6666, 0xff996666, 0xff666666, 0xff336666, 0xff006666, 0xffff3366, 0xffcc3366, 0xff993366,
   0xff663366, 0xff333366, 0xff003366, 0xffff0066, 0xffcc0066, 0xff990066, 0xff660066, 0xff330066,
   0xff000066, 0xffffff33, 0xffccff33, 0xff99ff33, 0xff66ff33, 0xff33ff33, 0xff00ff33, 0xffffcc33,
   0xffcccc33, 0xff99cc33, 0xff66cc33, 0xff33cc33, 0xff00cc33, 0xffff9933, 0xffcc9933, 0xff999933,
   0xff669933, 0xff339933, 0xff009933, 0xffff6633, 0xffcc6633, 0xff996633, 0xff666633, 0xff336633,
   0xff006633, 0xffff3333, 0xffcc3333, 0xff993333, 0xff663333, 0xff333333, 0xff003333, 0xffff0033,
   0xffcc0033, 0xff990033, 0xff660033, 0xff330033, 0xff000033, 0xffffff00, 0xffccff00, 0xff99ff00,
   0xff66ff00, 0xff33ff00, 0xff00ff00, 0xffffcc00, 0xffcccc00, 0xff99cc00, 0xff66cc00, 0xff33cc00,
   0xff00cc00, 0xffff9900, 0xffcc9900, 0xff999900, 0xff669900, 0xff339900, 0xff009900, 0xffff6600,
   0xffcc6600, 0xff996600, 0xff666600, 0xff336600, 0xff006600, 0xffff3300, 0xffcc3300, 0xff993300,
   0xff663300, 0xff333300, 0xff003300, 0xffff0000, 0xffcc0000, 0xff990000, 0xff660000, 0xff330000,
   0xff0000ee, 0xff0000dd, 0xff0000bb, 0xff0000aa, 0xff000088, 0xff000077, 0xff000055, 0xff000044,
   0xff000022, 0xff000011, 0xff00ee00, 0xff00dd00, 0xff00bb00, 0xff00aa00, 0xff008800, 0xff007700,
   0xff005500, 0xff004400, 0xff002200, 0xff001100, 0xffee0000, 0xffdd0000, 0xffbb0000, 0xffaa0000,
   0xff880000, 0xff770000, 0xff550000, 0xff440000, 0xff220000, 0xff110000, 0xffeeeeee, 0xffdddddd,
   0xffbbbbbb, 0xffaaaaaa, 0xff888888, 0xff777777, 0xff555555, 0xff444444, 0xff222222, 0xff111111
};	// vox_default_palette

// Размер заголовка в байтах
static const uint8_t vox_header_size = 4;

// Заголовок VOX файла
static const char vox_header[ 4 ] = "VOX ";

// Количество поддерживаемых чанков
static const uint8_t vox_chank_count = 15;

// Массив названий чанков
static const char *vox_header_chunk[ 15 ] = {	"MAIN",	// Заголовок
												"PACK",	// Количество моделей в файле
												"SIZE",	// Размер модели
												"XYZI",	// Воксели модели
												"RGBA",	// Палитра модели
												// Неиспользуемые:
												"rCAM",	// Информация о камере
												"rOBJ",	// Аттрибуты рендеринга
												"IMAP",	// Индексация палитры
												"nTRN",	// Трансформация моделей
												"nGRP",	// Группировка моделей
												"nSHP",	// Описание формы модели
												"LAYR",	// Описание слоев
												"MATL",	// Описание материалов модели
												"MATT",	// Старое описание материалов модели
												"NOTE"	// Названия цветов палитры
											};	// vox_header_chunk

// Внутренняя функция разбора чанка файла .vox
// Принимает указатель на файловый дескриптор, указатель на результат разбора и указатель на буфер .vox для записи моделей
// Возвращает признак ошибки или достижения конца файла: true - парсинг чанка успешен, false - достигнут конец файла или произошла ошибка.
static bool _vox_parse_chunk( FILE *file, vox_open_res_e *open_res_ptr, vox_buffer_s *vox_buffer_ptr ) {

}	// _vox_parse_chunk

vox_open_res_e vv_vox_read_file( const char *file_name, vox_buffer_s *vox_buffer_ptr ) {
	vox_open_res_e open_res = vo_ok;

	// Открытие файла
	FILE *file = NULL;
	file = fopen( file_name, "rb" );

	// Проверка открытия файла
	if ( file == NULL ) {
		open_res = vo_open_error;
		goto lb_read_file_end;
	}	// if file open

	// Чтение заголовка и версии файла
	char		header_buffer[ 4 ];
	uint32_t	header_version;

	fread( header_buffer, sizeof( char ), vox_header_size, file );
	fread( &header_version, sizeof( uint32_t ), 1, file );

	// Проверка заголовка файла
	if ( memcmp( header_buffer, vox_header, vox_header_size ) ) {
		open_res = vo_wrong_format;
		goto lb_read_file_end;
	}	// if header

	// Проверка версии файла
	if ( header_version != VOX_VERSION ) {
		open_res = vo_wrong_version;
		goto lb_read_file_end;
	}	// if header version

lb_read_file_end:
	// Закрытие файла
	if ( file != NULL )
		fclose( file );
	return open_res;
}	// vv_vox_read_file

bool vv_vox_create_model( vox_model_s *vox_model_ptr, model_s *model_ptr ) {

}	// vv_vox_create_model

/*
bool vv_vox_file_read( const char *file_name, model_vox_s *model_vox_ptr, uint8_t index ) {
	// Результат чтения файла
	bool read_result = true;

	FILE *file = NULL;
	file = fopen( file_name, "rb" );
	if ( file == NULL ) {
		read_result = false;
		goto lb_vox_file_read_end;
	}	// if
	printf( "file open ok" );

	char header[ 5 ];
	header[ 4 ] = '\0';
	uint32_t version;

	fread( header, 4, 1, file );
	fread( &version, 4, 1, file );

	//printf( "header = %s\n", header );
	//printf( "version = %u\n", version );

	// read main
	uint32_t chunk_size, children_size;
	fread( header, 4, 1, file );
	fread( &chunk_size, 4, 1, file );
	fread( &children_size, 4, 1, file );

	printf( "header = %s\n", header );
	printf( "chunk_size = %u\n", chunk_size );
	printf( "file_size = %u\n", children_size );


	// size
	fread( header, 4, 1, file );
	fread( &chunk_size, 4, 1, file );
	fread( &children_size, 4, 1, file );
	fread( &model_vox_ptr->size_x, 4, 1, file );
	fread( &model_vox_ptr->size_y, 4, 1, file );
	fread( &model_vox_ptr->size_z, 4, 1, file );


	printf( "header = %s\n", header );
	printf( "chunk_size = %u\n", chunk_size );
	printf( "children_size = %u\n", children_size );
	printf( "size_x = %u\n", model_vox_ptr->size_x );
	printf( "size_y = %u\n", model_vox_ptr->size_y );
	printf( "size_z = %u\n", model_vox_ptr->size_z );


	// read xyzi
	fread( header, 4, 1, file );
	fread( &chunk_size, 4, 1, file );
	fread( &children_size, 4, 1, file );
	fread( &model_vox_ptr->voxel_count, 4, 1, file );

	printf( "header = %s\n", header );
	printf( "chunk_size = %u\n", chunk_size );
	printf( "children_size = %u\n", children_size );
	printf( "vox count = %u\n", model_vox_ptr->voxel_count );
	printf( "\n" );

	model_vox_ptr->voxel = malloc( sizeof( voxel_vox_s ) * model_vox_ptr->voxel_count );
	fread( model_vox_ptr->voxel, sizeof( voxel_vox_s ), model_vox_ptr->voxel_count, file );

	// read rgba
	bool rgba_read = false;
	while ( !rgba_read ) {
		fread( header, 4, 1, file );
		fread( &chunk_size, 4, 1, file );
		fread( &children_size, 4, 1, file );
		//printf( "header = %s\n", header );
		//printf( "chunk_size = %u\n", chunk_size );
		//printf( "children_size = %u\n", children_size );

		if ( header[ 0 ] == 'R' && header[ 1 ] == 'G' ) {
			//printf( "here yay!" );

			model_vox_ptr->color_palette = malloc( sizeof( color_s ) * 256 );
			fread( model_vox_ptr->color_palette, sizeof( color_s ), 256, file );
			rgba_read = true;
		} else {
			//printf( "skip chunk\n" );
			fseek( file, chunk_size, SEEK_CUR );
		}
	}


	//for ( uint32_t voxel = 0; voxel < model_vox_ptr->voxel_count; voxel++ )
	//	printf( "voxel #%u x = %u, y = %u, z = %u, c = %u\n", voxel,
	//			model_vox_ptr->voxel[ voxel ].x,
	//			model_vox_ptr->voxel[ voxel ].y,
	//			model_vox_ptr->voxel[ voxel ].z,
	//			model_vox_ptr->voxel[ voxel ].color_index );


lb_vox_file_read_end:
	return read_result;
}	// vv_vox_file_read

void vv_vox_create_model( model_vox_s *model_vox_ptr, model_s *model_ptr ) {
	voxel_buffer_s *vb_ptr = malloc( model_vox_ptr->size_x * model_vox_ptr->size_y * sizeof( voxel_buffer_s ) );
	memset( vb_ptr, 0x00, model_vox_ptr->size_x * model_vox_ptr->size_y * sizeof( voxel_buffer_s ) );

	model_ptr->size_x = model_vox_ptr->size_x;
	model_ptr->size_y = model_vox_ptr->size_y;

	model_ptr->position_x = 0;
	model_ptr->position_y = 0;
	model_ptr->position_z = 0;
	const uint32_t *cb_ptr = model_vox_ptr->color_palette;
	if ( cb_ptr == NULL )
		cb_ptr = vox_default_palette;

	for( uint32_t i = 0; i < model_vox_ptr->voxel_count; i++ ) {
		voxel_vox_s *voxel = &model_vox_ptr->voxel[ i ];
		uint32_t id = ( voxel->x * model_ptr->size_x ) + voxel->y;
		//printf( "set x: %u, y: %u, z: %u, id: %u\n", voxel->x, voxel->y, voxel->z, id );
		voxel_buffer_s *vbp = &vb_ptr[ id ];
		vbp->color[ voxel->z ].word = cb_ptr[ voxel->color_index - 1 ];
		if ( voxel->z >= vbp->count )
			vbp->count = ( voxel->z + 1 );
	}

	model_ptr->cell = malloc( model_ptr->size_x * model_ptr->size_y * sizeof( cell_s ) );
	memset( model_ptr->cell, 0x00, model_ptr->size_x * model_ptr->size_y * sizeof( cell_s ) );

	//printf( "model sx = %u, model sy = %u", model_ptr->size_x, model_ptr->size_y );

	for ( uint16_t x = 0; x < model_ptr->size_x; x++ )
		for ( uint16_t y = 0; y < model_ptr->size_y; y++ ) {
			uint32_t id = ( x * model_ptr->size_x ) + y;
			vv_write_vb_to_cell( &vb_ptr[ id ], &model_ptr->cell[ id ] );
			vv_cell_optimize( &model_ptr->cell[ id ] );
			vv_cell_push_front( &model_ptr->cell[ id ], 5 );
		}

}	// vv_vox_create_model
*/
