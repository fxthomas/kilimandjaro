#include "3dview.h"

/*
Specular reflexion factors
*/
GLfloat SPECREFL_NULL0[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat SPECREFL_WATER0[] = { 0.05f, 0.1f, 0.2f, 1.0f };

void rendering_free (rendering* r) {
    printf ("[3D VIEW] Freeing rendering context...");
    glDeleteLists (r->terrain, 1);
    glDeleteLists (r->water, 1);
    glDeleteLists (r->axes, 1);
    r->terrain = 0;
    r->axes = 0;
    r->water = 0;
    printf ("done!\n");
}

void terrain_3d_init (simulation s) {
    printf ("[3D VIEW] Setting up basic OpenGL parameters for rendering..."); fflush (stdout);
    if (s.depthtest) glEnable (GL_DEPTH_TEST); // Depth test ON
    if (s.culling) {
        glFrontFace(GL_CCW); // Cull faces (ccw) ON
        glEnable (GL_CULL_FACE);
    }

    if (s.lighting) {
        glEnable (GL_LIGHTING); // Lighting ON -> Nice landscape
        glLightModeli (GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
        glLightModelfv (GL_LIGHT_MODEL_AMBIENT, s.light_ambient);
        glLightfv (GL_LIGHT0, GL_DIFFUSE, s.light_diffuse);
        glLightfv (GL_LIGHT0, GL_SPECULAR, s.light_specular);
        glEnable (GL_LIGHT0);

        glEnable (GL_COLOR_MATERIAL); // Color tracking
        glColorMaterial (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, SPECREFL_NULL0);
        glMateriali (GL_FRONT_AND_BACK, GL_SHININESS, 0);

        glEnable (GL_BLEND); // Blending ON -> Water
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    printf ("done!\n");
}

void terrain_3d_camera (simulation s, int w, int h) {
    glViewport (0, 0, w, h);

    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
    gluLookAt (s.camera_position[0], s.camera_position[1], s.camera_position[2], s.camera_position[0]+s.camera_direction[0], s.camera_position[1]+s.camera_direction[1], s.camera_position[2]+s.camera_direction[2], s.camera_normal[0], s.camera_normal[1], s.camera_normal[2]);

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective (PROJECT_FOV, ((float)w)/((float)h), PROJECT_NEAR, PROJECT_FAR);
}

void terrain_3d_light (simulation s) {
    printf ("[3D VIEW] Updating light color..."); fflush (stdout);
    glLightfv (GL_LIGHT0, GL_POSITION, s.light_position);
    glLightfv (GL_LIGHT0, GL_SPOT_DIRECTION, s.light_direction);
    glLightModelfv (GL_LIGHT_MODEL_AMBIENT, s.light_ambient);
    glLightfv (GL_LIGHT0, GL_DIFFUSE, s.light_diffuse);
    printf ("done!\n");
}

GLuint terrain_3d_prerender_mesh (terrain t, tc_profile tcp, void (*f)(double)) {
    int i,j;
    GLuint TL;
    float x,y;
    float v1[3], v2[3], v3[3], n[3], c[4];
    TL = glGenLists (1);

    // Begin Call List
    glNewList (TL, GL_COMPILE);
        glBegin (GL_TRIANGLES);
            glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, SPECREFL_NULL0);
            glMateriali (GL_FRONT_AND_BACK, GL_SHININESS, 0);
            for (i = 0; i < t.lsize-1; i++) {
                if (f) f(((double)i*t.lsize)/((double)((t.lsize-1)*t.lsize)));
                for (j = 0; j < t.lsize-1; j++) {
                    // ==================
                    // | First triangle |
                    // ==================

                    terrain_coords (&t, i, j, &x, &y);
                    v1[0] = x;
                    v1[1] = y;
                    v1[2] = terrain_get_height (&t, i, j);
                    tc_profile_search (&tcp, v1[2], c); // Search for the vertex color
                    glColor4fv (c);
                    glVertex3fv (v1);

                    terrain_coords (&t, i+1, j, &x, &y);
                    v2[0] = x;
                    v2[1] = y;
                    v2[2] = terrain_get_height (&t, i+1, j);
                    tc_profile_search (&tcp, v2[2], c); // Search for the vertex color
                    glColor4fv (c);
                    glVertex3fv (v2);

                    terrain_coords (&t, i, j+1, &x, &y);
                    v3[0] = x;
                    v3[1] = y;
                    v3[2] = terrain_get_height (&t, i, j+1);
                    vertices_normal (v1, v2, v3, n);
                    tc_profile_search (&tcp, v3[2], c); // Search for the vertex color
                    glColor4fv (c);
                    glNormal3fv (n);
                    glVertex3fv (v3);

                    // ===================
                    // | Second triangle |
                    // ===================

                    terrain_coords (&t, i, j+1, &x, &y);
                    v1[0] = x;
                    v1[1] = y;
                    v1[2] = terrain_get_height (&t, i, j+1);
                    tc_profile_search (&tcp, v1[2], c); // Search for the vertex color
                    glColor4fv (c);
                    glVertex3fv (v1);

                    terrain_coords (&t, i+1, j, &x, &y);
                    v2[0] = x;
                    v2[1] = y;
                    v2[2] = terrain_get_height (&t, i+1, j);
                    tc_profile_search (&tcp, v2[2], c); // Search for the vertex color
                    glColor4fv (c);
                    glVertex3fv (v2);

                    terrain_coords (&t, i+1, j+1, &x, &y);
                    v3[0] = x;
                    v3[1] = y;
                    v3[2] = terrain_get_height (&t, i+1, j+1);
                    vertices_normal (v1, v2, v3, n);
                    tc_profile_search (&tcp, v3[2], c); // Search for the vertex color
                    glColor4fv (c);
                    glNormal3fv (n);
                    glVertex3fv (v3);
                }
            }
        glEnd ();
    glEndList ();
    return TL;
}

GLuint terrain_3d_prerender_water (terrain t) {
    int WL = glGenLists (1);
    glNewList (WL, GL_COMPILE);
        // Render water (transparent blue plane)
        glBegin (GL_QUADS);
            glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, SPECREFL_WATER0);
            glMateriali (GL_FRONT_AND_BACK, GL_SHININESS, 100);
            glColor4f (WATER_RED, WATER_GREEN, WATER_BLUE, WATER_ALPHA);
            glNormal3f (0.0f, 0.0f, 1.0f);
            glVertex3f (-10*t.size_x, 10*t.size_y, 0.0f);
            glVertex3f (-10*t.size_x, -10*t.size_y, 0.0f);
            glVertex3f (10*t.size_x, -10*t.size_y, 0.0f);
            glVertex3f (10*t.size_x, 10*t.size_y, 0.0f);
        glEnd ();
    glEndList ();
    return WL;
}

GLuint terrain_3d_prerender_axes (terrain t) {
    GLuint AL = glGenLists (1);
    glNewList (AL, GL_COMPILE);
        glBegin (GL_LINES);
            // Axes
            glColor4f (1.0f, 0.0f, 0.0f, 1.0f);
            glVertex3f (-t.size_x, 0.0f, 0.0f);
            glVertex3f (t.size_x, 0.0f, 0.0f);
            glColor4f (0.0f, 1.0f, 0.0f, 1.0f);
            glVertex3f (0.0f, -t.size_y, 0.0f);
            glVertex3f (0.0f, t.size_y, 0.0f);
            glColor4f (1.0f, 1.0f, 1.0f, 1.0f);
            glVertex3f (0.0f, 0.0f, 0.0f);
            glVertex3f (0.0f, 0.0f, 100.0f);
        glEnd ();
    glEndList ();
    return AL;
}

rendering terrain_3d_prerender (terrain t, tc_profile tcp, void (*f)(double)) {
    printf ("[3D VIEW] Pre-rendering view..."); fflush (stdout);
    rendering r;
    r.terrain   = terrain_3d_prerender_mesh (t, tcp, f);
    r.water     = terrain_3d_prerender_water (t);
    r.axes      = terrain_3d_prerender_axes (t);
    printf ("done!\n");
    return r;
}

void terrain_3d_display (simulation s, rendering r) {
    glClearColor (0.6, 0.6, 1.0, 1.0);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glCallList (r.terrain);
    if (s.water) glCallList (r.water);
    if (s.axes) glCallList (r.axes);
    if (s.light_dir) {
        glBegin (GL_LINES);
            // Light Direction
            glColor4f (1.0f, 1.0f, 1.0f, 1.0f);
            glVertex3f (0., 0., 0.);
            glVertex3f (s.light_position[0]*200, s.light_position[1]*200, s.light_position[2]*200);
        glEnd ();
    }
}

void sun_update (simulation* s) {
    printf ("[3D VIEW] Updating sun position..."); fflush (stdout);
    GLfloat az = s->sun_azimuth / 180. * 3.14;
    GLfloat el = s->sun_elevation / 180. * 3.14;
    s->light_direction[0] = -cosf (az)*cosf(el);
    s->light_direction[1] = -sinf (az)*cosf(el);
    s->light_direction[2] = -sinf (el);
    s->light_direction[3] = 0.0f;
    s->light_position[0] = -s->light_direction[0];
    s->light_position[1] = -s->light_direction[1];
    s->light_position[2] = -s->light_direction[2];
    s->light_position[3] = 0.0f;
    printf ("done!\n");
}

void vertices_normal (float v1[3], float v2[3], float v3[3], float out[3]) {
    float vv[2][3];
    vv[0][0] = v2[0] - v1[0];
    vv[0][1] = v2[1] - v1[1];
    vv[0][2] = v2[2] - v1[2];
    vv[1][0] = v3[0] - v1[0];
    vv[1][1] = v3[1] - v1[1];
    vv[1][2] = v3[2] - v1[2];
    out[0] = vv[0][1]*vv[1][2] - vv[1][1]*vv[0][2];
    out[1] = vv[1][0]*vv[0][2] - vv[0][0]*vv[1][2];
    out[2] = vv[0][0]*vv[1][1] - vv[1][0]*vv[0][1];
    float c = sqrtf (out[0]*out[0] + out[1]*out[1] + out[2]*out[2]);
    out[0] /= c;
    out[1] /= c;
    out[2] /= c;
}

void vectors_normal (float v1[3], float v2[3], float out[3]) {
    out[0] = v1[1]*v2[2] - v2[1]*v1[2];
    out[1] = v2[0]*v1[2] - v1[0]*v2[2];
    out[2] = v1[0]*v2[1] - v2[0]*v1[1];
    float c = sqrtf (out[0]*out[0] + out[1]*out[1] + out[2]*out[2]);
    out[0] /= c;
    out[1] /= c;
    out[2] /= c;
}

void vector_normalize (float out[3]) {
    float c = sqrtf (out[0]*out[0] + out[1]*out[1] + out[2]*out[2]);
    out[0] /= c;
    out[1] /= c;
    out[2] /= c;
}

void vector_scalarm (float out[3], float s) {
    out[0] *= s;
    out[1] *= s;
    out[2] *= s;
}

void vectors_add (float out[3], float in[3]) {
    out[0] += in[0];
    out[1] += in[1];
    out[2] += in[2];
}

void vectors_subs (float out[3], float in[3]) {
    out[0] -= in[0];
    out[1] -= in[1];
    out[2] -= in[2];
}

void vector_rotate (float out[3], float axe[3], float angle) {
    float x2, xy, y2, z2, xz, yz, c, s;
    c = cosf (angle);
    s = sinf (angle);
    x2 = axe[0]*axe[0];
    y2 = axe[1]*axe[1];
    z2 = axe[2]*axe[2];
    xy = axe[0]*axe[1];
    xz = axe[0]*axe[2];
    yz = axe[1]*axe[2];

    float v[3];
    v[0] = (x2 + (1-x2))*out[0] + (xy*(1-c) - axe[2]*s)*out[1] + (xz*(1-c)+axe[1]*s)*out[2];
    v[1] = (xy*(1-c)+axe[2]*s)*out[0] + (y2+(1-y2)*c)*out[1] + (yz*(1-c)-axe[0]*s)*out[2];
    v[2] = (xz*(1-c)-axe[1]*s)*out[0] + (yz*(1-c)+axe[0]*s)*out[1] + (z2+(1-z2)*c)*out[2];
    out[0] = v[0];
    out[1] = v[1];
    out[2] = v[2];
}

/*
OFF-SCREEN RENDERING
*/
void terrain_3d_offscreen_render (char* filename, int w, int h, simulation s, terrain t, tc_profile tcp, void (*f)(double)) {
    printf ("\n[3D VIEW] Rendering in progress\n"); fflush (stdout);
    int attrib[] = {GLX_RGBA, GLX_RED_SIZE, 8, GLX_BLUE_SIZE, 8, GLX_GREEN_SIZE, 8, GLX_ALPHA_SIZE, 8, GLX_DEPTH_SIZE, 8, None};
    GLXPixmap glxpixmap = 0;

    printf ("[3D VIEW]  > Creating Pixmap..."); fflush (stdout);
    Display* disp = XOpenDisplay (NULL); assert (disp);
    XVisualInfo* xvi = glXChooseVisual (disp, DefaultScreen (disp), attrib); assert (xvi);
    GLXContext ctx = glXCreateContext (disp, xvi, NULL, False); assert (ctx);
    Pixmap pixmap = XCreatePixmap (disp, DefaultRootWindow (disp), w, h, xvi->depth); assert (pixmap);
    glxpixmap = glXCreateGLXPixmap (disp, xvi, pixmap);
    printf ("done!\n");

    printf ("[3D VIEW]  > Rendering terrain...\n");
    glXMakeCurrent (disp, glxpixmap, ctx);
    terrain_3d_init (s);
    terrain_3d_camera (s, w, h);
    terrain_3d_light (s);
    rendering r = terrain_3d_prerender (t, tcp, f);
    terrain_3d_display (s, r);
    glFlush ();
    glFinish ();
    rendering_free (&r);
    printf ("done!\n");

    printf ("[3D VIEW]  > Reading pixels...");
    unsigned char* pixels = (unsigned char*)malloc (4*(unsigned long int)w*(unsigned long int)h*sizeof(unsigned char));
    glReadBuffer (GL_FRONT_AND_BACK);
    glReadPixels (0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    FIBITMAP* fiBitmap = FreeImage_Allocate (w, h, 32, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
    printf ("done!\n");

    printf ("[3D VIEW]  > Writing bitmap...");
    RGBQUAD color;
    unsigned long int i, j;
    for (i = 0; i < w; i++) {
        if (i%20 == 0) f (1. + ((double)i)/((double)w));
        for (j = 0; j < h; j++) {
            color.rgbRed = pixels [4L*(j*(unsigned int)w + i)];
            color.rgbGreen = pixels [4L*(j*(unsigned long int)w + i) + 1L];
            color.rgbBlue = pixels [4L*(j*(unsigned long int)w + i) + 2L];
            color.rgbReserved = 255;
            FreeImage_SetPixelColor (fiBitmap, i, j, &color);
        }
    }
    printf ("done!\n");

    printf ("[3D VIEW]  > Writing to %s...", filename); fflush (stdout);
    FreeImage_Save (FIF_PNG, fiBitmap, filename, 0);
    printf ("done!\n[3D VIEW]  > Freeing...");
    FreeImage_Unload (fiBitmap);
    free (pixels);
    XFreePixmap (disp, pixmap);
    XCloseDisplay (disp);
    printf ("done!\n\n");
}
