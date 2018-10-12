#include "objects.h"

/**
* \brief Constructor to initialize object of coordinate
* \param  void
* 
*/
coordinate::coordinate(){
	x=0.0;
	y=0.0;
	z=0.0;
}

/**
* \brief Constructor to initialize object of coordinate
* \param  coordinates x, y, z
* 
*/
coordinate::coordinate(float x,float y, float z){
	this->x=x;
	this->y=y;
	this->z=z;
}

coordinate::coordinate(vertex a){
	this->x=a.v.x;
	this->y=a.v.y;
	this->z=a.v.z;
}

coordinate coordinate::add(coordinate a){
	return coordinate(this->x+a.x,this->y+a.y,this->z+a.z);
}

coordinate coordinate::subtract(coordinate a){
	return coordinate(this->x-a.x,this->y-a.y,this->z-a.z);
}

// *
// * \brief Function to check equality of 2 objects of coordinate type
// * \param  coordinates of the end points
// * 

// bool coordinate::equals(int x, int y, int z){
// 	return (this->x==x&&this->y==y&&this->z==z)
// }

/**
* \brief Constructor to initialize object of vertex
* \param void
* 
*/
vertex::vertex(){
	numEdges=0;
}

/**
* \brief Constructor to initialize object of vertex
* \param coordinate and number of edges
* 
*/

vertex::vertex(coordinate a,int numEdges){
	this->v=a;
	this->numEdges=numEdges;
}

/**
* \brief Constructor to initialize object of vertex
* \param coordinates x, y, z and number of neighbours of vertex
* 
*/

vertex::vertex(float x,float y, float z, int numEdges){
	this->v=coordinate(x,y,z);
	this->numEdges=numEdges;
}

coordinate vertexMapping::getCoordinate(int index){
	return vertexCoordinate[index-1];
}

void vertexMapping::addCoordinate(coordinate v){
	vertexCoordinate.push_back(v);
}

void vertex::printVertex(){
	cout<<v.x<<" "<<v.y<<" "<<v.z<<" "<<numEdges<<endl;
}


