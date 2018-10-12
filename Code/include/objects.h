#ifndef	OBJECTS_H
#define OBJECTS_H
#include <bits/stdc++.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <boost/functional/hash.hpp>
using namespace std;


struct vertex;
struct coordinate;
struct edge;
struct coordinatePair;
struct face;
struct plane;
struct coordinateHasher;
struct coordinatePairHasher;
bool compareFloats(float,float);
bool inverse(float A[3][3], float inverse[3][3]);
void getCofactor(float A[3][3], float temp[3][3], int p, int q, int n);
float determinant(float A[3][3], int n);
void adjoint(float A[3][3], float adj[3][3]);
void transpose(float A[3][3], float transpose[3][3]);
void findRotMatrixAndOrigin(plane xy, plane yz, plane zx, float rotMatrix[3][3], coordinate &v0);
void changeFrame(coordinate &v, float rotMatrix[3][3], coordinate v0);

/**
* \brief Struct representing plane of form ax+by+cz=d
* 
*/
struct plane{
	/**
	* \brief a in ax+by+cz=d
	*/
	float a;

	/**
	* \brief b in ax+by+cz=d
	*/	
	float b;

	/**
	* \brief c in ax+by+cz=d 
	*/
	float c;

	/**
	* \brief d in ax+by+cz=d 
	*/
	float d; 

	plane();
	plane(float,float,float,float);
	bool isCoplanar(plane);
	bool isParallel(plane);
};

/**
* \brief Struct representing only the coordinates of a point
* 
*/
struct coordinate{
	float x,y,z;

	coordinate();
	coordinate(float,float,float);
	coordinate(vertex);
	coordinate add(coordinate);
	coordinate subtract(coordinate);
	//bool equals(coordinate);

	/**
	* \brief Checks for equality of 2 coordinate type objects
	* 
	* \param  Coordinate to be compared with the current coordinate
	* \return boolean that returns true if the 2 objects are equal
	* 
	*/
	bool operator ==(const coordinate &other) const
	{
		return (compareFloats(this->x,other.x) && compareFloats(this->y,other.y) && compareFloats(this->z,other.z));
	}

	bool operator < (const coordinate& v2) const
	{
		if(compareFloats(this->x,v2.x)&&compareFloats(this->y,v2.y)&&(this->z<v2.z))return true;
		else if(compareFloats(this->x,v2.x)&&this->y<v2.y) return true;
		else if(this->x<v2.x) return true;
		return false;		
	}
	
};

/**
* \brief Struct representing only the coordinates of end points of edge
* 
*/
struct coordinatePair{
	coordinate v1, v2;

	coordinatePair();
	coordinatePair(coordinate, coordinate);
	coordinatePair(float,float,float,float,float,float);
	coordinatePair(edge);
	void convertToStandardForm();
	void printCoordinatePair();

	/**
	* \brief Checks for equality of 2 coordinatePair type objects
	* 
	* \param  coordinatePair to be compared with the current coordinatePair
	* \return boolean that returns true if the 2 objects are equal
	* 
	*/
	bool operator ==(const coordinatePair &other) const
	{
		//other.convertToStandardForm();
		//convertToStandardForm();
		return (v1.operator ==(other.v1) && v2.operator==(other.v2));
	}
};

/**
* \brief Struct representing vertex (x,y,z)
* 
*/
struct vertex{
	coordinate v;

	/**
	* \brief Number of edges adjacent to vertex
	*/
	vector<coordinate> adjacentCoordinates;
	float numEdges; 

	vertex();
	vertex(coordinate,int numEdges=0);
	vertex(float,float,float,int numEdges=0);
	void printVertex(); 
};

/**
* \brief Struct representing directed edge from v1 to v2
* 
*/
struct edge{
	coordinate v1, v2; //not vertex v1,v2 bcoz there shouldnt be copy of vertex object her, changes wont be reflected

	/**
	* \brief True if edge is hidden in XY projection
	*/	 
	bool hiddenXY;

	/**
	* \brief True if edge is hidden in XY projection
	*/
	bool hiddenYZ;

	/**
	* \brief True if edge is hidden in XY projection
	*/
	bool hiddenZX;
	int numFaces;

	edge();
	edge(coordinate v1, coordinate v2,bool hidXY=false,bool hidYZ=false,bool hidZX=false);
	edge(float,float,float,float,float,float,bool hidXY=false,bool hidYZ=false,bool hidZX=false);
	edge(coordinatePair,bool hidXY=false,bool hidYZ=false,bool hidZX=false);
	float length2D(int);
	float length3D();
	void convertToStandardForm();
	void printEdge();
};

/**
* \brief Struct representing polygon face 
* 
*/
struct face{
	
	/**
	* \brief List of edges constituting the face
	*/
	vector<coordinatePair> edges; 
	vector<coordinate> vertices;

	/**
	* \brief Corresponding infinte plane of face
	*/
	plane corrPlane;
	glm::vec3 normal;
	bool corrected;

	face();
	face(vector<coordinatePair>);
	void addEdge(coordinatePair);
	void addEdge(coordinate,coordinate);
	void addEdge(vector<coordinatePair>);
	void addEdge(float,float,float,float,float,float);
	bool hasEdge(coordinatePair);
	bool hasEdge(edge);
	void findCorrPlane();
	plane getCorrPlane();
	void combineCoplanarFaces(face,edge);
	void printFace();
	void findNormal();
};

/**
* \brief Struct containing Hash function for coodinate type
* 
*/
struct coordinateHasher{

	std::size_t operator()(const coordinate& k) const
	{
		std::size_t res = 17;
        res = res * 31 + std::hash<int>()( k.x );
        res = res * 31 + std::hash<int>()( k.y );
        res = res * 31 + std::hash<int>()( k.z );
        return res;
	}
};

/**
* \brief Struct containing Hash function for coodinatePair type
* 
*/
struct coordinatePairHasher{

	std::size_t operator()(const coordinatePair& k) const
	{
		std:: size_t res = 17;
        res = res * 31 + std::hash<int>()( k.v1.x );
        res = res * 31 + std::hash<int>()( k.v1.y );
        res = res * 31 + std::hash<int>()( k.v1.z );
        res = res * 31 + std::hash<int>()( k.v2.x );
        res = res * 31 + std::hash<int>()( k.v2.y );
        res = res * 31 + std::hash<int>()( k.v2.z );
        return res;
	}
};

struct vertexMapping{
	vector<coordinate> vertexCoordinate;
	coordinate getCoordinate(int);
	void addCoordinate(coordinate);
};

#endif