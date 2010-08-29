#ifndef __GTK_S9XWINDOW_H
#define __GTK_S9XWINDOW_H

#include <gtk/gtk.h>

#ifdef USE_OPENGL
#include <GL/gl.h>
#endif

#include <glade/glade.h>

#include "gtk_s9x.h"
#include "gtk_glade_window.h"

#define SIZE_FLAG_NO_DRAWING     -1
#define SIZE_FLAG_DIRTY          -2

class Snes9xWindow : public GladeWindow
{
    public:
        Snes9xWindow (Snes9xConfig *config);

        /* Pause related functions */
        void pause_from_focus_change (void);
        void unpause_from_focus_change (void);
        void focus_notify (int state);
        void pause_from_user (void);
        void unpause_from_user (void);
        unsigned char is_paused (void);
        void propagate_pause_state (void);

        /* Fullscreen functions */
        void enter_fullscreen_mode (void);
        void leave_fullscreen_mode (void);
        void toggle_fullscreen_mode (void);
        void finish_fullscreen (void);

        /* Cursor modifying functions */
        void show_mouse_cursor (void);
        void hide_mouse_cursor (void);

        /* Rom-related functions */
        void open_rom_dialog (void);
        void save_state_dialog (void);
        void load_state_dialog (void);
        void configure_widgets (void);
        void save_spc_dialog (void);
        int try_open_rom (const char *filename);
        const char *open_movie_dialog (bool readonly);
        void movie_seek_dialog (void);
        void open_multicart_dialog (void);
        void show_rom_info (void);

        /* GTK-base-related functions */
        void show (void);
        void show_status_message (const char *message);
        void update_statusbar (void);
        void draw_background (int rect_x = -1,
                              int rect_y = -1,
                              int rect_w = -1,
                              int rect_h = -1);
        void set_menu_item_selected (const char *name);
        void set_mouseable_area (int x, int y, int width, int height);
        void set_menu_item_accel_to_binding (const char *name,
                                             const char *binding);
        void reset_screensaver (void);
        void update_accels (void);
        void toggle_ui (void);
        void resize_viewport (int width, int height);

        Snes9xConfig   *config;
        unsigned char  user_pause;
        unsigned char  sys_pause;
        int            last_width, last_height;
        int            mouse_region_x, mouse_region_y;
        int            mouse_region_width, mouse_region_height;
        int            nfs_width, nfs_height, nfs_x, nfs_y;
        int            fullscreen_state;
        int            maximized_state;
        int            focused;
        int            paused_from_focus_loss;
        uint16         mouse_loc_x, mouse_loc_y;
        GdkPixbuf      *icon, *splash;
        GdkCursor      *cursor;
        GtkDrawingArea *drawing_area;
        GtkWidget      *recent_menu;
};

#endif /* __GTK_S9XWINDOW_H */
