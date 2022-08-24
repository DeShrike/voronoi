#include <stdlib.h>
#include "utils.h"

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

int max(int a, int b)
{
   return a > b ? a : b;
}

int min(int a, int b)
{
   return a < b ? a : b;
}

int chebyshev_dist(int x1, int y1, int x2, int y2)
{
   return max(abs(x1 - x2), abs(y1 - y2));
}

int minkowski_dist(int x1, int y1, int x2, int y2)
{
   return min(abs(x1 - x2), abs(y1 - y2));
}
