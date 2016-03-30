#files
LOCAL_FOLDER = localProcessing/
LOCAL_SOURCES = $(LOCAL_FOLDER)math_utils.c $(LOCAL_FOLDER)mpu.c $(LOCAL_FOLDER)print_utils.c
LOCAL_DEPS = $(LOCAL_SOURCES:.c=.h)
LOCAL_OBJECTS = $(LOCAL_SOURCES:.c=.o)
SOURCE = imuviewer.cpp
EXECUTABLE = imuviewer
DEPENDENCIES = $(LOCAL_FOLDER)program_parameters.h

# compiler setup
CC = g++
FLAGS = -O0 -g -Wall
FLAGS_LOCAL = -O0 -g -c -Wall

# libraries
INCPATH = -I .
LIBPATH = -L /usr/lib64
# /usr/local/lib64 for bullet and osgworks (if needed). /usr/local/lib for osgaudio and openal
# see below
# as well
#minimal libs, no shadow
MLIBS = -losg -losgViewer -lOpenThreads -losgUtil -losgGA 
#default libs


# need to add 
# export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib64:/usr/local/lib
# for executable as well

#g++ -o t *.cpp -losg -losgViewer -losgGA -losgDB -losgUtil -lOpenThreads -losgText -losgWidget -losgAnimation -losgParticle -losgFX -losgManipulator -losgSim -lGL -losgTerrain

#make
all: $(LOCAL_OBJECTS) $(EXECUTABLE)

$(EXECUTABLE): $(SOURCE) $(DEPENDENCIES)
	$(CC) $(FLAGS) -o $(EXECUTABLE) $(MLIBS) $(INCPATH) $(LIBPATH) $(SOURCE) $(LOCAL_SOURCES)

$(LOCAL_OBJECTS): $(LOCAL_SOURCES) $(LOCAL_DEPS)
	$(CC) $(FLAGS_LOCAL) -o $@ $<

clean:
	rm $(LOCAL_OBJECTS) $(EXECUTABLE)
