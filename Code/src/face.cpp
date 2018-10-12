#include "objects.h"

face::face(){
	corrected=false;
}

face::face(vector<coordinatePair> e){
	for(int i=0;i<e.size();++i)
	{
		edges.push_back(e[i]);
	}
	corrected=false;
	findCorrPlane();
}

void face::addEdge(coordinatePair e){
	edges.push_back(e);
}

void face::addEdge(coordinate c1,coordinate c2){
	edges.push_back(coordinatePair(c1,c2));
}

void face::addEdge(float x1,float y1,float z1,float x2, float y2, float z2){
	edges.push_back(coordinatePair(x1,y1,z1,x2,y2,z2));
}

void face::addEdge(vector<coordinatePair> e){
	for(int i=0;i<e.size();++i)
	{
		edges.push_back(e[i]);
	}
	findCorrPlane();
}

void face::findCorrPlane(){
	float x1=edges[0].v1.x, y1=edges[0].v1.y, z1=edges[0].v1.z, 
		  x2=edges[0].v2.x, y2=edges[0].v2.y, z2=edges[0].v2.z,
		  x3=edges[1].v1.x, y3=edges[1].v1.y, z3=edges[1].v1.z, 
		  x4=edges[1].v2.x, y4=edges[1].v2.y, z4=edges[1].v2.z; 
	float a0=x2-x1,a1=y2-y1,a2=z2-z1,b0=x4-x3,b1=y4-y3,b2=z4-z3;
	float p1=a1*b2-a2*b1, p2=-a0*b2+a2*b0, p3=a0*b1-a1*b0;
	this->corrPlane.a=p1;
	this->corrPlane.b=p2;
	this->corrPlane.c=p3;
	this->corrPlane.d=p1*x1+p2*y1+p3*z1;
}

bool face::hasEdge(edge a){
	coordinatePair e(a);
	for(int i=0;i<edges.size();++i)
	{
		if(edges[i].operator==(e)) return true;
	}
	return false;
}

bool face::hasEdge(coordinatePair e){
	for(int i=0;i<edges.size();++i)
	{
		if(edges[i].operator==(e)) return true;
	}
	return false;
}

plane face:: getCorrPlane(){
	return corrPlane;
}

plane::plane(){
	a=0;b=0;c=0;d=0;
}

plane::plane(float a,float b,float c, float d){
	this->a=a;
	this->b=b;
	this->c=c;
	this->d=d;
}

void face::printFace(){
	for(int i=0;i<edges.size();++i)
	{
		edges[i].printCoordinatePair();
	}
}

bool plane::isCoplanar(plane p2){
	bool a0=false,b0=false,c0=false,d0=false;
	if(compareFloats(a,0.0)) 	a0=true;
	if(compareFloats(b,0.0)) 	b0=true;
	if(compareFloats(c,0.0))	c0=true;
	if(compareFloats(d,0.0))	d0=true;
	plane temp;
	
	if(!a0)
	{
		temp.a=p2.a;
		temp.b=b*(p2.a/a);
		temp.c=c*(p2.a/a);
		temp.d=d*(p2.a/a);
		if(compareFloats(temp.a,p2.a)&&compareFloats(temp.b,p2.b)&&compareFloats(temp.c,p2.c)&&compareFloats(temp.d,p2.d))	return true;
		return false;
	}

	if(!b0)
	{
		temp.b=p2.b;
		temp.a=a*(p2.b/b);
		temp.c=c*(p2.b/b);
		temp.d=d*(p2.b/b);
		if(compareFloats(temp.a,p2.a)&&compareFloats(temp.b,p2.b)&&compareFloats(temp.c,p2.c)&&compareFloats(temp.d,p2.d))	return true;
		return false;
	}

	if(!c0)
	{
		temp.c=p2.c;
		temp.b=b*(p2.c/c);
		temp.a=a*(p2.c/c);
		temp.d=d*(p2.c/c);
		if(compareFloats(temp.a,p2.a)&&compareFloats(temp.b,p2.b)&&compareFloats(temp.c,p2.c)&&compareFloats(temp.d,p2.d))	return true;
		return false;
	}

	if(!d0)
	{
		temp.d=p2.d;
		temp.a=a*(p2.d/d);
		temp.c=c*(p2.d/d);
		temp.b=b*(p2.d/d);
		if(compareFloats(temp.a,p2.a)&&compareFloats(temp.b,p2.b)&&compareFloats(temp.c,p2.c)&&compareFloats(temp.d,p2.d))	return true;
		return false;
	}
	return false;
}

bool plane::isParallel(plane p2){
	bool a0=false,b0=false,c0=false;
	if(compareFloats(a,0.0)) 	a0=true;
	if(compareFloats(b,0.0)) 	b0=true;
	if(compareFloats(c,0.0))	c0=true;
	plane temp;
	
	if(!a0)
	{
		temp.a=p2.a;
		temp.b=b*(p2.a/a);
		temp.c=c*(p2.a/a);
		if(compareFloats(temp.a,p2.a)&&compareFloats(temp.b,p2.b)&&compareFloats(temp.c,p2.c))	return true;
		return false;
	}

	if(!b0)
	{
		temp.b=p2.b;
		temp.a=a*(p2.b/b);
		temp.c=c*(p2.b/b);
		if(compareFloats(temp.a,p2.a)&&compareFloats(temp.b,p2.b)&&compareFloats(temp.c,p2.c))	return true;
		return false;
	}

	if(!c0)
	{
		temp.c=p2.c;
		temp.b=b*(p2.c/c);
		temp.a=a*(p2.c/c);
		if(compareFloats(temp.a,p2.a)&&compareFloats(temp.b,p2.b)&&compareFloats(temp.c,p2.c))	return true;
		return false;
	}

	
	return false;
}

void face::combineCoplanarFaces(face f2,edge a){
	coordinatePair c(a);
	if(edges.begin()->operator==(c)){
		vector<coordinatePair> tem;
		tem.push_back(edges[edges.size()-1]);
		tem.insert(tem.end(),edges.begin(),edges.end()-1);
		edges = tem;
	}
	else if((edges.end()-1)->operator==(c)){
		coordinatePair tem = edges[0];
		edges.erase(edges.begin());
		edges.push_back(tem);
	}
	if(f2.edges.begin()->operator==(c)){
		vector<coordinatePair> tem;
		tem.push_back(f2.edges[f2.edges.size()-1]);
		tem.insert(tem.end(),f2.edges.begin(),f2.edges.end()-1);
		f2.edges = tem;
	}
	else if((f2.edges.end()-1)->operator==(c)){
		coordinatePair tem = f2.edges[0];
		f2.edges.erase(f2.edges.begin());
		f2.edges.push_back(tem);
	}
	auto ita=this->edges.begin();auto itb=this->edges.begin();auto itbminus=this->edges.begin();auto itbplus=this->edges.begin();auto itaminus=this->edges.begin();auto itaplus=this->edges.begin();
	for(ita=this->edges.begin();ita!=this->edges.end();++ita)
	{
		if(ita->operator==(c))	break;
	}

	for(itb=f2.edges.begin();itb!=f2.edges.end();++itb)
	{
		if(itb->operator==(c))	break;
	}
	vector<coordinatePair> temp;
	itaminus=ita; itaminus--;
	itaplus=ita; itaplus++;
	itbminus=itb; itbminus--;
	itbplus=itb; itbplus++;
	coordinate vreq;
	if(itaminus->v1.operator==(a.v1)||itaminus->v2.operator==(a.v1))
	{
		vreq=a.v1;
	}

	else
	{
		vreq=a.v2;
	}
	temp.insert(temp.end(),this->edges.begin(),ita);
	vector<coordinatePair> temp1,temp2;
	temp1.insert(temp1.end(),f2.edges.begin(),itb);
	temp2.insert(temp2.end(),itbplus,f2.edges.end());
	if(itbplus->v1.operator==(vreq)||itbplus->v2.operator==(vreq))
	{
		temp.insert(temp.end(),temp2.begin(),temp2.end());
		temp.insert(temp.end(),temp1.begin(),temp1.end());
		//this->edges.insert(ita,itbplus,f2.edges.end());
	}

	else
	{
		temp.insert(temp.end(),temp1.rbegin(),temp1.rend());
		temp.insert(temp.end(),temp2.rbegin(),temp2.rend());
		//this->edges.insert(ita,f2.edges.begin(),itb);	
	}
	temp.insert(temp.end(),itaplus,this->edges.end());
	this->edges=temp;
	vertices.clear();
	coordinate c1=edges[0].v1, c2=edges[0].v2; coordinate cv;
	if(edges[1].v1.operator==(c1)||edges[1].v2.operator==(c1))
	{
		vertices.push_back(c2);
		cv=c1;
		vertices.push_back(cv);
	}
	else
	{
		vertices.push_back(c1);
		cv=c2;
		vertices.push_back(cv);
	}

	for(int j=1;j<edges.size()-1;++j)
	{
		coordinate temp=edges[j].v1.add(edges[j].v2);
		cv=temp.subtract(cv);
		vertices.push_back(cv);
	}

}

void face::findNormal(){
	glm::vec3 v1(vertices[1].x-vertices[0].x,vertices[1].y-vertices[0].y,vertices[1].z-vertices[0].z);
	glm::vec3 v2(vertices[2].x-vertices[1].x,vertices[2].y-vertices[1].y,vertices[2].z-vertices[1].z);
	normal=glm::normalize(glm::cross(v1,v2));
}	