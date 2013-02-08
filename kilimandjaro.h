#ifndef __KILIMANDJARO_H__
#define __KILIMANDJARO_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <math.h>
#include <GL/gl.h>
#include <gdk/gdkgl.h>
#include <gtk/gtk.h>
#include <gtk/gtkgl.h>
#include <GL/glu.h>
#include <gdk/gdkkeysyms.h>
#include <FreeImage.h>

#include "profiles/rv_profile.h"
#include "profiles/tc_profile.h"
#include "generator/terrain.h"
#include "generator/3dview.h"

GLfloat LIGHT_AMBIENT[] = { 0.78f / 5, 0.8f / 5, 0.4f / 5, 1.0f };
GLfloat LIGHT_DIFFUSE[] = { 0.76f, 0.8f, 0.17f, 1.0f };
GLfloat LIGHT_SPECULAR[] = { 1.0f, 1.0f, 0.5f, 1.0f };
GLfloat VECTOR4F_NULL[] = { 0.0f, 0.0f, 0.0f, 0.0f };
GLfloat CAMERA_POSITION0[] = { -6000.0f, -6000.0f, 6000.0f };
GLfloat CAMERA_DIRECTION0[] = { 0.58f, 0.58f, -0.58f };
GLfloat CAMERA_NORMAL0[] = { 0.58f, 0.58f, 0.58f };
GLfloat CAMERA_RIGHT0[3];

/*
GTK Window definition
*/

GtkWidget* gwWindow;
GtkWidget* gtkTable;
GtkWidget* giImage;
GtkWidget* gsHScale;
GtkWidget* gsVScale;
GtkWidget* glLabelSun;
GtkWidget* glLabelAz;
GtkWidget* glLabelEl;
GtkWidget* glSunlightColor;
GtkWidget* ghsvSunLightColor;
GtkWidget* gvbLabels;
GtkWidget* gtkCameraPosition;
GtkWidget* gtkCameraDirection;
GtkWidget* glCameraPosition;
GtkWidget* glCameraDirection;
GtkWidget* glCameraNormal;
GtkWidget* glX;
GtkWidget* glY;
GtkWidget* glZ;
GtkWidget* glDX;
GtkWidget* glDY;
GtkWidget* glDZ;
GtkWidget* glNX;
GtkWidget* glNY;
GtkWidget* glNZ;
GtkWidget* geX;
GtkWidget* geY;
GtkWidget* geZ;
GtkWidget* geDX;
GtkWidget* geDY;
GtkWidget* geDZ;
GtkWidget* geNX;
GtkWidget* geNY;
GtkWidget* geNZ;
GtkWidget* gbRegenTerrain;
GtkWidget* geRegenSizeX;
GtkWidget* geRegenSizeY;
GtkWidget* gbRender;
GtkWidget* geRenderWidth;
GtkWidget* geRenderHeight;
GtkWidget* ghbRenderSize;
GtkWidget* ghbRegenSize;
GtkWidget* glTerrainColorChooser;
GtkWidget* glRandomVariableChooser;
GtkWidget* gpProgress;
GtkWidget* gbRenderFileChooser;
GtkListStore* glTCC;
GtkListStore* glRVC;
GtkAdjustment* gVAdjustment;
GtkAdjustment* gHAdjustment;
GdkGLConfig* glConfig;

/*
Random terrain
*/
tc_profile tcp;
rv_profile rvp;
terrain t;
simulation s;
rendering r;
int w, h; // Drawing Area Width and Height

/*
Creates a pixbuf for an icon

Arguments
- filename : Icon file name

Return Value : GdkPixbuf* containing the icon
*/
GdkPixbuf* create_pixbuf (const char* filename);

/*
Callback functions called when the sun position if modified on the sliders
*/
gboolean callback_elevation (GtkWidget *gwWidget, gpointer data);
gboolean callback_azimuth (GtkWidget *gwWidget, gpointer data);

/*
Generate terrain
*/
int generate (char* rv, char* tc, float size_x, float size_y, void (*f)(gdouble));

/*
Reset camera position
*/
void camera_reset ();

/*
Scene rendering callback
*/
gboolean render (GtkWidget *gwWidget, GdkEventExpose *gdkEvent);

/*
Scene configuration callback
*/
gboolean configure (GtkWidget* gwWidget, GdkEventConfigure* gdkEvent);

/*
Sets up main OpenGL states (lighting, base material, ...)
*/
void setup (GtkWidget* gwWidget);

/*
Handles keyboard events
*/
gboolean callback_keypressed (GtkWidget* gwWidget, GdkEventKey* gdkEvent, gpointer data);
gboolean callback_keyreleased (GtkWidget* gwWidget, GdkEventKey* gdkEvent, gpointer data);
gboolean callback_camerapos (GtkWidget* gwWidget, GdkEventFocus* gdkFocus, gpointer data);
gboolean callback_grabfocus (GtkWidget* gwWidget, GdkEventMotion* geMotion, gpointer data);

/*
Regenerate terrain
*/
gboolean callback_regen (GtkWidget* gwWidget, gpointer data);

/*
Render terrain to a file
*/
gboolean callback_render_file (GtkWidget* gwWidget, gpointer data);

/*
Handles sunlight color changes
*/
gboolean callback_sunlight (GtkWidget *gwWidget, gpointer data);

/*
Converts float to string
*/
char* float_to_string (float f);

/*
Converts int to string
*/
char* int_to_string (int i);

/*
Frees all resources
*/
void free_all ();

/*
File Filters
*/
int filter_rvp (const struct dirent* d) {
    return (strncmp (d->d_name+(strlen(d->d_name)-4), ".rvp", 3) == 0);
}
int filter_tcp (const struct dirent* d) {
    return (strncmp (d->d_name+(strlen(d->d_name)-4), ".tcp", 3) == 0);
}

#endif
