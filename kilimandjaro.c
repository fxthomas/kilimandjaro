#include "kilimandjaro.h"

int generate (char* rv, char* tc, float size_x, float size_y, void (*f)(double)) {
    FILE* file_in = fopen (rv, "r");
    if (!file_in) {
        printf ("\n[MAIN] Unable to open Random Variable Profile!\n");
        return 4;
    }
    if (!rv_profile_read(file_in, &rvp)) {
        printf ("\n[MAIN] Invalid RV profile!\n");
        return 5;
    }
    fclose (file_in);

    file_in = fopen (tc, "r");
    if (!file_in) {
        printf ("\n[MAIN] Unable to open Terrain Color Profile!\n");
        return 2;
    }
    if (!tc_profile_read(file_in, &tcp)) {
        printf ("\n[MAIN] Invalid Terrain Color profile!\n");
        return 3;
    }
    fclose (file_in);

    t = terrain_allocate(size_x, size_y, &rvp);
    terrain_generate (0., &t, f);
    return 0;
}

int main (int argc, char** argv) {
    // ====================
    // | Generate terrain |
    // ====================

    // Basic parameters
    rvp = rv_profile_create();
    tcp = tc_profile_create();
    s.water = 1;
    s.lighting = 1;
    s.culling = 1;
    s.axes = 1;
    s.depthtest = 1;
    s.light_dir = 1;
    memcpy (s.light_ambient, LIGHT_AMBIENT, 4*sizeof(GLfloat));
    memcpy (s.light_diffuse, LIGHT_DIFFUSE, 4*sizeof(GLfloat));
    memcpy (s.light_specular, LIGHT_SPECULAR, 4*sizeof(GLfloat));
    memcpy (s.light_position, VECTOR4F_NULL, 4*sizeof(GLfloat));
    memcpy (s.light_direction, VECTOR4F_NULL, 4*sizeof(GLfloat));
    vectors_normal (CAMERA_DIRECTION0, CAMERA_NORMAL0, CAMERA_RIGHT0);
    camera_reset ();
    s.sun_azimuth = 0.0f;
    s.sun_elevation = 0.0f;

    // Generation
    int ret = generate ("Default.rvp", "Default.tcp", 4000., 4000., NULL);
    if (ret != 0) return ret;

    // ===========
    // | Display |
    // ===========

    // OpenGL & GTK+ Setup
    FreeImage_Initialise (FALSE);
    gtk_init (&argc, &argv);
    gdk_gl_init (&argc, &argv);
    glConfig = gdk_gl_config_new_by_mode (GDK_GL_MODE_DEPTH | GDK_GL_MODE_RGBA | GDK_GL_MODE_DOUBLE);
    if (!glConfig) g_assert_not_reached ();


    // Window creation
    gwWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size (GTK_WINDOW (gwWindow), 1024, 0);
    gtk_window_set_position (GTK_WINDOW (gwWindow), GTK_WIN_POS_CENTER);
    gtk_window_set_title (GTK_WINDOW (gwWindow), "Kilimandjaro");
    gtk_window_set_icon (GTK_WINDOW (gwWindow), create_pixbuf ("Icon.png"));
    g_signal_connect (G_OBJECT (gwWindow), "destroy", G_CALLBACK (gtk_main_quit), G_OBJECT (gwWindow));

    // Table layout
    gtkTable = gtk_table_new (4, 5, FALSE);

    // OpenGL Area
    giImage = gtk_drawing_area_new ();
    gtk_drawing_area_size (GTK_DRAWING_AREA (giImage), 1024, 1000);
    gtk_widget_add_events (giImage, GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK | GDK_BUTTON_PRESS_MASK);
    GTK_WIDGET_SET_FLAGS (giImage, GTK_CAN_FOCUS);
    if (!gtk_widget_set_gl_capability (giImage, glConfig, NULL, TRUE, GDK_GL_RGBA_TYPE)) g_assert_not_reached ();
    g_signal_connect (giImage, "expose-event", G_CALLBACK (render), NULL);
    g_signal_connect (giImage, "configure-event", G_CALLBACK (configure), NULL);
    g_signal_connect (giImage, "key-press-event", G_CALLBACK (callback_keypressed), NULL);
    g_signal_connect (giImage, "key-release-event", G_CALLBACK (callback_keyreleased), NULL);
    g_signal_connect (giImage, "button-press-event", G_CALLBACK (callback_grabfocus), NULL);

    // ====================
    // | Light parameters |
    // ====================

    // Information
    gvbLabels = gtk_vbox_new (FALSE, 0);
    glLabelSun = gtk_label_new ("Sun angles");
    glLabelAz = gtk_label_new ("Azimuth : ");
    glLabelEl = gtk_label_new ("Elevation : ");
    glSunlightColor = gtk_label_new ("Sunlight color : ");

    gtk_box_pack_start (GTK_BOX (gvbLabels), glLabelSun, TRUE, TRUE, TRUE);
    gtk_box_pack_start (GTK_BOX (gvbLabels), glLabelAz, TRUE, TRUE, TRUE);
    gtk_box_pack_start (GTK_BOX (gvbLabels), glLabelEl, TRUE, TRUE, TRUE);
    gtk_box_pack_start (GTK_BOX (gvbLabels), glSunlightColor, TRUE, TRUE, TRUE);

    // Sunlight color selector
    ghsvSunLightColor = gtk_hsv_new ();
    gdouble ch,cs,cv;
    gtk_rgb_to_hsv ((gdouble)s.light_ambient[0], (gdouble)s.light_ambient[1], (gdouble)s.light_ambient[2], &ch, &cs, &cv);
    gtk_hsv_set_color (GTK_HSV(ghsvSunLightColor), ch, cs, cv);
    g_signal_connect (ghsvSunLightColor, "changed", G_CALLBACK (callback_sunlight), NULL);

    // Vertical Scale (Elevation)
    gVAdjustment = (GtkAdjustment*)gtk_adjustment_new (90., 0., 90., 1., 5., 0.);
    gsVScale = gtk_vscale_new (gVAdjustment);
    gtk_scale_set_draw_value (GTK_SCALE (gsVScale), FALSE);
    g_signal_connect (gVAdjustment, "value-changed", G_CALLBACK (callback_elevation), NULL);

    // Horizontal Scale (Azimuth)
    gHAdjustment = (GtkAdjustment*) gtk_adjustment_new (0., 0., 360., 5., 20., 0.);
    gsHScale = gtk_hscale_new (gHAdjustment);
    gtk_scale_set_draw_value (GTK_SCALE (gsHScale), FALSE);
    g_signal_connect (gHAdjustment, "value-changed", G_CALLBACK (callback_azimuth), NULL);

    // =====================
    // | Camera parameters |
    // =====================

    gtkCameraPosition = gtk_table_new (12, 2, FALSE);
    glCameraPosition = gtk_label_new ("Camera position");
    glCameraDirection = gtk_label_new ("Camera direction");
    glCameraNormal = gtk_label_new ("Camera normal");
    glX = gtk_label_new ("X");
    glY = gtk_label_new ("Y");
    glZ = gtk_label_new ("Z");
    glDX = gtk_label_new ("dX");
    glDY = gtk_label_new ("dY");
    glDZ = gtk_label_new ("dZ");
    glNX = gtk_label_new ("nX");
    glNY = gtk_label_new ("nY");
    glNZ = gtk_label_new ("nZ");
    geX = gtk_entry_new ();
    geY = gtk_entry_new ();
    geZ = gtk_entry_new ();
    geDX = gtk_entry_new ();
    geDY = gtk_entry_new ();
    geDZ = gtk_entry_new ();
    geNX = gtk_entry_new ();
    geNY = gtk_entry_new ();
    geNZ = gtk_entry_new ();
    char st[100];
    sprintf (st, "%f", s.camera_position[0]);
    gtk_entry_set_text (GTK_ENTRY (geX), st);
    sprintf (st, "%f", s.camera_position[1]);
    gtk_entry_set_text (GTK_ENTRY (geY), st);
    sprintf (st, "%f", s.camera_position[2]);
    gtk_entry_set_text (GTK_ENTRY (geZ), st);
    sprintf (st, "%f", s.camera_direction[0]);
    gtk_entry_set_text (GTK_ENTRY (geDX), st);
    sprintf (st, "%f", s.camera_direction[1]);
    gtk_entry_set_text (GTK_ENTRY (geDY), st);
    sprintf (st, "%f", s.camera_direction[2]);
    gtk_entry_set_text (GTK_ENTRY (geDZ), st);
    sprintf (st, "%f", s.camera_normal[0]);
    gtk_entry_set_text (GTK_ENTRY (geNX), st);
    sprintf (st, "%f", s.camera_normal[1]);
    gtk_entry_set_text (GTK_ENTRY (geNY), st);
    sprintf (st, "%f", s.camera_normal[2]);
    gtk_entry_set_text (GTK_ENTRY (geNZ), st);
    g_signal_connect (geX, "focus-out-event", G_CALLBACK (callback_camerapos), NULL);
    g_signal_connect (geY, "focus-out-event", G_CALLBACK (callback_camerapos), NULL);
    g_signal_connect (geZ, "focus-out-event", G_CALLBACK (callback_camerapos), NULL);
    g_signal_connect (geDX, "focus-out-event", G_CALLBACK (callback_camerapos), NULL);
    g_signal_connect (geDY, "focus-out-event", G_CALLBACK (callback_camerapos), NULL);
    g_signal_connect (geDZ, "focus-out-event", G_CALLBACK (callback_camerapos), NULL);
    g_signal_connect (geNX, "focus-out-event", G_CALLBACK (callback_camerapos), NULL);
    g_signal_connect (geNY, "focus-out-event", G_CALLBACK (callback_camerapos), NULL);
    g_signal_connect (geNZ, "focus-out-event", G_CALLBACK (callback_camerapos), NULL);
    gtk_table_attach (GTK_TABLE (gtkCameraPosition), glCameraPosition, 0, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
    gtk_table_attach (GTK_TABLE (gtkCameraPosition), glX, 0, 1, 1, 2, GTK_SHRINK, GTK_SHRINK, 0, 0);
    gtk_table_attach (GTK_TABLE (gtkCameraPosition), glY, 0, 1, 2, 3, GTK_SHRINK, GTK_SHRINK, 0, 0);
    gtk_table_attach (GTK_TABLE (gtkCameraPosition), glZ, 0, 1, 3, 4, GTK_SHRINK, GTK_SHRINK, 0, 0);
    gtk_table_attach_defaults (GTK_TABLE (gtkCameraPosition), geX, 1, 2, 1, 2);
    gtk_table_attach_defaults (GTK_TABLE (gtkCameraPosition), geY, 1, 2, 2, 3);
    gtk_table_attach_defaults (GTK_TABLE (gtkCameraPosition), geZ, 1, 2, 3, 4);
    gtk_table_attach (GTK_TABLE (gtkCameraPosition), glCameraDirection, 0, 2, 4, 5, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
    gtk_table_attach (GTK_TABLE (gtkCameraPosition), glDX, 0, 1, 5, 6, GTK_SHRINK, GTK_SHRINK, 0, 0);
    gtk_table_attach (GTK_TABLE (gtkCameraPosition), glDY, 0, 1, 6, 7, GTK_SHRINK, GTK_SHRINK, 0, 0);
    gtk_table_attach (GTK_TABLE (gtkCameraPosition), glDZ, 0, 1, 7, 8, GTK_SHRINK, GTK_SHRINK, 0, 0);
    gtk_table_attach_defaults (GTK_TABLE (gtkCameraPosition), geDX, 1, 2, 5, 6);
    gtk_table_attach_defaults (GTK_TABLE (gtkCameraPosition), geDY, 1, 2, 6, 7);
    gtk_table_attach_defaults (GTK_TABLE (gtkCameraPosition), geDZ, 1, 2, 7, 8);
    gtk_table_attach (GTK_TABLE (gtkCameraPosition), glCameraNormal, 0, 2, 8, 9, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
    gtk_table_attach (GTK_TABLE (gtkCameraPosition), glNX, 0, 1, 9, 10, GTK_SHRINK, GTK_SHRINK, 0, 0);
    gtk_table_attach (GTK_TABLE (gtkCameraPosition), glNY, 0, 1, 10, 11, GTK_SHRINK, GTK_SHRINK, 0, 0);
    gtk_table_attach (GTK_TABLE (gtkCameraPosition), glNZ, 0, 1, 11, 12, GTK_SHRINK, GTK_SHRINK, 0, 0);
    gtk_table_attach_defaults (GTK_TABLE (gtkCameraPosition), geNX, 1, 2, 9, 10);
    gtk_table_attach_defaults (GTK_TABLE (gtkCameraPosition), geNY, 1, 2, 10, 11);
    gtk_table_attach_defaults (GTK_TABLE (gtkCameraPosition), geNZ, 1, 2, 11, 12);
    GtkWidget* gaCameraPosition = gtk_alignment_new (0., 0., 1., 0.);
    GtkWidget* gvbSettings = gtk_vbox_new (FALSE, 5);
    gtk_container_add (GTK_CONTAINER (gaCameraPosition), gtkCameraPosition);
    gtk_box_pack_start (GTK_BOX (gvbSettings), gaCameraPosition, TRUE, TRUE, TRUE);

    // ======================
    // | Terrain parameters |
    // ======================

    glRVC = gtk_list_store_new (1, G_TYPE_STRING);
    glTCC = gtk_list_store_new (1, G_TYPE_STRING);
    GtkTreeIter giterRVC, giterTCC;
    struct dirent** namelist;
    int n, i;

    // Get Random Variable file list
    n = scandir (".", &namelist, filter_rvp, alphasort);
    if (n < 0) {
        printf ("\n[MAIN] Could not open local directory! Aborting...\n");
        free_all ();
        return 1;
    }
    for (i = 0; i < n; i++) {
        gtk_list_store_append (glRVC, &giterRVC);
        gtk_list_store_set (glRVC, &giterRVC, 0, namelist[i]->d_name, -1);
        free (namelist[i]);
    }
    free (namelist);

    // Get Terrain Color file list
    n = scandir (".", &namelist, filter_tcp, alphasort);
    if (n < 0) {
        printf ("\n[MAIN] Could not open local directory! Aborting...\n");
        free_all ();
        return 1;
    }
    for (i = 0; i < n; i++) {
        gtk_list_store_append (glTCC, &giterTCC);
        gtk_list_store_set (glTCC, &giterTCC, 0, namelist[i]->d_name, -1);
        free (namelist[i]);
    }
    free (namelist);

    GtkCellRenderer* cell = gtk_cell_renderer_text_new ();
    GtkTreeViewColumn* col_rvp = gtk_tree_view_column_new_with_attributes ("Random Variable Profile", cell, "text", 0, NULL);
    GtkTreeViewColumn* col_tcp = gtk_tree_view_column_new_with_attributes ("Terrain Color Profile", cell, "text", 0, NULL);
    glRandomVariableChooser = gtk_tree_view_new_with_model (GTK_TREE_MODEL (glRVC));
    gtk_tree_view_append_column (GTK_TREE_VIEW (glRandomVariableChooser), col_rvp);
    glTerrainColorChooser = gtk_tree_view_new_with_model (GTK_TREE_MODEL (glTCC));
    gtk_tree_view_append_column (GTK_TREE_VIEW (glTerrainColorChooser), col_tcp);
    gtk_box_pack_start (GTK_BOX (gvbSettings), glRandomVariableChooser, TRUE, TRUE, TRUE);
    gtk_box_pack_start (GTK_BOX (gvbSettings), glTerrainColorChooser, TRUE, TRUE, TRUE);

    // Regenerate terrain button
    geRegenSizeX = gtk_entry_new ();
    geRegenSizeY = gtk_entry_new ();
    gtk_entry_set_text (GTK_ENTRY (geRegenSizeX), "4000.");
    gtk_entry_set_text (GTK_ENTRY (geRegenSizeY), "4000.");
    ghbRegenSize = gtk_hbox_new (FALSE, 5);
    GtkWidget* glSize;
    GtkWidget* glMSize;
    glSize = gtk_label_new ("Size : ");
    glMSize = gtk_label_new ("x");
    gtk_box_pack_start (GTK_BOX (ghbRegenSize), glSize, FALSE, FALSE, FALSE);
    gtk_box_pack_start (GTK_BOX (ghbRegenSize), geRegenSizeX, TRUE, TRUE, TRUE);
    gtk_box_pack_start (GTK_BOX (ghbRegenSize), glMSize, FALSE, FALSE, FALSE);
    gtk_box_pack_start (GTK_BOX (ghbRegenSize), geRegenSizeY, TRUE, TRUE, TRUE);
    gtk_box_pack_start (GTK_BOX (gvbSettings), ghbRegenSize, FALSE, FALSE, FALSE);
    gbRegenTerrain = gtk_button_new_with_label ("Generate Terrain");
    g_signal_connect (gbRegenTerrain, "clicked", G_CALLBACK (callback_regen), NULL);
    gpProgress = gtk_progress_bar_new ();
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (gpProgress), 0.0);
    gtk_box_pack_start (GTK_BOX (gvbSettings), gbRegenTerrain, FALSE, FALSE, FALSE);

    // Render to bitmap
    geRenderHeight = gtk_entry_new ();
    geRenderWidth = gtk_entry_new ();
    gtk_entry_set_text (GTK_ENTRY (geRenderHeight), "2048");
    gtk_entry_set_text (GTK_ENTRY (geRenderWidth), "2048");
    ghbRenderSize = gtk_hbox_new (FALSE, 5);
    GtkWidget* glRSize;
    GtkWidget* glRMSize;
    glRSize = gtk_label_new ("Size : ");
    glRMSize = gtk_label_new ("x");
    gtk_box_pack_start (GTK_BOX (ghbRenderSize), glRSize, FALSE, FALSE, FALSE);
    gtk_box_pack_start (GTK_BOX (ghbRenderSize), geRenderWidth, TRUE, TRUE, TRUE);
    gtk_box_pack_start (GTK_BOX (ghbRenderSize), glRMSize, FALSE, FALSE, FALSE);
    gtk_box_pack_start (GTK_BOX (ghbRenderSize), geRenderHeight, TRUE, TRUE, TRUE);
    gtk_box_pack_start (GTK_BOX (gvbSettings), ghbRenderSize, FALSE, FALSE, FALSE);
    gbRender = gtk_button_new_with_label ("Render to file");
    g_signal_connect (gbRender, "clicked", G_CALLBACK (callback_render_file), NULL);
    gtk_box_pack_start (GTK_BOX (gvbSettings), gbRender, FALSE, FALSE, FALSE);
    gtk_box_pack_start (GTK_BOX (gvbSettings), gpProgress, FALSE, FALSE, FALSE);

    // ==========
    // | Layout |
    // ==========

    gtk_table_attach (GTK_TABLE (gtkTable), ghsvSunLightColor, 1, 2, 0, 1, GTK_SHRINK, GTK_SHRINK, 0, 0);
    gtk_table_attach (GTK_TABLE (gtkTable), gvbLabels, 0, 1, 0, 1, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
    gtk_table_attach (GTK_TABLE (gtkTable), gvbSettings, 3, 4, 0, 4, GTK_SHRINK, GTK_EXPAND | GTK_FILL, 10, 10);
    gtk_table_attach_defaults (GTK_TABLE (gtkTable), giImage, 0, 2, 1, 4);
    gtk_table_attach (GTK_TABLE (gtkTable), gsVScale, 2, 3, 0, 4, GTK_SHRINK, GTK_EXPAND | GTK_FILL, 20, 20);
    gtk_table_attach (GTK_TABLE (gtkTable), gsHScale, 0, 4, 4, 5, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 5, 5);

    // ==================
    // | Initialization |
    // ==================

    gtk_container_add (GTK_CONTAINER (gwWindow), gtkTable);
    gtk_widget_show_all (gwWindow);
    setup (GTK_WIDGET(giImage));

    gtk_main (); // Main loop

    // ====================
    // | Deinitialization |
    // ====================

    free_all ();
    FreeImage_DeInitialise ();

    return 0;
}

void free_all () {
    rv_profile_free (&rvp);
    tc_profile_free (&tcp);
    terrain_free (&t);
    rendering_free (&r);
}

void setup (GtkWidget* gwWidget) {
    printf ("[MAIN] Window Setup Event\n");
    GdkGLContext* glContext = gtk_widget_get_gl_context (gwWidget);
    GdkGLDrawable* glDrawable = gtk_widget_get_gl_drawable (gwWidget);

    if (!gdk_gl_drawable_gl_begin (glDrawable, glContext)) g_assert_not_reached ();

    terrain_3d_init (s);
    r = terrain_3d_prerender (t, tcp, NULL); // Pre-render terrain

    gdk_gl_drawable_gl_end (glDrawable);
}

gboolean configure (GtkWidget* gwWidget, GdkEventConfigure* gdkEvent) {
    printf ("[MAIN] Window Resize Event\n");
    GdkGLContext* glContext = gtk_widget_get_gl_context (gwWidget);
    GdkGLDrawable* glDrawable = gtk_widget_get_gl_drawable (gwWidget);

    if (!gdk_gl_drawable_gl_begin (glDrawable, glContext)) g_assert_not_reached ();

    sun_update (&s);
    w = gdkEvent->width;
    h = gdkEvent->height;
    terrain_3d_camera (s, gdkEvent->width, gdkEvent->height);
    terrain_3d_light (s);

    gdk_gl_drawable_gl_end (glDrawable);
    return TRUE;
}

gboolean render (GtkWidget *gwWidget, GdkEventExpose *gdkEvent) {
    GdkGLContext* glContext = gtk_widget_get_gl_context (gwWidget);
    GdkGLDrawable* glDrawable = gtk_widget_get_gl_drawable (gwWidget);

    if (!gdk_gl_drawable_gl_begin (glDrawable, glContext)) g_assert_not_reached ();
    // Update Terrain Window
    terrain_3d_display (s, r);

    gdk_gl_drawable_swap_buffers (glDrawable);
    gdk_gl_drawable_gl_end (glDrawable);

    // Update Info Labels
    char sbuf[100];
    sprintf (sbuf, "Azimuth : %f°", s.sun_azimuth);
    gtk_label_set_text (GTK_LABEL (glLabelAz), sbuf);
    sprintf (sbuf, "Elevation : %f°", s.sun_elevation);
    gtk_label_set_text (GTK_LABEL (glLabelEl), sbuf);
    sprintf (sbuf, "Sunlight color : (%d, %d, %d)", (int)(255*s.light_diffuse[0]), (int)(255*s.light_diffuse[1]), (int)(255*s.light_diffuse[2]));
    gtk_label_set_text (GTK_LABEL (glSunlightColor), sbuf);
    return TRUE;
}

void camera_reset () {
    memcpy (s.camera_position, CAMERA_POSITION0, 3*sizeof(GLfloat));
    memcpy (s.camera_direction, CAMERA_DIRECTION0, 3*sizeof(GLfloat));
    memcpy (s.camera_normal, CAMERA_NORMAL0, 3*sizeof(GLfloat));
    memcpy (s.camera_right, CAMERA_RIGHT0, 3*sizeof(GLfloat));
}

gboolean callback_keypressed (GtkWidget* gwWidget, GdkEventKey* gdkEvent, gpointer data) {
    float v[3];
    switch (gdkEvent->keyval) {
        case GDK_t: // Go upwards
            vectors_add (s.camera_position, s.camera_normal);
            terrain_3d_camera (s, w, h);
            break;
        case GDK_g: // Go downwards
            vectors_subs (s.camera_position, s.camera_normal);
            terrain_3d_camera (s, w, h);
            break;
        case GDK_a: // Go Left
            vectors_subs (s.camera_position, s.camera_right);
            terrain_3d_camera (s, w, h);
            break;
        case GDK_e: // Go Right
            vectors_add (s.camera_position, s.camera_right);
            terrain_3d_camera (s, w, h);
            break;
        case GDK_z: // Go Forward
            vectors_add (s.camera_position, s.camera_direction);
            terrain_3d_camera (s, w, h);
            break;
        case GDK_q: // Turn left
            memcpy (v, s.camera_normal, 3*sizeof(GLfloat));
            vector_normalize (v);
            vector_rotate (s.camera_direction, v, 0.05f);
            vector_rotate (s.camera_right, v, 0.05f);
            terrain_3d_camera (s, w, h);
            break;
        case GDK_s: // Go Backwards
            vectors_subs (s.camera_position, s.camera_direction);
            terrain_3d_camera (s, w, h);
            break;
        case GDK_d: // Turn Right
            memcpy (v, s.camera_normal, 3*sizeof(GLfloat));
            vector_normalize (v);
            vector_rotate (s.camera_direction, v, -0.05f);
            vector_rotate (s.camera_right, v, -0.05f);
            terrain_3d_camera (s, w, h);
            break;
        case GDK_f: // Turn Upwards
            memcpy (v, s.camera_right, 3*sizeof(GLfloat));
            vector_normalize (v);
            vector_rotate (s.camera_normal, v, -0.05f);
            vector_rotate (s.camera_direction, v, -0.05f);
            terrain_3d_camera (s, w, h);
            break;
        case GDK_r: // Turn Downwards
            memcpy (v, s.camera_right, 3*sizeof(GLfloat));
            vector_normalize (v);
            vector_rotate (s.camera_normal, v, 0.05f);
            vector_rotate (s.camera_direction, v, 0.05f);
            terrain_3d_camera (s, w, h);
            break;
        case GDK_c: // Reset Position
            camera_reset ();
            terrain_3d_camera (s, w, h);
            break;
        case GDK_x: // Axes ON/OFF
            s.axes = 1 - s.axes;
            break;
        case GDK_w: // Water ON/OFF
            s.water = 1 - s.water;
            break;
        case GDK_l:
            s.light_dir = 1 - s.light_dir;
            break;
        case GDK_plus: // Speed +
            vector_scalarm (s.camera_direction, 2.0f);
            vector_scalarm (s.camera_right, 2.0f);
            vector_scalarm (s.camera_normal, 2.0f);
            break;
        case GDK_minus: // Speed -
            vector_scalarm (s.camera_direction, 0.5f);
            vector_scalarm (s.camera_right, 0.5f);
            vector_scalarm (s.camera_normal, 0.5f);
            break;
        default: return TRUE;
    }
    gtk_widget_queue_draw_area (giImage, 0, 0, giImage->allocation.width, giImage->allocation.height);
    return TRUE;
}

void camera_entry_update () {
    char st[100];
    sprintf (st, "%f", s.camera_position[0]);
    gtk_entry_set_text (GTK_ENTRY (geX), st);
    sprintf (st, "%f", s.camera_position[1]);
    gtk_entry_set_text (GTK_ENTRY (geY), st);
    sprintf (st, "%f", s.camera_position[2]);
    gtk_entry_set_text (GTK_ENTRY (geZ), st);
    sprintf (st, "%f", s.camera_direction[0]);
    gtk_entry_set_text (GTK_ENTRY (geDX), st);
    sprintf (st, "%f", s.camera_direction[1]);
    gtk_entry_set_text (GTK_ENTRY (geDY), st);
    sprintf (st, "%f", s.camera_direction[2]);
    gtk_entry_set_text (GTK_ENTRY (geDZ), st);
    sprintf (st, "%f", s.camera_normal[0]);
    gtk_entry_set_text (GTK_ENTRY (geNX), st);
    sprintf (st, "%f", s.camera_normal[1]);
    gtk_entry_set_text (GTK_ENTRY (geNY), st);
    sprintf (st, "%f", s.camera_normal[2]);
    gtk_entry_set_text (GTK_ENTRY (geNZ), st);
}

gboolean callback_keyreleased (GtkWidget* gwWidget, GdkEventKey* gdkEvent, gpointer data) {
    camera_entry_update ();
    return TRUE;
}

gboolean callback_camerapos (GtkWidget* gwWidget, GdkEventFocus* gdkFocus, gpointer data) {
    float dir[3]; // Camera Direction
    float pos[3]; // Camera Position
    float nrm[3];
    if (gwWidget == geX || gwWidget == geY || gwWidget == geZ) {
        pos[0] = (float)atof (gtk_entry_get_text (GTK_ENTRY (geX)));
        pos[1] = (float)atof (gtk_entry_get_text (GTK_ENTRY (geY)));
        pos[2] = (float)atof (gtk_entry_get_text (GTK_ENTRY (geZ)));
        memcpy (s.camera_position, pos, sizeof(GLfloat)*3);
    }
    if (gwWidget == geDX || gwWidget == geDY || gwWidget == geDZ) {
        dir[0] = (float)atof (gtk_entry_get_text (GTK_ENTRY (geDX)));
        dir[1] = (float)atof (gtk_entry_get_text (GTK_ENTRY (geDY)));
        dir[2] = (float)atof (gtk_entry_get_text (GTK_ENTRY (geDZ)));
        memcpy (s.camera_direction, dir, sizeof(GLfloat)*3);
        s.camera_normal[1] = dir[2]/(dir[0]/dir[1] - dir[1]);
        s.camera_normal[0] = -s.camera_normal[1]*dir[0]/dir[1];
        s.camera_normal[2] = 1.;
        vector_normalize (s.camera_normal);
        vector_normalize (s.camera_direction);
        vectors_normal (s.camera_direction, s.camera_normal, s.camera_right);
    }
    if (gwWidget == geNX || gwWidget == geNY || gwWidget == geNZ) {
        nrm[0] = (float) atof (gtk_entry_get_text (GTK_ENTRY (geNX)));
        nrm[1] = (float) atof (gtk_entry_get_text (GTK_ENTRY (geNY)));
        nrm[2] = (float) atof (gtk_entry_get_text (GTK_ENTRY (geNZ)));
        memcpy (s.camera_normal, nrm, sizeof(GLfloat)*3);
        vector_normalize (s.camera_normal);
        vectors_normal (s.camera_direction, s.camera_normal, s.camera_right);
    }
    camera_entry_update ();
    terrain_3d_camera (s, w, h);
    gtk_widget_queue_draw_area (giImage, 0, 0, giImage->allocation.width, giImage->allocation.height);
    return FALSE;
}

gboolean callback_elevation (GtkWidget* gwWidget, gpointer data) {
    s.sun_elevation = 90. - (GLfloat) gtk_adjustment_get_value (GTK_ADJUSTMENT (gwWidget));
    sun_update (&s);
    terrain_3d_light (s);
    gtk_widget_queue_draw_area (giImage, 0, 0, giImage->allocation.width, giImage->allocation.height);
    return TRUE;
}

gboolean callback_azimuth (GtkWidget *gwWidget, gpointer data) {
    s.sun_azimuth = (GLfloat) gtk_adjustment_get_value (GTK_ADJUSTMENT (gwWidget));
    sun_update (&s);
    terrain_3d_light (s);
    gtk_widget_queue_draw_area (giImage, 0, 0, giImage->allocation.width, giImage->allocation.height);
    return TRUE;
}

void callback_progress_gen (double fraction) {
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (gpProgress), fraction/2);
    while (gtk_events_pending ()) gtk_main_iteration ();
}

void callback_progress_render (double fraction) {
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (gpProgress), .5 + fraction/2);
    while (gtk_events_pending ()) gtk_main_iteration ();
}

gboolean callback_regen (GtkWidget* gwWidget, gpointer data) {
    gchar* sRV;
    char sRVFile[1024] = "./";
    gchar* sTC;
    char sTCFile[1024] = "./";
    GtkTreeSelection* selTC;
    GtkTreeIter iterTC;
    GtkTreeSelection* selRV;
    GtkTreeIter iterRV;
    selRV = gtk_tree_view_get_selection (GTK_TREE_VIEW (glRandomVariableChooser));
    selTC = gtk_tree_view_get_selection (GTK_TREE_VIEW (glTerrainColorChooser));
    if (!gtk_tree_selection_get_selected (selRV, NULL, &iterRV)) return TRUE;
    if (!gtk_tree_selection_get_selected (selTC, NULL, &iterTC)) return TRUE;
    gtk_tree_model_get (GTK_TREE_MODEL (glRVC), &iterRV, 0, &sRV, -1);
    gtk_tree_model_get (GTK_TREE_MODEL (glTCC), &iterTC, 0, &sTC, -1);
    strcat (sRVFile, sRV);
    strcat (sTCFile, sTC);

    free_all ();
    if (generate (sRVFile, sTCFile, (float)atof (gtk_entry_get_text (GTK_ENTRY (geRegenSizeX))), (float)atof (gtk_entry_get_text (GTK_ENTRY (geRegenSizeY))), callback_progress_gen) != 0) exit(10);
    g_free (sRV);
    g_free (sTC);
    camera_reset ();
    camera_entry_update ();

    GdkGLContext* glContext = gtk_widget_get_gl_context (giImage);
    GdkGLDrawable* glDrawable = gtk_widget_get_gl_drawable (giImage);

    if (!gdk_gl_drawable_gl_begin (glDrawable, glContext)) g_assert_not_reached ();

    printf ("    ° Configure\n");
    terrain_3d_init (s);
    r = terrain_3d_prerender (t, tcp, callback_progress_render);
    printf ("    ° Setup\n");
    sun_update (&s);
    terrain_3d_camera (s, w, h);
    terrain_3d_light (s);

    gdk_gl_drawable_gl_end (glDrawable);

    gtk_widget_queue_draw_area (giImage, 0, 0, giImage->allocation.width, giImage->allocation.height);
    return TRUE;
}

gboolean callback_sunlight (GtkWidget* gwWidget, gpointer data) {
    gdouble cr, cg, cb, ch, cs, cv;
    gtk_hsv_get_color (GTK_HSV(ghsvSunLightColor), &ch, &cs, &cv);
    gtk_hsv_to_rgb (ch, cs, cv, &cr, &cg, &cb);
    s.light_ambient[0] = (float)cr/5.;
    s.light_ambient[1] = (float)cg/5.;
    s.light_ambient[2] = (float)cb/5.;
    s.light_diffuse[0] = (float)cr;
    s.light_diffuse[1] = (float)cg;
    s.light_diffuse[2] = (float)cb;
    terrain_3d_light (s);
    gtk_widget_queue_draw_area (giImage, 0, 0, giImage->allocation.width, giImage->allocation.height);
    return TRUE;
}

gboolean callback_grabfocus (GtkWidget* gwWidget, GdkEventMotion* geMotion, gpointer data) {
    gtk_widget_grab_focus (gwWidget);
    return TRUE;
}

gboolean callback_render_file (GtkWidget* gwWidget, gpointer data) {
    GtkWidget* dlg;
    dlg = gtk_file_chooser_dialog_new ("Save rendering to...", GTK_WINDOW (gwWindow), GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
    if (gtk_dialog_run (GTK_DIALOG (dlg)) == GTK_RESPONSE_ACCEPT) {
        char* filename;
        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dlg));
        gtk_widget_destroy (dlg);
        terrain_3d_offscreen_render (filename, atoi (gtk_entry_get_text (GTK_ENTRY (geRenderWidth))), atoi (gtk_entry_get_text (GTK_ENTRY (geRenderHeight))), s, t, tcp, callback_progress_gen);
        g_free (filename);
    } else gtk_widget_destroy (dlg);
    return TRUE;
}

GdkPixbuf* create_pixbuf (const char* filename) {
    GdkPixbuf* pixbuf;
    GError* err = NULL;
    pixbuf = gdk_pixbuf_new_from_file (filename, &err);
    if (!pixbuf) {
        fprintf (stderr, "%s\n", err->message);
        g_error_free (err);
    }
    return pixbuf;
}
