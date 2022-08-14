#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "voronoi.h"
#include "utils.h"

#define WIDTH 800
#define HEIGHT 600
#define OUTPUT_FILE_PATH "output.png"

#define SEED_COUNT 10
#define SEED_MARKER_RADIUS 5
#define SEED_MARKER_COLOR COLOR_WHITE

#define BACKGROUND_COLOR 0xFF181818

#define BRIGHT_RED      0xFF3449FB
#define BRIGHT_GREEN    0xFF26BBB8
#define BRIGHT_YELLOW   0xFF2FBDFA
#define BRIGHT_BLUE     0xFF98A583
#define BRIGHT_PURPLE   0xFF9B86D3
#define BRIGHT_AQUA     0xFF7CC08E
#define BRIGHT_ORANGE   0xFF1980FE

static Color32 palette[] = {
   BRIGHT_RED,
   BRIGHT_GREEN,
   BRIGHT_YELLOW,
   BRIGHT_BLUE,
   BRIGHT_PURPLE,
   BRIGHT_AQUA,
   BRIGHT_ORANGE,
};

#define palette_count (sizeof(palette) / sizeof(palette[0]))

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

int sqr_dist(int x1, int y1, int x2, int y2)
{
   int dx = x1 - x2;
   int dy = y1 - y2;
   return dx * dx + dy * dy;
}

void fill_circle(Image* image, int cx, int cy, int radius, Color32 color)
{
   int x0 = cx - radius;
   int y0 = cy - radius;
   int x1 = cx + radius;
   int y1 = cy + radius;

   for (int x = x0; x <= x1; ++x)
   {
      if (0 <= x && x < WIDTH)
      {
         for (int y = y0; y <= y1; ++y)
         {
            if (0 <= y && y < HEIGHT)
            {
               if (sqr_dist(cx, cy, x, y) <= radius * radius)
               {
                  SETPIXEL(image, x, y, color);
               }
            }
         }
      }
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

void render_seed_markers(Image* image)
{
   for (int s = 0; s < SEED_COUNT; s++)
   {
      fill_circle(image, seeds[s].x, seeds[s].y, SEED_MARKER_RADIUS, SEED_MARKER_COLOR);
   }
}

void render_voronoi(Image* image)
{
   for (int y = 0; y < image->height; ++y)
   {
      for (int x = 0; x < image->width; ++x)
      {
         int best_dist = 1000000;
         int best_seed = -1;
         for (size_t i = 0; i < SEED_COUNT; ++i)
         {
            int dist = sqr_dist(seeds[i].x, seeds[i].y, x, y);
            if (dist < best_dist)
            {
               best_dist = dist;
               best_seed = i;
            }
         }

         SETPIXEL(image, x, y, palette[best_seed % palette_count]);
      }
   }
}

int main(void)
{
   // srand(time(0));
   Image *image = alloc_image(WIDTH, HEIGHT);

   fill_color(image, BACKGROUND_COLOR);
   generate_random_seeds();

   render_voronoi(image);
   render_seed_markers(image);

   printf("Saving %s\n", OUTPUT_FILE_PATH);
   int ret = save_image_as_png(OUTPUT_FILE_PATH, image->width, image->height, image->pixels);
   if (ret)
   {
      fprintf(stderr, "Saving image as PNG failed (%s)\n", ret);
   }

   free_image(image);
   return 0;
}
