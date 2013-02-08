#include "rv_profile.h"

int rv_profile_alloc (rv_profile* p, unsigned long int max) {
  p->n = max;
  p->height = (float*) malloc (max*sizeof (float));
  return (p->height != NULL);
}

rv_profile rv_profile_create () {
  rv_profile p;
  p.n = 0;
  p.height = NULL;
  return p;
}

int rv_profile_is_alloc (rv_profile* p) {
  return (p->n != 0 || p->height != NULL);
}

void rv_profile_free (rv_profile* p) {
  free (p->height);
  p->height = NULL;
  p->n = 0;
}

int rv_profile_read (FILE* file_in, rv_profile* p) {
  unsigned long int i = 0, max = 0;
  if (fscanf (file_in, "%lu", &max) != 1) {
      printf ("\n[RVP] IO Error!\n");
      return 0;
  }
  printf ("\n[RVP] Reading %lu elements...", max);
  if (rv_profile_is_alloc (p)) rv_profile_free (p);
  rv_profile_alloc (p, max);
  for (i = 0; i < p->n; i++) {
    printf (".");
    if (fscanf (file_in, "%f", (p->height + i)) != 1) {
        printf ("\n[RVP] IO Error!\n");
        return 0;
    }
  }
  printf ("done!\n");
  return 1;
}

void rv_profile_write (FILE* file_out, rv_profile *p) {
  unsigned long int i = 0;
  fprintf (file_out, "%lu\n", p->n);
  for (i = 0; i < p->n; i++) {
    fprintf (file_out, "%f\n", p->height[i]);
  }
}
