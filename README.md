# SereneNotes
A simple and lightweight note taking application written completely in C using the GTK Toolkit

### Project Overview
* This project is a simple note-taking application built using the GTK toolkit in C. It allows users to create, edit, and delete text-based notes, which are stored as individual files on the system.
* GTK serves as the primary graphical user interface toolkit and it completely handles the UI building of this application. It uses various GtkWidgets such as windows, grid layouts, text views and buttons and arranges them accordingly 
* Input-Output of files is handled by the GIO and GLib system library APIs that comes with GTK4.



### How to execute
To build on Windows, follow the steps:
* Recommended to use [MSYS2](https://www.msys2.org/) which provides a Unix-like environment and a command-line interface for compiling and building on Linux software that runs on Windows.
* Run the `MinGW64` environment that comes with `MSYS2`.
* Install and set up git:  
```
pacman -S git
```
* Install `gtk4` for the `MINGW64` environment:  
```
pacman -S mingw-w64-x86_64-gtk4
```
* Clone repository to your computer using SSH:  
```
git clone git@github.com:vihankarnik/SereneNotes.git
```
* Go to `/src` inside the new folder created and compile the C code:  
```
cd serenenotes/src
gcc app.c -o app.exe $(pkg-config --cflags --libs gtk4)
```
* Run the compiled file:
```
./app.exe
```
