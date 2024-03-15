#include <stdio.h>
#include <gtk/gtk.h>
#include <stdlib.h>

static void activate(GtkApplication *app, gpointer data){
    GtkWidget *window;
    window = gtk_application_window_new(app);

    gtk_window_set_title(GTK_WINDOW(window), "SereneNotes");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);


    gtk_window_present(GTK_WINDOW(window));
}

// compile using this
// gcc app.c -o app.exe $(pkg-config --cflags --libs gtk4)
int main(){
    GtkApplication *app;    // why do we need this?
    //gtk_init();   //GtkApplication object does it for us
    int status;

    app = gtk_application_new("my.first.app", G_APPLICATION_DEFAULT_FLAGS);
    
    //GtkWidget *window;
    //window = gtk_application_window_new(app);
    //window = gtk_window_new();
    //gtk_application_add_window(app, GTK_WINDOW(window));

        // once application starts, it makes a new window
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
        // this runs the application till we close it
    status = g_application_run(G_APPLICATION(app), 0, NULL);
        // this releases the reference app
    g_object_unref(app);

    //g_signal_connect(window, "destroy", G_CALLBACK(g_main_quit()), NULL);
    //while(1){
    //    g_main_context_iteration (NULL, TRUE);  // this calls the main function again
    //}

    return status;
}

