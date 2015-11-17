/*
 * Sources:
 * http://www.cuboslocos.com/tutorials/OSG-BasicAnimation
 * http://trac.openscenegraph.org/projects/osg/wiki/Support/Tutorials/BasicGeometry
 * http://trac.openscenegraph.org/projects/osg/attachment/wiki/Support/Tutorials/Tuto2.zip
 * http://trac.openscenegraph.org/projects/osg/attachment/wiki/Support/Tutorials/Tuto9.zip
 */

#include <stdio.h>
#include <stdlib.h>
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
#define LENGTH 	4.0
#define WIDTH	4.0
#define HEIGHT	1.0

/*
 * Function Prototypes
 */
void updateScene(FILE* fp);
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
	transform = new osg::PositionAttitudeTransform;
	FILE* fp;
	
	//Light
	osg::ref_ptr<osg::Group> lightGroup = createLight();
	transform->addChild(lightGroup);
	
	//IMU
	osg::Geode* imuGeode = createIMU(LENGTH, WIDTH, HEIGHT);
	
	//Transform
	transform->setPosition(osg::Vec3(0.0f, 0.0f, 0.0f));
	transform->addChild(imuGeode);
	viewer.setSceneData(transform); 
	viewer.setCameraManipulator(new osgGA::TrackballManipulator());
	
	//Run
	while(!viewer.done()){
		viewer.frame();
		updateScene(fp);
	}
	
	//Return
	return 0;
}

/*
 * Update setScene
 */
void updateScene(FILE* fp){
	//Variables
	float w, x, y, z;
	int status;
	
	//Open UART Connection
	status = system("chmod +x /dev/ttyACM0");
	if (status != 0){
		printf("Could not set permissions.\r\n");
		return;
	}
	fp = fopen("/dev/ttyACM0", "r");
	
	//Get Quaternion from UART
	if (fp == NULL){
		printf("Could not open file\r\n");
		return;
	} else {
		status = fscanf(fp, "%f %f %f %f", &w, &x, &y, &z);
		if (status != 4){
			printf("Could not get Quaternions.\r\n");
			return;
		}
		
		//Close UART Connection
		status = fclose(fp);
		if (status != 0){
			printf("Could not close file.\r\n");
		}
		
		//Update Scene
		osg::Quat quat = osg::Quat(w, x, y, z);
		transform->setAttitude(osg::Quat(w, x, y, z));
	}
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
	osg::Vec3 A = osg::Vec3(-0.5f*length, -0.5f*width, 0.5f*height);
	osg::Vec3 B = osg::Vec3(0.5f*length, -0.5f*width, 0.5f*height);
	osg::Vec3 C = osg::Vec3(0.5f*length, 0.5f*width, 0.5f*height);
	osg::Vec3 D = osg::Vec3(-0.5f*length, 0.5f*width, 0.5f*height);
	osg::Vec3 E = osg::Vec3(-0.5f*length, -0.5f*width, -0.5f*height);
	osg::Vec3 F = osg::Vec3(0.5f*length, -0.5f*width, -0.5f*height);
	osg::Vec3 G = osg::Vec3(0.5f*length, 0.5f*width, -0.5f*height);
	osg::Vec3 H = osg::Vec3(-0.5f*length, 0.5f*width, -0.5f*height);
	
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