#include <stdio.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
GFile *file;
GtkTextBuffer *textBuffer;
bool EDITED = FALSE;

void setActiveFile(GtkWidget *window, GFile *file) {
    EDITED = FALSE;
    gtk_window_set_title(GTK_WINDOW(window), g_file_get_basename(file));

    char *OPEN_FILE_PATH = "assets/notes.txt";

    g_object_unref(file);
}

void textBufferToFile(GFile *file){
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(textBuffer, &start, &end);

    char *text = gtk_text_buffer_get_text(textBuffer, &start, &end, FALSE);

    g_file_set_contents("assets/notes.txt", text, -1, NULL);    //couldnt figure out how to use GFile here
    printf("saved");
    EDITED = FALSE;
    gsize len = strlen(text);
    //these dont work in this version
    //g_file_replace_contents_async(file, text, len, NULL, FALSE, G_FILE_CREATE_NONE, NULL, NULL, NULL);
    //g_file_replace(file, )
    //g_file_replace_contents(file, "hey", 3, NULL, TRUE, G_FILE_CREATE_NONE, NULL, NULL, NULL);
}

static void onTextChange(GtkWidget *textBuffer, GtkWidget *window) {
    if (EDITED) {
        return;
    }

    EDITED = TRUE;

    textBufferToFile(file);

    //const char *title = gtk_window_get_title(GTK_WINDOW(window));
    //size_t titleLength = strlen(title);

    //char newTitle[titleLength + 2];
    //sprintf(newTitle, "* %s", title);

    //gtk_window_set_title(GTK_WINDOW(window), newTitle);
}
void buildTextView(GtkWidget *window, GtkWidget *grid, int i) {
    GtkWidget *textView = gtk_text_view_new_with_buffer(textBuffer);    // textView is a container that displays a buffer

    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textView), 1);    // 1- Character wise wrapping

    GtkWidget *scrolledWindow = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);  // Set scroll policies
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolledWindow), textView);

    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(textView), 2);  // number of pixels to pad
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(textView), 2);
    gtk_text_view_set_top_margin(GTK_TEXT_VIEW(textView), 2);
    gtk_text_view_set_bottom_margin(GTK_TEXT_VIEW(textView), 2);

    g_signal_connect(textBuffer, "changed", G_CALLBACK(onTextChange), window);

    gtk_widget_set_hexpand (scrolledWindow, TRUE);
    gtk_widget_set_vexpand (scrolledWindow, TRUE);
    gtk_widget_set_size_request(scrolledWindow, 400, 300);
    gtk_grid_attach(GTK_GRID(grid), scrolledWindow, i, 0, 1, 1);
    //hierarchy- window > overlay > grid > scrolledWindow > textView
}

void fileToTextBuffer(GFile *file) {
    char *text;
    gsize len;

    g_file_load_contents(file, NULL, &text, &len, NULL, NULL);

    gtk_text_buffer_set_text(textBuffer, text, len);
    g_free(text);
}

static void activate(GtkApplication *app){
    GtkWidget *window = gtk_application_window_new(app);

    gtk_window_set_title(GTK_WINDOW(window), "SereneNotes");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    fileToTextBuffer(file); //extracts file to populate textBuffer

    GtkWidget *grid = gtk_grid_new();
    GtkWidget *overlay = gtk_overlay_new();
    gtk_overlay_set_child(GTK_OVERLAY(overlay), grid);

    for(int i = 0; i < 1; i++){

        buildTextView(window, grid, i);  // this will make the text appear and edit
        
    }
    gtk_window_set_child(GTK_WINDOW(window), overlay);
    gtk_widget_set_halign (overlay, GTK_ALIGN_CENTER);
    gtk_widget_set_valign (overlay, GTK_ALIGN_START);
    
    //gtk_container_add(GTK_CONTAINER(window), grid);

    if(file){
        setActiveFile(window, file);
    }

    gtk_window_present(GTK_WINDOW(window));
}


// compile using this
// gcc app.c -o app.exe $(pkg-config --cflags --libs gtk4)
int main(){
    file = g_file_new_for_path("assets/notes.txt");
    textBuffer = gtk_text_buffer_new(NULL);
    GtkApplication *app;    // why do we need this?
    //gtk_init();   //GtkApplication object does it for us
    int status;

    app = gtk_application_new("my.first.app", G_APPLICATION_HANDLES_OPEN);
    
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

