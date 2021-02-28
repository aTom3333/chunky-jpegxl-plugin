# Chunky plugin for JPEG-XL export

This is a plugin for [chunky][1] that add an option to export the render
as a JPEG-XL image.

## Usage

This plugin is not really user-friendly and requires the user to build it themself.

[JPEG-XL][2] being a brand-new image format (as of February 2021), the only library able to encode (and decode) them is 
the [reference implementation][3] so you need to follow their instructions to build it and install it.

Next step is to build the native dynamic library of this plugin, the code is in the `src/native` folder.
Build the library with
```
cd src/native
mkdir build
cd build
cmake ..
cmake --build .
```
This will build the dynamic library (`jpegxl_writer.dll` or `libjepgxl_writer.so`) in the `src/native/build` directory.
Copy this file in the `src/main/resources` directory

You can now build the java part of the plugin by running
```
./gradlew jar
```
and retrieve the .jar file in the `build/libs` directory (and add it as plugin to chunky).

Last thing te be careful of is the libjxl and libjxl_threads need to be found by the OS when the 
native library of the plugin is loaded. This means adding the directory where those library reside to the
PATH environment variable used to locate dynamic library (that is $PATH on Windows and $LD_LIBRARY_PATH on linux).
For example on my system, the libjxl is installed in `/usr/local/lib` so I run
```
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH;/usr/local/lib"
java -jar ChunkyLauncher.jar
``` 
To run chunky while making sure the library will be located.

[1]: http://chunky.llbit.se
[2]: https://jpeg.org/jpegxl/
[3]: https://gitlab.com/wg1/jpeg-xl