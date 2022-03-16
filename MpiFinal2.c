#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <stddef.h>
#include <time.h>
#include <sys/sysinfo.h>
#include <sys/time.h>

double rectangle_rule(double, double,int);
double trapezoid_rule(double, double,int);
double simpson_rule(double,double,int);
double midpoint_rule(double,double,int);

double function(double x);

int main(int argc, char * argv[]){
	int rank = 0,size = 0;

	double a,b;
	int n;
  double result_rectangle = 0, result_midpoint = 0,result_trapezoid = 0,result_simpsons = 0,
  total_result_rectangle = 0,total_result_trapezoid = 0,total_result_simpson = 0,total_result_midpoint = 0;
	
  struct timeval start, end;

  gettimeofday(&start, NULL);

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
	}
	
	MPI_Bcast(&a,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Bcast(&b,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);

  if(rank < 4){
     result_rectangle = rectangle_rule(a,b,n);
  }else if(rank < 8) { 
    result_midpoint = midpoint_rule(a,b,n);
  }
  else if (rank <12) { 
    result_trapezoid = trapezoid_rule(a,b,n);
  }
  else if ( rank<16 ){
    result_simpsons = simpson_rule(a,b,n);
  }

  if( rank == 0 ){ 
    MPI_Reduce(&result_rectangle,&total_result_rectangle,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
    printf("Resultado total metodo del rectangulo: %f\n",total_result_rectangle);
  }
  if ( rank == 4){
		MPI_Reduce(&result_midpoint,&total_result_midpoint,1,MPI_DOUBLE,MPI_SUM,4,MPI_COMM_WORLD);
		printf("Resultado total metodo del punto medio %f\n",total_result_midpoint);
  }
  if (rank == 8){
		MPI_Reduce(&result_trapezoid,&total_result_trapezoid,1,MPI_DOUBLE,MPI_SUM,8,MPI_COMM_WORLD);
		printf("Resultado total metodo trapezoidal %f\n",total_result_trapezoid);  
  }
  if (rank == 12){
    MPI_Reduce(&result_simpsons,&total_result_simpson,1,MPI_DOUBLE,MPI_SUM,12,MPI_COMM_WORLD);
		printf("Resultado total metodo de Simpson %f\n",total_result_simpson);
  }

 	MPI_Finalize();	

	if(rank == 0){
		 gettimeofday(&end, NULL);
    double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
         end.tv_usec - start.tv_usec) / 1.e6;
    double tiempo = delta*1000000; //= /1.e-6
    printf("\nTiempo en ejecucion: %fs \n",tiempo);
	}
	
}

double function(double x){
  double f;
  f = (x);
  return f;
}

double rectangle_rule(double limiteinf, double limitesup,int nro_intervalos){

  int rank = 0, size = 0;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Comm_size(MPI_COMM_WORLD,&size);
    /* step = (4 - 2) / 2 = 1;
       step_local = 1/4 = 0.25;

       local_Begin = 2 ;                  //proceso 0 va a ir en el for desde el 2 hasta el 2.50;
       local_end = 2 + 0.25*2 = 2.50 ;                
    */
    /*
       step = (4-2)/2 = 1;
       step_local = 1/4 = 0.25;

       local_Begin = 2 + 1*0.25*2 = 2.50
       local_end = 2.50 + 0.25*2 = 3
    */
  double step = (limitesup - limiteinf) / nro_intervalos; 
  double step_local = step/size;

  double local_Begin = limiteinf + rank*step_local*nro_intervalos;
  double local_end = local_Begin + step_local*nro_intervalos;
	
   double integral = 0;
   int i;
   for (i = local_Begin; i <= local_end; i++){
       integral += step * function(limiteinf + (i-1) * step);
    }
  return integral;
}

double trapezoid_rule(double limiteinf, double limitesup, int nro_intervalos){
  int rank = 0, size = 0;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Comm_size(MPI_COMM_WORLD,&size);

  double step = (limitesup - limiteinf) / nro_intervalos; 
  double step_local = step/size;

  double local_Begin = limiteinf + rank*step_local*nro_intervalos;
  double local_end = local_Begin + step_local*nro_intervalos;
  
  double integral = 0, sum = 0;
  int i;

  for (i = local_Begin; i <= local_end; i++){
    sum += function(limiteinf + (i*step) );
  }
  integral = (step/2)*( function(limiteinf) + function(limitesup) + 2*sum ); 
  return integral;
}

double simpson_rule(double limiteinf, double limitesup,int nro_intervalos){
  int rank = 0, size = 0;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Comm_size(MPI_COMM_WORLD,&size);

  double step = (limitesup - limiteinf) / nro_intervalos; 
  double step_local = step/size;

  double local_Begin = limiteinf + rank*step_local*nro_intervalos;
  double local_end = local_Begin + step_local*nro_intervalos;

  double sum = 0, integral = 0;
  int i;
    for (i = local_Begin; i <= local_end; i++){
      if ( i%2 == 0){
        sum += 2 * function( limiteinf + i*step );
      }
      else{
        sum += 4 * function( limiteinf + i*step );      
      }
    }
  integral = (step/3) * (function(limiteinf) + function(limitesup) + sum);
  return integral;
}

double midpoint_rule(double limiteinf, double limitesup, int nro_intervalos){
  int rank = 0, size = 0;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Comm_size(MPI_COMM_WORLD,&size);

  double step = (limitesup - limiteinf) / nro_intervalos; 
  double step_local = step/size;

  double local_Begin = limiteinf + rank*step_local*nro_intervalos;
  double local_end = local_Begin + step_local*nro_intervalos;
    
  double sum = 0, integral = 0;
  int i;
    
    for (i = local_Begin; i <= local_end; i++){
       sum += function( (limiteinf + step/2) + i*step);
    }
  integral = step*sum;
  return integral;
}

