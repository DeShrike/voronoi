#ifndef _PNGIMAGE_H
#define _PNGIMAGE_H

typedef uint32_t Color32;
typedef struct
{
   int width;
   int height;
   Color32 *pixels;
} Image;

#define INDEX(i, x, y)  (i->width * (y)) + (x)
#define SETPIXEL(i, x, y, c) (i->pixels[ (i->width * (y)) + (x) ] = (c))

Image* alloc_image(int width, int height);
void free_image(Image* image);
int save_image_as_png(const char *filepath, int width, int height, void *pixels);
void fill_circle(Image* image, int cx, int cy, int radius, Color32 color);
void fill_color(Image* image, Color32 color);

#endif
