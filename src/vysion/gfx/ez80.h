#ifndef ez80_include_file
#define ez80_include_file

#ifdef __cplusplus
extern "C" {
#endif

#define ez80_width 16
#define ez80_height 16
#define ez80_size 258
#define ez80 ((gfx_sprite_t*)ez80_data)
extern unsigned char ez80_data[258];

#ifdef __cplusplus
}
#endif

#endif
