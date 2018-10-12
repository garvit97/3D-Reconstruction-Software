#include "faces.h"
#include "edges.h"
#include "vertices.h"
#include "inout.h"
#include "objects.h"
#include <iostream>
#include <bits/stdc++.h>


bool quit = false;
/**
*We will use gtk library for graphic input/ouput.
*/
bool sortFace1(coordinatePair a,coordinatePair b)
{
	if(a.v1.operator <(b.v1))return true;
	if(a.v1.operator ==(b.v1)&&a.v2.operator <(b.v2))return true;
	return false;
}

int main(int argc, char** argv){
	while(true){
		faces f3D;
		edges &e3D=f3D.getEdges();
		faces ff,fs,ft;
		edges &ef = ff.getEdges(),
			  &es = fs.getEdges(),
			  &et = ft.getEdges();
		vertices &vf = ef.getVertices(),
				 &vt = et.getVertices(),
				 &vs = es.getVertices();
		bool mode;
		quit = input(ef,es,et,f3D,mode);
		if(quit)
			break;
		if(mode){
			f3D.correctFaceOrientation();
			vertices &v3D=e3D.getVertices();
			e3D.breakEdges(false,1);
			e3D.breakEdges(false,2);
			e3D.breakEdges(false,3);
			f3D.findNatureOfEdges();
			output(f3D,mode);
		}
		else{
			et.breakEdges(true);
			ef.breakEdges(true);		
			es.breakEdges(true);
			e3D.getProbableVertices(vf,vs,vt);
			e3D.getProbableEdges(ef,es,et);
			e3D.removeRedundantEdges();
			e3D.correctMesh();
			f3D.makeFaces();
			f3D.func(ef,es,et);
			f3D.rule7();
			f3D.removePseudoFaces(ef,es,et);
			f3D.findContainment();
			f3D.correctFaceOrientation();
			output(f3D,mode);
		}
	}
}