#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "voronoi.h"
#include "utils.h"

#define WIDTH 800
#define HEIGHT 600
#define OUTPUT_FILE_PATH "output.png"

#define INDEX(i, x, y)  (i->height * (y)) + (x)

#define SEED_COUNT 10

typedef struct
{
   int x, y;
} Point;

static Point seeds[SEED_COUNT];

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

void fill_color(Image* image, Color32 color)
{
   for (size_t y = 0; y < image->height; ++y)
   {
      for (size_t x = 0; x < image->width; ++x)
      {
         // int ix = y * image->height + x;
         int ix = INDEX(image, x, y);
         image->pixels[ix] = color;
      }
   }
}

void fill_circle(Image* image, int x, int y, int radius, Color32 color)
{

   // UNIMPLEMENTED("Fill circle");
}

void show_seeds(Image* image)
{
   for (int s = 0; s < SEED_COUNT; s++)
   {
      fill_circle(image, seeds[s].x, seeds[s].y, 2, COLOR_BLACK);
   }
}

void generate_random_seeds(void)
{
   for (size_t i = 0; i < SEED_COUNT; ++i)
   {
      seeds[i].x = rand() % WIDTH;
      seeds[i].y = rand() % HEIGHT;
   }
}

int main(void)
{
   Image *image = alloc_image(WIDTH, HEIGHT);

   fill_color(image, COLOR_BLUE);
   generate_random_seeds();
   show_seeds(image);

   printf("Saving %s\n", OUTPUT_FILE_PATH);
   int ret = save_image_as_png(OUTPUT_FILE_PATH, image->width, image->height, image->pixels);
   if (ret)
   {
      fprintf(stderr, "Saving image as PNG failed (%s)\n", ret);
   }

   free_image(image);
   return 0;
}
