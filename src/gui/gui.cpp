
#include "gui.hpp"


void gui::load_gui(GtkApplication *application, gpointer user_data) {

    GtkBuilder *builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "xml/glade.glade", NULL);

    GObject *window = gtk_builder_get_object(builder, "main_window");
    gtk_window_set_application(GTK_WINDOW(window), application);

    GObject *button = gtk_builder_get_object(builder, "button_start_server");
    g_signal_connect(button, "clicked", G_CALLBACK(gui::button_start_callback), GTK_BUTTON(button));

    gui::ip_entry = GTK_ENTRY(gtk_builder_get_object(builder, "input_ip"));
    gui::port_entry = GTK_ENTRY(gtk_builder_get_object(builder, "input_port"));

    gtk_widget_set_visible(GTK_WIDGET(window), TRUE);

    g_object_unref(builder);

}

void gui::exit_callback(GtkWindow *window) {
    gtk_window_close(window);
}

void gui::button_start_callback(GtkButton *button) {

}


