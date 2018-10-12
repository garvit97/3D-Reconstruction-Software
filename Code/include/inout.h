#ifndef INOUT_H
#define INOUT_H
#include "vertices.h"
#include "faces.h"
#include "edges.h"

bool input(edges&, edges&, edges&, faces&, bool&);
void output(faces,bool);

#endif