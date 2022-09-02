#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "voronoi.h"
#include "utils.h"
#include "perlin.h"
#include "pngimage.h"

// Select one: //////////////////////
#define EUCLIDEAN
// #define MANHATTAN
// #define CHEBYSHEV
// #define MINKOWSKI
/////////////////////////////////////

#define WIDTH 2560
#define HEIGHT 1440

// #define WIDTH 800
// #define HEIGHT 600

// #define WIDTH 640
// #define HEIGHT 480

#ifdef EUCLIDEAN
#define OUTPUT_FILE_PATH "output/voronoi_euclidean.png"
#endif

#ifdef CHEBYSHEV
#define OUTPUT_FILE_PATH "output/voronoi_chebyshev.png"
#endif

#ifdef MINKOWSKI
#define OUTPUT_FILE_PATH "output/voronoi_minkowski.png"
#endif

#ifdef MANHATTAN
#define OUTPUT_FILE_PATH "output/voronoi_manhattan.png"
#endif

#define FRAME_COUNT      1000
#define OUTPUT_FILE_MASK "output/output%04d.png"

#define SEED_COUNT 200
#define SEED_MARKER_RADIUS 3
#define SEED_MARKER_COLOR COLOR_BLACK

#define BACKGROUND_COLOR 0xFF181818

Color32* palette = NULL;
int palette_size = 0;

// #define palette_count (sizeof(palette) / sizeof(palette[0]))

int seeds_to_use = SEED_COUNT;

typedef struct
{
   int x, y;
   int dx, dy;
} Point;

static Point seeds[SEED_COUNT];

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
   for (int s = 0; s < seeds_to_use; s++)
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
         for (size_t i = 0; i < seeds_to_use; ++i)
         {
#ifdef EUCLIDEAN
            int dist = sqr_dist(seeds[i].x, seeds[i].y, x, y);
#endif
#ifdef MANHATTAN
            int dist = manhattan_dist(seeds[i].x, seeds[i].y, x, y);
#endif
#ifdef CHEBYSHEV
            int dist = chebyshev_dist(seeds[i].x, seeds[i].y, x, y);
#endif
#ifdef MINKOWSKI
            int dist = minkowski_dist(seeds[i].x, seeds[i].y, x, y);
#endif
			double noise = 1.0; // Perlin_Get2d(x, y, 0.01, 1); // * 1000.0;
            if ((double)dist * noise < (double)best_dist)
            // if ((double)dist < (double)best_dist + noise)
            {
               best_dist = dist;
               best_seed = i;
            }
         }

         SETPIXEL(image, x, y, palette[best_seed % palette_size]);
      }
   }
}

int main(void)
{
	/*
	Keyframe kfs[] = {
		{ 0, COLOR_BLACK },
		{ WIDTH / 8 * 1, BRIGHT_YELLOW1 },
		{ WIDTH / 8 * 2, BRIGHT_BLUE1 },
		{ WIDTH / 8 * 3, BRIGHT_RED1 },
		{ WIDTH / 8 * 4, BRIGHT_GREEN1 },
		{ WIDTH / 8 * 5, BRIGHT_PURPLE1 },
		{ WIDTH / 8 * 6, BRIGHT_AQUA1 },
		{ WIDTH / 8 * 7, BRIGHT_ORANGE1 },
		{ WIDTH, COLOR_BLACK }
	};*/

	Keyframe kfs[] = {
		{ 0, COLOR_BLACK },
		{ WIDTH / 8 * 1, COLOR_RED },
		{ WIDTH / 8 * 2, COLOR_BLACK },
		{ WIDTH / 8 * 5, COLOR_MAGENTA },
		{ WIDTH / 8 * 6, COLOR_BLUE },
		{ WIDTH / 8 * 7, COLOR_BLACK },
		{ WIDTH, COLOR_BLACK }
    };

	palette = generate_palette(kfs, sizeof(kfs) / sizeof(kfs[0]), &palette_size);

	Image *image = alloc_image(WIDTH, HEIGHT);

	for (int y = 0; y < image->height; ++y)
	{
    	for (int x = 0; x < image->width; ++x)
     	{
			/*
   	  		double nr = Perlin_Get2d(x, y, 0.01, 2);
			nr *= 255.0;
			int r = (int)nr;
			r = clamp(r, 0, 255);

   	  		double ng = Perlin_Get2d(x + 1000, y, 0.01, 2);
			ng *= 255.0;
			int g = (int)ng;
			g = clamp(g, 0, 255);

   	  		double nb = Perlin_Get2d(x, y + 1000, 0.01, 2);
			nb *= 255.0;
			int b = (int)nb;
			b = clamp(b, 0, 255);

			SETPIXEL(image, x, y, RGB(r, g, b));
			*/

   	  		double n = Perlin_Get2d(x, y + 1000, 0.01, 2);
			int ix = (int)blend(0,  palette_size, n);
			SETPIXEL(image, x, y, palette[ix]);
        }
    }

   printf("Saving %s\n", OUTPUT_FILE_PATH);
   int ret = save_image_as_png(OUTPUT_FILE_PATH, image->width, image->height, image->pixels);
   if (ret)
   {
      fprintf(stderr, "Saving image as PNG failed (%s)\n", ret);
   }


   	free_image(image);
	image = NULL;

   	free_palette(palette);
   	palette = NULL;

   	return 0;
}

int mainR(void)
{
   	Image *image = alloc_image(WIDTH, HEIGHT);

   	fill_color(image, BACKGROUND_COLOR);

	Keyframe kfs[] = {
		{ 0, COLOR_BLACK },
		{ WIDTH / 8 * 1, BRIGHT_YELLOW1 },
		{ WIDTH / 8 * 2, BRIGHT_BLUE1 },
		{ WIDTH / 8 * 3, BRIGHT_RED1 },
		{ WIDTH / 8 * 4, BRIGHT_GREEN1 },
		{ WIDTH / 8 * 5, BRIGHT_PURPLE1 },
		{ WIDTH / 8 * 6, BRIGHT_AQUA1 },
		{ WIDTH / 8 * 7, BRIGHT_ORANGE1 },
		{ WIDTH, COLOR_BLACK }
	};
	/*
	Keyframe kfs[] = {
		{ 0, COLOR_BLACK },
		{ WIDTH / 8 * 1, COLOR_RED },
		{ WIDTH / 8 * 2, COLOR_YELLOW },
		{ WIDTH / 8 * 3, COLOR_CYAN },
		{ WIDTH / 8 * 4, COLOR_GREEN },
		{ WIDTH / 8 * 5, COLOR_MAGENTA },
		{ WIDTH / 8 * 6, COLOR_BLUE },
		{ WIDTH / 8 * 7, COLOR_ORANGE },
		{ WIDTH, COLOR_BLACK }
   };*/

	palette = generate_palette(kfs, sizeof(kfs) / sizeof(kfs[0]), &palette_size);

	for (int x = 0; x < palette_size; ++x)
	{
		for (int y = 0; y < HEIGHT; ++y)
		{
			SETPIXEL(image, x, y, palette[x]);
		}
	}

   printf("Saving %s\n", OUTPUT_FILE_PATH);
   int ret = save_image_as_png(OUTPUT_FILE_PATH, image->width, image->height, image->pixels);
   if (ret)
   {
      fprintf(stderr, "Saving image as PNG failed (%s)\n", ret);
   }

   free_image(image);
   image = NULL;

   free_palette(palette);
   palette = NULL;

	return 0;
}

int mainV(void)
{
   srand(time(0));
   // srand(2);
   Image *image = alloc_image(WIDTH, HEIGHT);

   fill_color(image, BACKGROUND_COLOR);
   generate_random_seeds();

   Keyframe kfs[] = {
		{ 0, COLOR_RED },
		{ 10, COLOR_YELLOW },
		{ 20, COLOR_CYAN },
		{ 30, COLOR_GREEN },
		{ 40, COLOR_MAGENTA },
		{ 50, COLOR_BLUE },
		{ 60, COLOR_ORANGE },
		{ 70, COLOR_BLACK }
   };

   palette = generate_palette(kfs, sizeof(kfs) / sizeof(kfs[0]), &palette_size);

   render_voronoi(image);
   render_seed_markers(image);

   printf("Saving %s\n", OUTPUT_FILE_PATH);
   int ret = save_image_as_png(OUTPUT_FILE_PATH, image->width, image->height, image->pixels);
   if (ret)
   {
      fprintf(stderr, "Saving image as PNG failed (%s)\n", ret);
   }

   free_image(image);
   image = NULL;

   free_palette(palette);
   palette = NULL;

   return 0;
}

int mainX(void)
{
   srand(time(0));
   Image *image = alloc_image(WIDTH, HEIGHT);

   generate_random_seeds();

   // seeds_to_use = 10;

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

		if (frame % 20 == 0)
		{
			if (seeds_to_use < SEED_COUNT)
			{
				seeds_to_use++;
				printf("Seeds: %d\n", seeds_to_use);
			}
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
