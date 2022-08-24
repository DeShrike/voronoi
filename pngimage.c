#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <png.h>
#include "pngimage.h"

Image* alloc_image(int width, int height)
{
   Image *i = (Image*)malloc(sizeof(Image));
   i->pixels = (Color32*)malloc(sizeof(Color32) * width * height);
   i->width = width;
   i->height = height;
   return i;
}

void free_image(Image* image)
{
   free(image->pixels);
   free(image);
}

int save_image_as_png(const char *filepath, int width, int height, void *pixels)
{
   png_byte **row_pointers = NULL;
   png_structp png = NULL;
   png_infop info = NULL;
   FILE *fp = NULL;
   int pixel_size = 3;
   int depth = 8;
   int x, y;

   fp = fopen(filepath, "wb");
   if (fp == NULL)
   {
      fprintf(stderr, "ERROR: could create open file %s: %s\n", filepath, strerror(errno));
      return 1;
   }

   png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   if (png == NULL)
   {
      fprintf(stderr, "ERROR: png_create_write_struct failed: %s\n", strerror(errno));
      return 2;
   }

   info = png_create_info_struct(png);
   if (info == NULL)
   {
      fprintf(stderr, "ERROR: png_create_info_struct failed: %s\n", strerror(errno));
      return 3;
   }

   if (setjmp(png_jmpbuf(png)))
   {
      fprintf(stderr, "ERROR: setjmp(png_jmpbuf)) failed: %s\n", strerror(errno));
      return 4;
   }

   // Output is 8 bit depth, RGB format.
   png_set_IHDR(
      png,
      info,
      width, height,
      depth,
      PNG_COLOR_TYPE_RGB,
      PNG_INTERLACE_NONE,
      PNG_COMPRESSION_TYPE_DEFAULT,
      PNG_FILTER_TYPE_DEFAULT
   );

   row_pointers = png_malloc (png, height * sizeof(png_byte*));

   uint32_t* p = (uint32_t*)pixels;

   for (y = 0; y < height; y++)
   {
      png_byte *row = png_malloc (png, sizeof(uint8_t) * width * pixel_size);
      row_pointers[y] = row;
      for (x = 0; x < width; x++)
      {
         int ix = y * width + x;
         uint32_t pixel = p[ix];

         *row++ = (pixel & 0x0000FF) >> 0;
         *row++ = (pixel & 0x00FF00) >> 8;
         *row++ = (pixel & 0xFF0000) >> 16;
      }
   }

   png_init_io(png, fp);
   png_set_rows(png, info, row_pointers);
   png_write_png(png, info, PNG_TRANSFORM_IDENTITY, NULL);
   png_write_end(png, NULL);

   int ret = fclose(fp);
   assert(ret == 0);

   for (y = 0; y < height; y++)
   {
        png_free(png, row_pointers[y]);
   }

   png_destroy_write_struct(&png, &info);

   return 0;
}

void fill_circle(Image* image, int cx, int cy, int radius, Color32 color)
{
   int x0 = cx - radius;
   int y0 = cy - radius;
   int x1 = cx + radius;
   int y1 = cy + radius;
   for (int x = x0; x <= x1; ++x)
   {
      if (0 <= x && x < image->width)
      {
         for (int y = y0; y <= y1; ++y)
         {
            if (0 <= y && y < image->height)
            {
			   if ((cx - x) * (cx - x) + (cy - y) * (cy - y) <= radius * radius)
               {
                  SETPIXEL(image, x, y, color);
               }
            }
         }
      }
   }
}

void fill_color(Image* image, Color32 color)
{
   for (size_t y = 0; y < image->height; ++y)
   {
      for (size_t x = 0; x < image->width; ++x)
      {
         int ix = INDEX(image, x, y);
         image->pixels[ix] = color;
      }
   }
}
