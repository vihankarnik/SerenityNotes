#include <stdio.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define MAX_FILES 3
bool EDITED = FALSE;

typedef struct{
    GFile *file;
    GtkTextBuffer *textBuffer;
    char *filepath;
} FileData;

FileData Files[MAX_FILES];

    //gtk_window_set_title(GTK_WINDOW(window), g_file_get_basename(file));

void fileToTextBuffer(GFile *file, GtkTextBuffer *textBuffer) {
    char *text;
    gsize len;

    g_file_load_contents(file, NULL, &text, &len, NULL, NULL);  //loads text from the file
    gtk_text_buffer_set_text(textBuffer, text, len);    //dumps the text into textBuffer

    g_free(text);
}

void loadFiles(){
    for(int i = 0; i < MAX_FILES; i++){
        char filepath[40];  // file1.txt, file2.txt
        sprintf(filepath, "assets/note%d.txt", i);
        Files[i].filepath = filepath;
        Files[i].file = g_file_new_for_path(filepath);
        Files[i].textBuffer = gtk_text_buffer_new(NULL);
        fileToTextBuffer(Files[i].file, Files[i].textBuffer);
        printf("\n%s\n", Files[i].filepath);
    }
}

void textBufferToFile(GFile *file, GtkTextBuffer *textBuffer, int i){
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(textBuffer, &start, &end);

    char *text = gtk_text_buffer_get_text(textBuffer, &start, &end, FALSE);

    g_file_replace_contents(file, text, strlen(text), NULL, FALSE, G_FILE_CREATE_NONE, NULL, NULL, NULL);
    EDITED = FALSE;

    //filepath encoding didnt let me use this
    //g_file_set_contents(filepath, text, len, NULL);    //couldnt figure out how to use GFile here
    //these dont work in this version
    //g_file_replace_contents_async(file, text, len, NULL, FALSE, G_FILE_CREATE_NONE, NULL, NULL, NULL);
    //g_file_replace(file, text, len, fakjsdlkfjalskdjf)
    //g_file_replace_contents(file, "hey", 3, NULL, TRUE, G_FILE_CREATE_NONE, NULL, NULL, NULL);
}

void saveFiles(){
    for(int i = 0; i < MAX_FILES; i++){
        textBufferToFile(Files[i].file, Files[i].textBuffer, i);
    }
    EDITED = FALSE;
}

static void onTextChange(GtkTextBuffer currentTextBuffer) {
    if(EDITED)
        return;
    EDITED = TRUE;
    saveFiles(Files);

    //testing
    //const char *title = gtk_window_get_title(GTK_WINDOW(window));
    //size_t titleLength = strlen(title);
    //char newTitle[titleLength + 2];
    //sprintf(newTitle, "* %s", title);
    //gtk_window_set_title(GTK_WINDOW(window), newTitle);
}
void buildTextView(GtkWidget *window, GtkWidget *grid, int i) {
    GtkTextBuffer *currentTextBuffer = Files[i].textBuffer;
    GtkWidget *textView = gtk_text_view_new_with_buffer(currentTextBuffer);    // textView is a container that displays a buffer

    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textView), 1);    // 1- Character wise wrapping

    GtkWidget *scrolledWindow = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);  // Set scroll policies
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolledWindow), textView);

    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(textView), 2);  // number of pixels to pad
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(textView), 2);
    gtk_text_view_set_top_margin(GTK_TEXT_VIEW(textView), 2);
    gtk_text_view_set_bottom_margin(GTK_TEXT_VIEW(textView), 2);

    g_signal_connect(currentTextBuffer, "changed", G_CALLBACK(onTextChange), NULL);

    gtk_widget_set_hexpand (scrolledWindow, TRUE);
    gtk_widget_set_vexpand (scrolledWindow, TRUE);
    gtk_widget_set_size_request(scrolledWindow, 400, 300);
    gtk_grid_attach(GTK_GRID(grid), scrolledWindow, i, 0, 1, 1);
    //hierarchy- window > overlay > grid > scrolledWindow > textView
}

static void activate(GtkApplication *app){
    GtkWidget *window = gtk_application_window_new(app);

    gtk_window_set_title(GTK_WINDOW(window), "SereneNotes");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    loadFiles();    //Files is a global array of FileData which contains GFile and its textBuffer

    GtkWidget *grid = gtk_grid_new();
    GtkWidget *overlay = gtk_overlay_new();
    gtk_overlay_set_child(GTK_OVERLAY(overlay), grid);
    gtk_window_set_child(GTK_WINDOW(window), overlay);
        //hierarchy- window > overlay > grid > scrolledWindow > textView

    for(int i = 0; i < MAX_FILES; i++){
        buildTextView(window, grid, i);  // this will make the text appear and edit
    }
    gtk_widget_set_halign (overlay, GTK_ALIGN_CENTER);
    gtk_widget_set_valign (overlay, GTK_ALIGN_START);

    gtk_window_present(GTK_WINDOW(window));
}


// compile using this
// gcc app.c -o app.exe $(pkg-config --cflags --libs gtk4)
int main(){
    GtkApplication *app;
    //gtk_init();   //GtkApplication object does it for us
    int status;

    app = gtk_application_new("my.first.app", G_APPLICATION_HANDLES_OPEN);
    

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

