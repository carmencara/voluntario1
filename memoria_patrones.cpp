//---------------------------------------------------------------------
//                                                                    |
//         ALGORITMO DE METROPOLIS PARA EL MODELO DE HOPFIELD         |
//                                                                    |
//  Objetivo: Estudiar cómo depende el número de patrones recordados  |
//  en función del número de patrones almacenados.                    |
//                                                                    |
//  Procedimiento:                                                    |
//    1. Crear 50 patrones aleatorios (C = 1,2,3,...,P)               |
//    2. Para cada número de patrones C, ejecutar el programa 100     |
//       veces y comprobar cuántos patrones R se recuerdan.           |
//    3. Promediar los valores de los patrones recordados R.          |
//                                                                    |
//  Fijado:   N = 20    T = 1E-4                                      |
//            P = 50 (número máximo de patrones almacenados)          |
//                                                                    |
//---------------------------------------------------------------------

#include <cstdlib>    // Funciones srand() y rand()
#include <ctime>      // Función time
#include <iostream>
#include <fstream>    // Usar ficheros
#include <cmath>
#include "gsl_rng.h"  //Libreria para generación de números aleatorios
#include <sys/time.h>

#define N 20          // Tamaño de la matriz de spines
#define P 50          // Número máximo de patrones aleatorios C creados

static double w[N*N][N*N];    // Pesos sinápticos, w[N*i+j][k*N+l] es la interacción entre (i,j) y (k,l)

using namespace std;

// FUNCIONES QUE SE VAN A UTILIZAR
void LeerFichero(int q[][N*N]);
void Calcular_w(int q[][N*N], double w[N*N][N*N], double a[]);
void Calcular_t(double t[][N], int q[][N*N], double a[], double w[N*N][N*N]);
double Calcular_deltaE(int s[][N], int q[][N*N], double t[][N], double a[], double w[N*N][N*N], int x, int y);
void Calcula_solap(double solap[P], int s[][N], int q[][N*N], double a[], int n_patron);
long int SemillaTiempo();
int Random_int(int max, gsl_rng *tau);
double Random_double(gsl_rng *tau);

/*---------------------------------------------------------------------
|                           FUNCIÓN PRINCIPAL                         |
---------------------------------------------------------------------*/
int main()
{   
    // ------------------- DECLARACIÓN DE VARIABLES -------------------
    int N2;                // Paso de Montecarlo
    int i,j,k,l,n,m;       // Contadores
    int s[N][N];           // Matriz de spines
    int q[P][N*N];         // Matriz del patrón inicial
    int x,y;               // Coordenadas aleatorias en la red
    int simulacion;        // Número de simulaciones que se repiten
    int n_patron;          // Iteración en el número de patrones (desde 1 hasta P)
    int patron_recordado;  // Número de patrones recordados
    double t[N][N];        // Matriz de umbral de disparo (theta)
    double T;              // Temperatura
    double aux;            // Real auxiliar para asignar cond. iniciales aleatorias
    double deltaE;         // DeltaE = 2s[n][m](s[n+1][m] + s[n-1][m] + s[n][m+1] + s[n][m-1])
    double expo;           // expo=exp(-deltaE/T)
    double p;              // p = min(1, expo)
    double xi;             // Número aleatorio entre 0 y 1
    double a[P];           // Variable a que aparece en la expresión de los pesos sinápticos
    double solap[P];       // Solapamiento
    double iter;           // Número de iteraciones
    bool aleatorio;        // Elegir si los elementos de la red son aleatorios/constantes
    ofstream recordados;   // Fichero para guardar los patrones recordados
    ifstream patron;       // Fichero con el patrón inicial a recordar "pollitos.txt"
    gsl_rng *tau;          // Para números aleatorios con gsl
    int semilla;           // Para números aleatorios con gsl


    // ------------------ CONFIGURACIONES INICIALES -------------------
    // Paso de Montecarlo
    N2=N*N;
    // Temperatura inicial
    T=1E-4;
    // Establecer la semilla para generar números aleatorios
    semilla=SemillaTiempo();
    tau = gsl_rng_alloc(gsl_rng_taus);
    gsl_rng_set(tau,semilla);

    // Abrir ficheros para guardar resultados
    recordados.open("patrones_recordados.txt");

    // -------------- CREO PATRONES INICIALES ALEATORIOS ---------------
    for(m=0; m<P; m++) // Para cada patrón
    {
        for(i=0;i<N;i++)
        {
            for(j=0;j<N;j++)
            {
                // Inicializo aleatoriamente algunos spines a +1 y otros a -1
                aux=Random_int(100,tau);
                if(aux<50) q[m][N*i+j]=0;
                else q[m][N*i+j]=1;
            }
        }
    }

    // --------------------- CALCULAR VARIABLE "a" ---------------------
    for(m=0;m<P;m++){
        a[m]=0;
        for(i=0;i<N;i++){
            for(j=0;j<N;j++){
                a[m]=a[m]+1.0*q[m][i*N+j];
            }
        }
        a[m]=a[m]/N2;
    }
    
    // ------------------- CALCULAR PESOS SINÁPTICOS -------------------
    Calcular_w(q, w, a);

    // ------------------ CALCULAR UMBRAL DE DISPARO -------------------
    Calcular_t (t, q, a, w);

    // -------------------- ALGORITMO DE MONTECARLO --------------------
    // Repito el programa 100 veces
    for (simulacion=0; simulacion<100; simulacion++)
    {
        // Y lo repito para cada uno de los patrones
        for (n_patron=1; n_patron<=P; n_patron++)
        {
            // Inicializo el contador de patrones recordados
            patron_recordado = 0;

            // Inicializo las neuronas de la red aleatoriamente
            for(i=0;i<N;i++)
            {
                for(j=0;j<N;j++)
                {
                    aux=Random_int(100,tau);
                    if(aux<50) s[i][j]=0;
                    else s[i][j]=1;
                }
            }

            // Itero en el tiempo (pasos de Montecarlo)
            iter = 100*N2;
            for(n=0;n<iter;n++)
            {
                // Elijo neurona aleatoria
                x=rand()%N;
                y=rand()%N;
                // Calculo delta E
                deltaE = Calcular_deltaE(s, q, t, a, w, x, y);
                // Evalúo p = min(1, exp(−deltaE/T))-
                expo=exp(-1.0*deltaE/T);
                if(expo<1) p=expo;
                else p=1;
                // Genero número aleatorio xi entre [0,1]. Si xi<p, cambio el spin
                xi=Random_double(tau);
                if(xi<p) s[x][y]=1-s[x][y];                
            }

            // Calculo los solapamientos
            Calcula_solap(solap, s, q, a, n_patron);

            // Calculo el número de patrones recordados
            for (m=0; m<n_patron; m++)
            {
                if (abs(solap[m])>0.75) patron_recordado++;
            }

            // Escribo en el fichero 'patrones_recordados.txt'
            // Columnas -> patrones creados (1,2,...50)
            // Filas -> cada una de las 100 simulaciones
            recordados << patron_recordado << '\t';
        }
        // Salto de línea para cada simulación
        recordados << endl;
    }

    // Cerrar el fichero y finalizar
    recordados.close();
    return 0;
}

/*---------------------------------------------------------------------
|           FUNCIONES PARA CALCULAR VARIABLES DEL ALGORITMO           |
---------------------------------------------------------------------*/
// Función que calcula el peso sináptico omega
void Calcular_w(int q[][N*N], double w[N*N][N*N], double a[])
{
    int i,j,k,l,n;    // Contadores
    
    for(i=0;i<N;i++)
    {
        for(j=0;j<N;j++)
        {
            for(k=0;k<N;k++)
            {
                for(l=0;l<N;l++)
                {
                    w[N*i+j][N*k+l]=0;
                    if((i==k)&&(j==l)) w[N*i+j][N*k+l]=0;
                    else
                    {
                        for(n=0; n<P; n++)
                        {
                            w[N*i+j][N*k+l]=w[N*i+j][N*k+l]+(1.0*q[n][i*N+j]-a[n])*(1.0*q[n][k*N+l]-a[n])/(N*N);
                        }
                    }
                }
            }
        }
    }

    return;    
}

// Función que calcula el umbral de disparo Theta
void Calcular_t (double t[][N], int q[][N*N], double a[], double w[N*N][N*N])
{
    int i,j,k,l;    // Contadores
    
    for(i=0;i<N;i++)
    {
        for(j=0;j<N;j++)
        {
            t[i][j] = 0;
            for(k=0;k<N;k++)
            {
                for(l=0;l<N;l++)
                {

                    t[i][j] = t[i][j] + 0.5*w[i*N+j][k*N+l];
                }
            }
        }
    }
    return;
}

// Función que devuelve deltaE a partir de una posición (x,y) aleatoria
double Calcular_deltaE(int s[][N], int q[][N*N], double t[][N], double a[], double w[N*N][N*N], int x, int y)
{
    // x, y son las posiciones aleatorias en la red
    int i,j;
    double deltaE;

    deltaE = 0;
    for(i=0;i<N;i++)
    {
        for(j=0;j<N;j++)
        {
            deltaE = deltaE + (w[x*N+y][i*N+j]*s[i][j]);
        }
    }
    deltaE = (-0.5*deltaE+t[x][y])*(1-2*s[x][y]);

    return deltaE;
}

// Función que calcula el SOLAPAMIENTO
void Calcula_solap(double solap[P], int s[][N], int q[][N*N], double a[], int n_patron)
{
    int i, j, m;    // Contadores
    
    for (m=1; m<=n_patron; m++)
    {
        solap[m] = 0.0;
        for(i=0;i<N;i++)
        {
            for(j=0;j<N;j++)
            {
                solap[m] = solap[m] + (q[m][i*N+j]-a[m])*(s[i][j]-a[m]);
            }
        }
        solap[m] = 1.0*solap[m]/(a[m]*(1-a[m])*N*N);
    }

    return;
}

/*---------------------------------------------------------------------
|                  FUNCIONES PARA NÚMEROS ALEATORIOS                  |
---------------------------------------------------------------------*/

// Función que crea una semilla para generar números aleatorios, basada en el tiempo actual
long int SemillaTiempo()
{
    struct timeval tv;
    gettimeofday(&tv,0);
    return (tv.tv_sec + tv.tv_usec);
}

// Función que genera un número ENTERO ALEATORIO en un rango [min,max]
int Random_int(int max, gsl_rng *tau)
{   
    // Generar un número aleatorio entre 0 y max (ambos incluidos)
    int numero_aleatorio = gsl_rng_uniform_int(tau,max+1);

    return numero_aleatorio;
}

// Función que genera un número REAL ALEATORIO en un rango [0,1]
double Random_double(gsl_rng *tau)
{
    // Generar un número aleatorio entre 0 y 1
    double numero_aleatorio = gsl_rng_uniform(tau);

    return numero_aleatorio;

}