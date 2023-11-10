#include "vv_vox.h"

// Макросы
#ifndef VOX_CHUNK_COUNT
#define VOX_CHUNK_COUNT 15		// Общее количество чанков
#endif // VOX_CHUNK_COUNT

#ifndef VOX_ALLOW_CHUNK_COUNT
#define VOX_ALLOW_CHUNK_COUNT 5	// Количество поддерживаемых чанков
#endif // VOX_ALLOW_CHUNK_COUNT

#ifndef VOX_COLOR_PALETTE_COUNT
#define VOX_COLOR_PALETTE_COUNT 256	// Количество цветов в палитер
#endif // VOX_COLOR_PALETTE_COUNT

// Инициализация палитры цветов файла .vox по умолчанию (RGBA)
const uint32_t vox_default_palette[ VOX_COLOR_PALETTE_COUNT ] = {
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

// Перечисление результатов разбора чанка (менять одновременно с vox_open_res_e)
typedef enum {
	vp_ok = 0,			// Успешно
	vp_wrong_format,	// Некорректный формат чанка
	vp_memory_error,	// Ошибка памяти
	vp_read_error		// Ошибка чтения файла
} vox_parse_res_e;

// Размер заголовка в байтах
static const uint8_t vox_header_size = 4;

// Заголовок VOX файла
static const unsigned char vox_header[ 4 ] = "VOX ";

// Массив названий чанков
static const char *vox_header_chunk[ VOX_CHUNK_COUNT ] = {	"MAIN",	// Заголовок
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

// Идентификатор читаемой модели. Начинается с нуля.
static uint8_t model_read_id = 0;

// Внутренняя функция выделения памяти под новую модель в буфере.
// Осуществляет перераспределение памяти, увеличивает количество моделей на 1.
// Возвращает признак успеха выделения памяти под модель.
static bool _vox_buffer_append_model( vox_buffer_s *buffer_ptr ) {
	// Признак успеха выделения памяти под модель
	bool append_result = true;

	// Выделение памяти под модели
	vox_model_s *model_ptr = calloc( ( buffer_ptr->model_count + 1 ), sizeof( vox_model_s ) );

	// Если память не выделена
	if ( !model_ptr ) {
#ifdef VOX_PRINT_DEBUG
		printf( "vox buffer append model: memory allocation error\n" );
#endif // VOX_PRINT_DEBUG
		append_result = false;
		goto lb_buffer_append_model_end;
	}	// if model_ptr

	// Копирование имеющихся моделей в выделенную память
	if ( buffer_ptr->model_count )
		memcpy( model_ptr, buffer_ptr->model_ptr, sizeof( vox_model_s ) * buffer_ptr->model_count );

	// Очистка памяти по старому указателю
	free( buffer_ptr->model_ptr );

	// Обнуление параметров модели
	memset( &( model_ptr[ buffer_ptr->model_count ] ), 0x00, sizeof( vox_model_s ) );

	// Установка нового указателя и увеличение количества моделей
	buffer_ptr->model_ptr = model_ptr;
	buffer_ptr->model_count++;

lb_buffer_append_model_end:
	return append_result;
}	// _vox_buffer_append_model

static vox_parse_res_e _vox_parse_chunk_main( FILE *file, vox_chunk_header_u *chunk_header, vox_buffer_s *buffer_ptr ) {
	// Сохранение размера файла (байт)
	buffer_ptr->file_size = chunk_header->children_size;
#ifdef VOX_PRINT_DEBUG
	printf( "vox parse chunk main: success! file size: %u\n", buffer_ptr->file_size );
#endif // VOX_PRINT_DEBUG
	return vp_ok;
}	// _vox_parse_chunk_main

static vox_parse_res_e _vox_parse_chunk_pack( FILE *file, vox_chunk_header_u *chunk_header, vox_buffer_s *buffer_ptr ) {
	// Результат разбора чанка
	vox_parse_res_e parse_res = vp_ok;

	// Чтение количества моделей в файле (u32)
	if ( fread( &( buffer_ptr->model_count ), sizeof( uint32_t ), 1, file ) != 1 ) {
		// Чтение не удалось, ошибка чтения
#ifdef VOX_PRINT_DEBUG
		printf( "vox parse chunk pack: read error\n" );
#endif // VOX_PRINT_DEBUG
		parse_res = vp_read_error;
		goto lb_parse_chunk_pack_end;
	}	// if fread

	// Проверка количества моделей в файле ( 1 .. 15 )
	if ( buffer_ptr->model_count && ( buffer_ptr->model_count < 16 ) ) {
		// Неверное количество моделей в файле, некорректный формат файла
#ifdef VOX_PRINT_DEBUG
		printf( "vox parse chunk pack: wrong model count[%u]\n", buffer_ptr->model_count );
#endif // VOX_PRINT_DEBUG
		parse_res = vp_wrong_format;
		goto lb_parse_chunk_pack_end;
	}	// if model_count

	// Выделение памяти под модели
	buffer_ptr->model_ptr = calloc( buffer_ptr->model_count, sizeof( vox_model_s ) );

	// Обнуление массива моделей
	memset( buffer_ptr->model_ptr, 0x00, sizeof( vox_model_s ) * buffer_ptr->model_count );

	// Если память не выделена
	if ( !buffer_ptr->model_ptr ) {
		// Память не выделена, ошибка памяти
#ifdef VOX_PRINT_DEBUG
		printf( "vox parse chunk pack: memory allocation error\n" );
#endif // VOX_PRINT_DEBUG
		parse_res = vp_memory_error;
		goto lb_parse_chunk_pack_end;
	}	// if model_ptr

#ifdef VOX_PRINT_DEBUG
	printf( "vox parse chunk pack: success! model count: %u\n", buffer_ptr->model_count );
#endif // VOX_PRINT_DEBUG

lb_parse_chunk_pack_end:
	return parse_res;
}	// _vox_parse_chunk_pack

static vox_parse_res_e _vox_parse_chunk_size( FILE *file, vox_chunk_header_u *chunk_header, vox_buffer_s *buffer_ptr ) {
	// Результат разбора чанка
	vox_parse_res_e parse_res = vp_ok;

	// Проверка перехода на следующую модель
	// Если под текущую модель выделена память
	if ( model_read_id < buffer_ptr->model_count )
		// Если в текущую модель прочитаны воксели
		if ( buffer_ptr->model_ptr[ model_read_id ].voxel_ptr )
			// Переход к следующей модели
			model_read_id++;

	// Проверка наличия памяти, выделенной под модель
	if ( model_read_id >= buffer_ptr->model_count ) {
		// Выделение памяти под модель
		if ( !_vox_buffer_append_model( buffer_ptr ) ) {
			// Ошибка выделения памяти под модель
#ifdef VOX_PRINT_DEBUG
			printf( "vox parse chunk size: memory allocation error\n" );
#endif // VOX_PRINT_DEBUG
			parse_res = vp_memory_error;
			goto lb_parse_chunk_size_end;
		}	// if _vox_buffer_append_model
	}	// if model_read_id

	// Получение указателя на текущую модель
	vox_model_s *model_ptr = &( buffer_ptr->model_ptr[ model_read_id ] );

	// Чтение размера модели по осям X, Y, Z. u32 каждая компонента
	if ( fread( &( model_ptr->size_x ), sizeof( uint32_t ) * 3, 1, file ) != 1 ) {
		// Чтение не удалось, ошибка чтения
#ifdef VOX_PRINT_DEBUG
		printf( "vox parse chunk size: read error\n" );
#endif // VOX_PRINT_DEBUG
		parse_res = vp_read_error;
		goto lb_parse_chunk_size_end;
	}	// if fread

	// Проверка размера модели
	if ( model_ptr->size_x && model_ptr->size_y && model_ptr->size_z && ( model_ptr->size_z < 256 ) ) {
#ifdef VOX_PRINT_DEBUG
		printf( "vox parse chunk size: sucess! id: %u, x: %u, y: %u, z: %u\n", model_read_id,
				model_ptr->size_x, model_ptr->size_y, model_ptr->size_z );
#endif // VOX_PRINT_DEBUG
	} else {
		// Размер модели некорректный
#ifdef VOX_PRINT_DEBUG
		printf( "vox parse chunk size: wrong model size\n" );
#endif // VOX_PRINT_DEBUG
		parse_res = vp_wrong_format;
	}	// if model_ptr size

lb_parse_chunk_size_end:
	return parse_res;
}	// _vox_parse_chunk_size

static vox_parse_res_e _vox_parse_chunk_xyzi( FILE *file, vox_chunk_header_u *chunk_header, vox_buffer_s *buffer_ptr ) {
	// Результат разбора чанка
	vox_parse_res_e parse_res = vp_ok;

	// Получение указателя на текущую модель
	vox_model_s *model_ptr = &( buffer_ptr->model_ptr[ model_read_id ] );

	// Чтение количества вокселей модели (u32)
	if ( fread( &( model_ptr->voxel_count ), sizeof( uint32_t ), 1, file ) != 1 ) {
		// Чтение не удалось, ошибка чтения
#ifdef VOX_PRINT_DEBUG
		printf( "vox parse chunk xyzi: voxel count read error\n" );
#endif // VOX_PRINT_DEBUG
		parse_res = vp_read_error;
		goto lb_parse_chunk_xyzi_end;
	}	// if fread

	// Выделение памяти под воксели модели
	model_ptr->voxel_ptr = malloc( sizeof( vox_voxel_u ) * model_ptr->voxel_count );

	// Если память под воксели не выделена
	if ( !model_ptr->voxel_ptr ) {
#ifdef VOX_PRINT_DEBUG
		printf( "vox parse chunk xyzi: memory alloc error\n" );
#endif // VOX_PRINT_DEBUG
		parse_res = vp_memory_error;
		goto lb_parse_chunk_xyzi_end;
	}	// if !voxel_ptr

	// Чтение вокселей модели
	if ( fread( model_ptr->voxel_ptr, sizeof( vox_voxel_u ), model_ptr->voxel_count, file ) != model_ptr->voxel_count ) {
		// Чтение не удалось, ошибка чтения
#ifdef VOX_PRINT_DEBUG
		printf( "vox parse chunk xyzi: voxel data read error\n" );
#endif // VOX_PRINT_DEBUG
		parse_res = vp_read_error;
		goto lb_parse_chunk_xyzi_end;
	}	// if fread

#ifdef VOX_PRINT_DEBUG
		printf( "vox parse chunk xyzi: read voxel data done. count = %u\n", model_ptr->voxel_count );
#endif // VOX_PRINT_DEBUG

lb_parse_chunk_xyzi_end:
	return parse_res;
}	// _vox_parse_chunk_xyzi

static vox_parse_res_e _vox_parse_chunk_rgba( FILE *file, vox_chunk_header_u *chunk_header, vox_buffer_s *buffer_ptr ) {
	// Результат разбора чанка
	vox_parse_res_e parse_res = vp_ok;

	// Получение указателя на текущую модель
	vox_model_s *model_ptr = &( buffer_ptr->model_ptr[ model_read_id ] );

	// Выделение памяти под палитру цветов модели
	model_ptr->color_palette = malloc( sizeof( color_u ) * VOX_COLOR_PALETTE_COUNT );

	// Если память подпалитру цветов не выделена
	if ( !model_ptr->color_palette ) {
#ifdef VOX_PRINT_DEBUG
		printf( "vox parse chunk rgba: memory alloc error\n" );
#endif // VOX_PRINT_DEBUG
		parse_res = vp_memory_error;
		goto lb_parse_chunk_rgba_end;
	}	// if !color_palette_ptr

	// Чтение вокселей модели
	if ( fread( model_ptr->color_palette, sizeof( color_u ), VOX_COLOR_PALETTE_COUNT, file ) != VOX_COLOR_PALETTE_COUNT ) {
		// Чтение не удалось, ошибка чтения
#ifdef VOX_PRINT_DEBUG
		printf( "vox parse chunk rgba: color palette read error\n" );
#endif // VOX_PRINT_DEBUG
		parse_res = vp_read_error;
		goto lb_parse_chunk_rgba_end;
	}	// if fread

#ifdef VOX_PRINT_DEBUG
		printf( "vox parse chunk rgba: color palette read done\n" );
#endif // VOX_PRINT_DEBUG

	// Установка палитры для всех моделей до текущей (в файле версии 200 чанк RGBA может идти в конце для всех моделей)
	vox_model_s *id_model_ptr = buffer_ptr->model_ptr;
	while ( id_model_ptr < ( buffer_ptr->model_ptr + model_read_id ) ) {
		// Если заданы воксели модели и не задана палитра
		if ( id_model_ptr->voxel_ptr && !( id_model_ptr->color_palette ) )
			id_model_ptr->color_palette = model_ptr->color_palette;

		// Переход к следующей модели
		id_model_ptr++;
	}	// while

lb_parse_chunk_rgba_end:
	return parse_res;
}	// _vox_parse_chunk_rgba

// Внутренняя функция разбора чанка файла .vox
// Принимает указатель на файловый дескриптор, указатель на результат разбора и указатель на буфер .vox для записи моделей
// Возвращает признак ошибки или достижения конца файла: true - парсинг чанка успешен, false - достигнут конец файла или произошла ошибка.
static bool _vox_parse_chunk( FILE *file, vox_open_res_e *open_res_ptr, vox_buffer_s *buffer_ptr ) {
	// Результат разбора чанка
	bool parse_result = true;

	// Ленивая инициализация массива функций разбора чанков
	static vox_parse_res_e ( *parse_func_array[ VOX_ALLOW_CHUNK_COUNT ] )( FILE *, vox_chunk_header_u *, vox_buffer_s * ) = {
		_vox_parse_chunk_main,	// id 0, MAIN
		_vox_parse_chunk_pack,	// id 1, PACK
		_vox_parse_chunk_size,	// id 2, SIZE
		_vox_parse_chunk_xyzi,	// id 3, XYZI
		_vox_parse_chunk_rgba	// id 4, GRBA
	};	// parse_func_array

	// Буфер заголовка чанка
	vox_chunk_header_u chunk_header;

	// Чтение заголовка чанка
	if ( fread( &chunk_header, sizeof( vox_chunk_header_u ), 1, file ) != 1 ) {
		// Заголовок не прочитан. Если достигнут конец файла
		if ( feof( file ) )
			parse_result = false;
		else {
			// Конец файла не достигнут, ошибка чтения
#ifdef VOX_PRINT_DEBUG
			printf( "vox parse chunk: header read error\n" );
#endif // VOX_PRINT_DEBUG
			parse_result = false;
			*open_res_ptr = vp_read_error;
		}	// if feof
		goto lb_parse_chunk_end;
	}	// if fread

	// Флаг определения типа чанка
	bool chunk_search_flag = false;

	// Проход по всем типам чанков
	for ( uint8_t chunk_id = 0; chunk_id < VOX_CHUNK_COUNT; chunk_id++ ) {
		// Сравнение заголовка чанка
		if ( !memcmp( chunk_header.header, vox_header_chunk[ chunk_id ], vox_header_size ) ) {
			// Установка признака успешного чтения чанка
			chunk_search_flag = true;
			// Если чанк поддерживается
			if ( chunk_id < VOX_ALLOW_CHUNK_COUNT ) {
				// Разбор чанка
				vox_parse_res_e parse_res = parse_func_array[ chunk_id ]( file, &chunk_header, buffer_ptr );

				// Сохранение результата разбора чанка.
				// Каст vox_open_res_e к vox_parse_res_e !
				*open_res_ptr = ( vox_open_res_e )( parse_res );
			} else
				// Чанк не поддерживается, переход к концу чанка
				fseek( file, chunk_header.chunk_size, SEEK_CUR );

			break;
		}	// if !memcmp
	}	// for chunk_id

	// Если чанк не определен, ошибка формата файла
	if ( !chunk_search_flag ) {
#ifdef VOX_PRINT_DEBUG
		printf( "vox parse chunk: unknown chunk\n" );
#endif // VOX_PRINT_DEBUG
		parse_result = false;
		*open_res_ptr = vo_wrong_format;
	}	// if !chunk_search_flag

lb_parse_chunk_end:
	return parse_result;
}	// _vox_parse_chunk

vox_buffer_s* vv_vox_create_buffer( void ) {
	// Выделение памяти под буфер
	vox_buffer_s *buffer_ptr = NULL;
	buffer_ptr = malloc( sizeof( vox_buffer_s ) );

	// Если память успешно выделена
	if ( buffer_ptr ) {
		// Обнуление полей структуры
		buffer_ptr->file_size	= 0;
		buffer_ptr->model_count	= 0;
		buffer_ptr->model_ptr	= NULL;
	}	// if buffer_ptr
#ifdef VOX_PRINT_DEBUG
	else
		printf( "vox create buffer: memory allocation error\n" );
#endif // VOX_PRINT_DEBUG
	return buffer_ptr;
}	// vv_vox_create_buffer

void vv_vox_free_buffer( vox_buffer_s *buffer_ptr ) {
	// Если указатель на буфер валиден
	if ( buffer_ptr ) {
		// Если в буфере есть модели, очистка
		if ( buffer_ptr->model_count )
			free( buffer_ptr->model_ptr );
		// Удаление буфера
		free( buffer_ptr );
	}	// if buffer_ptr
}	// vv_vox_free_buffer

vox_open_res_e vv_vox_read_file( const char *file_name, vox_buffer_s *vox_buffer_ptr ) {
	// Результат чтения файла
	vox_open_res_e open_res = vo_ok;

	// Открытие файла
	FILE *file = NULL;
	file = fopen( file_name, "rb" );

	// Проверка открытия файла
	if ( file == NULL ) {
		// Файл не открыт, ошибка открытия
#ifdef VOX_PRINT_DEBUG
		printf( "vox read file: open error\n" );
#endif // VOX_PRINT_DEBUG
		open_res = vo_open_error;
		goto lb_read_file_end;
	}	// if file open

	// Чтение заголовка и версии файла
	vox_header_s header;
	if ( fread( &header, sizeof( vox_header_s ), 1, file ) != 1 ) {
		// Заголовок не прочитан, ошибка чтения
#ifdef VOX_PRINT_DEBUG
		printf( "vox read file: read header error\n" );
#endif // VOX_PRINT_DEBUG
		open_res = vo_read_error;
		goto lb_read_file_end;
	}	// if fread

	// Проверка заголовка файла
	if ( memcmp( header.header, vox_header, vox_header_size ) ) {
		// Заголовок файла не совпадает, некорректный формат файла
#ifdef VOX_PRINT_DEBUG
		printf( "vox read file: file header error\n" );
#endif // VOX_PRINT_DEBUG
		open_res = vo_wrong_format;
		goto lb_read_file_end;
	}	// if header

	// Проверка версии файла
	if ( ( header.version != VOX_VERSION_150 ) && ( header.version != VOX_VERSION_200 ) ) {
		// Версия файла некорректна, ошибка версии
#ifdef VOX_PRINT_DEBUG
		printf( "vox read file: file version error [%u]\n", header.version );
#endif // VOX_PRINT_DEBUG
		open_res = vo_wrong_version;
		goto lb_read_file_end;
	}	// if header version

	// Инициализация id читаемой модели
	model_read_id = 0;

#ifdef VOX_PRINT_DEBUG
		printf( "vox read file: start parse file. version [%u]\n", header.version );
#endif // VOX_PRINT_DEBUG

	// Разбор чанков файла
	while ( _vox_parse_chunk( file, &open_res, vox_buffer_ptr ) ) {
		;
	}	// _while vox_parse_chunk

#ifdef VOX_PRINT_DEBUG
		printf( "vox read file: parse file done.\n" );
#endif // VOX_PRINT_DEBUG

lb_read_file_end:
	// Закрытие файла
	if ( file != NULL )
		fclose( file );
	return open_res;
}	// vv_vox_read_file

// Внутренняя функция проверки валидности вокселя модели.
// Проверяет координаты и индекс цвета.
static bool _vox_check_voxel( vox_model_s *vox_model_ptr, uint32_t id ) {
	// Результат выполнения операции
	bool check_result = false;

	// Получение указателя на воксель
	vox_voxel_u *voxel_ptr = &( vox_model_ptr->voxel_ptr[ id ] );

	// Проверка координат вокселя
	if ( voxel_ptr->x < vox_model_ptr->size_x &&
		 voxel_ptr->y < vox_model_ptr->size_y &&
		 voxel_ptr->z < vox_model_ptr->size_z && voxel_ptr->color_index )
		 check_result = true;

	return check_result;
}	// _vox_check_voxel

bool vv_vox_create_model( vox_model_s *vox_model_ptr, model_s *model_ptr ) {
	// Результат создания модели
	bool create_result = true;

	// Размер плоскости модели (XY)
	uint32_t model_flat_size = ( vox_model_ptr->size_x * vox_model_ptr->size_y );

	// Создание матрицы буферов вокселей
	voxel_buffer_s *vb_ptr = NULL;
	vb_ptr = calloc( model_flat_size, sizeof( voxel_buffer_s ) );

	// Проверка выделения памяти
	if ( !vb_ptr ) {
		// Ошибка выделения памяти
		create_result = false;
		goto lb_create_model_end;
	}	// if !vb_ptr

	// Обнуление матрицы буферов вокселей
	memset( vb_ptr, 0x00, model_flat_size * sizeof( voxel_buffer_s ) );

	// Инициализация размеров модели
	model_ptr->size_x = vox_model_ptr->size_x;
	model_ptr->size_y = vox_model_ptr->size_y;
	model_ptr->size_z = 0;

	// Обнуление позиции модели
	model_ptr->position_x = 0;
	model_ptr->position_y = 0;
	model_ptr->_position_h = 1;	// Базовая высота

	// Получение указателя на палитру цветов (u32 RGBA)
	const uint32_t *color_palette_ptr = vox_model_ptr->color_palette;
	if ( color_palette_ptr == NULL )
		color_palette_ptr = vox_default_palette;

	// Обработка цветов модели.
	// Цикл по вокселям модели
	vox_voxel_u *voxel_ptr = vox_model_ptr->voxel_ptr;
	while ( voxel_ptr < ( vox_model_ptr->voxel_ptr + vox_model_ptr->voxel_count ) ) {
		// Получение id вокселя/буфера вокселей
		uint32_t id = ( voxel_ptr->y * model_ptr->size_x ) + voxel_ptr->x;

		// Проверка координат и цветового индекса вокселя
		if ( !_vox_check_voxel( vox_model_ptr, id ) ) {
#ifdef VOX_PRINT_DEBUG
			printf( "vox create model: voxel check fail. id: %u, x: %u, y: %u, z: %u, i: %u\n",
				id, voxel_ptr->x, voxel_ptr->y, voxel_ptr->z, voxel_ptr->color_index );
#endif // VOX_PRINT_DEBUG
			goto lb_create_model_while_end;
		}	// if !voxel_check

		// Получение указателя на буфер вокселей
		voxel_buffer_s *vb_id_ptr = ( vb_ptr + id );

		// Установка цвета вокселя в буфере (u32)
		vb_id_ptr->color[ voxel_ptr->z ].word = color_palette_ptr[ voxel_ptr->color_index - 1 ];

		// Обновление количества вокселей в буфере
		if ( vb_id_ptr->count < voxel_ptr->z )
			vb_id_ptr->count = ( voxel_ptr->z + 1 );

		// Обновление высоты модели
		if ( model_ptr->size_z < voxel_ptr->z )
			model_ptr->size_z = ( voxel_ptr->z + 1 );

lb_create_model_while_end:
		// Переход к следующему вокселю
		voxel_ptr++;
	}	// while

	// Выделение памяти под ячейки модели
	model_ptr->cell = calloc( model_flat_size, sizeof( cell_s ) );

	// Проверка выделения памяти
	if ( !model_ptr->cell ) {
		// Ошибка выделения памяти
		create_result = false;
		goto lb_create_model_end;
	}	// !cell_ptr

	// Обнуление матрицы ячеек модели
	memset( model_ptr->cell, 0x00, model_flat_size * sizeof( cell_s ) );

	// Цикл по ячейкам модели
	for ( uint32_t id = 0; id < model_flat_size; id++ ) {
		// Получение указателя на ячейку
		cell_s *cell_ptr = ( model_ptr->cell + id );

		// Преобразование буфера вокселей в ячейку
		vv_write_vb_to_cell( ( vb_ptr + id ), cell_ptr );

		// Замена нулевого цвета на прозрачный
		vv_cell_update_null( cell_ptr );

		// Оптимизация ячейки
		vv_cell_optimize( cell_ptr );

		// Формирование нулевого сегмента для установки высоты
		if ( !vv_cell_push_front( cell_ptr, 1 ) ) {
#ifdef VOX_PRINT_DEBUG
			printf( "vox create model: push height segment fail. id: %u\n", id );
#endif // VOX_PRINT_DEBUG
			create_result = false;
			goto lb_create_model_end;
		}	// if vv_cell_push
	}	// for id

lb_create_model_end:
	return create_result;
}	// vv_vox_create_model
