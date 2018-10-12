#ifndef FACES_H
#define FACES_H
#include "objects.h"
#include "edges.h"

/**
* \brief Class representing list of faces of 3D object
* 
*/
class faces{
	/**
	* \brief list of faces 
	*/
	vector<face> f;
	edges e;

public:
	faces();
	faces(edges);
	void addFace(face); 
	void checkHidden(edge&);
	void findNatureOfEdges(); 
	void makeFaces();
	bool removePseudoFaces(edges,edges,edges); 
	void correctFaceOrientation(); 
	void findContainment();
	edges & getEdges();
	vector<face> & getFaces();
	void printFaces();
	void printEverything();
	bool removingEdgeIsOK(edge);
	bool rule2();
	bool func(edges,edges,edges);
	bool checkContradiction(edge,edges,edges,edges);
	bool rule3(edge a);
	bool rule7();
	void reverseVertices(int);
	void reverseVertices1(int);
	bool positionLess(int,int,int);
	bool positionLess1(int,int,int);
	int positionOfVertex(int,coordinate);
	void iterateFace(vector<coordinate>,glm::vec3);
	int positionOfDummy(int);
};

#endif