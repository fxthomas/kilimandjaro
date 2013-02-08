#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include <stdlib.h>
#include <time.h>
#include "../profiles/rv_profile.h"

#define KEEP_BASE_BORDER 1
#define NO_BASE_BORDER 0

typedef struct _terrain {
  float  size_x; // Flat terrain size
  float  size_y;
  float  max_height;
  float  min_height;
  rv_profile* rvp; // Random Variable Profile
  unsigned long int tsize; // Table length
  unsigned long int lsize; // 1-dim table length
  float* heights;
} terrain;

/*
Returns uniform random variable between -1 and 1
*/
float randf ();

/*
Sets the height of a given point in a terrain
Arguments
- t : Pointer to a terrain structure
- x, y : Coordinates of the point
- value : New height of the point
- border : Whether the terrain border stays at the base height or not (KEEP_BASE_BORDER, NO_BASE_BORDER)
*/
void terrain_set_height (terrain* t, unsigned long int x, unsigned long int y, float value, int border);

/*
Returns the height of a given point in a terrain
Arguments
- t : Pointer to a terrain structure
- x, y : Coordinates of the point
*/
float terrain_get_height (terrain* t, unsigned long int x, unsigned long int y);

/*
Allocates memory for a terrain. Must be freed with terrain_free.
*/
terrain terrain_allocate (float size_x, float size_y, rv_profile* rvp);

/*
Returns 1 if terrain is allocated in memory, 0 else
*/
int terrain_is_alloc (terrain* terrain);

/*
Frees terrain in memory
*/
void terrain_free (terrain* terrain);

/*
Fractal terrain generator
Arguments
- base_height : The "sea level" of the generated terrain, usually 0
- terrain : Pointer to a terrain structure. Must be defined and allocated first
- f : A pointer to a progress update callback function
*/
void terrain_generate (float base_height, terrain* terrain, void (*f)(double));

/*
Prints terrain heights to stdout
*/
void terrain_print (terrain* t);

/*
Transforms table (i,j) coordinates into planar terrain coordinates
*/
void terrain_coords (terrain* t, int i, int j, float* x, float* y);

#endif
