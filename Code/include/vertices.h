#ifndef VERTICES_H
#define VERTICES_H
#include "objects.h"

/**
* \brief Class representing list of vertices
* 
*/
class vertices{
	/**
	* \brief list of vertices
	*/	
	unordered_map<coordinate,vertex,coordinateHasher> v;
 
public:
	//void changeFrame(plane,plane,plane);
	vertices();
	bool addVertex(vertex);
	bool addVertex(coordinate,int numEdges=0);
	bool addVertex(float,float,float,int numEdges=0);
	unordered_map<coordinate,vertex,coordinateHasher> & getVertices();
	void printVertices();
};

#endif