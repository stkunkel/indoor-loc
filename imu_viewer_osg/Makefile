#files
SOURCE = imuviewer.cpp
EXECUTABLE = imuviewer

#compiler setup
CC = g++
FLAGS =  -O0 -g -Wall
FLAGS_LOCAL = -O0 -g -c -Wall

#includes and libraries
INCPATH = -I. -I/usr/include/ -I/usr/include/X11/ -I/usr/local/include/GL
LIBPATH =  -L. -L/usr/lib -L/usr/X11R6/lib -L/usr/local/lib
LIBS = -lm -ldl -lGL -lGLU -lpthread -lXext -lX11
MINCPATH = -I/home/ruppi/OpenSceneGraph/openscenegraph-3.2.1/OpenSceneGraph/include
MLIBPATH = -L/home/ruppi/OpenSceneGraph/openscenegraph-3.2.1/OpenSceneGraph/lib
MLIBS = -losg -losgDB -losgFX -losgGA -losgParticle -losgSim -losgText -losgUtil -losgTerrain -losgManipulator -losgViewer -losgWidget -losgShadow -losgAnimation -losgVolume -lOpenThreads

#make
all:  $(EXECUTABLE)

$(EXECUTABLE): $(SOURCE) $(DEPENDENCIES)
	$(CC) $(FLAGS) -o $(EXECUTABLE) $(SOURCE) $(INCPATH) $(MINCPATH) $(LIBPATH) $(LIBS) $(MLIBPATH) $(MLIBS) 

clean:
	rm $(EXECUTABLE)