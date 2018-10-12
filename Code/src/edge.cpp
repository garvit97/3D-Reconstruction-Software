#include "objects.h"

/**
* \brief Constructor to initialize  object of coordinatePair
* \param  void
* 
*/
coordinatePair::coordinatePair(){

}

coordinatePair::coordinatePair(edge e){
	this->v1=e.v1;
	this->v2=e.v2;
	this->convertToStandardForm();
}

/**
* \brief Constructor to initialize object of coordinatePair
* \param  v1 and v2 are coordinates of end points
* 
*/
coordinatePair::coordinatePair(coordinate v1, coordinate v2){
	this->v1=v1;
	this->v2=v2;
	this->convertToStandardForm();
}

/**
* \brief Constructor to initialize object of coordinatePair
* \param  coordinates of the end points
* 
*/
coordinatePair::coordinatePair(float x1,float y1,float z1,float x2,float y2,float z2){
	this->v1=coordinate(x1,y1,z1);
	this->v2=coordinate(x2,y2,z2);
	this->convertToStandardForm();
}

/**
* \brief Converts pair of vertices in standard form
* 
* \param  a is the coordinate to be added, numEdges is the number of edges that the vertex is part of
* \return boolean that returns true if face is valid and not already present
* 
*1st vertex should have lower value of coordinates than the 2nd vertex (priority order -> x>y>z)
*/
void coordinatePair::convertToStandardForm(){
	if(v2.operator <(v1))
	{
		coordinate temp=v1;
		v1=v2;
		v2=temp;
	}
}

/**
* \brief Constructor to initialize object of edge
* \param void
* 
*/
edge::edge(){
	hiddenXY=false;
	hiddenYZ=false;
	hiddenZX=false;
}

/**
* \brief Constructor to initialize object of edge
* \param  coordinates of end points
* 
*/
edge::edge(coordinate v1,coordinate v2,bool hidXY,bool hidYZ,bool hidZX){
	this->v1=v1;
	this->v2=v2;
	this->numFaces=0;
	this->hiddenXY=hidXY;
	this->hiddenYZ=hidYZ;
	this->hiddenZX=hidZX;
	this->convertToStandardForm();
}

edge::edge(coordinatePair a,bool hidXY,bool hidYZ,bool hidZX){
	this->v1=a.v1;
	this->v2=a.v2;
	this->numFaces=0;
	this->hiddenXY=hidXY;
	this->hiddenYZ=hidYZ;
	this->hiddenZX=hidZX;
	this->convertToStandardForm();
}

/**
* \brief Constructor to initialize object of edge
* \param  coordinates of the end points
* 
*/
edge::edge(float x1, float y1, float z1, float x2, float y2, float z2,bool hidXY,bool hidYZ,bool hidZX){
	coordinate c1=coordinate(x1,y1,z1);
	coordinate c2=coordinate(x2,y2,z2);
	this->v1=c1;
	this->v2=c2;
	this->numFaces=0;
	this->hiddenXY=hidXY;
	this->hiddenYZ=hidYZ;
	this->hiddenZX=hidZX;
	this->convertToStandardForm();
}

/**
* \brief Converts edge in standard form
* 
* \param  a is the coordinate to be added, numEdges is the number of edges that the vertex is part of
* \return boolean that returns true if face is valid and not already present
* 
*1st vertex should have lower value of coordinates than the 2nd vertex (priority order -> x>y>z)
*/
void edge::convertToStandardForm(){
	if(v2.operator <(v1))
	{
		coordinate temp=v1;
		v1=v2;
		v2=temp;
	}
}

float edge::length2D(int view=1){
	if(view==1) return sqrt(pow(v1.x-v2.x,2)+pow(v1.y-v2.y,2));
	if(view==2) return sqrt(pow(v1.z-v2.z,2)+pow(v1.y-v2.y,2));
	return sqrt(pow(v1.x-v2.x,2)+pow(v1.z-v2.z,2));
}

float edge::length3D(){
	return sqrt(pow(v1.x-v2.x,2)+pow(v1.y-v2.y,2)+pow(v1.z-v2.z,2));
}

void edge::printEdge(){
	cout<<"("<<v1.x<<","<<v1.y<<","<<v1.z<<")"<<" ("<<v2.x<<","<<v2.y<<","<<v2.z<<")"<<hiddenXY<<" "<<hiddenYZ<<" "<<hiddenZX<<" "<<numFaces<<endl;
}

void coordinatePair::printCoordinatePair(){
	cout<<"("<<v1.x<<","<<v1.y<<","<<v1.z<<")"<<" ("<<v2.x<<","<<v2.y<<","<<v2.z<<")"<<endl;
}



