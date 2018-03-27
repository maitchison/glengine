# glengine
COSC367 Computer Graphics Assignment - by Matthew Aitchison

A Minecraft style terrain enviroment with a lighthouse and 5 animated objects.

The engine is built using OpenGL 2.0 and uses a simple per vertex specular + diffuse lighting model.

## Compile

Compile and run
./go.sh 

The linking is set up for a linux enviroment, and will not build on OSX or Windows.

## Usage

Run the program with 
./go.sh

Use arrow keys to navigate, space to jump.

F1 change camera view
F2 switch day / night lighting
F3 change render mode
F4 enable flying (press space repeatedly to fly)
F5 toggle HQ / LQ mode

## Known bugs

Compiling on MAC requires changing the linking options.  This also seems to change the random number generator causing a different height field from what is intended.

