#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <png.h>

#define WIDTH 800
#define HEIGHT 600

#define OUTPUT_FILE_PATH "output.png"

// 0xAABBGGRR
#define COLOR_RED 0xFF0000FF

typedef uint32_t Color32;

static Color32 image[HEIGHT][WIDTH];

void fill_color(Color32 color)
{
   for (size_t y = 0; y < HEIGHT; ++y)
   {
      for (size_t x = 0; x < WIDTH; ++x)
      {
         image[y][x] = color;
      }
   }
}

void save_image_as_png(const char *filepath)
{
   printf("Saving %s\n", filepath);
   FILE *f = fopen(filepath, "wb");
   if (f == NULL)
   {
      fprintf(stderr, "ERROR: could not open file %s: %s\n", filepath, strerror(errno));
      exit(1);
   }

   png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

   png_infop info = png_create_info_struct(png);
   if (!info)
   {
      fprintf(stderr, "ERROR: png_create_info_struct failed: %s\n", strerror(errno));
      exit(2);
   }

   if (setjmp(png_jmpbuf(png)))
   {
      fprintf(stderr, "ERROR: setjmp(png_jmpbif)) failed: %s\n", strerror(errno));
      exit(3);
   }

   png_init_io(png, f);

   // Output is 8bit depth, RGBA format.
   png_set_IHDR(
      png,
      info,
      WIDTH, HEIGHT,
      8,
      PNG_COLOR_TYPE_RGBA,
      PNG_INTERLACE_NONE,
      PNG_COMPRESSION_TYPE_DEFAULT,
      PNG_FILTER_TYPE_DEFAULT
   );
   png_write_info(png, info);

   png_write_image(png, image);
   png_write_end(png, NULL);

   int ret = fclose(f);
   assert(ret == 0);

   png_destroy_write_struct(&png, &info);
}

int main(void)
{
   fill_color(COLOR_RED);
   save_image_as_png(OUTPUT_FILE_PATH);
   return 0;
}
