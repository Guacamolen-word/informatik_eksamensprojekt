 //[ Adam ]
 
#include "gui.hpp"


void gui::load_gui() {

    GtkBuilder *builder = gtk_builder_new_from_file("xml/glade.glade");

    GObject *window = gtk_builder_get_object(builder, "main_window");

    GObject *button = gtk_builder_get_object(builder, "button_start_server");
    gui::signal_handler = g_signal_connect(button, "clicked", G_CALLBACK(gui::button_start_callback), GTK_BUTTON(button));
    gui::start_stop_button = GTK_BUTTON(button);

    gui::ip_entry = GTK_ENTRY(gtk_builder_get_object(builder, "input_ip"));
    gui::port_entry = GTK_ENTRY(gtk_builder_get_object(builder, "input_port"));
    gui::user_entry = GTK_ENTRY(gtk_builder_get_object(builder, "input_user"));
    gui::password_entry = GTK_ENTRY(gtk_builder_get_object(builder, "input_password"));
    gui::db_entry = GTK_ENTRY(gtk_builder_get_object(builder, "input_database"));

    gui::cert_file = GTK_FILE_CHOOSER(gtk_builder_get_object(builder, "file_cert"));
    gui::key_file = GTK_FILE_CHOOSER(gtk_builder_get_object(builder, "file_key"));
    gui::https_checkbox = GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "check_https"));
	g_signal_connect(window, "destroy", G_CALLBACK(gui::exit_callback), NULL);

//    gtk_window_set_default_size(GTK_WINDOW(window), 350, 380);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

    gtk_widget_set_visible(GTK_WIDGET(window), TRUE);
    gtk_window_present(GTK_WINDOW(window));

    g_object_unref(builder);

    gtk_main();

}

void gui::exit_callback(GtkWindow *window) {
    if(gui::server != NULL) {
        gui::button_stop_callback(NULL);
    }
    gtk_window_close(window);
    exit(0);
}

void gui::button_start_callback(GtkButton *button) {
    (void)button;

    int port = std::atoi( gtk_entry_get_text(gui::port_entry) );

    server = NULL;
    if( gtk_toggle_button_get_active( gui::https_checkbox ) == TRUE  ) {
        gui::server = new networking::server( 
                gtk_entry_get_text(gui::ip_entry),
                port, 
                true, "./certs/cert.pem", "./certs/key.pem",
                gtk_entry_get_text(gui::user_entry),
                gtk_entry_get_text(gui::password_entry),
                gtk_entry_get_text(gui::db_entry));
    }else {
        gui::server = new networking::server(
                gtk_entry_get_text(gui::ip_entry),
                port, 
                false, "", "",
                gtk_entry_get_text(gui::user_entry),
                gtk_entry_get_text(gui::password_entry),
                gtk_entry_get_text(gui::db_entry));
    }

    if(server == NULL)
        return;
    server_thread = std::thread(networking::server_starter, server);

    g_signal_handler_disconnect(gui::start_stop_button, gui::signal_handler);
    gtk_button_set_label(gui::start_stop_button, "Stop server");

    gui::signal_handler =
        g_signal_connect(gui::start_stop_button, "clicked", G_CALLBACK(gui::button_stop_callback), NULL);

}


void gui::button_stop_callback(GtkButton *button) {
    (void)button;

    std::cout << "Stopping server\n";

    server->stop();
    server_thread.join();

    gui::server = NULL;
    g_signal_handler_disconnect(gui::start_stop_button, gui::signal_handler);
    gtk_button_set_label(gui::start_stop_button , "Start server");
    gui::signal_handler =
        g_signal_connect(gui::start_stop_button, "clicked", G_CALLBACK(gui::button_start_callback), NULL);

    delete server;

}

