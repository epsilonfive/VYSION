#ifndef taskbaricon_include_file
#define taskbaricon_include_file

#ifdef __cplusplus
extern "C" {
#endif

#define taskbaricon_width 32
#define taskbaricon_height 16
#define taskbaricon_size 514
#define taskbaricon ((gfx_sprite_t*)taskbaricon_data)
extern unsigned char taskbaricon_data[514];

#ifdef __cplusplus
}
#endif

#endif
