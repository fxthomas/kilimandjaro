#ifndef __RV_PROFILE_H__
#define __RV_PROFILE_H__

#include <stdio.h>
#include <stdlib.h>

typedef struct _rvpprofile {
  unsigned long int n; // Max. iteration count
  float* height; // Random Variable Height profile, from iteration 0 to n-1
} rv_profile;

int rv_profile_is_alloc (rv_profile* p);

/*
Memory allocation for a n-iteration profile
*/
int rv_profile_alloc (rv_profile* p, unsigned long int max);

/*
Memory freeing
*/
void rv_profile_free (rv_profile* p);

/*
Creates NULL profile
*/
rv_profile rv_profile_create ();

/*
Reads RV Profile from file
*/
int rv_profile_read (FILE* file_in, rv_profile* p);

/*
Writes out RV Profile
*/
void rv_profile_write (FILE* file_out, rv_profile* p);

#endif
