#include <Field.hpp>

int main()
{
  const int Nx = 100;
  const int Ny = 100;
  const int Nz = 200;

  // allocate storage
  double ***T;
  double *x, *y, *z;

  T = new double**[Nx];
  for(int i = 0; i < Nx; i++)
  {
    T[i] = new double*[Ny];
    for(int j = 0; j < Ny; j++)
      T[i][j] = new double[Nz];
  }
  x = new double[Nx];
  y = new double[Ny];
  z = new double[Nz];

  // setup coordinates
  double xmin = -5, xmax =  5, dx = (xmax - xmin)/(Nx-1);
  double ymin = -5, ymax =  5, dy = (ymax - ymin)/(Ny-1);
  double zmin =  0, zmax = 20, dz = (zmax - zmin)/(Nz-1);

  for(int i = 0; i < Nx; i++)
    x[i] = xmin + i*dx;
  for(int i = 0; i < Ny; i++)
    y[i] = ymin + i*dy;
  for(int i = 0; i < Nz; i++)
    z[i] = zmin + i*dz;

  // set initial temperature distribution to T(x,y,z) = x*y*z
  for(int i = 0; i < Nx; i++)
  {
    for(int j = 0; j < Ny; j++)
    {
      for(int k = 0; k < Nz; k++)
      {
        T[i][j][k] = x[i]*y[j]*z[k];
      }
    }
  }
        
  // do work
 
  // cleanup
  for(int i = 0; i < Nx; i++)
  {
    for(int j = 0; j < Ny; j++)
      delete[] T[i][j];
    delete[] T[i];
  }
  
  delete[] x;
  delete[] y;
  delete[] z;

  
  return 0;
}
