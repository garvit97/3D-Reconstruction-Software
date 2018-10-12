# Reconstruction of object from 3 orthographic projections
The software structure is designed to 
1) Interactively take input as 3D model and create orthographic projections on any random plane.
2) Interactively take input as 2D projections(without Correspondence information) and create 3D model.

## Getting Started


### Prerequisites

To install glew:

```
cd Code
cd external/glew-1.13.0
make
sudo -s
make install
make clean
```

To install openGL:

```
cd Code
cd external/glfw-3.1.2
mkdir build
cd build
cmake ..
make
sudo make install
```

To install glm:

```
cd Code
cd external/glm-0.9.7.1
mkdir build
cd build
cmake ..
make
sudo make install
```


### Installing
 For Mac type:

```
cd Code/src
make clean
make mac
```

For Linux type:

```
cd Code/src
make clean
make linux
```

## Usage

To start the program type

```
./app
```

The software opens with a window with 3 buttons on it-
1) 3D to 2D
2) 2D to 3D
3) Quit

Clicking on 3D to 2D button or 2D to 3D button opens a File Explorer for the user to select the file that contains the model that the user wants to transform.
The user can make any number of conversions and choose when to quit the software, by clicking on the Quit button.

## Authors

+ Garvit Gupta
**guptagarvit1997@gmail.com**

+ Madhur Singal
**madhursingal08@gmail.com**

*Course Project under [**Prof. Subhashis Banerjee**](http://http://www.cse.iitd.ernet.in/~suban/)*


