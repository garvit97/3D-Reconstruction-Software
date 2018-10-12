#include "objects.h"

bool compareFloats(float A, float B) 
{
   float diff = A - B;
   float EPSILON=0.00001;
   return (diff < EPSILON) && (-diff < EPSILON);
}
// Function to get cofactor of A[p][q] in temp[][]. n is current
// dimension of A[][]
void getCofactor(float A[3][3], float temp[3][3], int p, int q, int n)
{
    int i = 0, j = 0;
 
    // Looping for each element of the matrix
    for (int row = 0; row < n; row++)
    {
        for (int col = 0; col < n; col++)
        {
            //  Copying into temporary matrix only those element
            //  which are not in given row and column
            if (row != p && col != q)
            {
                temp[i][j++] = A[row][col];
 
                // Row is filled, so increase row index and
                // reset col index
                if (j == n - 1)
                {
                    j = 0;
                    i++;
                }
            }
        }
    }
}
 
/* Recursive function for finding determinant of matrix.
   n is current dimension of A[][]. */
float determinant(float A[3][3], int n)
{
    float D = 0; // Initialize result
 
    //  Base case : if matrix contains single element
    if (n == 1)
        return A[0][0];
 
    float temp[3][3]; // To store cofactors
 
    int sign = 1;  // To store sign multiplier
 
     // Iterate for each element of first row
    for (int f = 0; f < n; f++)
    {
        // Getting Cofactor of A[0][f]
        getCofactor(A, temp, 0, f, n);
        D += sign * A[0][f] * determinant(temp, n - 1);
 
        // terms are to be added with alternate sign
        sign = -sign;
    }
 
    return D;
}
 
// Function to get adjoint of A[N][N] in adj[N][N].
void adjoint(float A[3][3], float adj[3][3])
{
    // if (N == 1)
    // {
    //     adj[0][0] = 1;
    //     return;
    // }
 
    // temp is used to store cofactors of A[][]
    int sign = 1; float temp[3][3];
 
    for (int i=0; i<3; i++)
    {
        for (int j=0; j<3; j++)
        {
            // Get cofactor of A[i][j]
            getCofactor(A, temp, i, j, 3);
 
            // sign of adj[j][i] positive if sum of row
            // and column indexes is even.
            sign = ((i+j)%2==0)? 1: -1;
 
            // Interchanging rows and columns to get the
            // transpose of the cofactor matrix
            adj[j][i] = (sign)*(determinant(temp, 3-1));
        }
    }
}
 
// Function to calculate and store inverse, returns false if
// matrix is singular
bool inverse(float A[3][3], float inverse[3][3])
{
    // Find determinant of A[][]
    float det = determinant(A, 3);
    if (compareFloats(det,0.0))
    {
        return false;
    }
 
    // Find adjoint
    float adj[3][3];
    adjoint(A, adj);
 
    // Find Inverse using formula "inverse(A) = adj(A)/det(A)"
    for (int i=0; i<3; i++)
        for (int j=0; j<3; j++)
            inverse[i][j] = adj[i][j]/det;
 
    return true;
}

void transpose(float A[3][3], float transpose[3][3])
{
	for(int i=0;i<3;i++) 
	{
    	for(int j=0;j<3;j++)
    	{
      		transpose[j][i] = A[i][j];
    	}
  	}
}

void findRotMatrixAndOrigin(plane xy, plane yz, plane zx, float rotMatrix[3][3], coordinate &v0){
	float den1=sqrt(xy.a*xy.a+xy.b*xy.b+xy.c*xy.c),
		  den2=sqrt(yz.a*yz.a+yz.b*yz.b+yz.c*yz.c),
		  den3=sqrt(zx.a*zx.a+zx.b*zx.b+zx.c*zx.c);

	float temp[3][3]={xy.a/den1, xy.b/den1, xy.c/den1,
					  yz.a/den2, yz.b/den2, yz.c/den2,
					  zx.a/den3, zx.b/den3, zx.c/den3};
	//for finding v0
	float temp1[3][3]={xy.a, xy.b, xy.c,
					   yz.a, yz.b, yz.c,
					   zx.a, zx.b, zx.c};
	inverse(temp1,rotMatrix);
	v0.x=rotMatrix[0][0]*xy.d+rotMatrix[0][1]*yz.d+rotMatrix[0][2]*zx.d;
	v0.y=rotMatrix[1][0]*xy.d+rotMatrix[1][1]*yz.d+rotMatrix[1][2]*zx.d;
	v0.z=rotMatrix[2][0]*xy.d+rotMatrix[2][1]*yz.d+rotMatrix[2][2]*zx.d;

	transpose(temp, temp1);
	inverse(temp1, rotMatrix);
}

void changeFrame(coordinate &v, float rotMatrix[3][3], coordinate v0){
	float coord[3]={v.x, v.y, v.z};	
	v.x=rotMatrix[0][0]*coord[0]+rotMatrix[0][1]*coord[1]+rotMatrix[0][2]*coord[2]-v0.x;
	v.y=rotMatrix[1][0]*coord[0]+rotMatrix[1][1]*coord[1]+rotMatrix[1][2]*coord[2]-v0.y;
	v.z=rotMatrix[2][0]*coord[0]+rotMatrix[2][1]*coord[1]+rotMatrix[2][2]*coord[2]-v0.z;
}