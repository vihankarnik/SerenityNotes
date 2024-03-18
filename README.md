## SereneNotes

### How to execute
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
git clone git@github.com:vihankarnik/beam.git
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
