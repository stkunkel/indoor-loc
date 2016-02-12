#files
LOCAL_FOLDER = localProcessing/
LOCAL_SOURCES = $(LOCAL_FOLDER)math_utils.c $(LOCAL_FOLDER)mpu.c $(LOCAL_FOLDER)print_utils.c
LOCAL_DEPS = $(LOCAL_SOURCES:.c=.h)
LOCAL_OBJECTS = $(LOCAL_SOURCES:.c=.o)
SOURCE = imuviewer.cpp
EXECUTABLE = imuviewer
DEPENDENCIES = $(LOCAL_FOLDER)program_parameters.h

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
all: $(LOCAL_OBJECTS) $(EXECUTABLE)

$(EXECUTABLE): $(SOURCE) $(DEPENDENCIES)
	$(CC) $(FLAGS) -o $(EXECUTABLE) $(SOURCE) $(LOCAL_SOURCES) $(INCPATH) $(MINCPATH) $(LIBPATH) $(LIBS) $(MLIBPATH) $(MLIBS) 

$(LOCAL_OBJECTS): $(LOCAL_SOURCES) $(LOCAL_DEPS)
	$(CC) $(FLAGS_LOCAL) -o $@ $<

clean:
	rm $(LOCAL_OBJECTS) $(EXECUTABLE)