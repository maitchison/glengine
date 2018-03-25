echo "---------------------------------------------------------------------------------------------------"
g++ -std=c++11 -w -o glengine.exe main.cpp utils.cpp material.cpp scenegraph.cpp customobjects.cpp -lGL -lGLU -lglut && ./glengine.exe
