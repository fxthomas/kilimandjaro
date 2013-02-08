#include "tc_profile.h"


int tc_profile_alloc (tc_profile* p, int max) {
  p->n        = max;
  p->altitude = (interval*) malloc (max*sizeof (interval));
  p->c        = (color   *) malloc (max*sizeof (color   ));
  return (p->altitude != NULL && p->c != NULL);
}

tc_profile tc_profile_create () {
  tc_profile p;
  p.n        = 0;
  p.altitude = NULL;
  p.c        = NULL;
  return p;
}

int tc_profile_is_alloc (tc_profile* p) {
  return (p->n != 0 || p->altitude != NULL);
}

void tc_profile_free (tc_profile* p) {
  free (p->altitude);
  free (p->c);
  p->altitude = NULL;
  p->c = NULL;
  p->n = 0;
}

int tc_profile_read (FILE* file_in, tc_profile* p) {
  int i = 0, max = 0;
  if (fscanf (file_in, "%d", &max) != 1) {
      printf ("\nIO Error!\n");
      return 0;
  }
  printf ("[TCP] Reading %d elements...", max);
  if (tc_profile_is_alloc (p)) tc_profile_free (p);
  tc_profile_alloc (p, max);
  for (i = 0; i < p->n; i++) {
    printf (".");
    float r = 0.0f, g = 0.0f, b = 0.0f;
    if (fscanf (file_in, "%f%f%f%f%f", &(p->altitude[i].a), &(p->altitude[i].b), &r, &g, &b) != 5) {
        printf ("\n[TCP] IO Error!\n");
        return 0;
    }
    // Tests if the RGB values are within correct range
    if (r > 255) r = 255;
    if (r < 0) r = 0;
    if (g > 255) g = 255;
    if (g < 0) g = 0;
    if (b > 255) b = 255;
    if (b < 0) b = 0;

    p->c[i].r = r;
    p->c[i].g = g;
    p->c[i].b = b;
  }
  printf ("done!\n");
  return 1;
}

void tc_profile_write (FILE* file_out, tc_profile *p) {
  int i = 0;
  fprintf (file_out, "%d\n", p->n);
  for (i = 0; i < p->n; i++) {
    fprintf (file_out, "%f %f %f %f %f\n", p->altitude[i].a, p->altitude[i].b, p->c[i].r, p->c[i].g, p->c[i].b);
  }
}

void tc_profile_search (tc_profile* p, float height, float c[4]) {
    int i = 0;
    for (i = 0; i < p->n; i++) {
        if (p->altitude[i].a <= height && p->altitude[i].b > height) {
            c[0] = p->c[i].r;
            c[1] = p->c[i].g;
            c[2] = p->c[i].b;
            c[3] = 1.0f;
            return;
        }
    }
    c[0] = 1.0f;
    c[1] = 1.0f;
    c[2] = 1.0f;
    c[3] = 1.0f;
}
