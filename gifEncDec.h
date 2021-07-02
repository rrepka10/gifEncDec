#ifndef GIFENCDEC_H
#define GIFENCDEC_H

#include <stdint.h>
#include <sys/types.h>

#define MAX_PALETTE  (256)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct { uint8_t r; uint8_t g; uint8_t b; } pixel;

/*----------------------------------------------------------------------------
  We have to dummy out this structure because H needs 9 bits!  S & V only 
  really need 7.  We will steal the top bit from S to be the 9th bit of H
  H should take the values 0..360.  
  S should take the values 0.100
  V should take the values 0..100  
----------------------------------------------------------------------------*/
#define  S_MASK_LOW   (0x7f)
#define  S_MASK_HIGH  (0x80)
typedef struct { uint8_t h; uint8_t s; uint8_t v; } hsvPixel;

// Decode
typedef struct ge_GIF {
    uint16_t w, h;
    int depth;
    int fd;
    int offset;
    int nframes;
    uint8_t *frame, *back;
    uint32_t partial;
    uint8_t buffer[0xFF];
} ge_GIF;

// Encode
typedef struct gd_Palette {
    int size;
    uint8_t colors[0x100 * 3];
} gd_Palette;

typedef struct gd_GCE {
    uint16_t delay;
    uint8_t tindex;
    uint8_t disposal;
    int input;
    int transparency;
} gd_GCE;

typedef struct gd_GIF {
    int fd;
    off_t anim_start;
    uint16_t width, height;
    uint16_t depth;
    uint16_t loop_count;
    gd_GCE gce;
    gd_Palette *palette;
    gd_Palette lct, gct;
    void (*plain_text)(
        struct gd_GIF *gif, uint16_t tx, uint16_t ty,
        uint16_t tw, uint16_t th, uint8_t cw, uint8_t ch,
        uint8_t fg, uint8_t bg
    );
    void (*comment)(struct gd_GIF *gif);
    void (*application)(struct gd_GIF *gif, char id[8], char auth[3]);
    uint16_t fx, fy, fw, fh;
    uint8_t bgindex;
    uint8_t *canvas, *frame;
} gd_GIF;


// Encode
ge_GIF *ge_new_gif2(const char *fname, uint16_t width, uint16_t height, uint8_t *palette, 
                                    int depth, int loop);
void ge_add_frame(ge_GIF *gif, uint16_t delay);
void ge_close_gif(ge_GIF* gif);
uint8_t pallatize64( pixel pix );
uint8_t pallatize256( pixel pix );
int genPallette(pixel *image, int h, int w, int palSize, pixel *palette);
int createGIF(pixel *RGBframe, uint8_t *IndxFrame, int w, int h, pixel *palette, int palLen);

//Decode
gd_GIF *gd_open_gif(const char *fname);
int gd_get_frame(gd_GIF *gif);
void gd_render_frame(gd_GIF *gif, uint8_t *buffer);
int gd_is_bgcolor(gd_GIF *gif, uint8_t color[3]);
void gd_rewind(gd_GIF *gif);
void gd_close_gif(gd_GIF *gif);

// other
void writePPM(const char *filename, int x, int y, pixel *img);
pixel HSVtoRGB(hsvPixel  hsv);
hsvPixel RGBtoHSV(pixel pix);

#endif /* GIFENCDEC_H */



