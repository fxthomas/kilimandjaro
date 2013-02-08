#ifndef __VIEW__
#define __VIEW__

#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <assert.h>
#include <X11/Xlib.h>
#include <X11/X.h>
#include <FreeImage.h>

#include "../profiles/tc_profile.h"
#include "terrain.h"

/*
Predefined rendering constraints
*/
#define WATER_ALPHA 0.5f
#define WATER_RED 0.0f
#define WATER_GREEN 0.1f
#define WATER_BLUE 1.0f
#define PROJECT_NEAR 10.0f
#define PROJECT_FAR 100000.0f
#define PROJECT_FOV 60.0f

/*
Simulation parameters
*/
typedef struct {
    int depthtest;
    int culling;
    int lighting;
    int water;
    int axes;
    int light_dir;
    GLfloat sun_azimuth;
    GLfloat sun_elevation;
    GLfloat light_ambient[4];
    GLfloat light_diffuse[4];
    GLfloat light_specular[4];
    GLfloat light_position[4];
    GLfloat light_direction[4];
    GLfloat camera_position[3];
    GLfloat camera_direction[3];
    GLfloat camera_normal[3];
    GLfloat camera_right[3];
} simulation;

/*
Pre-rendering call lists
*/
typedef struct {
    GLuint terrain;
    GLuint water;
    GLuint axes;
} rendering;

void rendering_free (rendering* r);

/*
Computes the normal of a triangle
Arguments
- v1, v2, v3 : 3 vertices defining a triangle

Return value
- out : A 3D vector containing the normal to the triangle
*/
void vertices_normal (float v1[3], float v2[3], float v3[3], float out[3]);

/*
Computes the normal of a triangle
Arguments
- v1, v2 : 2 vectors defining a triangle

Return value
- out : A 3D vector containing the normal to the triangle
*/
void vectors_normal (float v1[3], float v2[3], float out[3]);

/*
Adds 2 vectors
Returns the result in out
*/
void vectors_add (float out[3], float in[3]);

/*
Substracts 2 vectors
Returns the result (out - in) in out
*/
void vectors_subs (float out[3], float in[3]);

/*
Multiplies out by s
Returns the result in out
*/
void vector_scalarm (float out[3], float s);

/*
Rotates vector out according to angle
*/
void vector_rotate (float out[3], float axe[3], float angle);

/*
Normalizes a vector
*/
void vector_normalize (float out[3]);

/*
Updates sun position vector based on azimuth and elevation angles
Arguments
- s : Simulation parameters
*/
void sun_update (simulation* s);

/*
Initializes basic OpenGL states
*/
void terrain_3d_init (simulation s);

/*
Configures the camera position
*/
void terrain_3d_camera (simulation s, int w, int h);

/*
Updates light position and direction
*/
void terrain_3d_light (simulation s);

/*
Pre-renders respectively the terrain, water and axes
*/
GLuint terrain_3d_prerender_mesh (terrain t, tc_profile tcp, void (*f)(double));
GLuint terrain_3d_prerender_water (terrain t);
GLuint terrain_3d_prerender_axes (terrain t);

/*
Pre-renders the whole picture
Arguments
- t : Simulation parameters
- tcp : Terrain color profile

Returns a rendering structure containing the indexes of the pre-render Call Lists
*/
rendering terrain_3d_prerender (terrain t, tc_profile tcp, void (*f)(double));

/*
Displays terrain according to simulation parameters
Arguments
- s : Simulation parameters
- r : Terrain pre-render
*/
void terrain_3d_display (simulation s, rendering r);

/*
Prepares off-screen buffer for rendering
*/
void terrain_3d_offscreen_render (char* filename, int w, int h, simulation s, terrain t, tc_profile tcp, void (*f)(double));

#endif
