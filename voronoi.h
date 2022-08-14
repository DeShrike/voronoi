#ifndef _VORONOI_H
#define _VORONOI_H

// 0xAABBGGRR
#define COLOR_RED   0xFF0000FF
#define COLOR_GREEN 0xFF00FF00
#define COLOR_BLUE  0xFFFF0000

#define COLOR_WHITE  0xFFFFFFFF
#define COLOR_BLACK  0xFF000000

typedef uint32_t Color32;

typedef struct
{
   int width;
   int height;
   Color32 *pixels;
} Image;

#endif
