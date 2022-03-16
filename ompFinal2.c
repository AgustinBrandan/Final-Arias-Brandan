#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <sys/sysinfo.h>
#include <sys/time.h>

void Regla_Rectangulo(double, double,int);
void Regla_Trapecio(double, double,int);
void Regla_Simpson(double,double,int);
void Regla_PuntoMedio(double,double,int);

double fx(double x);



int main (int argc, char *argv[]) {
  int  num_threads, tid, chunk = 1;

  if (argc > 2)
    chunk = atoi(argv[1]);

  
  double a;   //limite inferior
  double b;   //limite superior 
  int n;

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
  printf("\n");


  struct timeval start, end;
   
    gettimeofday(&start, NULL);

   
    #pragma omp parallel shared(a,b,n) private (tid)
    {
       #pragma omp sections nowait
       {
          #pragma omp section
          {
               tid = omp_get_thread_num();
               printf("\nHola! Resolvi con el hilo %d el metodo de Simpson\n", tid);
               Regla_Simpson(a,b,n);
          }
          #pragma omp section
          {
               tid = omp_get_thread_num();
               printf("\nHola! Resolvi con el hilo %d el metodo del Tapecio\n", tid);
               Regla_Trapecio(a,b,n);
          }
          #pragma omp section
          {
               tid = omp_get_thread_num();
               printf("\nHola! Resolvi con el hilo %d el metodo del Rectangulo\n", tid);
               Regla_Rectangulo(a,b,n);
          }
          #pragma omp section
          {
               tid = omp_get_thread_num();
               printf("\nHola! Resolvi con el hilo %d el metodo del Punto Medio\n", tid);
               Regla_PuntoMedio(a,b,n);
          }
        }
    }
   gettimeofday(&end, NULL);
    double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
         end.tv_usec - start.tv_usec) / 1.e6;
    double tiempo = delta*1000000; //= /1.e-6
    printf("\nTiempo en ejecucion: %fus \n",tiempo);

  return 0;
}

double fx(double x){
  double f;
  f = (x);
  return f;
}

void Regla_Rectangulo(double limiteinf, double limitesup,int nro_intervalos){
     
     double integral = 0;
     int i, tid;
     double step = (limitesup - limiteinf) / nro_intervalos;
     #pragma omp parallel shared(step,integral,nro_intervalos,limiteinf,limitesup) private(tid)
	   {
        
	    	tid = omp_get_thread_num();
        printf("Hola soy el hilo: %d\n",tid);
	    	#pragma omp for schedule(static) nowait 
	    	for(i = 0; i <= nro_intervalos;i++){
	      		integral	= integral + (step * fx(limiteinf + (i-1)*step)); 
            //fprintf(stdout,"(Hilo de Rectangle Method %d) suma = %lf\n",tid,integral);
	      }
	   }
     printf("El resultado de la integral del metodo del rectangulo es: %f\n", integral);
}


void Regla_Trapecio(double limiteinf, double limitesup, int nro_intervalos){
  
  double h, integral = 0, sum = 0;
  int i, tid;
  h = fabs(limitesup-limiteinf)/nro_intervalos;

  #pragma omp parallel shared(h,integral,nro_intervalos,limiteinf,limitesup) private(tid)
	{
     tid = omp_get_thread_num();
     #pragma omp for schedule(static) nowait
     for (i = 0; i <= nro_intervalos; i++){
       sum += fx(limiteinf + (i*h) );
     }
  }
  integral = (h/2)*( fx(limiteinf) + fx(limitesup) + 2*sum );
  printf("El resultado de la integral del metodo del trapezoide es: %f\n", integral);
}

void Regla_Simpson(double limiteinf, double limitesup, int nro_intervalos){

    double h,sum = 0, integral = 0;
    int i,tid;
    h = fabs( limitesup - limiteinf) / nro_intervalos;
    #pragma omp parallel shared(h,integral,nro_intervalos,limiteinf,limitesup) private(tid)
	  {
        tid = omp_get_thread_num();
        #pragma omp for schedule(static) nowait
        for (i = 0; i <= nro_intervalos; i++){
          if ( i%2 == 0){
            sum += 2 * fx( limiteinf + i*h );
          }
          else{
            sum += 4 * fx( limiteinf + i*h );      
          }
        }
    }
    integral = (h/3) * (fx(limiteinf) + fx(limitesup) + sum);
    printf("El resultado de la integral del metodo de simpson 1/3 es: %f\n", integral);
}

void Regla_PuntoMedio(double limiteinf, double limitesup, int nro_intervalos){
    
    double h, sum = 0, integral = 0;
    int i,tid;
    h = (limitesup-limiteinf)/nro_intervalos;
    #pragma omp parallel shared(h,integral,nro_intervalos,limiteinf,limitesup) private(tid)
	  {
       tid = omp_get_thread_num();
       #pragma omp for schedule(static) nowait
       for (i = 0; i <= nro_intervalos; i++){
         sum += fx( (limiteinf + h/2) + i*h);
       }
    }
    integral = h*sum;
    printf("El resultado de la integral del metodo del punto medio es: %f\n", integral);
}