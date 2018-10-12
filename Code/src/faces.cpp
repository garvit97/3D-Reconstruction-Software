#include "faces.h"
#include "edges.h"
#include "objects.h"
#include <glm/glm.hpp>
#include<glm/gtx/vector_angle.hpp>
extern float findFirstCoord(float,float,float,float,float,float,float,float);
extern float findSecondCoord(float,float,float,float,float,float,float,float);
extern float findThirdCoord(float,float,float,float,float,float,float,float);
faces::faces(){

}

/**
* \brief Constructor to initialize object of faces 
* \param  e is the set of edges
* \return void
*/
faces::faces(edges e){
	this->e= e;
}

/**
* \brief Adds a face to the list of faces f
* 
* \param  a is the face to be added
* \return boolean that returns true if face is valid and not already present
*/
void faces::addFace(face a){
	f.push_back(a);
}

// void faces::addFaces(vector<>){

// }

// *
// * \brief Transform all faces according to new coordinate system
// * 
// * \param  a, b and c are the 3 projection planes that are to be treated as XY, YZ and ZX planes respectively
// * \return void

// void faces::changeFrame(plane a,plane b,plane c){

// }

/**
* \brief Makes all possible faces and adds to f
* 
* \param  void
* \return void
*
* It makes all possible faces using Minimum Internal Angle algorithm and adds the faces to f.
*/

glm::vec3 findCorrectFacingNormal(glm::vec3 n1,glm::vec3 n2){
	glm::vec3 n = glm::cross(n1,n2);
	if(glm::dot(n,glm::vec3(0,1,0))<0){
		n = -n;
	}
	else if(glm::dot(n,glm::vec3(0,1,0))==0){
		if(glm::dot(n,glm::vec3(1,0,0))<0){
			n = -n;
		}
		else if(glm::dot(n,glm::vec3(1,0,0))==0){
			if(glm::dot(n,glm::vec3(0,0,1))<0)
				n = -n;
		}
	}
	return n;
}
float angle(vertex v1, vertex v2, vertex v3){
	glm::vec3 n1 = glm::normalize(glm::vec3(v2.v.x-v1.v.x, v2.v.y-v1.v.y, v2.v.z-v1.v.z));
	glm::vec3 n2 = glm::normalize(glm::vec3(v3.v.x-v2.v.x, v3.v.y-v2.v.y, v3.v.z-v2.v.z));
	float angle = glm::dot(n1,n2);
	return angle;
}
float angle1(vertex v1, vertex v2, vertex v3,glm::vec3 n){
	glm::vec3 n1 = glm::normalize(glm::vec3(v2.v.x-v1.v.x, v2.v.y-v1.v.y, v2.v.z-v1.v.z));
	glm::vec3 n2 = glm::normalize(glm::vec3(v3.v.x-v2.v.x, v3.v.y-v2.v.y, v3.v.z-v2.v.z));
	float angle = glm::dot(n1,n2);
	float ref = glm::dot(glm::cross(n1,n),n2);
	if(ref<0){
		angle = 2 - angle;
	}
	return angle;
}
bool orientationSame(vertex vi, vertex vj, vertex vk,vertex vnext){
	glm::vec3 v1(vj.v.x-vi.v.x,vj.v.y-vi.v.y,vj.v.z-vi.v.z);
	glm::vec3 v2(vk.v.x-vj.v.x,vk.v.y-vj.v.y,vk.v.z-vj.v.z);
	glm::vec3 v3(vnext.v.x-vk.v.x,vnext.v.y-vk.v.y,vnext.v.z-vk.v.z);
	glm::vec3 cp1 = glm::normalize(glm::cross(v1,v2));
	glm::vec3 cp2 = glm::normalize(glm::cross(v2,v3));
	return (cp1==cp2);
}

bool sortFace(coordinatePair a,coordinatePair b){
	if(a.v1.operator <(b.v1)) return true;
	if(a.v1.operator ==(b.v1)&&a.v2.operator <(b.v2)) return true;
	return false;
}

bool checkIfSamePlane(vertex v1, vertex v2, vertex v3, vertex v4){
	glm::vec3 n1(v2.v.x-v1.v.x, v2.v.y-v1.v.y, v2.v.z-v1.v.z);
	glm::vec3 n2(v3.v.x-v2.v.x, v3.v.y-v2.v.y, v3.v.z-v2.v.z);
	glm::vec3 n3(v4.v.x-v3.v.x, v4.v.y-v3.v.y, v4.v.z-v3.v.z);
	return glm::normalize(glm::cross(n1,n2))==glm::normalize(glm::cross(n2,n3))||glm::normalize(glm::cross(n1,n2))==-glm::normalize(glm::cross(n2,n3));
}
void order(face &a){
	vector<coordinatePair> fver = a.edges;
	coordinatePair min = fver[0];
	int p =0;
	for(int i=1;i<fver.size();++i){
		if(sortFace(fver[i],min)){
			p = i;
			min = fver[i];
		}
	}
	vector<coordinatePair> tem;
	tem.insert(tem.end(),fver.begin()+p,fver.end());
	tem.insert(tem.end(),fver.begin(),fver.begin()+p);
	a.edges = tem;
	if(!sortFace(tem[1],tem[tem.size()-1])){
		vector<coordinatePair> tem1;
		tem1.push_back(tem[0]);
		tem.erase(tem.begin());
		tem1.insert(tem1.end(),tem.rbegin(),tem.rend());
		a.edges = tem1;
	}
	
}
/**
* \brief Makes all possible faces and adds to f
* 
* \param  void
* \return void
*
* It makes all possible faces using Minimum Internal Angle algorithm and adds the faces to f.
*/
void faces::iterateFace(vector<coordinate> toAdd,glm::vec3 n){
	unordered_map<coordinate,vertex,coordinateHasher> &v3D =this->e.getVertices().getVertices();
	vertex vLast = v3D.find(toAdd[toAdd.size()-1])->second;
	vertex vSecLast = v3D.find(toAdd[toAdd.size()-2])->second;
	vertex init = v3D.find(toAdd[0])->second;
	vertex vj = v3D.find(toAdd[1])->second;
	vertex vk = v3D.find(toAdd[2])->second;
	float min1 = 5;
	for(auto itnext=vLast.adjacentCoordinates.begin(); itnext!=vLast.adjacentCoordinates.end();++itnext)
	{	
		auto itvnext=v3D.find(*itnext);
			// cout<<"vnext : ("<<vnext.v.x<<", "<<vnext.v.y<<", "<<vnext.v.z<<")"<<endl;
			vertex vnext=itvnext->second;
			if(vnext.v.operator==(init.v)){
				std::vector<coordinate> temp = toAdd;
				toAdd.push_back(coordinate(vnext.v));
				std::vector<coordinatePair> faceEdges;
				for(int k=1;k<toAdd.size();++k)
					faceEdges.push_back(coordinatePair(toAdd[k],toAdd[k-1]));
				face f1=face(faceEdges);
				f1.vertices = temp;
				this->addFace(f1);
				return;
			}
			else if(checkIfSamePlane(init,vj,vk,vnext)&&angle1(vj,vk,vnext,n)<min1&&(!vSecLast.v.operator==(vnext.v))){
				min1=angle1(vj,vk,vnext,n);
			}
	}
	if(min1==5)
		return;
	bool continueloop = false;
	for(auto itnext=vLast.adjacentCoordinates.begin(); itnext!=vLast.adjacentCoordinates.end();++itnext){
		auto itvnext=v3D.find(*itnext);
			vertex vnext=itvnext->second;
			if(checkIfSamePlane(init,vj,vk,vnext)&&angle1(vj,vk,vnext,n)==min1){
				continueloop = false;
				for(int t=0;t<toAdd.size();++t){
					if(v3D.find(toAdd[t])->second.v.operator==(vnext.v))
						continueloop = true;
				}
				if(continueloop)
					continue;
				std::vector<coordinate> temp = toAdd;
				temp.push_back(vnext.v);
				iterateFace(temp,n);
			}
	}	
}
void faces::makeFaces(){
	unordered_map<coordinate,vertex,coordinateHasher> &v3D =this->e.getVertices().getVertices();
	std::vector<coordinate> toAdd;
	for(auto itvertex=v3D.begin(); itvertex !=v3D.end(); ++itvertex){
		vertex vinit=itvertex->second;
		vertex vj,vk;
		toAdd.push_back(vinit.v);
		for(auto itneighbour=vinit.adjacentCoordinates.begin(); itneighbour!=vinit.adjacentCoordinates.end();++itneighbour){	
			auto itvj=v3D.find(*itneighbour);
			vj=itvj->second;
			toAdd.push_back(vj.v);
			float min1=5;vertex vktemp;
			std::vector<glm::vec3> faceNormals;
			faceNormals.clear();
			for(auto itforvk=vj.adjacentCoordinates.begin(); itforvk!=vj.adjacentCoordinates.end();++itforvk)
			{
				auto itvtemp=v3D.find(*itforvk);
				vktemp=itvtemp->second;
				if(!vinit.v.operator==(vktemp.v)){
					vk=vktemp;
					glm::vec3 n1 = glm::normalize(glm::vec3(vj.v.x-vinit.v.x, vj.v.y-vinit.v.y, vj.v.z-vinit.v.z));
					glm::vec3 n2 = glm::normalize(glm::vec3(vk.v.x-vj.v.x, vk.v.y-vj.v.y, vk.v.z-vj.v.z));
					faceNormals.push_back(glm::cross(n2,n1));
				}
			}
			for(int p = 0; p<faceNormals.size(); ++p){
				min1 = 5;
				vertex vmin;
				for(auto itforvk=vj.adjacentCoordinates.begin(); itforvk!=vj.adjacentCoordinates.end();++itforvk)
				{
					auto itvtemp=v3D.find(*itforvk);
					vktemp=itvtemp->second;
					vk = vktemp;
					glm::vec3 n1 = glm::normalize(glm::vec3(vj.v.x-vinit.v.x, vj.v.y-vinit.v.y, vj.v.z-vinit.v.z));
					glm::vec3 n2 = glm::normalize(glm::vec3(vk.v.x-vj.v.x, vk.v.y-vj.v.y, vk.v.z-vj.v.z));
					glm::vec3 n = glm::cross(n2,n1);
					if((!vinit.v.operator==(vktemp.v))&&angle(vinit,vj,vktemp)<min1&&n == faceNormals[p])
					{
						min1 = angle(vinit,vj,vktemp);
						vmin = vk;
					}			
				}
				std::vector<coordinate> temp = toAdd;
				temp.push_back(vmin.v);
				iterateFace(temp,faceNormals[p]);
			}
			toAdd.erase(toAdd.begin()+toAdd.size()-1);
		}
		toAdd.erase(toAdd.begin()+toAdd.size()-1);
	}		
	for(auto ita=f.begin(); ita!=f.end(); ++ita)
	{
		for(auto itb=f.begin();itb!=f.end();++itb)
		{	
			if(ita!=itb)
			{
				ita->findCorrPlane();
				itb->findCorrPlane();
				order(*ita);
				order(*itb);
				bool same=false;
				if(ita->edges.size()==itb->edges.size())
				{
					same=true;
					for(int i=0;i<ita->edges.size();++i)
					{
						if(!ita->edges[i].operator==(itb->edges[i]))
						{
							same=false;break;
						}
					}
				}
				if(same)
				{
					f.erase(itb);
					ita--;
					break;
				}
			}
		}
	}

	unordered_map<coordinatePair,edge,coordinatePairHasher> &e3D =this->e.getEdges();
	for(int i=0; i<f.size(); ++i)
	{
		for(int j=0;j<f[i].edges.size();++j)
		{			
			auto itedge=e3D.find(f[i].edges[j]);
			if(itedge!=e3D.end())
			{
				itedge->second.numFaces++;
			}
		}
	}
}
bool faces::removePseudoFaces(edges ef,edges es, edges et){
	unordered_map<coordinatePair,edge,coordinatePairHasher> e3d = e.getEdges();
	for(auto it = e3d.begin();it!=e3d.end();++it){
		if(it->second.numFaces>2){
			edges newEdges = e;
			faces newFaces(newEdges);
			std::vector<int> adjacentFaces;
			for(int i=0;i<f.size();i++){
				newFaces.addFace(f[i]);
				if(f[i].hasEdge(it->second))
					adjacentFaces.push_back(i);
			}
			for(int i=0;i<adjacentFaces.size();++i)
				for(int j=i+1;j<adjacentFaces.size();++j){
					int count=0;
					for(int k=0;k<adjacentFaces.size();++k){
						if(k!=j&&k!=i){
							vector<coordinatePair> faceEdges = f[adjacentFaces[k]-count].edges;
							for(int l=0;l<faceEdges.size();++l)
								(newFaces.getEdges().getEdges().find(faceEdges[l]))->second.numFaces--;
							newFaces.getFaces().erase(newFaces.getFaces().begin()+adjacentFaces[k]-count);
							count++;
						}
					}
					bool b1 = newFaces.func(ef,es,et);
					bool b2 = newFaces.rule7();
					if(b1&&b2) //if both rule 4 and rule 7 allow us to take these faces
						if(newFaces.removePseudoFaces(ef,es,et)){
							f = newFaces.getFaces();
							e = newFaces.getEdges();
							return true;
						}
					edges newEdges = e;
					newFaces = faces(newEdges);
					for(int k=0;k<f.size();k++)
						newFaces.addFace(f[k]);
							
				}
			return false;
		}
	} 
	if(rule2())
			return true;
	return false;
}
bool onSegment(coordinate p, coordinate q, coordinate r,int view){
    if(view==2)
    {
    	p.x=p.y;p.y=p.z;
    	q.x=q.y;q.y=q.z;
    	r.x=r.y;r.y=r.z;
    } 
    else if(view==3)
    {
    	p.y=p.x;p.x=p.z;
    	q.y=q.x;q.x=q.z;
    	r.y=r.x;r.x=r.z;
    } 
    if (r.x <= max(p.x, q.x) && r.x >= min(p.x, q.x) &&
            r.y <= max(p.y, q.y) && r.y >= min(p.y, q.y))
        return true;
    return false;
}

int orientation(coordinate p, coordinate q, coordinate r,int view){
	if(view==2)
    {
    	p.x=p.y;p.y=p.z;
    	q.x=q.y;q.y=q.z;
    	r.x=r.y;r.y=r.z;
    } 
    else if(view==3)
    {
    	p.y=p.x;p.x=p.z;
    	q.y=q.x;q.x=q.z;
    	r.y=r.x;r.x=r.z;
    } 
    float val = (r.y - p.y) * (q.x - r.x) -
              (r.x - p.x) * (q.y - r.y);

    if (compareFloats(val,0)) return 0;  // colinear
    return (val > 0)? 1: 2; // clock or counterclock wise
}

bool checkPointInsidePolygon(face poly, coordinate a, int view){ 
	bool oddNodes=false;

	for(int i=0;i<poly.edges.size();++i)
	{
		if(view==1)
		{
			if(onSegment(poly.edges[i].v1,poly.edges[i].v2,a,view)&&(orientation(poly.edges[i].v1,poly.edges[i].v2,a,view)==0)) return true;

			if((poly.edges[i].v1.y<a.y && poly.edges[i].v2.y>=a.y)||(poly.edges[i].v1.y>=a.y && poly.edges[i].v2.y<a.y))
			{
				if((poly.edges[i].v1.x+(a.y-poly.edges[i].v1.y)/(poly.edges[i].v2.y-poly.edges[i].v1.y)*(poly.edges[i].v2.x-poly.edges[i].v1.x))<a.x)
				{
					oddNodes=!oddNodes;
				}
			}
		}

		if(view==2)
		{
			if(onSegment(poly.edges[i].v1,poly.edges[i].v2,a,view)&&(orientation(poly.edges[i].v1,poly.edges[i].v2,a,view)==0)) return true;

			if((poly.edges[i].v1.z<a.z && poly.edges[i].v2.z>=a.z)||(poly.edges[i].v1.z>=a.z&& poly.edges[i].v2.z<a.z))
			{
				if((poly.edges[i].v1.y+(a.z-poly.edges[i].v1.z)/(poly.edges[i].v2.z-poly.edges[i].v1.z)*(poly.edges[i].v2.y-poly.edges[i].v1.y))<a.y)
				{
					oddNodes=!oddNodes;
				}
			}
		}

		if(view==3)
		{
			if(onSegment(poly.edges[i].v1,poly.edges[i].v2,a,view)&&(orientation(poly.edges[i].v1,poly.edges[i].v2,a,view)==0)) return true;

			if((poly.edges[i].v1.x<a.x && poly.edges[i].v2.x>=a.x)||(poly.edges[i].v1.x>=a.x && poly.edges[i].v2.x<a.x))
			{
				if((poly.edges[i].v1.z+(a.x-poly.edges[i].v1.x)/(poly.edges[i].v2.x-poly.edges[i].v1.x)*(poly.edges[i].v2.z-poly.edges[i].v1.z))<a.z)
				{
					oddNodes=!oddNodes;
				}
			}
		}		
	}
	return oddNodes;
}

bool faceAboveEdge(face poly, coordinate a, int view){
	if(view==1 && poly.corrPlane.c!=0){
		if(std::fabs((poly.corrPlane.a*a.x+poly.corrPlane.b*a.y+poly.corrPlane.c*a.z-poly.corrPlane.d))>=0.001&&(poly.corrPlane.a*a.x+poly.corrPlane.b*a.y+poly.corrPlane.c*a.z-poly.corrPlane.d)*(poly.corrPlane.a*a.x+poly.corrPlane.b*a.y-poly.corrPlane.d)>0.001) return true;
	}

	else if(view==2 && poly.corrPlane.a!=0){
		if(std::fabs((poly.corrPlane.a*a.x+poly.corrPlane.b*a.y+poly.corrPlane.c*a.z-poly.corrPlane.d))>=0.001&&(poly.corrPlane.a*a.x+poly.corrPlane.b*a.y+poly.corrPlane.c*a.z-poly.corrPlane.d)*(poly.corrPlane.b*a.y+poly.corrPlane.c*a.z-poly.corrPlane.d)>0.001) return true;
	}

	else if(view==3 && poly.corrPlane.b!=0){
		if(std::fabs((poly.corrPlane.a*a.x+poly.corrPlane.b*a.y+poly.corrPlane.c*a.z-poly.corrPlane.d))>=0.001&&(poly.corrPlane.a*a.x+poly.corrPlane.b*a.y+poly.corrPlane.c*a.z-poly.corrPlane.d)*(poly.corrPlane.a*a.x+poly.corrPlane.c*a.z-poly.corrPlane.d)>0.001) return true;
	}
	
	return false;
}

/**
* \brief Checks if given edge is hidden
* 
* \param  void
* \return void
*
* It compares edge a with all faces to check if it is hidden by any of the faces
*/
void faces::checkHidden(edge &a){
	bool do1=true,do2=true,do3=true;
	bool hidxy=false,hidyz=false,hidzx=false;
	if(compareFloats(a.length2D(1),0.0)) do1=false;
	if(compareFloats(a.length2D(2),0.0)) do2=false;
	if(compareFloats(a.length2D(3),0.0)) do3=false;
	coordinate mid=coordinate((a.v1.x+a.v2.x)/2,(a.v1.y+a.v2.y)/2,(a.v1.z+a.v2.z)/2);
	for(int i=0;i<f.size();++i)
	{	
		if(do1)
		{	
			if(faceAboveEdge(f[i],mid,1)&&checkPointInsidePolygon(f[i],mid,1))
			{
				hidxy=true; do1=false;
			}
		}

		if(do2)
		{	
			if(faceAboveEdge(f[i],mid,2)&&checkPointInsidePolygon(f[i],mid,2))
			{
				hidyz=true; do2=false;
			}
		}

		if(do3)
		{	
			if(faceAboveEdge(f[i],mid,3)&&checkPointInsidePolygon(f[i],mid,3))
			{
				hidzx=true; do3=false;
			}
		}	
	}
	a.hiddenXY=hidxy;
	a.hiddenYZ=hidyz;
	a.hiddenZX=hidzx;
}

void faces::findNatureOfEdges(){
	unordered_map<coordinatePair,edge,coordinatePairHasher> & e3D=this->e.getEdges();
	for(auto it=e3D.begin(); it!=e3D.end(); ++it)
	{
		edge& ecur=it->second;
		checkHidden(ecur);
	}
}

bool solveLinear(glm::vec3 l1, glm::vec3 l2, glm::vec2 &p){
	glm::mat2 det(l1[0],l2[0],l1[1],l2[1]);
	glm::vec2 c(l1[2],l2[2]);
	if(compareFloats(glm::determinant(det),0.0))
		return false;
	p = glm::inverse(det)*c;
	return true;
}

bool faces::rule2(){

	for(int i=0;i<f.size();++i)
	{
		for(int j=i+1;j<f.size();++j)
		{
			plane p1=f[i].corrPlane,p2=f[j].corrPlane;
			if(!p1.isParallel(p2))
			{
				glm::vec3 n1(p1.a,p1.b,p1.c);
				glm::vec3 n2(p2.a,p2.b,p2.c);
				glm::vec3 cp = glm::normalize(glm::cross(n1,n2));
				glm::vec3 p;
				glm::vec2 p2d;
				if(solveLinear(glm::vec3(p1.a,p1.b,p1.d),glm::vec3(p2.a,p2.b,p2.d),p2d))
					p = glm::vec3(p2d[0],p2d[1],0);
				else if(solveLinear(glm::vec3(p1.a,p1.c,p1.d),glm::vec3(p2.a,p2.c,p2.d),p2d))
					p = glm::vec3(p2d[0],0,p2d[1]);
				else if (solveLinear(glm::vec3(p1.b,p1.c,p1.d),glm::vec3(p2.b,p2.c,p2.d),p2d))
					p = glm::vec3(0,p2d[0],p2d[1]);
			
				std::vector<coordinatePair> f1Edges = f[i].edges;
				std::vector<coordinatePair> f2Edges = f[j].edges;
				int count=0,count1=0;
				for(int m=0;m<f1Edges.size();++m){
					float x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,z3,z4;
					x1 = p[0];
					x2 = p[0] + cp[0];
					y1 = p[1];
					y2 = p[1] + cp[1];
					z1 = p[2];
					z2 = p[2] + cp[2];
					x3 = f1Edges[m].v1.x;
					y3 = f1Edges[m].v1.y;
					z3 = f1Edges[m].v1.z;
					x4 = f1Edges[m].v2.x;
					y4 = f1Edges[m].v2.y;
					z4 = f1Edges[m].v2.z;
					float a=x2-x1, b=x3-x4, c=x3-x1, d=y2-y1, e=y3-y4, f=y3-y1;
					float den=a*e-b*d, num1=a*f-c*d, num2=c*e-b*f,s=-1,t=-1;
					if(compareFloats(den,0.0)){
						x1 = p[0];
						x2 = p[0] + cp[0];
						y1 = p[2];
						y2 = p[2] + cp[2];
						z1 = p[1];
						z2 = p[1] + cp[1];
						x3 = f1Edges[m].v1.x;
						y3 = f1Edges[m].v1.z;
						z3 = f1Edges[m].v1.y;
						x4 = f1Edges[m].v2.x;
						y4 = f1Edges[m].v2.z;
						z4 = f1Edges[m].v2.y;
						a=x2-x1, b=x3-x4, c=x3-x1, d=y2-y1, e=y3-y4, f=y3-y1;
						den=a*e-b*d, num1=a*f-c*d, num2=c*e-b*f;
					}
					if(compareFloats(den,0.0)){
						x1 = p[1];
						x2 = p[1] + cp[1];
						y1 = p[2];
						y2 = p[2] + cp[2];
						z1 = p[0];
						z2 = p[0] + cp[0];
						x3 = f1Edges[m].v1.y;
						y3 = f1Edges[m].v1.z;
						z3 = f1Edges[m].v1.x;
						x4 = f1Edges[m].v2.y;
						y4 = f1Edges[m].v2.z;
						z4 = f1Edges[m].v2.x;
						a=x2-x1, b=x3-x4, c=x3-x1, d=y2-y1, e=y3-y4, f=y3-y1;
						den=a*e-b*d, num1=a*f-c*d, num2=c*e-b*f;
					}
					if(!compareFloats(den,0.0)){
						s=num1/den;t=num2/den;
					}
					else
						continue;
					if((s==0||s==1)&&(z1+t*(z2-z1)==z3+s*(z4-z3)))
						count1++;
					if(s<1&&s>0&&(z1+t*(z2-z1)==z3+s*(z4-z3)))
						count++;
					if(count==2||count1==3)
						break;
				}
				if(count==2||count1==3){
					count=0;count1=0;
					for(int m=0;m<f2Edges.size();++m){
						float x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,z3,z4;
						x1 = p[0];
						x2 = p[0] + cp[0];
						y1 = p[1];
						y2 = p[1] + cp[1];
						z1 = p[2];
						z2 = p[2] + cp[2];
						x3 = f2Edges[m].v1.x;
						y3 = f2Edges[m].v1.y;
						z3 = f2Edges[m].v1.z;
						x4 = f2Edges[m].v2.x;
						y4 = f2Edges[m].v2.y;
						z4 = f2Edges[m].v2.z;
						float a=x2-x1, b=x3-x4, c=x3-x1, d=y2-y1, e=y3-y4, f=y3-y1;
						float den=a*e-b*d, num1=a*f-c*d, num2=c*e-b*f,s=-1,t=-1;
						if(compareFloats(den,0.0)){
							x1 = p[0];
							x2 = p[0] + cp[0];
							y1 = p[2];
							y2 = p[2] + cp[2];
							z1 = p[1];
							z2 = p[1] + cp[1];
							x3 = f2Edges[m].v1.x;
							y3 = f2Edges[m].v1.z;
							z3 = f2Edges[m].v1.y;
							x4 = f2Edges[m].v2.x;
							y4 = f2Edges[m].v2.z;
							z4 = f2Edges[m].v2.y;
							a=x2-x1, b=x3-x4, c=x3-x1, d=y2-y1, e=y3-y4, f=y3-y1;
							den=a*e-b*d, num1=a*f-c*d, num2=c*e-b*f;
						}
						if(compareFloats(den,0.0)){
							x1 = p[1];
							x2 = p[1] + cp[1];
							y1 = p[2];
							y2 = p[2] + cp[2];
							z1 = p[0];
							z2 = p[0] + cp[0];
							x3 = f2Edges[m].v1.y;
							y3 = f2Edges[m].v1.z;
							z3 = f2Edges[m].v1.x;
							x4 = f2Edges[m].v2.y;
							y4 = f2Edges[m].v2.z;
							z4 = f2Edges[m].v2.x;
							a=x2-x1, b=x3-x4, c=x3-x1, d=y2-y1, e=y3-y4, f=y3-y1;
							den=a*e-b*d, num1=a*f-c*d, num2=c*e-b*f;
						}
						if(!compareFloats(den,0.0)){
							s=num1/den;t=num2/den;
						}
						else
							continue;
						if((s==0||s==1)&&(z1+t*(z2-z1)==z3+s*(z4-z3)))
							count1++;
						if(s<1&&s>0&&(z1+t*(z2-z1)==z3+s*(z4-z3)))
							count++;
						if(count==2||count1==3)
							break;
					}
				}
				else
					continue;
				if(count==2||count1==3)
					return false;
			}
		}
	}
	return true;
}

bool faces::checkContradiction(edge a1, edges et,edges ef,edges es){
	unordered_map<coordinatePair,edge,coordinatePairHasher> e3D=this->e.getEdgesCopy();
	unordered_map<coordinatePair,edge,coordinatePairHasher> ete=et.getEdgesCopy();
	unordered_map<coordinatePair,edge,coordinatePairHasher> efe=ef.getEdgesCopy();
	unordered_map<coordinatePair,edge,coordinatePairHasher> ese=es.getEdgesCopy();
	//for view 1
	int view=1;
	if(view==1)
	{
		unordered_map<coordinatePair,edge,coordinatePairHasher> set1;
		unordered_map<coordinatePair,edge,coordinatePairHasher> set2;
		unordered_map<coordinatePair,edge,coordinatePairHasher> set3D;
		edge a2D=edge(a1.v1.x,a1.v1.y,0,a1.v2.x,a1.v2.y,0);
		for(auto it=ete.begin();it!=ete.end();++it)
		{
			if(it->second.hiddenXY) // set2 will have non hidden edges also
			{
				float x3=it->second.v1.x, x4=it->second.v2.x,
				  x1=a2D.v1.x, x2=a2D.v2.x,
				  y3=it->second.v1.y, y4=it->second.v2.y,
				  y1=a2D.v1.y, y2=a2D.v2.y;

				float a=x2-x1, b=x3-x4, c=x3-x1, d=y2-y1, e=y3-y4, f=y3-y1;
				float den=a*e-b*d, num1=a*f-c*d, num2=c*e-b*f;
				if(!(compareFloats(a,0.0)&&compareFloats(d,0.0))&&!(compareFloats(b,0.0)&&compareFloats(e,0.0)))
				{
					if(compareFloats(den,0.0)&&compareFloats(num1,0.0)&&compareFloats(num2,0.0)&&((x3-x1)/(x2-x1)>0&&(x3-x1)/(x2-x1)<1)&&((x4-x1)/(x2-x1)>0&&(x4-x1)/(x2-x1)<1))
					{
						set1.insert(make_pair(coordinatePair(it->first.v1,it->first.v2),it->second));
					}

					else if(!compareFloats(den,0.0))
					{
						set2.insert(make_pair(coordinatePair(it->first.v1,it->first.v2),it->second));
					}
				}

			}
			else{
				set2.insert(make_pair(coordinatePair(it->first.v1,it->first.v2),it->second));
			}
		}
		//breaking
		bool changes=true;
		while(changes)
		{
			changes=false;
			for(auto it1=set1.begin();it1!=set1.end();++it1)
			{
				for(auto it2=set2.begin();it2!=set2.end();++it2)
				{
					float x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,z3,z4;
					x1=it1->second.v1.x; x2=it1->second.v2.x;
					x3=it2->second.v1.x; x4=it2->second.v2.x;
					y1=it1->second.v1.y; y2=it1->second.v2.y;
					y3=it2->second.v1.y; y4=it2->second.v2.y;
					z1=it1->second.v1.z; z2=it1->second.v2.z;
					z3=it2->second.v1.z; z4=it2->second.v2.z;

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
								float znew1=findThirdCoord(a1.v1.x,a1.v1.y,a1.v1.z,a1.v2.x,a1.v2.y,a1.v2.z,xnew,ynew);//, znew2=findThirdCoord(x3,y3,z3,x4,y4,z4,xnew,ynew);
								z1=findThirdCoord(a1.v1.x,a1.v1.y,a1.v1.z,a1.v2.x,a1.v2.y,a1.v2.z,x1,y1);
								z2=findThirdCoord(a1.v1.x,a1.v1.y,a1.v1.z,a1.v2.x,a1.v2.y,a1.v2.z,x2,y2);
									newcoord1.x=xnew;
									newcoord1.y=ynew;
									newcoord1.z=znew1; 

								edge e1=edge(coordinate(x1,y1,z1),newcoord1,true,false,false);
								edge e2=edge(coordinate(x2,y2,z2),newcoord1,true,false,false);
								set1.erase(it1->first);	
								bool b1=set3D.insert(make_pair(coordinatePair(e1),e1)).second;
								bool b2=set3D.insert(make_pair(coordinatePair(e2),e2)).second;
								changes=b1|b2|changes; 
								
								if(changes){
									it1 = set1.begin();
									break;
								}
							}				
						}
					}
				}
			}
		}
		for(auto it=set3D.begin();it!=set3D.end();++it)
		{
			bool b3=rule3(it->second);
			if(!b3) return false;
		}
	}
	
	view=2;
	if(view==2)
	{
		unordered_map<coordinatePair,edge,coordinatePairHasher> set1;
		unordered_map<coordinatePair,edge,coordinatePairHasher> set2;
		unordered_map<coordinatePair,edge,coordinatePairHasher> set3D;
		edge a2D=edge(a1.v1.y,a1.v1.z,0,a1.v2.y,a1.v2.z,0);
		for(auto it=efe.begin();it!=efe.end();++it)
		{
			if(it->second.hiddenXY)
			{
				float x3=it->second.v1.x, x4=it->second.v2.x,
				  x1=a2D.v1.x, x2=a2D.v2.x,
				  y3=it->second.v1.y, y4=it->second.v2.y,
				  y1=a2D.v1.y, y2=a2D.v2.y;

				float a=x2-x1, b=x3-x4, c=x3-x1, d=y2-y1, e=y3-y4, f=y3-y1;
				float den=a*e-b*d, num1=a*f-c*d, num2=c*e-b*f;
				if(!(compareFloats(a,0.0)&&compareFloats(d,0.0))&&!(compareFloats(b,0.0)&&compareFloats(e,0.0)))
				{
					if(compareFloats(den,0.0)&&compareFloats(num1,0.0)&&compareFloats(num2,0.0)&&((x3-x1)/(x2-x1)>0&&(x3-x1)/(x2-x1)<1)&&((x4-x1)/(x2-x1)>0&&(x4-x1)/(x2-x1)<1))
					{
						set1.insert(make_pair(coordinatePair(it->first.v1,it->first.v2),it->second));
					}

					else if(!compareFloats(den,0.0))
					{
						set2.insert(make_pair(coordinatePair(it->first.v1,it->first.v2),it->second));
					}
				}

			}
			else
				set2.insert(make_pair(coordinatePair(it->first.v1,it->first.v2),it->second));
		}
		//breaking
		bool changes=true;
		while(changes)
		{
			changes=false;
			for(auto it1=set1.begin();it1!=set1.end();++it1)
			{
				for(auto it2=set2.begin();it2!=set2.end();++it2)
				{
					float x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,z3,z4;
					x1=it1->second.v1.x; x2=it1->second.v2.x;
					x3=it2->second.v1.x; x4=it2->second.v2.x;
					y1=it1->second.v1.y; y2=it1->second.v2.y;
					y3=it2->second.v1.y; y4=it2->second.v2.y;
					z1=it1->second.v1.z; z2=it1->second.v2.z;
					z3=it2->second.v1.z; z4=it2->second.v2.z;
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
								float znew1=findFirstCoord(a1.v1.x,a1.v1.y,a1.v1.z,a1.v2.x,a1.v2.y,a1.v2.z,xnew,ynew);//, znew2=findThirdCoord(x3,y3,z3,x4,y4,z4,xnew,ynew);
								z1=findFirstCoord(a1.v1.x,a1.v1.y,a1.v1.z,a1.v2.x,a1.v2.y,a1.v2.z,x1,y1);
								z2=findFirstCoord(a1.v1.x,a1.v1.y,a1.v1.z,a1.v2.x,a1.v2.y,a1.v2.z,x2,y2);
								
									newcoord1.y=xnew;
								 	newcoord1.z=ynew;
									newcoord1.x=znew1;
								

								edge e1=edge(coordinate(z1,x1,y1),newcoord1,false,true,false);
								edge e2=edge(coordinate(z2,x2,y2),newcoord1,false,true,false);

								set1.erase(it1->first);		
								bool b1=set3D.insert(make_pair(coordinatePair(e1),e1)).second;
								bool b2=set3D.insert(make_pair(coordinatePair(e2),e2)).second;
								changes=b1|b2|changes; 	
								
								if(changes){
									it1 = set1.begin();
									break;
								}
							}				
						}
					}
				}
			}
		}
		
		for(auto it=set3D.begin();it!=set3D.end();++it)
		{
			bool b3=rule3(it->second);
			if(!b3) return false;
		}
	}

	view=3;
	if(view==3)
	{
		unordered_map<coordinatePair,edge,coordinatePairHasher> set1;
		unordered_map<coordinatePair,edge,coordinatePairHasher> set2;
		unordered_map<coordinatePair,edge,coordinatePairHasher> set3D;
		edge a2D=edge(a1.v1.z,a1.v1.x,0,a1.v2.z,a1.v2.x,0);
		for(auto it=efe.begin();it!=efe.end();++it)
		{
			if(it->second.hiddenXY)
			{
				float x3=it->second.v1.x, x4=it->second.v2.x,
				  x1=a2D.v1.x, x2=a2D.v2.x,
				  y3=it->second.v1.y, y4=it->second.v2.y,
				  y1=a2D.v1.y, y2=a2D.v2.y;

				float a=x2-x1, b=x3-x4, c=x3-x1, d=y2-y1, e=y3-y4, f=y3-y1;
				float den=a*e-b*d, num1=a*f-c*d, num2=c*e-b*f;
				if(!(compareFloats(a,0.0)&&compareFloats(d,0.0))&&!(compareFloats(b,0.0)&&compareFloats(e,0.0)))
				{
					if(compareFloats(den,0.0)&&compareFloats(num1,0.0)&&compareFloats(num2,0.0)&&((x3-x1)/(x2-x1)>0&&(x3-x1)/(x2-x1)<1)&&((x4-x1)/(x2-x1)>0&&(x4-x1)/(x2-x1)<1))
					{
						set1.insert(make_pair(coordinatePair(it->first.v1,it->first.v2),it->second));
					}

					else if(!compareFloats(den,0.0))
					{
						set2.insert(make_pair(coordinatePair(it->first.v1,it->first.v2),it->second));
					}
				}

			}
			else
				set2.insert(make_pair(coordinatePair(it->first.v1,it->first.v2),it->second));
		}
		//breaking
		bool changes=true;
		while(changes)
		{
			changes=false;
			for(auto it1=set1.begin();it1!=set1.end();++it1)
			{
				for(auto it2=set2.begin();it2!=set2.end();++it2)
				{
					float x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,z3,z4;
					x1=it1->second.v1.x; x2=it1->second.v2.x;
					x3=it2->second.v1.x; x4=it2->second.v2.x;
					y1=it1->second.v1.y; y2=it1->second.v2.y;
					y3=it2->second.v1.y; y4=it2->second.v2.y;
					z1=it1->second.v1.z; z2=it1->second.v2.z;
					z3=it2->second.v1.z; z4=it2->second.v2.z;

					
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
								float znew1=findSecondCoord(a1.v1.x,a1.v1.y,a1.v1.z,a1.v2.x,a1.v2.y,a1.v2.z,xnew,ynew);//, znew2=findThirdCoord(x3,y3,z3,x4,y4,z4,xnew,ynew);
								z1=findSecondCoord(a1.v1.x,a1.v1.y,a1.v1.z,a1.v2.x,a1.v2.y,a1.v2.z,x1,y1);
								z2=findSecondCoord(a1.v1.x,a1.v1.y,a1.v1.z,a1.v2.x,a1.v2.y,a1.v2.z,x2,y2);
									newcoord1.z=xnew;
									newcoord1.x=ynew;
									newcoord1.y=znew1; 

								edge e1=edge(coordinate(y1,z1,x1),newcoord1,false,false,true);
								edge e2=edge(coordinate(y2,z2,x2),newcoord1,false,false,true);

								set1.erase(it1->first);	
								
								bool b1=set3D.insert(make_pair(coordinatePair(e1),e1)).second;
								bool b2=set3D.insert(make_pair(coordinatePair(e2),e2)).second;
								changes=b1|b2|changes; 
								
								if(changes){
									it1 = set1.begin();
									break;
								}
							}				
						}
					}
				}
			}
		}
		//checking		
		for(auto it=set3D.begin();it!=set3D.end();++it)
		{
			bool b3=rule3(it->second);
			if(!b3) return false;
		}
	}	
	return true;

}

bool faces::removingEdgeIsOK(edge a1){
	unordered_map<coordinatePair,edge,coordinatePairHasher> e3D=this->e.getEdgesCopy();
	e3D.erase(coordinatePair(a1));
	edges eXY;
	edges eYZ;
	edges eZX;
	edge aXY=edge(a1.v1.x,a1.v1.y,0,a1.v2.x,a1.v2.y,0);
	edge aYZ=edge(a1.v1.y,a1.v1.z,0,a1.v2.y,a1.v2.z,0);
	edge aZX=edge(a1.v1.z,a1.v1.x,0,a1.v2.z,a1.v2.x,0);
	for(auto it=e3D.begin();it!=e3D.end();++it)
	{
		coordinate c1=it->first.v1, c2=it->first.v2;
		eXY.addEdge(c1.x,c1.y,0,c2.x,c2.y,0);
		eYZ.addEdge(c1.y,c1.z,0,c2.y,c2.z,0);
		eZX.addEdge(c1.z,c1.x,0,c2.z,c2.x,0);
	}
	eXY.breakEdges(true,1);
	eYZ.breakEdges(true,2);
	eZX.breakEdges(true,3);
	bool boolXY=false, boolYZ=false, boolZX=false;
	unordered_map<coordinatePair,edge,coordinatePairHasher> &eXYe=eXY.getEdges();
	unordered_map<coordinatePair,edge,coordinatePairHasher> &eYZe=eYZ.getEdges();
	unordered_map<coordinatePair,edge,coordinatePairHasher> &eZXe=eZX.getEdges();
	for(auto it=eXYe.begin();it!=eXYe.end();++it)
	{
		float x1=it->second.v1.x, x2=it->second.v2.x,
			  x3=aXY.v1.x, x4=aXY.v2.x,
			  y1=it->second.v1.y, y2=it->second.v2.y,
			  y3=aXY.v1.y, y4=aXY.v2.y;

		float a=x2-x1, b=x3-x4, c=x3-x1, d=y2-y1, e=y3-y4, f=y3-y1;
		float den=a*e-b*d, num1=a*f-c*d, num2=c*e-b*f;
		if(compareFloats(den,0.0)&&compareFloats(num1,0.0)&&compareFloats(num2,0.0))
		{
			if(!compareFloats(x1,x2))
			{	if((x3-x1)/(x2-x1)>=-0.1&&(x3-x1)/(x2-x1)<=1.1&&(x4-x1)/(x2-x1)>=-0.1&&(x4-x1)/(x2-x1)<=1.1)
				{
					boolXY=true;
					break;
				}
			}

			else
			{	if((y3-y1)/(y2-y1)>=-0.1&&(y3-y1)/(y2-y1)<=1.1&&(y4-y1)/(y2-y1)>=-0.1&&(y4-y1)/(y2-y1)<=1.1)
				{
					boolXY=true;
					break;
				}
			}
			
		}
	}

	for(auto it=eYZe.begin();it!=eYZe.end();++it)
	{
		float x1=it->second.v1.x, x2=it->second.v2.x,
			  x3=aYZ.v1.x, x4=aYZ.v2.x,
			  y1=it->second.v1.y, y2=it->second.v2.y,
			  y3=aYZ.v1.y, y4=aYZ.v2.y;

		float a=x2-x1, b=x3-x4, c=x3-x1, d=y2-y1, e=y3-y4, f=y3-y1;
		float den=a*e-b*d, num1=a*f-c*d, num2=c*e-b*f;
		if(compareFloats(den,0.0)&&compareFloats(num1,0.0)&&compareFloats(num2,0.0))
		{
			if(!compareFloats(x1,x2))
			{	if((x3-x1)/(x2-x1)>=-0.1&&(x3-x1)/(x2-x1)<=1.1&&(x4-x1)/(x2-x1)>=-0.1&&(x4-x1)/(x2-x1)<=1.1)
				{
					boolYZ=true;
					break;
				}
			}

			else
			{	if((y3-y1)/(y2-y1)>=-0.1&&(y3-y1)/(y2-y1)<=1.1&&(y4-y1)/(y2-y1)>=-0.1&&(y4-y1)/(y2-y1)<=1.1)
				{
					boolYZ=true;
					break;
				}
			}
			
		}
	}

	for(auto it=eZXe.begin();it!=eZXe.end();++it)
	{
		float x1=it->second.v1.x, x2=it->second.v2.x,
			  x3=aZX.v1.x, x4=aZX.v2.x,
			  y1=it->second.v1.y, y2=it->second.v2.y,
			  y3=aZX.v1.y, y4=aZX.v2.y;

		float a=x2-x1, b=x3-x4, c=x3-x1, d=y2-y1, e=y3-y4, f=y3-y1;
		float den=a*e-b*d, num1=a*f-c*d, num2=c*e-b*f;
		if(compareFloats(den,0.0)&&compareFloats(num1,0.0)&&compareFloats(num2,0.0))
		{
			if(!compareFloats(x1,x2))
			{	if((x3-x1)/(x2-x1)>=-0.1&&(x3-x1)/(x2-x1)<=1.1&&(x4-x1)/(x2-x1)>=-0.1&&(x4-x1)/(x2-x1)<=1.1)
				{
					boolZX=true;
					break;
				}
			}

			else
			{	if((y3-y1)/(y2-y1)>=-0.1&&(y3-y1)/(y2-y1)<=1.1&&(y4-y1)/(y2-y1)>=-0.1&&(y4-y1)/(y2-y1)<=1.1)
				{
					boolZX=true;
					break;
				}
			}
			
		}
	}
	return (boolXY & boolYZ & boolZX);
}

//this will be called as: faces fcopy=f3D; bool possible=fcopy.func(); //to check if this config of faces is possible
bool faces::func(edges ef, edges es, edges et){
	unordered_map<coordinatePair,edge,coordinatePairHasher> & e3D=this->e.getEdges();
	unordered_map<coordinate,vertex,coordinateHasher> &v3D =this->e.getVertices().getVertices();
	bool changes=true, changes1=true,changes2=true,changes3=true;
	while(changes)
	{
	//step1
		changes = false;
		changes1=true;
		while(changes1)
		{
			changes1=false;		
			for(auto it=f.begin();it != f.end();++it)
			{
				bool wrongedge=false;
				for(auto ita=it->edges.begin();ita !=it->edges.end(); ++ita)
				{
					auto itedge=e3D.find(*ita);
					if(itedge->second.numFaces==1)
					{
						changes1=true;
						wrongedge=true;
						break;
					}
				}
				if(wrongedge)
				{
					for(auto ita=it->edges.begin();ita !=it->edges.end(); ++ita)
					{
						auto itedge=e3D.find(*ita);
						itedge->second.numFaces--;
					}
					f.erase(it);
					break;
				}
			}	
		}
	//step2
		changes2=true;
		while(changes2)
		{
			changes2 = false;
			for(auto it=e3D.begin();it!=e3D.end();++it)
			{
				if(it->second.numFaces==0)
				{	
					if(removingEdgeIsOK(it->second))
					{
						changes2=true;
						auto ita=v3D.find(it->second.v1); auto itb=v3D.find(it->second.v2);
						ita->second.numEdges--;
						itb->second.numEdges--;
						e3D.erase(it);
						break;
					}
					else return false;
				}
			}		
		}
	//step3
		changes3=true;
		while(changes3)
		{
			changes3 = false;
			for(auto it=e3D.begin();it!=e3D.end();++it)
			{
				if(!checkContradiction(it->second,ef,es,et))
				{
					if(removingEdgeIsOK(it->second))
					{
						changes3=true;
						auto ita=v3D.find(it->second.v1); auto itb=v3D.find(it->second.v2);
						ita->second.numEdges--;
						itb->second.numEdges--;
						bool changessub=true;
						while(changessub)
						{
							changessub=false;
							for(auto itf=f.begin();itf != f.end();++itf)
							{
								if(itf->hasEdge(it->first))
								{
									changessub=true;
									for(auto ita=itf->edges.begin();ita !=itf->edges.end(); ++itf)
									{
										auto itedge=e3D.find(*ita);
										itedge->second.numFaces--;
									}
									f.erase(itf);
									break;
								}
							}						
						}					
						e3D.erase(it);
						break;
					}
					else return false;
				}
			}
		}
	}
	//removing vertices
	for(auto it=v3D.begin();it !=v3D.end();++it)
	{
		if(it->second.numEdges==0)
		{
			v3D.erase(it);
		}
	}
	return true;
}

bool faces::rule3(edge a){
	bool do1=true,do2=true,do3=true;
	bool hidxy=false,hidyz=false,hidzx=false;
	if(compareFloats(a.length2D(1),0.0) || !a.hiddenXY) do1=false;
	if(compareFloats(a.length2D(2),0.0) || !a.hiddenYZ) do2=false; 
	if(compareFloats(a.length2D(3),0.0) || !a.hiddenZX) do3=false; 
	if(!do1&&!do2&&!do3) return true;
	coordinate mid=coordinate((a.v1.x+a.v2.x)/2,(a.v1.y+a.v2.y)/2,(a.v1.z+a.v2.z)/2);
	for(int i=0;i<f.size();++i)
	{
		if(do1){
			if(faceAboveEdge(f[i],mid,1)&&checkPointInsidePolygon(f[i],mid,1))
			{
				hidxy=true; do1=false;
			}
		}

		if(do2)
		{	
			if(faceAboveEdge(f[i],mid,2)&&checkPointInsidePolygon(f[i],mid,2))
			{
				hidyz=true; do2=false;
			}
		}

		if(do3)
		{	
			if(faceAboveEdge(f[i],mid,3)&&checkPointInsidePolygon(f[i],mid,3))
			{
				hidzx=true; do3=false;
			}
		}
	}
	if((a.hiddenXY&&!hidxy)||(a.hiddenYZ&&!hidyz)||(a.hiddenZX&&!hidzx))	return false;
	return true;
}


bool faces::rule7(){
	unordered_map<coordinatePair,edge,coordinatePairHasher> & e3D=this->e.getEdges();
	bool changes=true; bool edgefound=false;
	while(changes)
	{
		changes = false;
		for(auto it=e3D.begin();it!=e3D.end();++it)
		{
			edge a=it->second;
			if(a.numFaces==2)
				for(auto ita=f.begin();ita!=f.end();++ita)
				{
					for(auto itb=f.begin();itb!=f.end();++itb)
					{
						edgefound=false;
						if(ita!=itb&&ita->hasEdge(a)&&itb->hasEdge(a)&&ita->corrPlane.isCoplanar(itb->corrPlane))
						{
							if(removingEdgeIsOK(a))
							{
								edgefound=true;
								changes=true;
								ita->combineCoplanarFaces(*itb,a);
								f.erase(itb);
								e.removeEdge(a);
								break;
							}
							else{ 
								return false;
							}
						}
					}
					if(edgefound) break;
				}
			if(edgefound) break;
		}
	}
	return true;
}
/**
* \brief Removes Pseudo faces
* 
* \param  void
* \return void
*
* It removes all the Pseudo faces using Decision Chaining algorithm
*/
// void faces::removePseudoFaces(){

// }

/**
* \brief Corrects orientation of each face
* 
* \param  void
* \return void
*
* It finds the correct orientation of each face of the solid using Moebius Rule
*/


void faces::findContainment(){
	for(auto ita=f.begin();ita!=f.end();++ita)
	{
		for(auto itb=f.begin();itb!=f.end();++itb)
		{
			if(ita!=itb && ita->corrPlane.isCoplanar(itb->corrPlane))
			{
				coordinatePair a=itb->edges[0];
				coordinate mid=coordinate((a.v1.x+a.v2.x)/2,(a.v1.y+a.v2.y)/2,(a.v1.z+a.v2.z)/2);
				if((ita->corrPlane.c!=0 && checkPointInsidePolygon(*ita,mid,1))||(ita->corrPlane.a!=0 && checkPointInsidePolygon(*ita,mid,2))||(ita->corrPlane.b!=0 && checkPointInsidePolygon(*ita,mid,3)))
				{
					//itb contained inside ita
					ita->edges.push_back(coordinatePair(-1,-1,-1,-1,-1,-1));
					ita->edges.insert(ita->edges.end(),itb->edges.begin(),itb->edges.end());
					ita->vertices.push_back(coordinate(-1,-1,-1));
					ita->vertices.insert(ita->vertices.end(),itb->vertices.begin(),itb->vertices.end());
					f.erase(itb);
					ita = f.begin()-1;
					break;
				}
			}
		}
	}
}

/**
* \brief Returns the list of edges
* 
* \param  void
* \return Reference to the list of edges 
*
*/
edges & faces::getEdges(){
	return this->e;
}

/**
* \brief Returns the list of faces
* 
* \param  void
* \return Reference to the list of faces f 
*
*/
vector<face> & faces::getFaces(){
	return this->f;
}



void faces::printFaces(){
	for(int i=0;i<f.size();++i)
	{
		f[i].printFace();
	}
}

void faces::printEverything(){
	printFaces();
	e.printEdges();
	e.getVertices().printVertices();
}

void faces::correctFaceOrientation(){
	unordered_map<coordinate,vertex,coordinateHasher> &v3D =this->e.getVertices().getVertices();
	coordinate meanvertex(0,0,0);
	vertex leftVertex=v3D.begin()->second;
	for(auto it=v3D.begin();it!=v3D.end();++it)
	{
		if(it->first.operator<(leftVertex.v))
		{
			leftVertex=it->second;
		}
	}
	for(int i=0;i<leftVertex.adjacentCoordinates.size();++i)
	{
		meanvertex=meanvertex.add(leftVertex.adjacentCoordinates[i]);
	}
	meanvertex.x/=leftVertex.adjacentCoordinates.size();
	meanvertex.y/=leftVertex.adjacentCoordinates.size();
	meanvertex.z/=leftVertex.adjacentCoordinates.size();
	//find normals for all planes
	for(int i=0;i<f.size();++i)
	{
		f[i].findNormal();
		f[i].corrected=false;
	}
	int ireq;
	for(int i=0;i<f.size();++i)
	{
		if(f[i].hasEdge(coordinatePair(leftVertex.v,leftVertex.adjacentCoordinates[0]))&&(positionOfDummy(i)==f[i].vertices.size()))
		{
			ireq=i;break;
		}
	}
	coordinate temp=leftVertex.v.add(coordinate(f[ireq].normal[0],f[ireq].normal[1],f[ireq].normal[2]));
	//isse pehle vertex ke vextor ko corrPlane(normal) ke hisaab se arrange karna padega. iske baad ye dekhke ki normal yahi hooni chahiye ya opposite,vector ko same ya reverse krna padega
	if(((f[ireq].corrPlane.a*temp.x+f[ireq].corrPlane.b*temp.y+f[ireq].corrPlane.c*temp.z-f[ireq].corrPlane.d)*(f[ireq].corrPlane.a*meanvertex.x+f[ireq].corrPlane.b*meanvertex.y+f[ireq].corrPlane.c*meanvertex.z-f[ireq].corrPlane.d))>0.001)
	{
		std::reverse(f[ireq].vertices.begin(),f[ireq].vertices.end());
		f[ireq].normal=-f[ireq].normal;
	}
	//BFS
	list<int> queue;
	int hollowFace=-1;int icur;

	f[ireq].corrected=true;
	queue.push_back(ireq);

	while(!queue.empty())
	{
		icur=queue.front();
		queue.pop_front();

		int posOfDummy=positionOfDummy(icur);
		if(posOfDummy<f[icur].vertices.size()) hollowFace=icur;
		for(int i=0;i<posOfDummy-1;++i)
		{
			coordinate v1=f[icur].vertices[i], v2=f[icur].vertices[i+1];
			for(int j=0;j<f.size();++j)
			{
				if(f[j].hasEdge(coordinatePair(v1,v2))&&f[j].corrected==false)
				{
					int posofv1=positionOfVertex(j,v1), posofv2=positionOfVertex(j,v2);
					if(positionLess(j,posofv1,posofv2))
					{
						reverseVertices(j);
						f[j].normal=-f[j].normal;
						f[j].corrected=true;
						queue.push_back(j);
						break;
					}
					else
					{
						f[j].corrected=true;
						queue.push_back(j);
						break;
					}
				}
			}
		}
		//do what u r doing in above loop to edge formed by posOFFummy and 0
		coordinate v1=f[icur].vertices[posOfDummy-1], v2=f[icur].vertices[0];
		for(int j=0;j<f.size();++j)
		{
			if(f[j].hasEdge(coordinatePair(v1,v2))&&f[j].corrected==false)
			{
				int posofv1=positionOfVertex(j,v1), posofv2=positionOfVertex(j,v2);
				if(positionLess(j,posofv1,posofv2))
				{
					reverseVertices(j);
					f[j].normal=-f[j].normal;
					f[j].corrected=true;
					queue.push_back(j);
					break;
				}
				else
				{
					f[j].corrected=true;
					queue.push_back(j);
					break;
				}
			}
		}		
	}
	for(int i=0;i<f.size();++i)
	{
		if(positionOfDummy(i)<f[i].vertices.size())	f[i].corrected=false;
	}
	
	if(hollowFace==-1)	return;
	icur=hollowFace;
	int posOfDummy=positionOfDummy(hollowFace);
	vector<coordinate> inside;
	inside.insert(inside.end(),f[hollowFace].vertices.begin()+posOfDummy+1,f[hollowFace].vertices.end());
	face fnew;
	fnew.vertices=inside;
	fnew.findNormal();
	if(fnew.normal==f[hollowFace].normal)
	{
		std::reverse(fnew.vertices.begin(),fnew.vertices.end());
	}
	f[hollowFace].vertices.erase(f[hollowFace].vertices.begin()+posOfDummy+1,f[hollowFace].vertices.end()); 
	f[hollowFace].vertices.insert(f[hollowFace].vertices.end(),fnew.vertices.begin(),fnew.vertices.end()); 

	f[icur].corrected=true;
	queue.push_back(icur);
	while(!queue.empty())
	{
		icur=queue.front();
		queue.pop_front();

		posOfDummy=positionOfDummy(icur);
		if(posOfDummy<f[icur].vertices.size())
		{
			for(int i=posOfDummy+1;i<f[icur].vertices.size()-1;++i)
			{
				coordinate v1=f[icur].vertices[i], v2=f[icur].vertices[i+1];
				for(int j=0;j<f.size();++j)
				{
					if(f[j].hasEdge(coordinatePair(v1,v2))&&f[j].corrected==false)
					{
						int posofv1=positionOfVertex(j,v1), posofv2=positionOfVertex(j,v2);
						if(positionLess1(j,posofv1,posofv2))
						{
							reverseVertices1(j);
							if(positionOfDummy(j)==f[j].vertices.size())	f[j].normal=-f[j].normal;							
							f[j].corrected=true;
							queue.push_back(j);
							break;
						}
						else
						{
							f[j].corrected=true;
							queue.push_back(j);
							break;
						}
					}
				}
			}
			
			coordinate v1=f[icur].vertices[f[icur].vertices.size()-1], v2=f[icur].vertices[posOfDummy+1];
			for(int j=0;j<f.size();++j)
			{
				if(f[j].hasEdge(coordinatePair(v1,v2))&&f[j].corrected==false)
				{
					int posofv1=positionOfVertex(j,v1), posofv2=positionOfVertex(j,v2);
					if(positionLess1(j,posofv1,posofv2))
					{
						reverseVertices1(j);
						if(positionOfDummy(j)==f[j].vertices.size())	f[j].normal=-f[j].normal;							
						f[j].corrected=true;
						queue.push_back(j);
						break;
					}
					else
					{
						f[j].corrected=true;
						queue.push_back(j);
						break;
					}
				}
			}
		}

		else
		{
			for(int i=0;i<posOfDummy-1;++i)
			{
				coordinate v1=f[icur].vertices[i], v2=f[icur].vertices[i+1];
				for(int j=0;j<f.size();++j)
				{
					if(f[j].hasEdge(coordinatePair(v1,v2))&&f[j].corrected==false)
					{
						int posofv1=positionOfVertex(j,v1), posofv2=positionOfVertex(j,v2);
						if(positionLess1(j,posofv1,posofv2))
						{
							reverseVertices1(j);
							if(positionOfDummy(j)==f[j].vertices.size())	f[j].normal=-f[j].normal;							
							f[j].corrected=true;
							queue.push_back(j);
							break;
						}
						else
						{
							f[j].corrected=true;
							queue.push_back(j);
							break;
						}
					}
				}
			}
			
			coordinate v1=f[icur].vertices[posOfDummy-1], v2=f[icur].vertices[0];
			for(int j=0;j<f.size();++j)
			{
				if(f[j].hasEdge(coordinatePair(v1,v2))&&f[j].corrected==false)
				{
					int posofv1=positionOfVertex(j,v1), posofv2=positionOfVertex(j,v2);
					if(positionLess1(j,posofv1,posofv2))
					{
						reverseVertices1(j);
						if(positionOfDummy(j)==f[j].vertices.size())	f[j].normal=-f[j].normal;							
						f[j].corrected=true;
						queue.push_back(j);
						break;
					}
					else
					{
						f[j].corrected=true;
						queue.push_back(j);
						break;
					}
				}
			}
		}
	}
}

int faces::positionOfDummy(int j){
	for(int i=0;i<f[j].vertices.size();++i)
	{
		if(f[j].vertices[i].operator==(coordinate(-1,-1,-1)))	return i;
	}
	return f[j].vertices.size();
}

int faces::positionOfVertex(int j,coordinate v){
	for(int i=0;i<f[j].vertices.size();++i)
	{
		if(f[j].vertices[i].operator==(v)) return i;
	}
	return f[j].vertices.size();
}

bool faces::positionLess(int j,int pos1,int pos2){
	int posOfDummy=positionOfDummy(j);
	if(pos1==posOfDummy-1&&pos2==0)	return true;
	if(pos2==posOfDummy-1&&pos1==0)	return false;
	if(pos1<pos2)	return true;
	else	return false;	
}

bool faces::positionLess1(int j,int pos1,int pos2){
	int posOfDummy=positionOfDummy(j);
	if(posOfDummy==f[j].vertices.size())
	{
		if(pos1==posOfDummy-1&&pos2==0)	return true;
		if(pos2==posOfDummy-1&&pos1==0)	return false;
		if(pos1<pos2)	return true;
		else	return false;
	}

	else
	{
		if(pos1==f[j].vertices.size()-1 && pos2==posOfDummy+1)	return true;
		if(pos2==f[j].vertices.size()-1 && pos1==posOfDummy+1)	return false;
		if(pos1<pos2)	return true;
		else	return false;
	}

}

void faces::reverseVertices(int j){
	int posOfDummy=positionOfDummy(j);
	if(posOfDummy==f[j].vertices.size())
	{
		std::reverse(f[j].vertices.begin(),f[j].vertices.end());
	}

	else
	{
		vector<coordinate> temp;
		temp.insert(temp.end(),f[j].vertices.begin(),f[j].vertices.begin()+posOfDummy);
		std::reverse(temp.begin(),temp.end());
		temp.insert(temp.end(),f[j].vertices.begin()+posOfDummy,f[j].vertices.end());
		f[j].vertices=temp;
	}
}

void faces::reverseVertices1(int j){
	int posOfDummy=positionOfDummy(j);
	if(posOfDummy==f[j].vertices.size())
	{
		std::reverse(f[j].vertices.begin(),f[j].vertices.end());
	}

	else
	{
		vector<coordinate> temp;
		temp.insert(temp.end(),f[j].vertices.begin()+posOfDummy+1,f[j].vertices.end());
		std::reverse(temp.begin(),temp.end());
		temp.insert(temp.begin(),f[j].vertices.begin(),f[j].vertices.begin()+posOfDummy+1);//f[j].vertices.end());
		f[j].vertices=temp;
	}	
}