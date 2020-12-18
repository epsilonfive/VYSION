#ifndef appvar_include_file
#define appvar_include_file

#ifdef __cplusplus
extern "C" {
#endif

#define appvar_width 16
#define appvar_height 16
#define appvar_size 258
#define appvar ((gfx_sprite_t*)appvar_data)
extern unsigned char appvar_data[258];

#ifdef __cplusplus
}
#endif

#endif
