# Andor sCMOS examples
Andor sCMOS (Neo and Zyla) SDK3 examples focused on simplicity and speed.
Uses C++14 features, which any modern compiler can handle.

A bonus Python script is included that polls the C++ image.exe for an image, an inefficient yet simple method of checking out what the skys are doing with my auroral-aimed Neo.
Yes that could be done with ctypes, pyAndorNeo, etc. but my goal is always simplicity and expediency.

## Prereqs
You don't have to install Cmake and Make, but they do make compiling easier.

### Windows
step 4 is necessary because not all compilers understand how to link the `.dll` files directly; the `.lib` act as a shim to the `.dll`.

1. clone this repository
2. [install Andor SDK3 for Windows](https://www.scivision.co/andor-neo-windows-sdk3-install/)
3. copy from `c:/Program Files/Andor SDK3/` directory into `andor-scmos-examples/lib/` the files

        atcorem.lib
        atcore.dll
        atutilitym.lib
        atutility.dll
4. in `andor-scmos-examples/lib`
   ```posh
   rename atcorem.lib atcore.lib
   rename atutilitym.lib atutility.lib
   ```
5. (optional) install [Cmake](https://cmake.org/download/) (the `.msi` file)

### Linux
I use Andor cameras with Ubuntu 16.04. 
Other Linux versions would likely work as well.
I do NOT expect that Windows Subsystem for Linux would work, but have not tried it.

1. clone this repository
2. install [Andor SDK3 for Linux](https://www.scivision.co/andor-neo-linux-sdk3-install/)
3. install Cmake & Make
   ```sh
   apt install cmake make g++
   ```
   
## Compile Andor sCMOS example programs

from the `andor-cmos-examples` directory

```sh
cd bin

cmake ..   # mac, linux
cmake -G "MinGW Makefiles" ..   # windows

cmake --build .
```

If you don't have a camera, the SimCam will be used.

## Example programs

### List all connected Andor Neo

    ./listdevices

It will always show two Simcams in addition to any physical cameras you have.
Keep this number to use in the "image' program or else you could default to the wrong device or sim-device

### Acquire a single image

    ./image -?

with Simcam it saves `image.bmp` with a test pattern of bar gradients.
With the real camera it stores a dynamic-range compression 8-bit bitmap `.bmp` image.

I have hard-coded 4x4 binning for my work--feel free to make it a command-line option. 
I have also hard-coded on the Spurious Noise Filter and Static Blemish Correction.

More useful examples are in the Andor sCMOS SDK 3 user manual.
