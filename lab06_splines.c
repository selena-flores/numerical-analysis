#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "FPToolkit.c"
#define max_n 100

int clickandsave(double x[], double y[])
{
  double xp, yp, p[2];
  int numpoints;

  G_rgb(1, 0, 0); // red
  G_fill_rectangle(0, 0, 30, 30);

  G_rgb(0.5, 0.5, 0.5);
  numpoints = 0;
  while (0 == 0)
  {

    G_wait_click(p);
    xp = p[0];
    yp = p[1];

    if ((xp >= 0) && (xp <= 30) &&
        (yp >= 0) && (yp <= 30))
    {
      break;
    }

    G_fill_rectangle(xp - 2, yp - 2, 4, 4);

    x[numpoints] = xp;
    y[numpoints] = yp;
    numpoints++;
  }

  return numpoints;
}

int find_max_row_on_diagonal(double u[max_n][max_n], double v[max_n], int n, int i){
      
      int maxfabs = fabs(u[i][i]); 
      int besth = i ;
      
      for (int h = i ; h < n ; h++) {
        if (fabs(u[h][i]) > maxfabs) {
         maxfabs = fabs(u[h][i]); 
         besth = h; 
       }
      }
      if (maxfabs == 0) {
        return -1;
      }
      return besth;
}

void swap_row(int row, int index, double u[max_n][max_n], double v[max_n], int n){
  double temp[max_n][max_n];
  double t;
  for(int i = 0; i < n; i++){
    t = u[index][i];
    u[index][i] = u[row][i];
    u[row][i] = t;
  }
      t = v[index];
      v[index] = v[row];
      v[row] = t;
}

int solve(double u[max_n][max_n], double v[max_n], int n){

    
    for(int i = 0; i < n; i++){
      for(int k = i + 1; k < n; k++){

      int besth = find_max_row_on_diagonal(u, v, n, i);
      if(besth == -1) return -1;
      swap_row(i, besth, u, v, n);

        double q = -u[k][i]/u[i][i];
        for(int j = 0; j < n; j++){ // move along the row
          u[k][j] += q*u[i][j];
        }
        v[k] += q*v[i];
      }
    }
    // end of loop, in row echelon form
    // assume no row or column is full of zeroes
    v[n-1] /= u[n-1][n-1];
    u[n-1][n-1] /= u[n-1][n-1];

    // get zeroes along top-right
    for(int i = n - 1; i >= 0; i--){ // move along diagonal
      for(int k = i - 1; k >= 0; k--){ // move up rows
        double q = -u[k][i]/u[i][i];
        for(int j = n - 1; j >= i; j--){ // move along the row from right
          u[k][j] += q*u[i][j];
        }
        v[k] += q*v[i];
      }
    }// end of loop, // zeroes everywhere except diagonal

    for(int i = 0; i < n; i++){ // get 1's along diagonal
      v[i] /= u[i][i];
      u[i][i] /= u[i][i];
    }

    for(int i = 0; i < n; i++){
      for(int j = 0; j < n; j++){
        if(isnan(u[i][j])) return -1;
      }
    }

    return 1;

}

void print_system(double u[max_n][max_n], double v[max_n], int n){
  for(int i = 0; i < n; i++){
    for(int j = 0; j < n; j++){
      printf("%10.2lf", u[i][j]);
    }
    printf(" | %20.16lf", v[i]);
    printf("\n");
  }
}

void load_diagonal(double u[max_n][max_n], double x[8], int n){
  u[0][0] = 1;
  for(int i = 1; i < n; i++){
    u[2*i][2*i] = -(x[i + 1] - x[i - 1]);
    u[2*i - 1][2*i - 1] = (x[i] - x[i - 1])*(x[i] - x[i - 1]);
  }
    u[n - 1][n - 1] = 2*(x[(n + 2) / 2 - 1] - x[(n + 2)/2 - 2]);
}

void load_zeroes(double u[max_n][max_n], double v[max_n], int n){
  for(int i = 0; i < n; i++){
    for(int j = 0; j < n; j++){
      u[i][j] = 0;
    }
    v[i] = 0;
  }
}

void load_upper_diagonal(double u[max_n][max_n], double x[8], int n){
  u[0][1] = -(x[1] - x[0]);
  for(int i = 1; i < n; i++){
    u[2*i - 1][2*i - 1 + 1] = (x[i + 1] - x[i]);
    u[2*i][2*i + 1] = (x[i] - x[i - 1])*(x[i + 1] - x[i]);
  }
}

void load_lower_diagonal(double u[max_n][max_n], double x[8], int n){
  u[1][0] = (x[1] - x[0]);
  for(int i = 1; i < n; i++){
    u[2*i - 1 + 1 - 1][2*i - 1 - 1] = (x[i] - x[i - 1]);
    u[2*i + 1 - 1][2*i - 1] = (x[i] - x[i - 1])*(x[i] - x[i - 1]);
  }
    u[n - 1][n - 1 - 1] = 1;
}

void load_v(double v[max_n], double x[8], double y[8], int n){
  v[0] = 0;
  for(int i = 1; i < n - 1; i++){
    v[2*i - 1] = (y[i + 1] - y[i])/(x[i + 1] - x[i]) - (y[i] - y[i - 1])/(x[i] - x[i - 1]);
    v[2*i] = -(y[i + 1] - y[i])/(x[i + 1] - x[i]) + (y[i] - y[i - 1])/(x[i] - x[i - 1]);
  }
  v[n - 1] = 0;
}

void create_system(double u[max_n][max_n], double v[max_n], double x[8], double y[8], int n){

  load_zeroes(u, v, n);
  load_diagonal(u, x, n);
  load_upper_diagonal(u, x, n);
  load_lower_diagonal(u, x, n);
  load_v(v, x, y, n);

}

int main()
{

  int n = 8;
  // double x[max_n], y[max_n];

  double x[8] = {86, 177, 249, 311, 381, 416, 455, 551};
  double y[8] = {181, 211, 315, 460, 459, 311, 195, 155};
  double u[max_n][max_n], v[max_n];
  n *= 2;
  n -= 2;
  create_system(u, v, x, y, n);
  print_system(u, v, n);
  printf("----------------------------solved-------------\n");
  solve(u, v, n);
  print_system(u, v, n);

  // G_init_graphics(600, 600);
  // G_rgb(0, 0, 0);
  // G_clear();
  // n = clickandsave(x, y);
  // G_wait_key();
}