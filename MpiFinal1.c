#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <stddef.h>
#include <time.h>
#include <sys/sysinfo.h>
#include <sys/time.h>

void rectangle_rule(double, double,int);
void trapezoid_rule(double, double,int);
void simpson_rule(double,double,int);
void midpoint_rule(double,double,int);

double function(double x);


int main(int argc, char * argv[]){
	int rank = 0,size = 0;

	double a,b;
	int n;

 struct timeval start, end;


	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Comm_size(MPI_COMM_WORLD,&size);

	if(rank == 0){
		printf("Ingrese el valor del limite inferior:\n");
		scanf("%lf",&a);
		printf("Ingrese el valor del limite superior:\n");
		scanf("%lf",&b);
		if (a >= b){
			printf("El limite inferior tiene que ser menor al limite superior\n");
			return 0;
		}
		printf("Ingrese la cantidad de intervalos:\n");
		scanf("%d",&n);

      gettimeofday(&start, NULL);
	}
	
	MPI_Bcast(&a,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Bcast(&b,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);

		if ( rank== 0 ){
			printf("\nProceso : %d de %d del metodo del rectangulo.\n",rank,size);
			rectangle_rule(a,b,n);
    }

		if ( rank == 1 ){
			printf("\nProceso : %d de %d del metodo del punto medio.\n",rank,size);
			midpoint_rule(a,b,n);
    }
    if ( rank == 2 ){
			printf("\nProceso : %d de %d del metodo del trapecio.\n",rank,size);
			trapezoid_rule(a,b,n);
    }
    if ( rank == 3 ){
			printf("\nProceso : %d de %d del metodo de simpson 1/3.\n",rank,size);	
			simpson_rule(a,b,n);
    }
    
 	MPI_Finalize();	

	if(rank == 0){
	  gettimeofday(&end, NULL);
    double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
         end.tv_usec - start.tv_usec) / 1.e6;
    //double tiempo = delta*1000000; //= /1.e-6  
    printf("\nTiempo en ejecucion: %fs \n",delta);
	}	
}

double function(double x){
  double f;
  f = (x);
  return f;
}

void rectangle_rule(double limiteinf, double limitesup,int nro_intervalos){
     
     double integral = 0;
     int i;

     double step = (limitesup - limiteinf) / nro_intervalos;
     for (i = 0; i <= nro_intervalos; i++){
         integral += step * function(limiteinf + (i-1) * step);
     }

     printf("El resultado de la integral del metodo del rectangulo es: %f\n", integral);
}

void trapezoid_rule(double limiteinf, double limitesup, int nro_intervalos){
  
  double h, integral = 0, sum = 0;
  int i;

  h = fabs(limitesup-limiteinf)/nro_intervalos;
  for (i = 0; i <= nro_intervalos; i++){
    sum += function(limiteinf + (i*h) );
  }
  integral = (h/2)*( function(limiteinf) + function(limitesup) + 2*sum );
  
  printf("El resultado de la integral del metodo del trapezoide es: %f\n", integral);
}

void simpson_rule(double limiteinf, double limitesup, int nro_intervalos){

    double h,sum = 0, integral = 0;
    int i;

    h = fabs( limitesup - limiteinf) / nro_intervalos;
    for (i = 0; i <= nro_intervalos; i++){
      if ( i%2 == 0){
        sum += 2 * function( limiteinf + i*h );
      }
      else{
        sum += 4 * function( limiteinf + i*h );      
      }
    }
    integral = (h/3) * (function(limiteinf) + function(limitesup) + sum);
    
    printf("El resultado de la integral del metodo de simpson 1/3 es: %f\n", integral);
}

void midpoint_rule(double limiteinf, double limitesup, int nro_intervalos){
    
    double h, sum = 0, integral = 0;
    int i;
    
    h = (limitesup-limiteinf)/nro_intervalos;
    for (i = 0; i <= nro_intervalos; i++){
       sum += function( (limiteinf + h/2) + i*h);
    }
    integral = h*sum;

   printf("El resultado de la integral del metodo del punto medio es: %f\n", integral);
}

