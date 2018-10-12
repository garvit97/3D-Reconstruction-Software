#include "vertices.h"
#include "objects.h"

vertices::vertices(){
}

// *
// * \brief Transform all vertices according to new coordinate system
// * 
// * \param  a, b and c are the 3 projection planes that are to be treated as XY, YZ and ZX planes respectively
// * \return void

// void vertices::changeFrame(plane a,plane b,plane c){

// }

/**
* \brief Adds vertex to the list of vertices
* 
* \param  a is the vertex to be added
* \return boolean that returns true if face is valid and not already present
* 
*/
bool vertices::addVertex(vertex a){
	return v.insert(make_pair(coordinate(a),a)).second;
}

/**
* \brief Adds vertex to the list of vertices
* 
* \param  a is the coordinate to be added, numEdges is the number of edges that the vertex is part of
* \return boolean that returns true if face is valid and not already present
* 
*/
bool vertices::addVertex(coordinate a,int numEdges){
	vertex newvertex=vertex(a,numEdges);
	return v.insert(make_pair(a,newvertex)).second;
}

/**
* \brief Adds vertex to the list of vertices
* 
* \param  x, y, z are coordinates of the vertex, numEdges is the number of edges that the vertex is part of
* \return boolean that returns true if face is valid and not already present
* 
*/
bool vertices::addVertex(float x,float y,float z,int numEdges){
	vertex newvertex=vertex(x,y,z,numEdges);
	return addVertex(newvertex);
}


/**
* \brief Returns the list of vertices
* 
* \param  void
* \return Reference to the list of vertices
*
*/
unordered_map<coordinate,vertex,coordinateHasher> & vertices::getVertices(){
	return this->v;
}

void vertices::printVertices(){
	cout<<"Vertices:"<<endl;
	for(auto it=v.begin();it!=v.end();++it)
	{
		it->second.printVertex();
	}
	cout<<endl;
}
