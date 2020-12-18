#ifndef cursor_pointer_include_file
#define cursor_pointer_include_file

#ifdef __cplusplus
extern "C" {
#endif

#define cursor_pointer_width 16
#define cursor_pointer_height 16
#define cursor_pointer_size 258
#define cursor_pointer ((gfx_sprite_t*)cursor_pointer_data)
extern unsigned char cursor_pointer_data[258];

#ifdef __cplusplus
}
#endif

#endif
