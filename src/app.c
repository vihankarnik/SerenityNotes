#include <stdio.h>
#include <gtk/gtk.h>
#include <string.h>
#include <sys/stat.h>   //creating folders
#include <dirent.h> //counting files
#define MAX_FILES 12
bool EDITED = FALSE;
int fileCount;

GtkWidget *window;
GtkWidget *grid;

typedef struct{
    GFile *file;
    GtkWidget *scrolledWindow;
    GtkTextBuffer *textBuffer;
    char *filepath;
    GtkWidget *deleteButton;
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
    mkdir("assets"); // 0 if it makes a folder, -1 if already exist

        FILE *fptr = fopen("assets/note0.txt", "a");
        fclose(fptr);   // just to create the first file
        // counting files
    struct dirent *dir;
    DIR *d = opendir("assets");

    fileCount = -2; // . and .. also count as folders
    while((dir = readdir(d)) != NULL){
        if(fileCount >= 0){
            char newfilepath[20];
            sprintf(newfilepath, "assets/note%d.txt", fileCount);
            char oldfilepath[20];
            sprintf(oldfilepath, "assets/%s", dir->d_name);
            int value = rename(oldfilepath, newfilepath);
        }

        fileCount++;
    }

    closedir(d);

    for(int i = 0; i < fileCount; i++){
        char filepath[20];  // eg. assets/note0.txt
        sprintf(filepath, "assets/note%d.txt", i);

        Files[i].file = g_file_new_for_path(filepath);
        Files[i].textBuffer = gtk_text_buffer_new(NULL);
        Files[i].filepath = filepath;
            //this function reads the file and puts it in its own buffer
        fileToTextBuffer(Files[i].file, Files[i].textBuffer);
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
    //g_file_replace(file, text, len, fakjsdlkfjalskdjf)
}

void saveFiles(){
    for(int i = 0; i < fileCount; i++){
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


void deleteNote(GtkWidget *deleteButton, gpointer *data){
    int i = GPOINTER_TO_INT(data);

    printf("i is %d\n", i);
    gtk_grid_remove(GTK_GRID(grid), Files[i].scrolledWindow);
    gtk_grid_remove(GTK_GRID(grid), Files[i].deleteButton);
    // delete the file and rename all files
    g_file_delete(Files[i].file, NULL, NULL);
    loadFiles();    // load updated file list into Files
}

void createNewFile(GtkWidget *newFileButton, gpointer data){
    int i = GPOINTER_TO_INT(data);

    char newFilePath[20];
    sprintf(newFilePath, "assets/note%d.txt", i);
    FILE *fptr = fopen(newFilePath, "a");
    fclose(fptr);   // just to create the file

    loadFiles();    // load the new file into Files

    //first remote the last grid item then append textview and another plus button
    gtk_grid_remove(GTK_GRID(grid), newFileButton);

    GtkTextBuffer *currentTextBuffer = Files[i].textBuffer;
    GtkWidget *textView = gtk_text_view_new_with_buffer(currentTextBuffer);    // textView is a container that displays a buffer

    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textView), 1);    // 1- Character wise wrapping

    GtkWidget *scrolledWindow = gtk_scrolled_window_new();
    Files[i].scrolledWindow = scrolledWindow;
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);  // Set scroll policies
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolledWindow), textView);


    // TODO make the grid look better
        //tried adding some css to the scrolledWindows
    gtk_widget_add_css_class(scrolledWindow, "rounded");
    const gchar *css_data = ".rounded { border-radius: 12px; }";
    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_string(css_provider, css_data);


    gtk_grid_set_row_spacing(GTK_GRID(grid), 6);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 6);

    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(textView), 2);  // number of pixels to pad
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(textView), 2);
    gtk_text_view_set_top_margin(GTK_TEXT_VIEW(textView), 2);
    gtk_text_view_set_bottom_margin(GTK_TEXT_VIEW(textView), 2);

    g_signal_connect(currentTextBuffer, "changed", G_CALLBACK(onTextChange), NULL);

    gtk_widget_set_size_request(scrolledWindow, 400, 300);
    gtk_grid_attach(GTK_GRID(grid), scrolledWindow, i%4, i/4, 1, 1);    // keeps 4 notes in a row

        // delete button
    Files[i].deleteButton = gtk_button_new_with_label("Del");
        // Set the button's margin to push it to the top right corner
    gtk_widget_set_margin_start(GTK_WIDGET(Files[i].deleteButton), 380);
    gtk_widget_set_margin_top(GTK_WIDGET(Files[i].deleteButton), 280);

    g_signal_connect(Files[i].deleteButton, "clicked", G_CALLBACK(deleteNote), GINT_TO_POINTER(i));
    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(Files[i].deleteButton), i%4, i/4, 1, 1);


    i++;

        // make newfilebutton again
    GtkWidget *newNewFileButton = gtk_button_new_with_label("+");

    gpointer gi = GINT_TO_POINTER(i);
    g_signal_connect(newNewFileButton, "clicked", G_CALLBACK(createNewFile), gi);
    gtk_grid_attach(GTK_GRID(grid), newNewFileButton, i%4, i/4, 1, 1);
}

void buildTextView(GtkWidget *window, GtkWidget *grid, int i, int last) {
    GtkTextBuffer *currentTextBuffer = Files[i].textBuffer;
    gpointer gi = GINT_TO_POINTER(i);
    if(last){
        GtkWidget *newFileButton = gtk_button_new_with_label("+");

        g_signal_connect(newFileButton, "clicked", G_CALLBACK(createNewFile), gi);
        gtk_grid_attach(GTK_GRID(grid), newFileButton, i%4, i/4, 1, 1);
        return;
    }
    GtkWidget *textView = gtk_text_view_new_with_buffer(currentTextBuffer);    // textView is a container that displays a buffer

    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textView), 1);    // 1- Character wise wrapping

    GtkWidget *scrolledWindow = gtk_scrolled_window_new();
    Files[i].scrolledWindow = scrolledWindow;
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);  // Set scroll policies
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolledWindow), textView);

    // TODO make the grid look better
        //tried adding some css to the scrolledWindows
    gtk_widget_add_css_class(scrolledWindow, "rounded");
    const gchar *css_data = ".rounded { border-radius: 12px; }";
    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_string(css_provider, css_data);


    gtk_grid_set_row_spacing(GTK_GRID(grid), 6);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 6);


    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(textView), 2);  // number of pixels to pad
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(textView), 2);
    gtk_text_view_set_top_margin(GTK_TEXT_VIEW(textView), 2);
    gtk_text_view_set_bottom_margin(GTK_TEXT_VIEW(textView), 2);

    g_signal_connect(currentTextBuffer, "changed", G_CALLBACK(onTextChange), NULL);

    //gtk_widget_set_hexpand (scrolledWindow, TRUE);
    //gtk_widget_set_vexpand (scrolledWindow, TRUE);
    gtk_widget_set_size_request(scrolledWindow, 400, 300);
    gtk_grid_attach(GTK_GRID(grid), scrolledWindow, i%4, i/4, 1, 1);    // keeps 4 notes in a row
    //hierarchy- window > overlay > grid > scrolledWindow > textView

        // delete button
    Files[i].deleteButton = gtk_button_new_with_label("Del");
        // Set the button's margin to push it to the top right corner
    gtk_widget_set_margin_start(GTK_WIDGET(Files[i].deleteButton), 380);
    gtk_widget_set_margin_top(GTK_WIDGET(Files[i].deleteButton), 280);


    g_signal_connect(Files[i].deleteButton, "clicked", G_CALLBACK(deleteNote), GINT_TO_POINTER(i));
    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(Files[i].deleteButton), i%4, i/4, 1, 1);
}

static void activate(GtkApplication *app){
    window = gtk_application_window_new(app);

    gtk_window_set_title(GTK_WINDOW(window), "SerenityNotes");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    loadFiles();    //Files is a global array of FileData which contains GFile and its textBuffer

    grid = gtk_grid_new();
    GtkWidget *overlay = gtk_overlay_new();
    gtk_overlay_set_child(GTK_OVERLAY(overlay), grid);
    gtk_window_set_child(GTK_WINDOW(window), overlay);
        //hierarchy- window > overlay > grid > scrolledWindow > textView

    for(int i = 0; i < fileCount+1; i++){
        buildTextView(window, grid, i, i == fileCount);  // this will make the text appear and edit
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

    app = gtk_application_new("my.first.app", G_APPLICATION_HANDLES_OPEN);

        // once application starts, it makes a new window
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
        // this runs the application till we close it
    int status;
    status = g_application_run(G_APPLICATION(app), 0, NULL);
        // this releases the reference app
    g_object_unref(app);

    //g_signal_connect(window, "destroy", G_CALLBACK(g_main_quit()), NULL);
    //while(1){
    //    g_main_context_iteration (NULL, TRUE);  // this calls the main function again
    //}

    return status;
}

