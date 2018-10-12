CXX = g++ -std=c++11
IDIR = ../include
ODIR = ./obj
CXXFlAG = -I$(IDIR) `pkg-config --cflags gtk+-3.0`
_DEPS = faces.h edges.h vertices.h objects.h inout.h
SRC = faces.cpp edges.cpp vertices.cpp shader.cpp controls.cpp vertex.cpp edge.cpp face.cpp objects.cpp main.cpp input.cpp
OBJ = $(patsubst %,$(ODIR)/%,$(SRC:.cpp=.o))
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))
LIBS = -lglfw -lglew -framework OpenGL `pkg-config --libs gtk+-3.0`
LIBS_linux = -lGL -lGLU -lglut -lGLEW -lglfw3 -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor `pkg-config --libs gtk+-3.0`

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CXX) -c $< -o $@ $(CXXFlAG)

mac : $(OBJ)
	$(CXX) $^ -o app $(LIBS)
linux : $(OBJ)
	$(CXX) $^ -o app $(LIBS_linux)

.PHONY: clean
clean:
	rm -f $(ODIR)/*.o core
