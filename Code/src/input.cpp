#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "earcut.h"
GLFWwindow* window;
#include "shader.h"
#include "controls.h"
#include "inout.h"
#include "faces.h"
#include "edges.h"
#include <unistd.h>
#include <gtk/gtk.h>

std::vector<glm::vec3> vertices3d;
std::vector<glm::vec3> normals3d;
std::vector<uint16_t> indices3d;
std::vector<glm::vec3> xy, hiddenxy, yz, hiddenyz, zx, hiddenzx;
std::vector<glm::vec3> vfront, vside, vtop;
glm::vec3 center(0);
float max_value;
string filename;
GtkWidget *parent_window;
GtkWidget *dialog;
extern bool quit;

void open_dialog(){
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
	gint res;
	dialog = gtk_file_chooser_dialog_new ("Open File", GTK_WINDOW(parent_window), action, "_Cancel", GTK_RESPONSE_CANCEL, "_Open", GTK_RESPONSE_ACCEPT, NULL);
	gtk_widget_show_all (dialog);
	res = gtk_dialog_run (GTK_DIALOG (dialog));
	if (res == GTK_RESPONSE_ACCEPT){
	    GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
	    filename = gtk_file_chooser_get_filename (chooser);
	    gtk_widget_destroy (dialog);
	    gtk_widget_destroy (parent_window);
	    return;
  	}
  	else
  		gtk_widget_destroy (dialog);
}
void quit_window(GtkWidget *widget, gpointer mode){
	quit = true;
	gtk_widget_destroy (parent_window);
}
void set_true(GtkWidget *widget, gpointer mode){
	*(bool*)mode = true;
	open_dialog();
}	
void set_false(GtkWidget *widget, gpointer mode){
	*(bool*)mode = false;
	open_dialog();
}
void activate(GtkApplication *app, gpointer data){
	GtkWidget *button, *grid;
	parent_window = gtk_application_window_new (app);
  	gtk_window_set_title (GTK_WINDOW (parent_window), "Mode");
  	gtk_window_set_position(GTK_WINDOW(parent_window), GTK_WIN_POS_CENTER);
  	gtk_container_set_border_width (GTK_CONTAINER (parent_window), 50);

  	grid = gtk_grid_new ();
  	gtk_container_add (GTK_CONTAINER (parent_window), grid);

  	button = gtk_button_new_with_label ("3D to 2D");
 	g_signal_connect (button, "clicked", G_CALLBACK (set_true),data);
 	gtk_grid_attach (GTK_GRID (grid), button, 0, 0, 1, 1);

 	button = gtk_button_new_with_label ("2D to 3D");
 	g_signal_connect (button, "clicked", G_CALLBACK (set_false),data);
 	gtk_grid_attach (GTK_GRID (grid), button, 1, 0, 1, 1);

 	button = gtk_button_new_with_label ("Quit");
  	g_signal_connect(button, "clicked", G_CALLBACK (quit_window),NULL);
  	gtk_grid_attach (GTK_GRID (grid), button, 0, 1, 2, 1);

  	g_signal_connect (parent_window, "delete_event", G_CALLBACK (quit_window), NULL);

  	gtk_widget_show_all (parent_window);
  	
}
void getinput(bool &mode){
	gpointer data = &mode;
	GtkApplication *app;
 	app = gtk_application_new ("COP290.assignment.one", G_APPLICATION_FLAGS_NONE);
 	g_signal_connect (app, "activate", G_CALLBACK (activate), data);
  	g_application_run (G_APPLICATION (app),0,NULL);
 	g_object_unref (app);
}

bool display3d(std::vector<glm::vec3> vertices, std::vector<uint16_t> indices, std::vector<glm::vec3> normals){
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return false;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "3D Model", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return false;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited movement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

   	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "StandardShading.vertexshader", "StandardShading.fragmentshader" );

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

	// Generate a buffer for the indices as well
	GLuint elementbuffer;
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int16_t), &indices[0] , GL_STATIC_DRAW);

	// Get a handle for our "LightPosition" uniform
	glUseProgram(programID);
	GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

	//Define vertex attributes
	GLuint vertexPosID = glGetAttribLocation(programID, "vertexPosition_modelspace");
	GLuint vertexNormalID = glGetAttribLocation(programID, "vertexNormal_modelspace");

	// Set background colour
	glClearColor(0.0f, 0.0f, 0.4f, 1.0f);
	do{
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		usleep(100);
		// Use our shader
		glUseProgram(programID);
		if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
			// Cleanup VBO and shader
			glDeleteBuffers(1, &vertexbuffer);
			glDeleteBuffers(1, &normalbuffer);
			glDeleteBuffers(1, &elementbuffer);
			glDeleteProgram(programID);
			// Close OpenGL window and terminate GLFW
			glfwTerminate();
			return true;
		}
		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = getModelMatrix();
		glm::mat4 MVP = ProjectionMatrix*ViewMatrix * ModelMatrix;

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

		glm::vec3 lightPos = glm::vec3(0,0,25);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

		// 1st attribute buffer : vertices
		glEnableVertexAttribArray(vertexPosID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			vertexPosID,        // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2rd attribute buffer : normals
		glEnableVertexAttribArray(vertexNormalID);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(
			vertexNormalID,                   // attribute
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

		// Draw the triangles !
		glDrawElements(
			GL_TRIANGLES,      // mode
			indices.size(),    // count
			GL_UNSIGNED_SHORT,   // type
			(void*)0           // element array buffer offset
		);

		glDisableVertexAttribArray(vertexPosID);
		glDisableVertexAttribArray(vertexNormalID);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteBuffers(1, &elementbuffer);
	glDeleteProgram(programID);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	// Close OpenGL window and terminate GLFW
	glfwTerminate();
	return false;
}
bool display2d(std::vector<glm::vec3> vertices, int len, string str){

	char s[20];
	strcpy(s,str.c_str());
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return false;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 768, 768, s, NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return false;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "StandardShading2d.vertexshader", "StandardShading2d.fragmentshader" );
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	glUseProgram(programID);

	//Define vertex attributes
	GLuint vertexPosID = glGetAttribLocation(programID, "vertexPos");

	// Set background colour
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glDisable(GL_LINE_STIPPLE);
	glLineStipple(1,255);
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(2);
	glm::mat4 ModelMatrix = glm::translate(glm::mat4(),glm::vec3(-0.5,-0.5,0));
	do{
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);
		usleep(100);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		// Use our shader
		glUseProgram(programID);
		if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
			// Cleanup VBO and shader
			glDeleteBuffers(1, &vertexbuffer);
			glDeleteProgram(programID);
			// Close OpenGL window and terminate GLFW
			glfwTerminate();
			return true;
		}

		glEnableVertexAttribArray(vertexPosID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			vertexPosID,        // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		glEnable(GL_LINE_STIPPLE);
		glDrawArrays(GL_LINES, 0,len);
		glDisable(GL_LINE_STIPPLE);
		glDrawArrays(GL_LINES, len,vertices.size());
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteProgram(programID);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	// Close OpenGL window and terminate GLFW
	glfwTerminate();
	return false;
}
glm::vec3 normal(glm::vec3 a,glm::vec3 b, glm::vec3 c){
	glm::vec3 n1 = a-b;
	glm::vec3 n2 = c-b;
	return glm::cross(n2,n1);
}
void project3d(glm::vec3 n1,glm::vec3 n2, glm::vec3 n3,glm::vec3 v0,glm::vec3 &p){
	p = p - v0;
	p = glm::vec3(std::fabs(glm::dot(n1,p)),std::fabs(glm::dot(n2,p)),std::fabs(glm::dot(n3,p)));
}
void findOrigin(glm::vec4 p1, glm::vec4 p2, glm::vec4 p3, glm::vec3 &v0){
	glm::mat3 A (p1[0],p1[1],p1[2],p2[0],p2[1],p2[2],p3[0],p3[1],p3[2]);
	glm::vec3 d (p1[3],p2[3],p3[3]);
	v0 = glm::transpose(glm::inverse(A))*d;
}
glm::vec2 project(glm::vec3 a,glm::vec3 b, glm::vec3 c,glm::vec3 p){
	glm::vec3 n = normal(a,b,c);
	glm::vec3 x = glm::normalize(c-b);
	glm::vec3 y = glm::cross(n,x);
	glm::vec3 p0 = b;
	return glm::vec2(glm::dot((p-p0),x),glm::dot((p-p0),y));
}
void triangulate(std::vector<uint16_t> &indices,std::vector<glm::vec3> &normals,std::vector<float> query,std::vector<glm::vec3> vertices){
	using Point = std::array<float, 2>;
	std::vector<uint16_t> f_vertices;
	std::vector<std::vector<Point>> polygon;
	std::vector<Point> f;
	for(int i=1;i<query.size();++i){
		if(query[i]==-1){
			polygon.push_back(f);
			f = std::vector<Point>();
		}
		else{
			f_vertices.push_back(query[i]-1);
			glm::vec2 v = project(vertices[query[1]-1],vertices[query[2]-1],vertices[query[3]-1],vertices[query[i]-1]);
			f.push_back(Point{{v[0],v[1]}});
		}	
	}
	polygon.push_back(f);
	std::vector<uint16_t> index = mapbox::earcut<uint16_t>(polygon);

	for(int i=0;i<index.size();++i){
		int p = (i+3)/3-1;
		p = p*3;
		normals[f_vertices[index[i]]] = normals[f_vertices[index[i]]] + normal(vertices[f_vertices[index[p]]],vertices[f_vertices[index[p+1]]],vertices[f_vertices[index[p+2]]]);
		indices.push_back(f_vertices[index[i]]);
	}
}

std::vector<float> parseline(std::string query){
	std::vector<float> out;
	std::smatch match;
	std::regex ver3("^\\s*[vV]\\s*(\\-?\\d+\\.?\\d*)\\s+(\\-?\\d+\\.?\\d*)\\s+(\\-?\\d+\\.?\\d*)\\s*$"),
			ver2("^\\s*[vV]([fst])\\s*(\\-?\\d+\\.?\\d*)\\s+(\\-?\\d+\\.?\\d*)\\s*$"),
		  ed("^\\s*[eE]([fst])([hs])\\s*([1-9]\\d*)\\s+([1-9]\\d*)\\s*$"),
		  fa("^\\s*[fF]\\s*([1-9])\\d*\\s+[1-9]\\d*\\s+[1-9]\\d*"),
		  num("^([1-9]\\d*)"),
		  hole("^\\:\\s*[1-9]\\d*\\s+[1-9]\\d*\\s+[1-9]\\d*"),
		  pl("^\\s*[p|P]\\s*(\\-?\\d+\\.?\\d*)\\s+(\\-?\\d+\\.?\\d*)\\s+(\\-?\\d+\\.?\\d*)\\s+(\\-?\\d+\\.?\\d*)\\s*$");
	if(std::regex_search(query,match,ver3)){
		out.push_back(0);
		out.push_back(std::stof(match.str(1)));
		out.push_back(std::stof(match.str(2)));
		out.push_back(std::stof(match.str(3)));	
	}
	else if(std::regex_search(query,match,ver2)){
		out.push_back(3);
		if(match.str(1)=="f")
			out.push_back(1);
		else if(match.str(1)=="s")
			out.push_back(2);
		else
			out.push_back(3);
		out.push_back(std::stof(match.str(2)));
		out.push_back(std::stof(match.str(3)));
	}
	else if(std::regex_search(query,match,pl)){
		out.push_back(4);
		out.push_back(std::stof(match.str(1)));
		out.push_back(std::stof(match.str(2)));
		out.push_back(std::stof(match.str(3)));	
		out.push_back(std::stof(match.str(4)));	
	}
	else if(std::regex_search(query,match,ed)){
		out.push_back(1);
		if(match.str(1)=="f")
			out.push_back(1);
		else if(match.str(1)=="s")
			out.push_back(2);
		else
			out.push_back(3);
		if(match.str(2)=="h")
			out.push_back(1);
		else
			out.push_back(2);
		out.push_back(std::stof(match.str(3)));
		out.push_back(std::stof(match.str(4)));
	}
	else if(std::regex_search(query,match,fa)){
		out.push_back(2);
		int pos = match.position(1);
		while(true){
			std::string substring = query.substr(pos);
			if(std::regex_search(substring,match,num)){
				out.push_back(std::stof(match.str(1)));
				pos += match.str(1).length();
				while(query[pos]==' ')pos++;
			}
			else if(std::regex_search(substring,match,hole)){
				out.push_back(-1);
				pos++;
				while(query[pos]==' ')pos++;
			}
			else 
				break;
		}
	}
	return out;
}

bool input(edges &ef,edges &es, edges &et,faces &f,bool &mode){
	//initialize all objects
	quit = false;
	vertices3d.clear();
	normals3d.clear();
	indices3d.clear();
	xy.clear();
	hiddenxy.clear();
	yz.clear();
	hiddenyz.clear();
	zx.clear();
	hiddenzx.clear();
	vfront.clear();
	vside.clear();
	vtop.clear();

	center = glm::vec3(0);
	getinput(mode);
	if(quit)
		return true;
	vertices &vf = ef.getVertices();
	vertices &vt = et.getVertices();
	vertices &vs = es.getVertices();
	edges &e3d = f.getEdges();
	vertices &v3d = e3d.getVertices();

	std::string line;
	std::ifstream file(filename);

	std::vector<glm::vec4> planes;
	glm::vec3 n1(1,0,0);
	glm::vec3 n2(0,1,0);
	glm::vec3 n3(0,0,1);
	glm::vec3 v0(0,0,0);
	max_value = 0;
	if(file.is_open()){
		while(std::getline(file,line)){
			std::vector<float> query = parseline(line);
			if(mode == true){
				if(query[0] == 0){
					glm::vec3 ver(query[1],query[2],query[3]);
					vertices3d.push_back(ver);
					project3d(n1,n2,n3,v0,ver);
					coordinate cor(ver[0],ver[1],ver[2]);
					v3d.addVertex(cor);
				}
				else if(query[0] == 2){
					int face_start = query[1]-1;
					std::vector<coordinatePair> face_edges;
					std::vector<coordinate> faceVertices;
					for(int i=1;i<query.size();++i){
						if(query[i]==-1)
							faceVertices.push_back(coordinate(-1,-1,-1));
						else{
							glm::vec3 v1(vertices3d[query[i]-1][0],vertices3d[query[i]-1][1],vertices3d[query[i]-1][2]);
							project3d(n1,n2,n3,v0,v1);
							coordinate cVer(v1[0],v1[1],v1[2]);
							faceVertices.push_back(cVer);
						}
					}
					for(int i=1;i<query.size();++i){
						if(i == query.size()-1 || query[i+1]==-1){
							glm::vec3 v1(vertices3d[query[i]-1][0],vertices3d[query[i]-1][1],vertices3d[query[i]-1][2]);
							glm::vec3 v2(vertices3d[face_start][0],vertices3d[face_start][1],vertices3d[face_start][2]);
							project3d(n1,n2,n3,v0,v1);
							project3d(n1,n2,n3,v0,v2);
							coordinate c1(v1[0],v1[1],v1[2]);
							coordinate c2(v2[0],v2[1],v2[2]);
							if(i<query.size()-1)
								face_start = query[i+2]-1;
							if(i != query.size()-1)
								face_edges.push_back(coordinatePair(-1,-1,-1,-1,-1,-1));
							i++;
							if(i == query.size()){
								edge eTemp(c1.x,c1.y,c1.z,c2.x,c2.y,c2.z);
								eTemp.numFaces++;
								e3d.addEdge(eTemp);
								auto ita = v3d.getVertices().find(c1); auto itb=v3d.getVertices().find(c2); 
								ita->second.numEdges++;
								itb->second.numEdges++;
								ita->second.adjacentCoordinates.push_back(c2);
								itb->second.adjacentCoordinates.push_back(c1);
							}
							face_edges.push_back(coordinatePair(c1,c2));
						}
						else{
							glm::vec3 v1(vertices3d[query[i]-1][0],vertices3d[query[i]-1][1],vertices3d[query[i]-1][2]);
							glm::vec3 v2(vertices3d[query[i+1]-1][0],vertices3d[query[i+1]-1][1],vertices3d[query[i+1]-1][2]);
							project3d(n1,n2,n3,v0,v1);
							project3d(n1,n2,n3,v0,v2);
							coordinate c1(v1[0],v1[1],v1[2]);
							coordinate c2(v2[0],v2[1],v2[2]);
							edge eTemp(c1.x,c1.y,c1.z,c2.x,c2.y,c2.z);
							eTemp.numFaces++;
							e3d.addEdge(eTemp);
							auto ita = v3d.getVertices().find(c1); auto itb=v3d.getVertices().find(c2); 
							ita->second.numEdges++;
							itb->second.numEdges++;
							ita->second.adjacentCoordinates.push_back(c2);
							itb->second.adjacentCoordinates.push_back(c1);
							face_edges.push_back(coordinatePair(c1,c2));
						}
					}
					face temp(face_edges);
					temp.vertices = faceVertices;
					f.addFace(temp);
				}
				else if(query[0] == 4){
					planes.push_back(glm::vec4(query[1],query[2],query[3],query[4]));
					if(planes.size()==3){
						findOrigin(planes[0],planes[1],planes[2],v0);
						n3 = glm::normalize(glm::vec3(planes[0][0],planes[0][1],planes[0][2]));
						n1 = glm::normalize(glm::vec3(planes[1][0],planes[1][1],planes[1][2]));
						n2 = glm::normalize(glm::vec3(planes[2][0],planes[2][1],planes[2][2]));
					}
				}
			}
			else{
				if(query[0] == 3){

					if(max_value<query[2])
						max_value = query[2];
					if(max_value<query[3])
						max_value = query[3];
					if(query[1] == 1){
						vfront.push_back(glm::vec3(query[2],query[3],0));
						vf.addVertex(query[2],query[3],0);
					}
					else if(query[1] == 2){
						vside.push_back(glm::vec3(-query[3],query[2],0));
						vs.addVertex(query[2],query[3],0);
					}
					else{
						vtop.push_back(glm::vec3(query[3],-query[2],0));
						vt.addVertex(query[2],query[3],0);
					}
				}
				else if(query[0] == 1){
					if(query[1]==1){
						coordinate c1(vfront[query[3]-1][0],vfront[query[3]-1][1],vfront[query[3]-1][2]);
						coordinate c2(vfront[query[4]-1][0],vfront[query[4]-1][1],vfront[query[4]-1][2]);
						edge e2d(c1,c2);
						if(query[2]==1){
							e2d.hiddenXY = true;
							hiddenxy.push_back(vfront[query[3]-1]);
							hiddenxy.push_back(vfront[query[4]-1]);
						}
						else{
							e2d.hiddenXY = false;
							xy.push_back(vfront[query[3]-1]);
							xy.push_back(vfront[query[4]-1]);
						}
						ef.addEdge(e2d);

					}
					else if(query[1]==2){
						coordinate c1(vside[query[3]-1][1],-vside[query[3]-1][0],vside[query[3]-1][2]);
						coordinate c2(vside[query[4]-1][1],-vside[query[4]-1][0],vside[query[4]-1][2]);
						edge e2d(c1,c2);
						if(query[2]==1){
							e2d.hiddenXY = true;
							hiddenyz.push_back(vside[query[3]-1]);
							hiddenyz.push_back(vside[query[4]-1]);
						}
						else{
							e2d.hiddenXY = false;
							yz.push_back(vside[query[3]-1]);
							yz.push_back(vside[query[4]-1]);
						}
						es.addEdge(e2d);

					}
					else{
						coordinate c1(-vtop[query[3]-1][1],vtop[query[3]-1][0],vtop[query[3]-1][2]);
						coordinate c2(-vtop[query[4]-1][1],vtop[query[4]-1][0],vtop[query[4]-1][2]);
						edge e2d(c1,c2);
						if(query[2]==1){
							e2d.hiddenXY = true;
							hiddenzx.push_back(vtop[query[3]-1]);
							hiddenzx.push_back(vtop[query[4]-1]);
						}
						else{
							e2d.hiddenXY = false;
							zx.push_back(vtop[query[3]-1]);
							zx.push_back(vtop[query[4]-1]);
						}
						et.addEdge(e2d);
					}
				}
			}
		}
	}
	file.close();
	return quit;
}
float slope(glm::vec3 p1,glm::vec3 p2,glm::vec3 p3){
	
	return glm::dot(glm::normalize(p1-p2),glm::normalize(p3-p2));
}
void output(faces f,bool mode){
	float max;
	if(mode){
		std::unordered_map<coordinatePair,edge,coordinatePairHasher> &e3d = f.getEdges().getEdges();
		max = e3d.begin()->second.v1.x;
		for(auto i1 = e3d.begin(); i1 != e3d.end(); ++i1){
			edge ee = i1->second;
			if(ee.v1.x>max)max = ee.v1.x;
			else if(ee.v1.y>max)max = ee.v1.y;
			else if(ee.v1.z>max)max = ee.v1.z;
			else if(ee.v2.x>max)max = ee.v2.x;
			else if(ee.v2.y>max)max = ee.v2.y;
			else if(ee.v2.z>max)max = ee.v2.z;
			if(ee.hiddenXY){
				hiddenxy.push_back(glm::vec3(ee.v1.x,ee.v1.y,0));
				hiddenxy.push_back(glm::vec3(ee.v2.x,ee.v2.y,0));
			}
			else{
				xy.push_back(glm::vec3(ee.v1.x,ee.v1.y,0));
				xy.push_back(glm::vec3(ee.v2.x,ee.v2.y,0));
			}
			if(ee.hiddenYZ){
				hiddenyz.push_back(glm::vec3(-ee.v1.z,ee.v1.y,0));
				hiddenyz.push_back(glm::vec3(-ee.v2.z,ee.v2.y,0));
			}
			else{
				yz.push_back(glm::vec3(-ee.v1.z,ee.v1.y,0));
				yz.push_back(glm::vec3(-ee.v2.z,ee.v2.y,0));
			}
			if(ee.hiddenZX){
				hiddenzx.push_back(glm::vec3(ee.v1.x,-ee.v1.z,0));
				hiddenzx.push_back(glm::vec3(ee.v2.x,-ee.v2.z,0));
			}
			else{
				zx.push_back(glm::vec3(ee.v1.x,-ee.v1.z,0));
				zx.push_back(glm::vec3(ee.v2.x,-ee.v2.z,0));
			}
		}

		if(max<=0)max = 1;
		for(int i=0;i<hiddenxy.size();i++)
			for(int j=0;j<2;j++)
				hiddenxy[i][j] = hiddenxy[i][j]/max;
		for(int i=0;i<xy.size();i++)
			for(int j=0;j<2;j++)
				xy[i][j] = xy[i][j]/max;
		for(int i=0;i<hiddenyz.size();i++)
			for(int j=0;j<2;j++)
				hiddenyz[i][j] = (1-j) + hiddenyz[i][j]/max;
		for(int i=0;i<yz.size();i++)
			for(int j=0;j<2;j++)
				yz[i][j] = (1-j) + yz[i][j]/max;
		for(int i=0;i<hiddenzx.size();i++)
			for(int j=0;j<2;j++)
				hiddenzx[i][j] = j + hiddenzx[i][j]/max;
		for(int i=0;i<zx.size();i++)
			for(int j=0;j<2;j++)
				zx[i][j] = j + zx[i][j]/max;
		zx.push_back(glm::vec3(-0.1,1.1,0));
		zx.push_back(glm::vec3(0.3,1.1,0));
		zx.push_back(glm::vec3(-0.1,1.1,0));
		zx.push_back(glm::vec3(-0.1,0.7,0));
		yz.push_back(glm::vec3(1.1,-0.1,0));
		yz.push_back(glm::vec3(0.7,-0.1,0));
		yz.push_back(glm::vec3(1.1,-0.1,0));
		yz.push_back(glm::vec3(1.1,0.3,0));
		xy.push_back(glm::vec3(-0.1,-0.1,0));
		xy.push_back(glm::vec3(-0.1,0.3,0));
		xy.push_back(glm::vec3(-0.1,-0.1,0));
		xy.push_back(glm::vec3(0.3,-0.1,0));
		for(int i=0;i<hiddenxy.size();i+=2)
			for(int j=i+2;j<hiddenxy.size();j+=2){
				if(compareFloats(hiddenxy[i+1][0],hiddenxy[j+1][0])&&compareFloats(hiddenxy[i+1][1],hiddenxy[j+1][1])&&compareFloats(hiddenxy[i][0],hiddenxy[j][0])&&compareFloats(hiddenxy[i][1],hiddenxy[j][1])){
					hiddenxy.erase(hiddenxy.begin()+j);
					hiddenxy.erase(hiddenxy.begin()+j);
					j-=2;
				}
				else if(compareFloats(hiddenxy[i+1][0],hiddenxy[j][0])&&compareFloats(hiddenxy[i+1][1],hiddenxy[j][1])&&compareFloats(hiddenxy[i][0],hiddenxy[j+1][0])&&compareFloats(hiddenxy[i][1],hiddenxy[j+1][1])) {
					hiddenxy.erase(hiddenxy.begin()+j);
					hiddenxy.erase(hiddenxy.begin()+j);			
					j-=2;
				}
				else if(compareFloats(hiddenxy[i][0],hiddenxy[j+1][0])&&compareFloats(hiddenxy[i][1],hiddenxy[j+1][1])&&compareFloats(hiddenxy[i+1][0],hiddenxy[j][0])&&compareFloats(hiddenxy[i+1][1],hiddenxy[j][1])) {
					hiddenxy.erase(hiddenxy.begin()+j);
					hiddenxy.erase(hiddenxy.begin()+j);
					j-=2;
				}
				else if(compareFloats(hiddenxy[i][0],hiddenxy[j][0])&&compareFloats(hiddenxy[i][1],hiddenxy[j][1])&&compareFloats(hiddenxy[i+1][0],hiddenxy[j+1][0])&&compareFloats(hiddenxy[i+1][1],hiddenxy[j+1][1])) {
					hiddenxy.erase(hiddenxy.begin()+j);
					hiddenxy.erase(hiddenxy.begin()+j);
					j-=2;
				}
			}

		for(int i=0;i<hiddenyz.size();i+=2)
			for(int j=i+2;j<hiddenyz.size();j+=2){
				if(compareFloats(hiddenyz[i+1][0],hiddenyz[j+1][0])&&compareFloats(hiddenyz[i+1][1],hiddenyz[j+1][1])&&compareFloats(hiddenyz[i][0],hiddenyz[j][0])&&compareFloats(hiddenyz[i][1],hiddenyz[j][1])){
					hiddenyz.erase(hiddenyz.begin()+j);
					hiddenyz.erase(hiddenyz.begin()+j);
					j-=2;
				}
				else if(compareFloats(hiddenyz[i+1][0],hiddenyz[j][0])&&compareFloats(hiddenyz[i+1][1],hiddenyz[j][1])&&compareFloats(hiddenyz[i][0],hiddenyz[j+1][0])&&compareFloats(hiddenyz[i][1],hiddenyz[j+1][1])) {
					hiddenyz.erase(hiddenyz.begin()+j);
					hiddenyz.erase(hiddenyz.begin()+j);			
					j-=2;
				}
				else if(compareFloats(hiddenyz[i][0],hiddenyz[j+1][0])&&compareFloats(hiddenyz[i][1],hiddenyz[j+1][1])&&compareFloats(hiddenyz[i+1][0],hiddenyz[j][0])&&compareFloats(hiddenyz[i+1][1],hiddenyz[j][1])) {
					hiddenyz.erase(hiddenyz.begin()+j);
					hiddenyz.erase(hiddenyz.begin()+j);
					j-=2;
				}
				else if(compareFloats(hiddenyz[i][0],hiddenyz[j][0])&&compareFloats(hiddenyz[i][1],hiddenyz[j][1])&&compareFloats(hiddenyz[i+1][0],hiddenyz[j+1][0])&&compareFloats(hiddenyz[i+1][1],hiddenyz[j+1][1])) {
					hiddenyz.erase(hiddenyz.begin()+j);
					hiddenyz.erase(hiddenyz.begin()+j);
					j-=2;
				}
			}

		for(int i=0;i<hiddenzx.size();i+=2)
			for(int j=i+2;j<hiddenzx.size();j+=2){
				if(compareFloats(hiddenzx[i+1][0],hiddenzx[j+1][0])&&compareFloats(hiddenzx[i+1][1],hiddenzx[j+1][1])&&compareFloats(hiddenzx[i][0],hiddenzx[j][0])&&compareFloats(hiddenzx[i][1],hiddenzx[j][1])){
					hiddenzx.erase(hiddenzx.begin()+j);
					hiddenzx.erase(hiddenzx.begin()+j);
					j-=2;
				}
				else if(compareFloats(hiddenzx[i+1][0],hiddenzx[j][0])&&compareFloats(hiddenzx[i+1][1],hiddenzx[j][1])&&compareFloats(hiddenzx[i][0],hiddenzx[j+1][0])&&compareFloats(hiddenzx[i][1],hiddenzx[j+1][1])) {
					hiddenzx.erase(hiddenzx.begin()+j);
					hiddenzx.erase(hiddenzx.begin()+j);			
					j-=2;
				}
				else if(compareFloats(hiddenzx[i][0],hiddenzx[j+1][0])&&compareFloats(hiddenzx[i][1],hiddenzx[j+1][1])&&compareFloats(hiddenzx[i+1][0],hiddenzx[j][0])&&compareFloats(hiddenzx[i+1][1],hiddenzx[j][1])) {
					hiddenzx.erase(hiddenzx.begin()+j);
					hiddenzx.erase(hiddenzx.begin()+j);
					j-=2;
				}
				else if(compareFloats(hiddenzx[i][0],hiddenzx[j][0])&&compareFloats(hiddenzx[i][1],hiddenzx[j][1])&&compareFloats(hiddenzx[i+1][0],hiddenzx[j+1][0])&&compareFloats(hiddenzx[i+1][1],hiddenzx[j+1][1])) {
					hiddenzx.erase(hiddenzx.begin()+j);
					hiddenzx.erase(hiddenzx.begin()+j);
					j-=2;
				}
			}
		bool changes = true;
		while(changes){
			changes = false;
			for(int i=0;i<hiddenxy.size();i+=2)
				for(int j=i+2;j<hiddenxy.size();j+=2){
					if(compareFloats(hiddenxy[i][0],hiddenxy[j][0])&&compareFloats(hiddenxy[i][1],hiddenxy[j][1])){
							if(compareFloats(slope(hiddenxy[i+1],hiddenxy[i],hiddenxy[j+1]),-1)){
								hiddenxy.push_back(glm::vec3(hiddenxy[j+1]));
								hiddenxy.push_back(glm::vec3(hiddenxy[i+1]));
								hiddenxy.erase(hiddenxy.begin()+j);
								hiddenxy.erase(hiddenxy.begin()+j);
								hiddenxy.erase(hiddenxy.begin()+i);
								hiddenxy.erase(hiddenxy.begin()+i);
								i-=2;
								break;
								changes = true;
							}
							else if(compareFloats(slope(hiddenxy[i+1],hiddenxy[i],hiddenxy[j+1]),1)){
								if(glm::length(hiddenxy[i+1]-hiddenxy[i])>glm::length(hiddenxy[j+1]-hiddenxy[j])){
									hiddenxy.erase(hiddenxy.begin()+j);
									hiddenxy.erase(hiddenxy.begin()+j);
								}
								else{
									hiddenxy.erase(hiddenxy.begin()+i);
									hiddenxy.erase(hiddenxy.begin()+i);
								}
								i=-2;
								break;
								changes = true;
							}

					}
					else if(compareFloats(hiddenxy[i][0],hiddenxy[j+1][0])&&compareFloats(hiddenxy[i][1],hiddenxy[j+1][1])) {
						if(compareFloats(slope(hiddenxy[i+1],hiddenxy[i],hiddenxy[j]),-1)){
							hiddenxy.push_back(glm::vec3(hiddenxy[j]));
							hiddenxy.push_back(glm::vec3(hiddenxy[i+1]));
							hiddenxy.erase(hiddenxy.begin()+j);
							hiddenxy.erase(hiddenxy.begin()+j);
							hiddenxy.erase(hiddenxy.begin()+i);
							hiddenxy.erase(hiddenxy.begin()+i);				
							i-=2;
							break;
							changes = true;
						}
						else if(compareFloats(slope(hiddenxy[i+1],hiddenxy[i],hiddenxy[j]),1)){
								if(glm::length(hiddenxy[i+1]-hiddenxy[i])>glm::length(hiddenxy[j+1]-hiddenxy[j])){
									hiddenxy.erase(hiddenxy.begin()+j);
									hiddenxy.erase(hiddenxy.begin()+j);
								}
								else{
									hiddenxy.erase(hiddenxy.begin()+i);
									hiddenxy.erase(hiddenxy.begin()+i);
								}
								i=-2;
								break;
								changes = true;
						}

					}
					else if(compareFloats(hiddenxy[i+1][0],hiddenxy[j][0])&&compareFloats(hiddenxy[i+1][1],hiddenxy[j][1])) {
						if(compareFloats(slope(hiddenxy[i],hiddenxy[j],hiddenxy[j+1]),-1)){
							hiddenxy.push_back(glm::vec3(hiddenxy[j+1]));
							hiddenxy.push_back(glm::vec3(hiddenxy[i]));
							hiddenxy.erase(hiddenxy.begin()+j);
							hiddenxy.erase(hiddenxy.begin()+j);
							hiddenxy.erase(hiddenxy.begin()+i);
							hiddenxy.erase(hiddenxy.begin()+i);				
							i-=2;
							break;
							changes = true;
						}
						else if(compareFloats(slope(hiddenxy[i],hiddenxy[j],hiddenxy[j+1]),1)){
								if(glm::length(hiddenxy[i+1]-hiddenxy[i])>glm::length(hiddenxy[j+1]-hiddenxy[j])){
									hiddenxy.erase(hiddenxy.begin()+j);
									hiddenxy.erase(hiddenxy.begin()+j);
								}
								else{
									hiddenxy.erase(hiddenxy.begin()+i);
									hiddenxy.erase(hiddenxy.begin()+i);
								}
								i=-2;
								break;
								changes = true;
						}
					}
					else if(compareFloats(hiddenxy[i+1][0],hiddenxy[j+1][0])&&compareFloats(hiddenxy[i+1][1],hiddenxy[j+1][1])) {
						if(compareFloats(slope(hiddenxy[i],hiddenxy[j+1],hiddenxy[j]),-1)){
							hiddenxy.push_back(glm::vec3(hiddenxy[j]));
							hiddenxy.push_back(glm::vec3(hiddenxy[i]));
							hiddenxy.erase(hiddenxy.begin()+j);
							hiddenxy.erase(hiddenxy.begin()+j);
							hiddenxy.erase(hiddenxy.begin()+i);
							hiddenxy.erase(hiddenxy.begin()+i);				
							i-=2;
							break;
							changes = true;
						}
						else if(compareFloats(slope(hiddenxy[i],hiddenxy[j+1],hiddenxy[j]),1)){
								if(glm::length(hiddenxy[i+1]-hiddenxy[i])>glm::length(hiddenxy[j+1]-hiddenxy[j])){
									hiddenxy.erase(hiddenxy.begin()+j);
									hiddenxy.erase(hiddenxy.begin()+j);
								}
								else{
									hiddenxy.erase(hiddenxy.begin()+i);
									hiddenxy.erase(hiddenxy.begin()+i);
								}
								i=-2;
								break;
								changes = true;
						}
					}
				}
		}
		changes = true;
		while(changes){
			changes = false;
			for(int i=0;i<hiddenyz.size();i+=2)
				for(int j=i+2;j<hiddenyz.size();j+=2){
					if(compareFloats(hiddenyz[i][0],hiddenyz[j][0])&&compareFloats(hiddenyz[i][1],hiddenyz[j][1])){
							if(compareFloats(slope(hiddenyz[i+1],hiddenyz[i],hiddenyz[j+1]),-1)){
								hiddenyz.push_back(glm::vec3(hiddenyz[j+1]));
								hiddenyz.push_back(glm::vec3(hiddenyz[i+1]));
								hiddenyz.erase(hiddenyz.begin()+j);
								hiddenyz.erase(hiddenyz.begin()+j);
								hiddenyz.erase(hiddenyz.begin()+i);
								hiddenyz.erase(hiddenyz.begin()+i);
								i-=2;
								break;
								changes = true;
							}
							else if(compareFloats(slope(hiddenyz[i+1],hiddenyz[i],hiddenyz[j+1]),1)){
								if(glm::length(hiddenyz[i+1]-hiddenyz[i])>glm::length(hiddenyz[j+1]-hiddenyz[j])){
									hiddenyz.erase(hiddenyz.begin()+j);
									hiddenyz.erase(hiddenyz.begin()+j);
								}
								else{
									hiddenyz.erase(hiddenyz.begin()+i);
									hiddenyz.erase(hiddenyz.begin()+i);
								}
								i=-2;
								break;
								changes = true;
							}

					}
					else if(compareFloats(hiddenyz[i][0],hiddenyz[j+1][0])&&compareFloats(hiddenyz[i][1],hiddenyz[j+1][1])) {
						if(compareFloats(slope(hiddenyz[i+1],hiddenyz[i],hiddenyz[j]),-1)){
							hiddenyz.push_back(glm::vec3(hiddenyz[j]));
							hiddenyz.push_back(glm::vec3(hiddenyz[i+1]));
							hiddenyz.erase(hiddenyz.begin()+j);
							hiddenyz.erase(hiddenyz.begin()+j);
							hiddenyz.erase(hiddenyz.begin()+i);
							hiddenyz.erase(hiddenyz.begin()+i);				
							i-=2;
							break;
							changes = true;
						}
						else if(compareFloats(slope(hiddenyz[i+1],hiddenyz[i],hiddenyz[j]),1)){
								if(glm::length(hiddenyz[i+1]-hiddenyz[i])>glm::length(hiddenyz[j+1]-hiddenyz[j])){
									hiddenyz.erase(hiddenyz.begin()+j);
									hiddenyz.erase(hiddenyz.begin()+j);
								}
								else{
									hiddenyz.erase(hiddenyz.begin()+i);
									hiddenyz.erase(hiddenyz.begin()+i);
								}
								i=-2;
								break;
								changes = true;
						}

					}
					else if(compareFloats(hiddenyz[i+1][0],hiddenyz[j][0])&&compareFloats(hiddenyz[i+1][1],hiddenyz[j][1])) {
						if(compareFloats(slope(hiddenyz[i],hiddenyz[j],hiddenyz[j+1]),-1)){
							hiddenyz.push_back(glm::vec3(hiddenyz[j+1]));
							hiddenyz.push_back(glm::vec3(hiddenyz[i]));
							hiddenyz.erase(hiddenyz.begin()+j);
							hiddenyz.erase(hiddenyz.begin()+j);
							hiddenyz.erase(hiddenyz.begin()+i);
							hiddenyz.erase(hiddenyz.begin()+i);				
							i-=2;
							break;
							changes = true;
						}
						else if(compareFloats(slope(hiddenyz[i],hiddenyz[j],hiddenyz[j+1]),1)){
								if(glm::length(hiddenyz[i+1]-hiddenyz[i])>glm::length(hiddenyz[j+1]-hiddenyz[j])){
									hiddenyz.erase(hiddenyz.begin()+j);
									hiddenyz.erase(hiddenyz.begin()+j);
								}
								else{
									hiddenyz.erase(hiddenyz.begin()+i);
									hiddenyz.erase(hiddenyz.begin()+i);
								}
								i=-2;
								break;
								changes = true;
						}
					}
					else if(compareFloats(hiddenyz[i+1][0],hiddenyz[j+1][0])&&compareFloats(hiddenyz[i+1][1],hiddenyz[j+1][1])) {
						if(compareFloats(slope(hiddenyz[i],hiddenyz[j+1],hiddenyz[j]),-1)){
							hiddenyz.push_back(glm::vec3(hiddenyz[j]));
							hiddenyz.push_back(glm::vec3(hiddenyz[i]));
							hiddenyz.erase(hiddenyz.begin()+j);
							hiddenyz.erase(hiddenyz.begin()+j);
							hiddenyz.erase(hiddenyz.begin()+i);
							hiddenyz.erase(hiddenyz.begin()+i);				
							i-=2;
							break;
							changes = true;
						}
						else if(compareFloats(slope(hiddenyz[i],hiddenyz[j+1],hiddenyz[j]),1)){
								if(glm::length(hiddenyz[i+1]-hiddenyz[i])>glm::length(hiddenyz[j+1]-hiddenyz[j])){
									hiddenyz.erase(hiddenyz.begin()+j);
									hiddenyz.erase(hiddenyz.begin()+j);
								}
								else{
									hiddenyz.erase(hiddenyz.begin()+i);
									hiddenyz.erase(hiddenyz.begin()+i);
								}
								i=-2;
								break;
								changes = true;
						}
					}
				}
		}
		changes = true;
		while(changes){
			changes = false;
			for(int i=0;i<hiddenzx.size();i+=2)
				for(int j=i+2;j<hiddenzx.size();j+=2){
					if(compareFloats(hiddenzx[i][0],hiddenzx[j][0])&&compareFloats(hiddenzx[i][1],hiddenzx[j][1])){
							if(compareFloats(slope(hiddenzx[i+1],hiddenzx[i],hiddenzx[j+1]),-1)){
								hiddenzx.push_back(glm::vec3(hiddenzx[j+1]));
								hiddenzx.push_back(glm::vec3(hiddenzx[i+1]));
								hiddenzx.erase(hiddenzx.begin()+j);
								hiddenzx.erase(hiddenzx.begin()+j);
								hiddenzx.erase(hiddenzx.begin()+i);
								hiddenzx.erase(hiddenzx.begin()+i);
								i-=2;
								break;
								changes = true;
							}
							else if(compareFloats(slope(hiddenzx[i+1],hiddenzx[i],hiddenzx[j+1]),1)){
								if(glm::length(hiddenzx[i+1]-hiddenzx[i])>glm::length(hiddenzx[j+1]-hiddenzx[j])){
									hiddenzx.erase(hiddenzx.begin()+j);
									hiddenzx.erase(hiddenzx.begin()+j);
								}
								else{
									hiddenzx.erase(hiddenzx.begin()+i);
									hiddenzx.erase(hiddenzx.begin()+i);
								}
								i=-2;
								break;
								changes = true;
							}

					}
					else if(compareFloats(hiddenzx[i][0],hiddenzx[j+1][0])&&compareFloats(hiddenzx[i][1],hiddenzx[j+1][1])) {
						if(compareFloats(slope(hiddenzx[i+1],hiddenzx[i],hiddenzx[j]),-1)){
							hiddenzx.push_back(glm::vec3(hiddenzx[j]));
							hiddenzx.push_back(glm::vec3(hiddenzx[i+1]));
							hiddenzx.erase(hiddenzx.begin()+j);
							hiddenzx.erase(hiddenzx.begin()+j);
							hiddenzx.erase(hiddenzx.begin()+i);
							hiddenzx.erase(hiddenzx.begin()+i);				
							i-=2;
							break;
							changes = true;
						}
						else if(compareFloats(slope(hiddenzx[i+1],hiddenzx[i],hiddenzx[j]),1)){
								if(glm::length(hiddenzx[i+1]-hiddenzx[i])>glm::length(hiddenzx[j+1]-hiddenzx[j])){
									hiddenzx.erase(hiddenzx.begin()+j);
									hiddenzx.erase(hiddenzx.begin()+j);
								}
								else{
									hiddenzx.erase(hiddenzx.begin()+i);
									hiddenzx.erase(hiddenzx.begin()+i);
								}
								i=-2;
								break;
								changes = true;
						}

					}
					else if(compareFloats(hiddenzx[i+1][0],hiddenzx[j][0])&&compareFloats(hiddenzx[i+1][1],hiddenzx[j][1])) {
						if(compareFloats(slope(hiddenzx[i],hiddenzx[j],hiddenzx[j+1]),-1)){
							hiddenzx.push_back(glm::vec3(hiddenzx[j+1]));
							hiddenzx.push_back(glm::vec3(hiddenzx[i]));
							hiddenzx.erase(hiddenzx.begin()+j);
							hiddenzx.erase(hiddenzx.begin()+j);
							hiddenzx.erase(hiddenzx.begin()+i);
							hiddenzx.erase(hiddenzx.begin()+i);				
							i-=2;
							break;
							changes = true;
						}
						else if(compareFloats(slope(hiddenzx[i],hiddenzx[j],hiddenzx[j+1]),1)){
								if(glm::length(hiddenzx[i+1]-hiddenzx[i])>glm::length(hiddenzx[j+1]-hiddenzx[j])){
									hiddenzx.erase(hiddenzx.begin()+j);
									hiddenzx.erase(hiddenzx.begin()+j);
								}
								else{
									hiddenzx.erase(hiddenzx.begin()+i);
									hiddenzx.erase(hiddenzx.begin()+i);
								}
								i=-2;
								break;
								changes = true;
						}
					}
					else if(compareFloats(hiddenzx[i+1][0],hiddenzx[j+1][0])&&compareFloats(hiddenzx[i+1][1],hiddenzx[j+1][1])) {
						if(compareFloats(slope(hiddenzx[i],hiddenzx[j+1],hiddenzx[j]),-1)){
							hiddenzx.push_back(glm::vec3(hiddenzx[j]));
							hiddenzx.push_back(glm::vec3(hiddenzx[i]));
							hiddenzx.erase(hiddenzx.begin()+j);
							hiddenzx.erase(hiddenzx.begin()+j);
							hiddenzx.erase(hiddenzx.begin()+i);
							hiddenzx.erase(hiddenzx.begin()+i);				
							i-=2;
							break;
							changes = true;
						}
						else if(compareFloats(slope(hiddenzx[i],hiddenzx[j+1],hiddenzx[j]),1)){
								if(glm::length(hiddenzx[i+1]-hiddenzx[i])>glm::length(hiddenzx[j+1]-hiddenzx[j])){
									hiddenzx.erase(hiddenzx.begin()+j);
									hiddenzx.erase(hiddenzx.begin()+j);
								}
								else{
									hiddenzx.erase(hiddenzx.begin()+i);
									hiddenzx.erase(hiddenzx.begin()+i);
								}
								i=-2;
								break;
								changes = true;
						}
					}
				}
		}

		bool b = true;
		int l1,l2,l3;
		l1 = hiddenxy.size();
		l2 = hiddenyz.size();
		l3 = hiddenzx.size();
		hiddenxy.insert(hiddenxy.end(),xy.begin(),xy.end());
		hiddenyz.insert(hiddenyz.end(),yz.begin(),yz.end());
		hiddenzx.insert(hiddenzx.end(),zx.begin(),zx.end());
		std::vector<face> f3d = f.getFaces();
		vertices3d.clear();
		for(int i=0;i<f3d.size();++i){
			std::vector<coordinate> faceVertices = f3d[i].vertices;
			std::vector<float> query;
			query.push_back(0);
			int k;
			for(int m=0;m<faceVertices.size();++m){
				for(k=0;k<vertices3d.size();++k)
					if(compareFloats(vertices3d[k][0],faceVertices[m].x)&&compareFloats(vertices3d[k][1],faceVertices[m].y)&&compareFloats(vertices3d[k][2],faceVertices[m].z)){
						query.push_back(k+1);
						break;
					}
				if(k == vertices3d.size()){
					if(faceVertices[m].x==-1)
						query.push_back(-1);
					else{
						query.push_back(vertices3d.size()+1);
						vertices3d.push_back(glm::vec3(faceVertices[m].x,faceVertices[m].y,faceVertices[m].z));
						normals3d.push_back(glm::vec3(0,0,0));
					}
				}
			}
			triangulate(indices3d,normals3d,query,vertices3d);
		}
		center = glm::vec3(0,0,0);
		for(int i=0;i<vertices3d.size();++i)
			center+=vertices3d[i];
		for(int i=0;i<normals3d.size();++i)
			normals3d[i] = glm::normalize(normals3d[i]);
		for(int i=0;i<vertices3d.size();++i)
			vertices3d[i] = vertices3d[i] - center/glm::vec3(vertices3d.size());
		float max = vertices3d[0][0];
		for(int i=0;i<vertices3d.size();++i)
			for(int j=0;j<3;++j)
				if(max<vertices3d[i][j])
					max = vertices3d[i][j];
		if(max>0)
			for(int i=0;i<vertices3d.size();++i)
				vertices3d[i]/=glm::vec3(max);
		while(b){
			b = false;
			if(display3d(vertices3d,indices3d,normals3d))
				if(display2d(hiddenxy,l1,"Front View"))
					if(display2d(hiddenyz,l2,"Side View"))
						b = display2d(hiddenzx,l3,"Top View");
		}
	}
	else{
		max = max_value;
		if(max<=0)max = 1;

		for(int i=0;i<hiddenxy.size();i++)
			for(int j=0;j<2;j++)
				hiddenxy[i][j] = hiddenxy[i][j]/max;
		for(int i=0;i<xy.size();i++)
			for(int j=0;j<2;j++)
				xy[i][j] = xy[i][j]/max;
		for(int i=0;i<hiddenyz.size();i++)
			for(int j=0;j<2;j++)
				hiddenyz[i][j] = (1-j) + hiddenyz[i][j]/max;
		for(int i=0;i<yz.size();i++)
			for(int j=0;j<2;j++)
				yz[i][j] = (1-j) + yz[i][j]/max;
		for(int i=0;i<hiddenzx.size();i++)
			for(int j=0;j<2;j++)
				hiddenzx[i][j] = j + hiddenzx[i][j]/max;
		for(int i=0;i<zx.size();i++)
			for(int j=0;j<2;j++)
				zx[i][j] = j + zx[i][j]/max;
		zx.push_back(glm::vec3(-0.1,1.1,0));
		zx.push_back(glm::vec3(0.3,1.1,0));
		zx.push_back(glm::vec3(-0.1,1.1,0));
		zx.push_back(glm::vec3(-0.1,0.7,0));
		yz.push_back(glm::vec3(1.1,-0.1,0));
		yz.push_back(glm::vec3(0.7,-0.1,0));
		yz.push_back(glm::vec3(1.1,-0.1,0));
		yz.push_back(glm::vec3(1.1,0.3,0));
		xy.push_back(glm::vec3(-0.1,-0.1,0));
		xy.push_back(glm::vec3(-0.1,0.3,0));
		xy.push_back(glm::vec3(-0.1,-0.1,0));
		xy.push_back(glm::vec3(0.3,-0.1,0));
		bool b = true;
		int l1,l2,l3;
		l1 = hiddenxy.size();
		l2 = hiddenyz.size();
		l3 = hiddenzx.size();
		hiddenxy.insert(hiddenxy.end(),xy.begin(),xy.end());
		hiddenyz.insert(hiddenyz.end(),yz.begin(),yz.end());
		hiddenzx.insert(hiddenzx.end(),zx.begin(),zx.end());
		std::vector<face> f3d = f.getFaces();
		for(int i=0;i<f3d.size();++i){
			std::vector<coordinate> faceVertices = f3d[i].vertices;
			std::vector<float> query;
			query.push_back(0);
			int k;
			for(int m=0;m<faceVertices.size();++m){
				for(k=0;k<vertices3d.size();++k)
					if(compareFloats(vertices3d[k][0],faceVertices[m].x)&&compareFloats(vertices3d[k][1],faceVertices[m].y)&&compareFloats(vertices3d[k][2],faceVertices[m].z)){
						query.push_back(k+1);
						break;
					}
				if(k == vertices3d.size()){
					if(faceVertices[m].x==-1)
						query.push_back(-1);
					else{
						query.push_back(vertices3d.size()+1);
						vertices3d.push_back(glm::vec3(faceVertices[m].x,faceVertices[m].y,faceVertices[m].z));
						normals3d.push_back(glm::vec3(0,0,0));
					}
				}
			}
			triangulate(indices3d,normals3d,query,vertices3d);
		}
		center = glm::vec3(0,0,0);
		for(int i=0;i<vertices3d.size();++i)
			center+=vertices3d[i];
		for(int i=0;i<normals3d.size();++i)
			normals3d[i] = glm::normalize(normals3d[i]);
		for(int i=0;i<vertices3d.size();++i)
			vertices3d[i] = vertices3d[i] - center/glm::vec3(vertices3d.size());
		float max = vertices3d[0][0];
		for(int i=0;i<vertices3d.size();++i)
			for(int j=0;j<3;++j)
				if(max<vertices3d[i][j])
					max = vertices3d[i][j];
		if(max>0)
			for(int i=0;i<vertices3d.size();++i)
				vertices3d[i]/=glm::vec3(max);
		while(b){
			b = false;
				if(display2d(hiddenxy,l1,"Front View"))
					if(display2d(hiddenyz,l2,"Side View"))
						if(display2d(hiddenzx,l3,"Top View"))
								b = display3d(vertices3d,indices3d,normals3d);
		}
	}
}
