#ifndef EDGES_H
#define EDGES_H
#include "objects.h"
#include "vertices.h"

/**
* \brief Class representing list of edges 
* 
*/
class edges{ 
private:
	/**
	* \brief Hashmap of list of edges 
	*/
	unordered_map<coordinatePair,edge,coordinatePairHasher> e;
	/**
	* \brief list of vertices 
	*/
	vertices v; 
public:
	edges();
	bool addEdge(edge); 
	bool addEdge(coordinatePair);
	bool addEdge(coordinate,coordinate);
	bool addEdge(float,float,float,float,float,float);
	void removeEdge(coordinatePair);
	void removeEdge(edge);
	void breakEdges(bool,int view=1); 
	void removeRedundantEdges();
	void getProbableEdges(edges&,edges&,edges&);
	void getProbableVertices(vertices&,vertices&,vertices&);
	void correctMesh(); 
	vertices & getVertices();
	unordered_map<coordinatePair,edge,coordinatePairHasher> & getEdges();
	unordered_map<coordinatePair,edge,coordinatePairHasher> getEdgesCopy();
	void printEdges();
};

#endif