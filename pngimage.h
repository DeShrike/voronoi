#ifndef _PNGIMAGE_H
#define _PNGIMAGE_H

typedef uint32_t Color32;

typedef struct
{
   int width;
   int height;
   Color32 *pixels;
} Image;

typedef struct
{
	int pos;
	Color32 value;
} Keyframe;

#define INDEX(i, x, y)  (i->width * (y)) + (x)
#define SETPIXEL(i, x, y, c) (i->pixels[ (i->width * (y)) + (x) ] = (c))

#define RGB(r, g, b) (((0xFF000000 | r) | ((g) << 8)) | ((b) << 16))
#define BB(col) 		 (((col) & 0x00FF0000) >> 16)
#define GG(col) 		 (((col) & 0x0000FF00) >> 8)
#define RR(col) 		 ((col) & 0x000000FF)

Image* alloc_image(int width, int height);
void free_image(Image* image);

int save_image_as_png(const char* filepath, int width, int height, void *pixels);

void fill_circle(Image* image, int cx, int cy, int radius, Color32 color);
void fill_color(Image* image, Color32 color);

Color32* generate_palette(const Keyframe* keyframes, const int keyframe_count, int* palette_size);
void free_palette(Color32* palette);

float blend(float a, float b, float i);

#endif
