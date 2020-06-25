# To Run the Program

## Install 

### Boost (version 1.72.0)
1. Download and unzip boost library: https://sourceforge.net/projects/boost/files/boost-binaries/1.72.0/boost_1_72_0-bin-msvc-all-32-64.7z/download
2. Copy "boost_1_72_0\boost" folder to our project repo's "include" directory
3. Copy all .lib files in "boost_1_72_0\lib32-msvc-14.2" (don't copy cmake & .dlls) to folder to our project repo's "lib\Win32" directory
4. Copy all .lib files in "boost_1_72_0\lib64-msvc-14.2" (don't copy cmake & .dlls) to folder to our project repo's "lib\x64" directory

### Assimp (version 5.0.1)
1. Download the cmake-ed library files from: https://drive.google.com/file/d/1VCsYnV9MU1V1_fhXB-5pwY4ks9-lSN1i/view
2. Move all the files in "Win32\" to "lib\Win32\", and "x64\" to "lib\x64\"

## Run
- Start the client through VS solution
- Start the server by running "game_server.exe" by command-line under the "game_server" directory with the correct relative path to "game_server.exe"

# Development Warnings
- Graphics: All Animation should have its first and last keyframe at the edge of the timerange to allow Assimp to load the animation without errors.
