/*
 * Sources:
 * http://www.cuboslocos.com/tutorials/OSG-BasicAnimation
 * http://trac.openscenegraph.org/projects/osg/wiki/Support/Tutorials/BasicGeometry
 * http://trac.openscenegraph.org/projects/osg/attachment/wiki/Support/Tutorials/Tuto2.zip
 * http://trac.openscenegraph.org/projects/osg/attachment/wiki/Support/Tutorials/Tuto9.zip
 * https://en.wikibooks.org/wiki/Serial_Programming/termios
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
#define LENGTH 		2.0
#define WIDTH		1.0
#define HEIGHT		0.1
//#define DEVICE		"quat.txt"
#define DEVICE		"/dev/ttyACM0"
#define BAUDRATE 	B115200

/*
 * Function Prototypes
 */
void updateScene(int tty_fd);
int configureUart(int tty_fd);
int readUartLine(int tty_fd, char* line);
int configureStdout();
int printToStdout(char* line);
osg::ref_ptr<osg::Group> createLight();
osg::Geode* createIMU(float length, float width, float height);

/*
 * Global Variables
 */
osg::PositionAttitudeTransform* transform;

/*
 * Main
 */
int main(){
	//Variables
	osgViewer::Viewer viewer;
	osg::PositionAttitudeTransform* rootnode = new osg::PositionAttitudeTransform;
	transform = new osg::PositionAttitudeTransform;
	int tty_fd;
	int status;
	char* line = (char* ) malloc(500);
	char c;
	
	//Light
	osg::ref_ptr<osg::Group> lightGroup = createLight();
	transform->addChild(lightGroup);
	
	//IMU
	osg::Geode* imuGeode = createIMU(LENGTH, WIDTH, HEIGHT);
	
	//Transform
	transform->setPosition(osg::Vec3(0.0f, 0.0f, 0.0f));
	transform->addChild(imuGeode);
	
	//Root
	rootnode->setPosition(osg::Vec3(0.0f, -5.0f, 1.0f));
	//rootnode->setAttitude(osg::Quat(0.5, 0.5, 0.5, 0.5)); view IMU from above
	rootnode->setAttitude(osg::Quat(0.35355339059327384, 0.6123724356957946, 0.35355339059327384, 0.6123724356957945));
    rootnode->addChild(transform);
	
	//Viewer
	viewer.setSceneData(rootnode); 
	viewer.setCameraManipulator(new osgGA::TrackballManipulator());
	
	//Configure STDOUT
	/*status = configureStdout();
	if (status != 0){
		strcpy(line, "Could not configure STDOUT.\n\r");
		printToStdout(line);
		return 0;
	}
	strcpy(line, "STDOUT Configuration done.\n\r");
	printToStdout(line);
	*/
	
	
	//Open UART Connection
	strcpy(line, "chmod +r ");
	strcat(line, DEVICE);
	status = system(line);
	tty_fd = open(DEVICE,  O_RDWR | O_NOCTTY | O_NDELAY);
	if (tty_fd == -1){
		printf("Could not open port\r\n");
		return 0;
	}
	
	//Configure UART
	status = configureUart(tty_fd);
	
	
	
	//Run
	while(!viewer.done()){
		viewer.frame();
		updateScene(tty_fd);
		sleep(1);//usleep(16666);
	}
	
	//Close UART Connection
	close(tty_fd);
	
	//Return
	return 0;
}

/*
 * Update setScene
 */
void updateScene(int tty_fd){
	//Variables
	osg::Quat quat;
	float w, x, y, z;
	int status;
	char* line = (char*) malloc(500);
	
	//Initialize Quaternion
	quat = transform->getAttitude();
	w = quat.w();
	x = quat.x();
	y = quat.y();
	z = quat.z();
	
	//Get Line from UART
	status = readUartLine(tty_fd, line);
	if (status != 0){
		//printf("Could not read from UART (%d).\r\n", status);
		return;
	}
	
	//printf("%s\n\r", line);
	
	//Get Quaternions
	status = sscanf(line, "%f %f %f %f", &w, &x, &y, &z);
	if (status != 4 || w > 1.0 || x > 1.0 || y > 1.0 || z > 1.0){
		printf("Could not get Quaternion (%d).\r\n", status);
		return;
	}
	
	printf("%f %f %f %f\n\r", w, x, y, z);
	
	//Update Scene
	quat = osg::Quat(w, x, y, z);
	transform->setAttitude(osg::Quat(w, x, y, z));
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
	tio.c_cflag=CS8|CREAD|CLOCAL;           // 8n1, see termios.h for more information
	tio.c_lflag=0;
	tio.c_cc[VMIN]=1;
	tio.c_cc[VTIME]=5;

	//Open device file
	tty_fd=open(DEVICE, O_RDWR | O_NONBLOCK);      
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

	while(read(tty_fd,&c,1)>0){
		if (c != '\n'){
			strcat(line, &c);
		} else {
			return 0;
		}
	}
	
	return -1;

}

/*
 * Open STDOUT
 */
int configureStdout(){
	//Variables
	struct termios stdio;
	
	//Configuration
	memset(&stdio,0,sizeof(stdio));
	stdio.c_iflag=0;
	stdio.c_oflag=0;
	stdio.c_cflag=0;
	stdio.c_lflag=0;
	stdio.c_cc[VMIN]=1;
	stdio.c_cc[VTIME]=0;
	
	//Configure
	tcsetattr(STDOUT_FILENO,TCSANOW,&stdio);
	tcsetattr(STDOUT_FILENO,TCSAFLUSH,&stdio);
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);       // make the reads non-blocking
	
	return 0;
}

/*
 * Print line to STDOUT
 */
int printToStdout(char* line){
	//Variables
	int length, status;
	
	//Get length
	length = strlen(line);
	
	//Write
	status = write(STDOUT_FILENO, &line, length);
	
	return status;
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
	osg::Vec4f lightPosition (osg::Vec4f(-5.0,-5.0,5.0,5.0f));
  	osg::ref_ptr<osg::Light> myLight = new osg::Light;
	myLight->setLightNum(1);
	myLight->setPosition(lightPosition);
        myLight->setAmbient(osg::Vec4(0.2f,0.2f,0.2f,1.0f));
        myLight->setDiffuse(osg::Vec4(0.1f,0.4f,0.1f,1.0f));
        myLight->setConstantAttenuation(1.0f);
	lightSource1->setLight(myLight.get());

	lightSource1->setLocalStateSetModes(osg::StateAttribute::ON); 
       
	// create a local light
	osg::Vec4f lightPosition2 (osg::Vec4f(5.0,5.0,-5.0,-5.0f));
  	osg::ref_ptr<osg::Light> myLight2 = new osg::Light;
	myLight2->setLightNum(0);
	myLight2->setPosition(lightPosition2);
        myLight2->setAmbient(osg::Vec4(0.2f,0.2f,0.2f,1.0f));
        myLight2->setDiffuse(osg::Vec4(0.4f,0.1f,0.1f,1.0f));
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