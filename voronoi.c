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

#define FRAME_COUNT    9999
#define OUTPUT_FILE_MASK "output/output%04d.png"

#define SEED_COUNT 20
#define SEED_MARKER_RADIUS 5
#define SEED_MARKER_COLOR COLOR_BLACK

#define BACKGROUND_COLOR 0xFF181818

#define BRIGHT_RED      0xFF3449FB
#define BRIGHT_GREEN    0xFF26BBB8
#define BRIGHT_YELLOW   0xFF2FBDFA
#define BRIGHT_BLUE     0xFF98A583
#define BRIGHT_PURPLE   0xFF9B86D3
#define BRIGHT_AQUA     0xFF7CC08E
#define BRIGHT_ORANGE   0xFF1980FE

#define BRIGHT_RED2      0xFF49FB34
#define BRIGHT_GREEN2    0xFFBBB826
#define BRIGHT_YELLOW2   0xFFBDFA2F
#define BRIGHT_BLUE2     0xFFA58398
#define BRIGHT_PURPLE2   0xFF86D39B
#define BRIGHT_AQUA2     0xFFC08E7C
#define BRIGHT_ORANGE2   0xFF80FE19

#define BRIGHT_RED3      0xFFFB3449
#define BRIGHT_GREEN3    0xFFB826BB
#define BRIGHT_YELLOW3   0xFFFA2FBD
#define BRIGHT_BLUE3     0xFF8398A5
#define BRIGHT_PURPLE3   0xFFD39B86
#define BRIGHT_AQUA3     0xFF8E7CC0
#define BRIGHT_ORANGE3   0xFFFE1980

static Color32 palette[] = {
   BRIGHT_RED,
   BRIGHT_GREEN,
   BRIGHT_YELLOW,
   BRIGHT_BLUE,
   BRIGHT_PURPLE,
   BRIGHT_AQUA,
   BRIGHT_ORANGE,

   BRIGHT_RED2,
   BRIGHT_GREEN2,
   BRIGHT_YELLOW2,
   BRIGHT_BLUE2,
   BRIGHT_PURPLE2,
   BRIGHT_AQUA2,
   BRIGHT_ORANGE2,

   BRIGHT_RED3,
   BRIGHT_GREEN3,
   BRIGHT_YELLOW3,
   BRIGHT_BLUE3,
   BRIGHT_PURPLE3,
   BRIGHT_AQUA3,
   BRIGHT_ORANGE3,
};

#define palette_count (sizeof(palette) / sizeof(palette[0]))

typedef struct
{
   int x, y;
   int dx, dy;
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

int manhattan_dist(int x1, int y1, int x2, int y2)
{
   return abs(x1 - x2) + abs(y1 - y2);
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

void update_seeds(void)
{
   for (size_t i = 0; i < SEED_COUNT; ++i)
   {
      seeds[i].x += seeds[i].dx;
      seeds[i].y += seeds[i].dy;

      if (seeds[i].x < 0)
      {
         seeds[i].x = 0;
         seeds[i].dx *= -1;
      }

      if (seeds[i].x > WIDTH)
      {
         seeds[i].x = WIDTH - 1;
         seeds[i].dx *= -1;
      }

      if (seeds[i].y < 0)
      {
         seeds[i].y = 0;
         seeds[i].dy *= -1;
      }

      if (seeds[i].y > HEIGHT)
      {
         seeds[i].y = HEIGHT - 1;
         seeds[i].dy *= -1;
      }
   }
}

void generate_random_seeds(void)
{
   for (size_t i = 0; i < SEED_COUNT; ++i)
   {
      seeds[i].x = rand() % WIDTH;
      seeds[i].y = rand() % HEIGHT;

      while (seeds[i].dx == 0 && seeds[i].dy == 0)
      {
         seeds[i].dx = (rand() % 3) - 1;
         seeds[i].dy = (rand() % 3) - 1;
      }
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
            // int dist = sqr_dist(seeds[i].x, seeds[i].y, x, y);
            int dist = manhattan_dist(seeds[i].x, seeds[i].y, x, y);
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
   srand(time(0));
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

int mainX(void)
{
   srand(time(0));
   Image *image = alloc_image(WIDTH, HEIGHT);

   generate_random_seeds();

   for (int frame = 0; frame < FRAME_COUNT; ++frame)
   {
      fill_color(image, BACKGROUND_COLOR);

      render_voronoi(image);
      render_seed_markers(image);
      update_seeds();

      char filename[200];
      sprintf(filename, OUTPUT_FILE_MASK, frame);
      printf("Saving %s\n", filename);
      int ret = save_image_as_png(filename, image->width, image->height, image->pixels);
      if (ret)
      {
         fprintf(stderr, "Saving image as PNG failed (%s)\n", ret);
      }
   }

   free_image(image);
   return 0;
}

/*
ffmpeg.exe -framerate 30 -i output/output%04d.png -s:v 800x600
-c:v libx264
-profile:v high -crf 20 -pix_fmt yuv420p voronoi.mp4
*/
