#include "terrain.h"

/*
Returns 2^i
*/
inline unsigned long int pow2 (unsigned long int i) { // 2^i
    unsigned long int r = 1;
    for (;i>0;i--) r*=2;
    return r;
}

void terrain_generate (float base_height, terrain* terrain, void (*f)(double)) {
  unsigned long int i, j, k, pi = 1, r = terrain->lsize - 1;
  unsigned long int cj, ck, lj = 0, lk = 0, rj, rk, tj = 0, tk = 0, bj, bk; // Points to update and their heights
  float cv, lv = 0, rv, tv = 0, bv;
  unsigned long int tlj, tlk, trj, trk, blj, blk, brj, brk; // Parent nodes and their heights
  float tlv, trv, blv, brv;

  printf ("\n[TERRAIN] Generating %lux%lu terrain...\n > Initializing random variable generator...", terrain->lsize, terrain->lsize);
  srand (time (NULL));

  printf ("done!\n > Initializing table...");
  for (i = 0; i < terrain->tsize; i++) terrain->heights[i] = base_height;

  printf ("done!\n > Running...");

  for (i = 0; i < terrain->rvp->n; i++) { // For each subdivision depth...
      if (f) f(((double)pow2 (i))/((double)pow2(terrain->rvp->n - 1)));
      for (j = 0; j < pi; j++) { // For each grid node at subdivision i, update node
          for (k = 0; k < pi; k++) {
              // We use the data from the parent node (Top-Left, Top-Right, Bottom-Left, Bottom-Right)
              tlj = j*(r/pi);
              tlk = k*(r/pi);
              trj = tlj + (r/pi);
              trk = tlk;
              blj = tlj;
              blk = tlk + (r/pi);
              brj = trj;
              brk = blk;

              // Their heights
              tlv = terrain_get_height (terrain, tlj, tlk);
              trv = terrain_get_height (terrain, trj, trk);
              blv = terrain_get_height (terrain, blj, blk);
              brv = terrain_get_height (terrain, brj, brk);

              // Left, right, top, bottom and center points indexes
              // To avoid redundancy, we only need to update top and left when we're at the border of the grid
              if (k == 0) { // Top coordinates
                  tj = tlj + (r/pi/2);
                  tk = tlk;
              }
              if (j == 0) { // Left coordinates
                  lj = tlj;
                  lk = tlk + (r/pi/2);
              }
              bj = tlj + (r/pi/2); // Bottom coordinates
              bk = blk;
              cj = tlj + (r/pi/2); // Center coordinates
              ck = tlk + (r/pi/2);
              rj = trj; // Right coordinates
              rk = tlk + (r/pi/2);

              // Their heights
              cv = (tlv + brv) / 2; // Center
              if (k == 0) tv = (tlv + trv) / 2; // Top
              bv = (blv + brv) / 2; // Bottom
              if (j == 0) lv = (tlv + blv) / 2; // Left
              rv = (trv + brv) / 2; // Right

              // Add random variable
              cv += terrain->rvp->height[i]*randf (); // Center
              if (k == 0) tv += terrain->rvp->height[i]*randf (); // Top
              bv += terrain->rvp->height[i]*randf (); // Bottom
              if (j == 0) lv += terrain->rvp->height[i]*randf (); // Left
              rv += terrain->rvp->height[i]*randf (); // Right

              // Update Height map
              if (k == 0) terrain_set_height (terrain, tj, tk, tv, KEEP_BASE_BORDER);
              if (j == 0) terrain_set_height (terrain, lj, lk, lv, KEEP_BASE_BORDER);
              terrain_set_height (terrain, cj, ck, cv, KEEP_BASE_BORDER);
              terrain_set_height (terrain, rj, rk, rv, KEEP_BASE_BORDER);
              terrain_set_height (terrain, bj, bk, bv, KEEP_BASE_BORDER);
          }
      }
      pi *= 2;
  }
  printf ("done!\n");
}

terrain terrain_allocate (float size_x, float size_y, rv_profile* rvp) {
  terrain t;
  t.size_x  = size_x;
  t.size_y  = size_y;
  t.max_height = 0.;
  t.min_height = 0.;
  unsigned long int r = pow2(rvp->n) + 1;
  t.tsize   = r*r;
  t.lsize   = r;
  t.heights = (float*) malloc (sizeof(float) * t.tsize);
  t.rvp     = rvp;
  return t;
}

void terrain_free (terrain* terrain) {
  free (terrain->heights);
  rv_profile_free (terrain->rvp);
  terrain->size_x  = -1.;
  terrain->size_y  = -1.;
  terrain->tsize   = 0;
  terrain->lsize   = 0;
  terrain->max_height = 0;
  terrain->min_height = 0;
  terrain->rvp     = NULL;
  terrain->heights = NULL;
}

int terrain_is_alloc (terrain* terrain) {
  return (terrain->rvp != NULL && terrain->size_x > 0. && terrain->size_y > 0. && terrain->heights != NULL);
}

float terrain_get_height (terrain* t, unsigned long int x, unsigned long int y) {
    //printf ("Getting height : (%lu,%lu) = %lu\n", x, y, x + t->lsize*y);
    //printf ("Table size : %lu\n", t->tsize);
    return t->heights[x + t->lsize*y];
}

void terrain_set_height (terrain* t, unsigned long int x, unsigned long int y, float value, int border) {
    if (border && (x == 0 || y == 0 || x == t->lsize-1 || y == t->lsize-1)) return;
    t->heights[x + t->lsize*y] = value;
    if (value > t->max_height) t->max_height = value;
    if (value < t->min_height) t->max_height = value;
}

void terrain_print (terrain* t) {
    unsigned long int i,j;
    printf ("\n\n");
    for (j = 0; j < t->lsize; j++) {
        for (i = 0; i < t->lsize; i++) {
            printf("%f ", terrain_get_height (t, i, j));
        }
        printf ("\n");
    }
}

void terrain_coords (terrain* t, int i, int j, float* x, float* y) {
    *x = 2.0f*(0.5f - ((float)i)/((float)t->lsize))*t->size_x;
    *y = 2.0f*(0.5f - ((float)j)/((float)t->lsize))*t->size_y;
}

float randf () {
  return (1-2*(((float)rand ())/((float)RAND_MAX)));
}
