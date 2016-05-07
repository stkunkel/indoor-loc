/*
 * Sources:
 * http://www.cuboslocos.com/tutorials/OSG-BasicAnimation
 * http://trac.openscenegraph.org/projects/osg/wiki/Support/Tutorials/BasicGeometry
 * http://trac.openscenegraph.org/projects/osg/attachment/wiki/Support/Tutorials/Tuto2.zip
 * http://trac.openscenegraph.org/projects/osg/attachment/wiki/Support/Tutorials/Tuto9.zip
 * https://en.wikibooks.org/wiki/Serial_Programming/termios
 * Openscenegraph 3.0: Beginner's Guide: Right-hand coordinate system
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <osgViewer/Viewer>
#include <osg/Node>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osgViewer/Viewer>
#include <osg/PositionAttitudeTransform>
#include <osgGA/TrackballManipulator>

//#include <osgViewer/ViewerEventHandlers>
//#include <osgGA/StateSetManipulator>

/*
 * Defines
 */
#define LENGTH 		1.0//2.0
#define WIDTH		0.5//1.0
#define HEIGHT		0.05//0.1
#define DEVICE0		"/dev/ttyACM0"
#define DEVICE1		"/dev/ttyACM1"
#define FILENAME	"quatPos.mat"
#define BAUDRATE 	B115200
#define DELTA		0.01
#define FIFO_RATE						200//(Hz) May be adjusted to increase or decrease speed of viewing local data
#define IMUVIEWER_FREQ			10 			//Hz

/*
 * Function Prototypes
 */
int updateScene(int tty_fd);
int configureUart(int tty_fd);
int readUartLine(int tty_fd, char* line);
osg::ref_ptr<osg::Group> createLight();
osg::Geode* createIMU(float length, float width, float height);

/*
 * Global Variables
 */
osg::PositionAttitudeTransform* transform;
FILE* fp;

/*
 * Main
 */
int main(){
	//Variables
	osgViewer::Viewer viewer;
	osg::PositionAttitudeTransform* rootnode = new osg::PositionAttitudeTransform;
	transform = new osg::PositionAttitudeTransform;
	int tty_fd = 0;
	int status;
	int waitTime;
	
	//Light
	osg::ref_ptr<osg::Group> lightGroup = createLight();
	transform->addChild(lightGroup);
	
	//IMU
	osg::Geode* imuGeode = createIMU(LENGTH, WIDTH, HEIGHT);
	
	//Transform
	transform->setPosition(osg::Vec3(0.0f, 0.0f, 0.0f));
	transform->setAttitude(osg::Vec4(1.0, 0.0, 0.0, 0.0));
	transform->addChild(imuGeode);
	
	//Root
	//rootnode->setPosition(osg::Vec3(0.0f, 5.0f, 1.0f));
	//rootnode->setAttitude(osg::Quat(0.5, 0.5, 0.5, 0.5)); //view IMU from above
	rootnode->setAttitude(osg::Quat(0.35355339059327384, 0.6123724356957946, 0.35355339059327384, 0.6123724356957945));
	//rootnode->setAttitude(osg::Quat(0.9659258262890683, 0.0, 0.0, -0.25881904510252074)); //view IMU from above, rotate view 30° around x axis
	rootnode->addChild(transform);
	
	//Viewer
	viewer.setSceneData(rootnode); 
	viewer.setCameraManipulator(new osgGA::TrackballManipulator());
	
	//Open UART Connection
	tty_fd = open(DEVICE0,  O_RDWR | O_NOCTTY | O_NDELAY);
	if (tty_fd == -1){
		printf("Could not open port %s, trying %s.\r\n", DEVICE0, DEVICE1);
		  //DEVICE1
		  tty_fd = open(DEVICE1,  O_RDWR | O_NOCTTY | O_NDELAY);
		  if (tty_fd == -1){
		    printf("Could not open port %s, trying %s.\r\n", DEVICE1, FILENAME);
		    //FILE
		    fp = fopen(FILENAME, "r");
		    if (fp == NULL){
		      printf("Could not open file %s.\r\n", FILENAME);
		      return 0;
		    } else {
		      printf("Using Data From File.\r\n");
		      waitTime = 1000000/FIFO_RATE;
		    }
		  } else {
		    waitTime = 1000/IMUVIEWER_FREQ; //1000000
		  }
	} else {
	  waitTime = 1000/IMUVIEWER_FREQ;
	}
	
	//Configure UART
	status = configureUart(tty_fd);
	
	//Run
	while(!viewer.done()){
		viewer.frame();
		status = updateScene(tty_fd);
		if (status != 0){
		  break;
		}

		usleep(waitTime); //100000 --> 100ms --> works well with Zynq's print rate of 10Hz TODO: Test

	}
	
	//Close UART Connection
	if (tty_fd != -1){
	  close(tty_fd);
	}
	
	// Close File
	if (fp != NULL){
	  fclose(fp);
	}
	
	//Return
	return 0;
}

/*
 * Update setScene
 * Returns 0 if successful
 */
int updateScene(int tty_fd){
	//Variables
	osg::Quat quat;
	osg::Vec3 pos;
	float quaternion[4];
	float position[3];
	int status;
	char* line = (char*) malloc(500);
	size_t len = 0;
	
	//Initialize Quaternion
	quat = transform->getAttitude();
	quaternion[0] = quat.w();
	quaternion[1] = quat.x();
	quaternion[2] = quat.y();
	quaternion[3] = quat.z();
	
	//Initialize Position
	pos = transform->getPosition();
	position[0] = pos.x();
	position[1] = pos.y();
	position[2] = pos.z();
	
	// UART
	if (tty_fd != -1){
	
	  //Get Line from UART
	  status = read(tty_fd, line, 255);
	  if (status == 0){
	    //printf("Could not read from UART (%d).\r\n", status);
	    return 1;
	  }
	} else if (fp != NULL){ //File
	  // Read line
	  status = getline(&line, &len, fp);
	  if (status == -1){
	    return 1;
	  } else {
	    if (line[0] == '#'){
	      return 0;
	    }
	  }
	} else { //Neither UART nor File
	  return 1;
	}
	  
	//Get Quaternion and Position
	status = sscanf(line, "%f %f %f %f %f %f %f", &quaternion[0], &quaternion[1], &quaternion[2], &quaternion[3], &position[0], &position[1], &position[2]);
	if (status != 7 || abs(quaternion[0]) > 1.0 || abs(quaternion[1]) > 1.0 || abs(quaternion[2]) > 1.0 || abs(quaternion[3]) > 1.0){
	  //printf("Could not get data (%d).\r\n", status);
	  return 0;
	}
	  
	//Invert y to fit IMUs coordinate system to OSG
	quaternion[2] = -quaternion[2];
	position[2] = -position[2];
	
	//Scale Movement
	position[0] = position[0] / 10.0;
	position[1] = position[1] / 10.0;
	position[2] = position[2] / 10.0;
	
	//Print Quaternion and Position
	//printf("%f %f %f %f %f %f %f\n\r", quaternion[0], quaternion[1], quaternion[2], quaternion[3], position[0], position[1], position[2]); 
	
	//Update Scene
	transform->setAttitude(osg::Quat(quaternion[0], quaternion[1], quaternion[2], quaternion[3]));
	//transform->setPosition(osg::Vec3(position[0], position[1], position[2]));

	//Free memory
	free(line);
	
	//Return
	return 0;
}


/*
 * Configure UART Connection
 */
int configureUart(int tty_fd){
	//Variables
	struct termios tio;
	
	//Configuration
	memset(&tio,0,sizeof(tio));
	tio.c_iflag=0;
	tio.c_oflag=0;
	tio.c_cflag=CS8|CREAD|CLOCAL;           // 8-bit, reader, no modem printouts
	tio.c_lflag=ICANON;			//Canonical Mode

	//Open device file
	cfsetospeed(&tio,BAUDRATE);            // 115200 baud
	cfsetispeed(&tio,BAUDRATE);            // 115200 baud
	
	//Configure
	tcsetattr(tty_fd,TCSANOW,&tio);
	
	//if (read(tty_fd,&c,1)>0)        write(STDOUT_FILENO,&c,1);              // if new data is available on the serial port, print it out
	return 0;
}

/*
 * Read Line from UART
 */
int readUartLine(int tty_fd, char* line){
	char c;
	strcpy(line, "");
	int i = 0;

	while(read(tty_fd,&c,1)>0){
		if (c != '\n'){
			//strcat(line, &c);
			line[i] = c;
			i++;
			//printf("%s\r\n", line);
		} else {
			return 0;
		}
	}
	
	return -1;

}

/*
 * Create two light sources
 */
osg::ref_ptr<osg::Group> createLight(){
	//Variables
	osg::ref_ptr<osg::Group> lightGroup (new osg::Group);
	osg::ref_ptr<osg::LightSource> lightSource1 = new osg::LightSource;
	osg::ref_ptr<osg::LightSource> lightSource2 = new osg::LightSource;
	
	// create a local light.
	//osg::Vec4f lightPosition (osg::Vec4f(-5.0,-5.0,5.0,5.0f));
	osg::Vec4f lightPosition (osg::Vec4f(0.0,0.0,1.0,1.0f));
  	osg::ref_ptr<osg::Light> myLight = new osg::Light;
	myLight->setLightNum(1);
	myLight->setPosition(lightPosition);
        //myLight->setAmbient(osg::Vec4(0.2f,0.2f,0.2f,1.0f));
        //myLight->setDiffuse(osg::Vec4(0.1f,0.4f,0.1f,1.0f));
	myLight->setAmbient(osg::Vec4(1.0,1.0,1.0,1.0));
	myLight->setDiffuse(osg::Vec4(1.0,1.0,1.0,1.0));
	myLight->setSpecular(osg::Vec4(1,1,1,1));
        myLight->setConstantAttenuation(1.0f);
	lightSource1->setLight(myLight.get());

	lightSource1->setLocalStateSetModes(osg::StateAttribute::ON); 
       
	// create a local light
	//osg::Vec4f lightPosition2 (osg::Vec4f(5.0,5.0,-5.0,-5.0f));
	osg::Vec4f lightPosition2 (osg::Vec4f(0.0,0.0,-1.0,1.0f));
  	osg::ref_ptr<osg::Light> myLight2 = new osg::Light;
	myLight2->setLightNum(0);
	myLight2->setPosition(lightPosition2);
        //myLight->setAmbient(osg::Vec4(0.2f,0.2f,0.2f,1.0f));
        //myLight->setDiffuse(osg::Vec4(0.1f,0.4f,0.1f,1.0f));
	myLight2->setAmbient(osg::Vec4(1.0,1.0,1.0,1.0));
	myLight2->setDiffuse(osg::Vec4(1.0,1.0,1.0,1.0));
	myLight2->setSpecular(osg::Vec4(1,1,1,1));
        myLight2->setConstantAttenuation(1.0f);
	            
        lightSource2->setLight(myLight2.get());
	lightSource2->setLocalStateSetModes(osg::StateAttribute::ON); 
        
	
	//Add light sources to group
	lightGroup->addChild(lightSource1.get());
	lightGroup->addChild(lightSource2.get());
	
	return lightGroup;
}

/*
 * Create IMU Shape
 */
osg::Geode* createIMU(float length, float width, float height) {
	//Coordinates
	osg::Vec3 A = osg::Vec3(-0.5f*height, -0.5f*length, 0.5f*width);
	osg::Vec3 B = osg::Vec3(0.5f*height, -0.5f*length, 0.5f*width);
	osg::Vec3 C = osg::Vec3(0.5f*height, 0.5f*length, 0.5f*width);
	osg::Vec3 D = osg::Vec3(-0.5f*height, 0.5f*length, 0.5f*width);
	osg::Vec3 E = osg::Vec3(-0.5f*height, -0.5f*length, -0.5f*width);
	osg::Vec3 F = osg::Vec3(0.5f*height, -0.5f*length, -0.5f*width);
	osg::Vec3 G = osg::Vec3(0.5f*height, 0.5f*length, -0.5f*width);
	osg::Vec3 H = osg::Vec3(-0.5f*height, 0.5f*length, -0.5f*width);
	
	//Shape 
	osg::Vec3Array* coords = new osg::Vec3Array;
	coords->push_back(A); 	//A --> 0
	coords->push_back(B); 	//B --> 1
	coords->push_back(C); 	//C --> 2
	coords->push_back(D); 	//D --> 3
	coords->push_back(E); 	//E --> 4
	coords->push_back(F); 	//F --> 5
	coords->push_back(G); 	//G --> 6
	coords->push_back(H); 	//H --> 7
	
	//Colors
    osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); 	//red
	colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f) ); 	//green
	colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f) ); 	//blue
	colors->push_back(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f) ); 	//black
	
	colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); 	//red
	colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f) ); 	//green
	colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f) ); 	//blue
	colors->push_back(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f) ); 	//black

	
	//colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f) ); 	//index 1 green
	//colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f) ); 	//index 2 blue
	//colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); 	//index 3 red
	//colors->push_back(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f) ); 	//index 4 black
	//colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f) ); 	//index 5 white
	
	//Primitives
	osg::DrawElementsUInt* top = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
	top->push_back(0);
	top->push_back(1);
	top->push_back(2);
	top->push_back(3);
	
	osg::DrawElementsUInt* bottom = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
	bottom->push_back(4);
	bottom->push_back(7);
	bottom->push_back(6);
	bottom->push_back(5);
	
	osg::DrawElementsUInt* front = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
	front->push_back(1);
	front->push_back(5);
	front->push_back(4);
	front->push_back(0);
	
	osg::DrawElementsUInt* back = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
	back->push_back(3);
	back->push_back(2);
	back->push_back(6);
	back->push_back(7);
		
	osg::DrawElementsUInt* left = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
	left->push_back(0);
	left->push_back(3);
	left->push_back(7);
	left->push_back(4);
	
	osg::DrawElementsUInt* right = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
	right->push_back(2);
	right->push_back(1);
	right->push_back(5);
	right->push_back(6);
	
	//Geometry
	osg::Geometry* geom = new osg::Geometry;
	
	//Set Vertex Array
	geom->setVertexArray(coords);
	
	//Set Primitives
    geom->addPrimitiveSet(top);		// --> 0
	geom->addPrimitiveSet(bottom);	// --> 1
	geom->addPrimitiveSet(front);	// --> 2
	geom->addPrimitiveSet(back);	// --> 3
	geom->addPrimitiveSet(left);	// --> 4
	geom->addPrimitiveSet(right);	// --> 5
	
	//Set Colors
    geom->setColorArray(colors);
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	
	//Geode
    osg::Geode* geode = new osg::Geode;
    geode->addDrawable(geom);

	//Return
    return geode;
	
}
