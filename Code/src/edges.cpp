#include "edges.h"
#include "objects.h"



/**
* \brief Constructor to initialize empty object of edges
* \param  a is the edge to be added
* 
*/
edges::edges(){
}

/**
* \brief Adds new edge to the list of edges
* \param  a is the edge to be added
* \return boolean that returns true if edge is valid and not already present
*/
bool edges::addEdge(edge a){
//if edge is of 0 length, dont add it n return 0
	if(!compareFloats(a.length3D(),0.0))
	{
		return e.insert(make_pair(coordinatePair(a.v1,a.v2),a)).second;
	}
	return false;
}

/**
* \brief Adds new edge to the list of edges
* \param  a is the edge to be added
* \return boolean that returns true if edge is valid and not already present
*/
bool edges::addEdge(coordinatePair a){
//if edge is of 0 length, dont add it n return 0
	edge e1=edge(a);
	if(!compareFloats(e1.length3D(),0.0))
	{
		return e.insert(make_pair(a,e1)).second;
	}
	return false;
}

bool edges::addEdge(coordinate a,coordinate b){
	edge e1=edge(a,b);
	if(!compareFloats(e1.length3D(),0.0))
	{
		return e.insert(make_pair(coordinatePair(a,b),e1)).second;
	}
	return false;
}

/**
* \brief Adds new edge to the list of edges
* \param  x1, y1, z1 are coordinates of 1 end point, x2, y2, z2 are coordinates of other end point
* \return boolean that returns true if edge is valid and not already present
*/
bool edges::addEdge(float x1,float y1,float z1, float x2, float y2, float z2){
	edge e1=edge(x1,y1,z1,x2,y2,z2);
	if(!compareFloats(e1.length3D(),0.0))
	{
		return e.insert(make_pair(coordinatePair(x1,y1,z1,x2,y2,z2),e1)).second;
	}
	return false;
}

void edges::removeEdge(coordinatePair a){
	e.erase(a);
}

void edges::removeEdge(edge a){
	e.erase(coordinatePair(a.v1,a.v2));
}


// *
// * \brief Transform all edges according to new coordinate system
// * 
// * \param  a, b and c are the 3 projection planes that are to be treated as XY, YZ and ZX planes respectively
// * \return void

// void edges::changeFrame(plane a,plane b,plane c){

// }

bool compare(pair<coordinate,int> coord1,pair<coordinate,int> coord2){
	return (coord1.first.operator <(coord2.first));
}

float findFirstCoord(float x1,float y1,float z1,float x2,float y2,float z2,float ynew,float znew){
	float xnew=x1;
	if(!compareFloats(y1,y2))
	{
		xnew+=(ynew-y1)*(x2-x1)/(y2-y1);
		return xnew;
	}

	if(!compareFloats(z1,z2))
	{
		xnew+=(znew-z1)*(x2-x1)/(z2-z1);
		return xnew;
	}
	if(compareFloats(y1,ynew)&&compareFloats(z1,znew))	return -10;
	return -20;
}

float findSecondCoord(float x1,float y1,float z1,float x2,float y2,float z2,float znew,float xnew){
	float ynew=y1;
	if(!compareFloats(z1,z2))
	{
		ynew+=(znew-z1)*(y2-y1)/(z2-z1);
		return ynew;
	}

	if(!compareFloats(x1,x2))
	{
		ynew+=(xnew-x1)*(y2-y1)/(x2-x1);
		return ynew;
	}
	
	if(compareFloats(x1,xnew)&&compareFloats(z1,znew))	return -10;
	return -20;
}

float findThirdCoord(float x1,float y1,float z1,float x2,float y2,float z2,float xnew,float ynew){
	float znew=z1;
	if(!compareFloats(x1,x2))
	{
		znew+=(xnew-x1)*(z2-z1)/(x2-x1);
		return znew;
	}

	if(!compareFloats(y1,y2))
	{
		znew+=(ynew-y1)*(z2-z1)/(y2-y1);
		return znew;
	}
	
	if(compareFloats(y1,ynew)&&compareFloats(x1,xnew))	return -10;
	return -20;
}
void edges::removeRedundantEdges(){
	bool changes=true; bool innerbreak=false;
	while(changes)
	{
		changes=false;
		for(auto ita=e.begin();ita!=e.end();++ita)
		{
			for(auto itb=e.begin();itb!=e.end();++itb)
			{
				innerbreak=false;
				if(ita!=itb)
				{
					coordinate c1=ita->first.v1, c2=ita->first.v2, c3=itb->first.v1, c4=itb->first.v2;

					if(!compareFloats(c1.x,c2.x))
					{
						if(compareFloats(c3.y,findSecondCoord(c1.x,c1.y,c1.z,c2.x,c2.y,c2.z,c3.z,c3.x)) && compareFloats(c4.y,findSecondCoord(c1.x,c1.y,c1.z,c2.x,c2.y,c2.z,c4.z,c4.x)) && compareFloats(c3.z,findThirdCoord(c1.x,c1.y,c1.z,c2.x,c2.y,c2.z,c3.x,c3.y)) && compareFloats(c4.z,findThirdCoord(c1.x,c1.y,c1.z,c2.x,c2.y,c2.z,c4.x,c4.y)))
						{
							if(c3.x>=std::min(c1.x,c2.x)&&c3.x<=std::max(c1.x,c2.x)&&c4.x>=std::min(c1.x,c2.x)&&c4.x<=std::max(c1.x,c2.x))
							{
								auto itv1 = v.getVertices().find(ita->first.v1); auto itv2=v.getVertices().find(ita->first.v2); 
								itv1->second.numEdges--;
								itv2->second.numEdges--;

								vector<coordinate> &v1Adj = itv1->second.adjacentCoordinates;
								vector<coordinate> &v2Adj = itv2->second.adjacentCoordinates;
								for(int t=0;t<v1Adj.size();++t)
									if(v1Adj[t].operator==(itv2->first))
										v1Adj.erase(v1Adj.begin()+t);
								for(int t=0;t<v2Adj.size();++t)
									if(v2Adj[t].operator==(itv1->first))
										v2Adj.erase(v2Adj.begin()+t);
								removeEdge(ita->first);
								innerbreak=true;
								changes=true;
								break;
							}
						}
					}

					else if(!compareFloats(c1.y,c2.y))
					{
						if(compareFloats(c3.x,findFirstCoord(c1.x,c1.y,c1.z,c2.x,c2.y,c2.z,c3.y,c3.z)) && compareFloats(c4.x,findFirstCoord(c1.x,c1.y,c1.z,c2.x,c2.y,c2.z,c4.y,c4.z)) && compareFloats(c3.z,findThirdCoord(c1.x,c1.y,c1.z,c2.x,c2.y,c2.z,c3.x,c3.y)) && compareFloats(c4.z,findThirdCoord(c1.x,c1.y,c1.z,c2.x,c2.y,c2.z,c4.x,c4.y)))
						{
							if(c3.y>=std::min(c1.y,c2.y)&&c3.y<=std::max(c1.y,c2.y)&&c4.y>=std::min(c1.y,c2.y)&&c4.y<=std::max(c1.y,c2.y))
							{
								auto itv1 = v.getVertices().find(ita->first.v1); auto itv2=v.getVertices().find(ita->first.v2); 
								itv1->second.numEdges--;
								itv2->second.numEdges--;

								vector<coordinate> &v1Adj = itv1->second.adjacentCoordinates;
								vector<coordinate> &v2Adj = itv2->second.adjacentCoordinates;
								for(int t=0;t<v1Adj.size();++t)
									if(v1Adj[t].operator==(itv2->first))
										v1Adj.erase(v1Adj.begin()+t);
								for(int t=0;t<v2Adj.size();++t)
									if(v2Adj[t].operator==(itv1->first))
										v2Adj.erase(v2Adj.begin()+t);
								removeEdge(ita->first);
								innerbreak=true;
								changes=true;
								break;
							}
						}
					}

					else if(!compareFloats(c1.z,c2.z))
					{
						if(compareFloats(c3.x,findFirstCoord(c1.x,c1.y,c1.z,c2.x,c2.y,c2.z,c3.y,c3.z)) && compareFloats(c4.x,findFirstCoord(c1.x,c1.y,c1.z,c2.x,c2.y,c2.z,c4.y,c4.z)) && compareFloats(c3.y,findSecondCoord(c1.x,c1.y,c1.z,c2.x,c2.y,c2.z,c3.z,c3.x)) && compareFloats(c4.y,findSecondCoord(c1.x,c1.y,c1.z,c2.x,c2.y,c2.z,c4.z,c4.x)))
						{
							if(c3.z>=std::min(c1.z,c2.z)&&c3.z<=std::max(c1.z,c2.z)&&c4.z>=std::min(c1.z,c2.z)&&c4.z<=std::max(c1.z,c2.z))
							{
								auto itv1 = v.getVertices().find(ita->first.v1); auto itv2=v.getVertices().find(ita->first.v2); 
								itv1->second.numEdges--;
								itv2->second.numEdges--;

								vector<coordinate> &v1Adj = itv1->second.adjacentCoordinates;
								vector<coordinate> &v2Adj = itv2->second.adjacentCoordinates;
								for(int t=0;t<v1Adj.size();++t)
									if(v1Adj[t].operator==(itv2->first))
										v1Adj.erase(v1Adj.begin()+t);
								for(int t=0;t<v2Adj.size();++t)
									if(v2Adj[t].operator==(itv1->first))
										v2Adj.erase(v2Adj.begin()+t);
								removeEdge(ita->first);
								innerbreak=true;
								changes=true;
								break;
							}
						}
					}
				}
			}
			if(innerbreak)	break;
		}
	}
}

/**
* \brief Break edges for preprocessing
* 
* \param  flag is true for 2D to 3D and false for 3D to 2D
* \return void
*
* It traverses for all pairs of edges and looks for intersection in the projections and adds new edges to the list e and new vertex to v. It retains the original edges if flag is true (2D to 3D) but discards the original edges if flag is false (3D to 2D). 
*/
void edges::breakEdges(bool flag, int view){
	if(flag)
	{
		bool changes=true;
		while(changes)
		{
			changes=false;
			for (auto it1 = this->e.begin(); it1 != this->e.end(); ++it1)
			{
				auto it2 = it1;++it2;
				for(; it2 !=this->e.end(); ++it2)
				{
					float x1=it1->second.v1.x, x2=it1->second.v2.x,
						  x3=it2->second.v1.x, x4=it2->second.v2.x,
						  y1=it1->second.v1.y, y2=it1->second.v2.y,
						  y3=it2->second.v1.y, y4=it2->second.v2.y;

					float a=x2-x1, b=x3-x4, c=x3-x1, d=y2-y1, e=y3-y4, f=y3-y1;
					float den=a*e-b*d, num1=a*f-c*d, num2=c*e-b*f;
					
					if(!(compareFloats(a,0.0)&&compareFloats(d,0.0))&&!(compareFloats(b,0.0)&&compareFloats(e,0.0)))
					{
						if(!compareFloats(den,0.0))
						{
							float s=num1/den, t=num2/den;

							if((s>=0&&s<=1&&t>0&&t<1)||(s>0&&s<1&&t>=0&&t<=1))
							{
								coordinate newcoord;
								newcoord.x=x1+t*(x2-x1);
								newcoord.y=y1+t*(y2-y1);
								newcoord.z=0;

								vertex newvertex;
								newvertex.v=newcoord;

								edge e1=edge(it1->second.v1,newcoord,it1->second.hiddenXY);
								edge e2=edge(it1->second.v2,newcoord,it1->second.hiddenXY);
								edge e3=edge(it2->second.v1,newcoord,it2->second.hiddenXY);
								edge e4=edge(it2->second.v2,newcoord,it2->second.hiddenXY);					

								bool b0=this->v.addVertex(newvertex);
								bool b1=this->addEdge(e1);
								bool b2=this->addEdge(e2);
								bool b3=this->addEdge(e3);
								bool b4=this->addEdge(e4);
								changes=b1|b2|b3|b4|changes;
								// if(changes){
								// 	it1 = this->e.begin();
								// 	break;
								// }
							}					
						}

						else if(compareFloats(den,0.0)&&compareFloats(num1,0.0)&&compareFloats(num2,0.0))
						{
							if(((x3-x1)/(x2-x1)>=0&&(x3-x1)/(x2-x1)<=1)||((x4-x1)/(x2-x1)>=0&&(x4-x1)/(x2-x1)<=1)||((x1-x3)/(x4-x3)>=0&&(x1-x3)/(x4-x3)<=1)||((x2-x3)/(x4-x3)>=0&&(x2-x3)/(x4-x3)<=1))
							{
								//changes=true;

								edge e1=(it1->second.hiddenXY && it2->second.hiddenXY)?edge(it1->second.v1,it2->second.v1,true):edge(it1->second.v1,it2->second.v1,false);	
								//else	edge e1=edge(it1->second.v1,it2->second.v1,false);
								edge e2=(it1->second.hiddenXY && it2->second.hiddenXY)?edge(it1->second.v1,it2->second.v2,true):edge(it1->second.v1,it2->second.v2,false);
								edge e3=(it1->second.hiddenXY && it2->second.hiddenXY)?edge(it1->second.v2,it2->second.v1,true):edge(it1->second.v2,it2->second.v1,false);
								edge e4=(it1->second.hiddenXY && it2->second.hiddenXY)?edge(it1->second.v2,it2->second.v2,true):edge(it1->second.v2,it2->second.v2,false);
								bool b1=this->addEdge(e1);
								bool b2=this->addEdge(e2);
								bool b3=this->addEdge(e3);
								bool b4=this->addEdge(e4);
								changes=b1|b2|b3|b4|changes;
								// if(changes){
								// 	it1 = this->e.begin();
								// 	break;
								// }
							}
						}
					}
					
				}
			}
		}
	}

	else
	{
		bool changes=true;
		while(changes)
		{	
			changes=false;
			for (auto it1 = this->e.begin(); it1 != this->e.end(); ++it1)
			{
				auto it2=it1;
				it2++;
				for(; it2 !=this->e.end(); ++it2)
				{
					float x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,z3,z4;
					if(view==1) //XY proj
					{
						x1=it1->second.v1.x; x2=it1->second.v2.x;
						x3=it2->second.v1.x; x4=it2->second.v2.x;
						y1=it1->second.v1.y; y2=it1->second.v2.y;
						y3=it2->second.v1.y; y4=it2->second.v2.y;
						z1=it1->second.v1.z; z2=it1->second.v2.z;
						z3=it2->second.v1.z; z4=it2->second.v2.z;
					}

					else if(view==2) //YZ proj
					{
						x1=it1->second.v1.y, x2=it1->second.v2.y,
						x3=it2->second.v1.y, x4=it2->second.v2.y,
						y1=it1->second.v1.z, y2=it1->second.v2.z,
						y3=it2->second.v1.z, y4=it2->second.v2.z;
						z1=it1->second.v1.x; z2=it1->second.v2.x;
						z3=it2->second.v1.x; z4=it2->second.v2.x;
					}

					else if(view==3) //ZX proj
					{
						x1=it1->second.v1.z, x2=it1->second.v2.z,
						x3=it2->second.v1.z, x4=it2->second.v2.z,
						y1=it1->second.v1.x, y2=it1->second.v2.x,
						y3=it2->second.v1.x, y4=it2->second.v2.x;
						z1=it1->second.v1.y; z2=it1->second.v2.y;
						z3=it2->second.v1.y; z4=it2->second.v2.y;
					}
					coordinate c1(x1,y1,z1), c2(x2,y2,z2), c3(x3,y3,z3), c4(x4,y4,z4);

					float a=x2-x1, b=x3-x4, c=x3-x1, d=y2-y1, e=y3-y4, f=y3-y1;
					if(!(compareFloats(a,0.0)&&compareFloats(d,0.0))&&!(compareFloats(b,0.0)&&compareFloats(e,0.0)))
					{
						float den=a*e-b*d, num1=a*f-c*d, num2=c*e-b*f;
						
						if(!compareFloats(den,0.0))
						{
							float s=num1/den, t=num2/den;

							if((s>=0&&s<=1&&t>0&&t<1)||(s>0&&s<1&&t>=0&&t<=1))
							{
								coordinate newcoord1,newcoord2;
								float xnew=x1+t*(x2-x1), ynew=y1+t*(y2-y1); 
								float znew1=findThirdCoord(x1,y1,z1,x2,y2,z2,xnew,ynew), znew2=findThirdCoord(x3,y3,z3,x4,y4,z4,xnew,ynew);

								if(view==1)
								{
									newcoord1.x=newcoord2.x=xnew;
									newcoord1.y=newcoord2.y=ynew;
									newcoord1.z=znew1; newcoord2.z=znew2;
								}
								
								else if(view==2)
								{
									newcoord1.y=newcoord2.y=xnew;
									newcoord1.z=newcoord2.z=ynew;
									newcoord1.x=znew1; newcoord2.x=znew2;
								}

								else if(view==3)
								{
									newcoord1.z=newcoord2.z=xnew;
									newcoord1.x=newcoord2.x=ynew;
									newcoord1.y=znew1; newcoord2.y=znew2;
								}
								vertex newvertex1=vertex(newcoord1);
								vertex newvertex2=vertex(newcoord2);

								edge e1=edge(it1->second.v1,newcoord1);
								edge e2=edge(it1->second.v2,newcoord1);
								edge e3=edge(it2->second.v1,newcoord2);
								edge e4=edge(it2->second.v2,newcoord2);

								this->removeEdge(it1->first);	
								this->removeEdge(it2->first);				

								bool b0=this->v.addVertex(newvertex1)|this->v.addVertex(newvertex2);
								bool b1=this->addEdge(e1);
								bool b2=this->addEdge(e2);
								bool b3=this->addEdge(e3);
								bool b4=this->addEdge(e4);
								changes=b1|b2|b3|b4|changes;
								
								if(changes){
									it1 = this->e.begin();
									break;
								}
							}				
						}
					}		
				}
			}
		}
	}

}

/**
* \brief Gives all probable edges
* 
* \param  void
* \return void
*
* It generates all edges that are probable from the given projections (2D to 3D)and adds them to e.
*/
void edges::getProbableEdges(edges &et, edges &ef, edges &es){
	unordered_map<coordinatePair,edge,coordinatePairHasher> &ete = et.getEdges();
	unordered_map<coordinatePair,edge,coordinatePairHasher> &efe = ef.getEdges();
	unordered_map<coordinatePair,edge,coordinatePairHasher> &ese = es.getEdges();
	unordered_map<coordinate,vertex,coordinateHasher> &vtv =et.getVertices().getVertices();
	unordered_map<coordinate,vertex,coordinateHasher> &vfv =ef.getVertices().getVertices();
	unordered_map<coordinate,vertex,coordinateHasher> &vsv =es.getVertices().getVertices();
	for (auto it1 = ete.begin(); it1 != ete.end(); ++it1)
	{
		float x1=it1->first.v1.x, y1=it1->first.v1.y, x2=it1->first.v2.x, y2=it1->first.v2.y;
		for (auto it2 = efe.begin(); it2 != efe.end(); ++it2)
		{
			bool enter=false; float za, zb;
			if(compareFloats(y1,it2->first.v1.x) && compareFloats(y2,it2->first.v2.x))
			{
				enter=true;
				za=it2->first.v1.y, zb=it2->first.v2.y;
				if(enter)
				{
					coordinatePair toFind=coordinatePair(za,x1,0,zb,x2,0);
					auto it3=ese.find(toFind);
					if(it3!=ese.end())
					{
						this->addEdge(edge(x1,y1,za,x2,y2,zb,it1->second.hiddenXY,it2->second.hiddenXY,it3->second.hiddenXY));
						// look into this ki ye neeche waala sab karna hai (agar karna hai to ye bhi karna hi ki c1,c2 ke neighbour waale vectors update ho) ya ye sab addEdge me daalna hai???
						coordinate c1=coordinate(x1,y1,za); coordinate c2=coordinate(x2,y2,zb);
						unordered_map<coordinate,vertex,coordinateHasher> &v3D =this->v.getVertices();
						auto ita = v3D.find(c1); auto itb=v3D.find(c2); 
						if(ita != v3D.end() && itb != v3D.end())
						{
							ita->second.numEdges++;
							itb->second.numEdges++;
							ita->second.adjacentCoordinates.push_back(c2);
							itb->second.adjacentCoordinates.push_back(c1);
						}
					}				
				}
			}
			//can make it else if agar too many edges are coming
			if(compareFloats(y2,it2->first.v1.x) && compareFloats(it2->first.v2.x,y1))
			{
				enter=true;
				zb=it2->first.v1.y, za=it2->first.v2.y;
				if(enter)
				{
					coordinatePair toFind=coordinatePair(za,x1,0,zb,x2,0);
					auto it3=ese.find(toFind);
					if(it3!=ese.end())
					{
						this->addEdge(edge(x1,y1,za,x2,y2,zb,it1->second.hiddenXY,it2->second.hiddenXY,it3->second.hiddenXY));
						// look into this ki ye neeche waala sab karna hai (agar karna hai to ye bhi karna hi ki c1,c2 ke neighbour waale vectors update ho) ya ye sab addEdge me daalna hai???
						coordinate c1=coordinate(x1,y1,za); coordinate c2=coordinate(x2,y2,zb);
						unordered_map<coordinate,vertex,coordinateHasher> &v3D =this->v.getVertices();
						auto ita = v3D.find(c1); auto itb=v3D.find(c2); 
						if(ita != v3D.end() && itb != v3D.end())
						{
							ita->second.numEdges++;
							itb->second.numEdges++;
							ita->second.adjacentCoordinates.push_back(c2);
							itb->second.adjacentCoordinates.push_back(c1);
						}
					}				
				}
			}
		}
	}
	//parallel to x
	for (auto it1 = ete.begin(); it1 != ete.end(); ++it1)
	{
		float x1=it1->first.v1.x, y1=it1->first.v1.y, x2=it1->first.v2.x, y2=it1->first.v2.y,y;
		if(compareFloats(y1,y2))
		{	
			y=y1;
			for (auto it2 = ese.begin(); it2 != ese.end(); ++it2)
			{
				bool enter=false; float z;
				if(compareFloats(it2->first.v1.y,x1) && compareFloats(it2->first.v2.y,x2) && compareFloats(it2->first.v1.x,it2->first.v2.x))
				{
					enter=true;
					z=it2->first.v1.x;
				}

				if(compareFloats(it2->first.v1.y,x2) && compareFloats(it2->first.v2.y,x1) && compareFloats(it2->first.v1.x,it2->first.v2.x))
				{
					enter=true;
					z=it2->first.v1.x;
				}

				if(enter)
				{
					coordinate toFind=coordinate(y,z,0);
					auto it3=vfv.find(toFind);
					if(it3!=vfv.end())
					{
						this->addEdge(edge(x1,y,z,x2,y,z,it1->second.hiddenXY,false,it2->second.hiddenXY));
						coordinate c1(x1,y,z),c2(x2,y,z);
						unordered_map<coordinate,vertex,coordinateHasher> &v3D =this->v.getVertices();
						auto ita = v3D.find(c1); auto itb=v3D.find(c2); 
						if(ita != v3D.end() && itb != v3D.end())
						{
							ita->second.numEdges++;
							itb->second.numEdges++;
							ita->second.adjacentCoordinates.push_back(c2);
							itb->second.adjacentCoordinates.push_back(c1);
						}
					}									
				}
			}
		}		
	}
	//parallel to y
	for (auto it1 = efe.begin(); it1 != efe.end(); ++it1)
	{
		float y1=it1->first.v1.x, z1=it1->first.v1.y, y2=it1->first.v2.x, z2=it1->first.v2.y,z;
		if(compareFloats(z1,z2))
		{	
			z=z1;
			for (auto it2 = ete.begin(); it2 != ete.end(); ++it2)
			{
				bool enter=false; float x;
				if(compareFloats(it2->first.v1.y,y1) && compareFloats(it2->first.v2.y,y2) && compareFloats(it2->first.v1.x,it2->first.v2.x))
				{
					enter=true;
					x=it2->first.v1.x;
				}

				if(compareFloats(it2->first.v1.y,y2) && compareFloats(it2->first.v2.y,y1) && compareFloats(it2->first.v1.x,it2->first.v2.x))
				{
					enter=true;
					x=it2->first.v1.x;
				}

				if(enter)
				{
					coordinate toFind=coordinate(z,x,0);
					auto it3=vsv.find(toFind);
					if(it3!=vsv.end())
					{
						this->addEdge(edge(x,y1,z,x,y2,z,it2->second.hiddenXY,it1->second.hiddenXY,false));
						coordinate c1(x,y1,z),c2(x,y2,z);
						unordered_map<coordinate,vertex,coordinateHasher> &v3D =this->v.getVertices();
						auto ita = v3D.find(c1); auto itb=v3D.find(c2); 
						if(ita != v3D.end() && itb != v3D.end())
						{
							ita->second.numEdges++;
							itb->second.numEdges++;
							ita->second.adjacentCoordinates.push_back(c2);
							itb->second.adjacentCoordinates.push_back(c1);
						}
					}									
				}
			}
		}		
	}
	//parallel to z
	for (auto it1 = ese.begin(); it1 != ese.end(); ++it1)
	{
		float z1=it1->first.v1.x, x1=it1->first.v1.y, z2=it1->first.v2.x, x2=it1->first.v2.y,x;
		if(compareFloats(x1,x2))
		{	
			x=x1;
			for (auto it2 = efe.begin(); it2 != efe.end(); ++it2)
			{
				bool enter=false; float y;
				if(compareFloats(it2->first.v1.y,z1) && compareFloats(it2->first.v2.y,z2) && compareFloats(it2->first.v1.x,it2->first.v2.x))
				{
					enter=true;
					y=it2->first.v1.x;
				}

				if(compareFloats(it2->first.v1.y,z2) && compareFloats(it2->first.v2.y,z1) && compareFloats(it2->first.v1.x,it2->first.v2.x))
				{
					enter=true;
					y=it2->first.v1.x;
				}

				if(enter)
				{
					coordinate toFind=coordinate(x,y,0);
					auto it3=vtv.find(toFind);
					if(it3!=vtv.end())
					{
						this->addEdge(edge(x,y,z1,x,y,z2,false,it2->second.hiddenXY,it1->second.hiddenXY));
						coordinate c1(x,y,z1),c2(x,y,z2);
						unordered_map<coordinate,vertex,coordinateHasher> &v3D =this->v.getVertices();
						auto ita = v3D.find(c1); auto itb=v3D.find(c2); 
						if(ita != v3D.end() && itb != v3D.end())
						{
							ita->second.numEdges++;
							itb->second.numEdges++;
							ita->second.adjacentCoordinates.push_back(c2);
							itb->second.adjacentCoordinates.push_back(c1);
						}
					}									
				}
			}
		}		
	}
}

/**
* \brief Gives all probable vertices
* 
* \param  void
* \return void
*
* It generates all vertices that are probable from the given projections (2D to 3D)and adds them to v.
*/
void edges::getProbableVertices(vertices &vt, vertices &vf, vertices &vs){
	unordered_map<coordinate,vertex,coordinateHasher> &vtv = vt.getVertices();
	unordered_map<coordinate,vertex,coordinateHasher> &vfv = vf.getVertices();
	unordered_map<coordinate,vertex,coordinateHasher> &vsv = vs.getVertices();
	for (auto it1 = vtv.begin(); it1 != vtv.end(); ++it1)
	{
		float x=it1->first.x, y=it1->first.y;
		for (auto it2 = vfv.begin(); it2 != vfv.end(); ++it2)
		{
			if(it2->first.x==y)
			{
				float z=it2->first.y;
				auto it3=vsv.find(coordinate(z,x,0));
				if(it3!=vsv.end())
				{
					this->v.addVertex(x,y,z);
				}
			}			
		}
	}
	
}

/**
* \brief Corrects the mesh to remove probable edges
* 
* \param  void
* \return void
*
* It traverses across the set of edges e and removes the edges that have any of the endpoints having numEdges < 3.
*/
void edges::correctMesh(){
	unordered_map<coordinate,vertex,coordinateHasher> &v3D =this->v.getVertices();
	bool changes=true;
	while(changes)
	{
		changes=false;
		auto it=e.begin();
		while (it != e.end())
		{
			auto ita=v3D.find(it->second.v1); auto itb=v3D.find(it->second.v2);
			auto ittemp=it;
			++it;
			if(ita->second.numEdges<3||itb->second.numEdges<3)
			{
				ita->second.numEdges--;
				itb->second.numEdges--;
				e.erase(ittemp);
				changes=true;
			}
		}
	}

	changes = true;
	while(changes){
		changes = false;
		for(auto it=v3D.begin();it !=v3D.end();++it)
		{
			if(it->second.numEdges==0)
			{
				v3D.erase(it);
				changes = true;
				break;

			}
		}
	}
}

/**
* \brief Returns the list of vertices
* 
* \param  void
* \return Reference to the list of vertices
*
*/
vertices & edges::getVertices(){
	return this->v;
}

/**
* \brief Returns the list of edges
* 
* \param  void
* \return Reference to the list of edges e
*
*/
unordered_map<coordinatePair,edge,coordinatePairHasher> & edges::getEdges(){
	return this->e;
}

unordered_map<coordinatePair,edge,coordinatePairHasher> edges::getEdgesCopy(){
	return this->e;
}

void edges::printEdges(){
	cout<<"Edges:"<<endl;
	for(auto it=e.begin();it!=e.end();++it)
	{
		it->second.printEdge();
	}
	cout<<endl;
}