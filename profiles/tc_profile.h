#ifndef __TC_PROFILE_H__
#define __TC_PROFILE_H__

#include <stdio.h>
#include <stdlib.h>

typedef struct _interval {
  float a;
  float b;
} interval;

typedef struct _color {
  float r;
  float g;
  float b;
} color;

typedef struct _tcprofile {
  int n; // Terrain Color count
  interval* altitude; // Terrain Altitude, from 0 to n-1
  color* c; // Terrain Color, (0-255), from 0 to n-1
} tc_profile;

/*
Memory allocation for a n-altitude profile
*/
int tc_profile_alloc (tc_profile* p, int max);

/*
Memory freeing
*/
void tc_profile_free (tc_profile* p);

/*
Creates NULL profile
*/
tc_profile tc_profile_create ();

/*
Reads TC Profile from file
*/
int tc_profile_read (FILE* file_in, tc_profile* p);

/*
Writes out Terrain Colors
*/
void tc_profile_write (FILE* file_out, tc_profile* p);

/*
Matches a color to a height
*/
void tc_profile_search (tc_profile* p, float height, float c[4]);

#endif
