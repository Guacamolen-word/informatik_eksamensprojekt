 //[ Adam ]
#pragma once 

#include <gtk-3.0/gtk/gtk.h>
#include <gtk/gtkx.h>
#include <glib/gstdio.h>

#include "../networking/webserver.hpp"

namespace gui {

    static GtkEntry *ip_entry, *port_entry, *user_entry, *password_entry, *db_entry;
    static GtkFileChooser *cert_file, *key_file;
    static GtkButton *start_stop_button;
    static GtkToggleButton *https_checkbox;
    static int signal_handler;

    static std::thread server_thread;
    static networking::server *server;

    void load_gui();
    void exit_callback(GtkWindow *window);
    void button_start_callback(GtkButton *button);
    void button_stop_callback(GtkButton *button);
}


