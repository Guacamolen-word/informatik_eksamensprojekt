#pragma once

#include <gtk/gtk.h>

namespace gui {

    static GtkEntry *ip_entry, *port_entry;
    static GtkFileChooser *cert_file, *key_file;

    void load_gui(GtkApplication *application, gpointer user_data);
    void exit_callback(GtkWindow *window);
    void button_start_callback(GtkButton *button);
}

