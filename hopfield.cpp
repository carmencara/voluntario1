//---------------------------------------------------------------------
//                                                                    |
//         ALGORITMO DE METROPOLIS PARA EL MODELO DE HOPFIELD         |
//                                                                    |
//  Estudio de un solo patrón:                                        |
//    - Capacidad de la red de recordar el patrón "pollitos.txt"      |
//       · Condición inicial: patrón deformado                        |
//       · Condición inicial: aleatoria                               |
//    - Evolución del solapamiento para distintas temperaturas:       |
//      1E-1, 5E-1, 1E-2, 5E-2, 1E-3, 5E-3, 1E-4                      |
//                                                                    |
//---------------------------------------------------------------------

#include <cstdlib>    // Funciones srand() y rand()
#include <ctime>      // Función time
#include <iostream>
#include <fstream>    // Usar ficheros
#include <cmath>
#include "gsl_rng.h"  //Libreria para generación de números aleatorios
#include <sys/time.h>

#define N 150         // Tamaño de la matriz de spines

using namespace std;

// FUNCIONES QUE SE VAN A UTILIZAR
long int SemillaTiempo();
int Random_int(int max, gsl_rng *tau);
double Random_double(gsl_rng *tau);
//double DeltaEnergia(int s[N][N], double w[N][N][N][N], int x, int y);

/*---------------------------------------------------------------------
|                           FUNCIÓN PRINCIPAL                         |
---------------------------------------------------------------------*/
int main()
{   
    // ------------------- DECLARACIÓN DE VARIABLES -------------------
    int N2;                // Paso de Montecarlo
    int i,j,k,l,n;         // Contadores
    int s[N][N];           // Matriz de spines
    int q[N][N];           // Matriz del patrón inicial
    int x,y;               // Coordenadas aleatorias en la red
    double deformacion;    // Porcentaje de neuronas deformadas del patrón inicial
    double w[N][N][N][N];  // Matriz de pesos sinápticos
    double t[N][N];        // Matriz de umbral de disparo (theta)
    double T;              // Temperatura
    double aux;            // Real auxiliar para asignar cond. iniciales aleatorias
    double deltaE;         // DeltaE = 2s[n][m](s[n+1][m] + s[n-1][m] + s[n][m+1] + s[n][m-1])
    double expo;           // expo=exp(-deltaE/T)
    double p;              // p = min(1, expo)
    double xi;             // Número aleatorio entre 0 y 1
    double a;              // Variable a que aparece en la expresión de los pesos sinápticos
    double iter;           // Número de iteraciones
    double PMC;            // Contador para los pasos de Montecarlo (PMC)
    bool aleatorio;        // Elegir si los elementos de la red son aleatorios/constantes
    ofstream datos;        // Fichero para guardar los spines
    ifstream patron;       // Fichero con el patrón inicial a recordar "pollitos.txt"
    gsl_rng *tau;          // Para números aleatorios con gsl
    int semilla;           // Para números aleatorios con gsl


    // ------------------ CONFIGURACIONES INICIALES -------------------
    // Guardar el patrón inicial en la matriz q[N][N]
    patron.open("pollitos.txt");
    for(i=0;i<N;i++)
    {
        for(j=0;j<N;j++)
        {
            patron >> q[i][j];
        }
    }
    // Elegir si los elementos de la red son aleatorios (true) o los del patrón (false)
    aleatorio=false;
    // Paso de Montecarlo
    N2=N*N;
    // Temperatura inicial
    T=1E-2;
    // Establecer la semilla para generar números aleatorios
    semilla=SemillaTiempo();
    tau = gsl_rng_alloc(gsl_rng_taus);
    gsl_rng_set(tau,semilla);

    // Abrir fichero
    datos.open("datos_1patron.txt");

    // -------------------- CONDICIONES INICIALES ---------------------
    if(aleatorio==true) // Spines iniciales aleatorios
    {
        for(i=0;i<N;i++)
        {
            for(j=0;j<N;j++)
            {
                // Inicializo aleatoriamente algunos spines a +1 y otros a -1
                aux=Random_int(100,tau);
                if(aux<50) s[i][j]=-1;
                else s[i][j]=1;
            }
        }
    }
    else // Spines iniciales definidos: patrón inicial deformado un %
    {
        deformacion = 0.5;  // Porcentaje de neuronas deformadas
        for(i=0;i<N;i++)
        {
            for(j=0;j<N;j++)
            {
                s[i][j]=q[i][j]; // La red es igual que el patrón inicial
            }
        }
        for(i=0;i<trunc(N2*deformacion);i++)
        {
            // Elijo una neurona aleatoria
            x=rand()%N;
            y=rand()%N;
            // Le cambio su valor de spin (0 → 1 / 1 → 0)
            s[x][y]=1-s[x][y];
        }
    }

    // ---------------------- GUARDAR EN FICHERO -----------------------
    for(i=0;i<N;i++)
    {
        for(j=0;j<N-1;j++)
        {
            datos << s[i][j] << ", ";
        }
        datos << s[i][N-1] << endl;
    }
    datos << endl;

    // --------------------- CALCULAR VARIABLE "a" ---------------------
    a = 0;
    for(i=0;i<N;i++)
    {
        for(j=0;j<N;j++)
        {
            a = a + q[i][j];
        }
    }
    a = a / N2;

    // ------------------ CALCULAR PESOS SINÁPTICOS --------------------
    for(i=0;i<N;i++)
    {
        for(j=0;j<N;j++)
        {
            for(k=0;k<N;k++)
            {
                for(l=0;l<N;l++)
                {
                    w[i][j][k][l] = 1 / N2*(q[i][j]-a)*(q[k][l]-a);
                }
            }
        }
    }

    // ------------------ CALCULAR UMBRAL DE DISPARO -------------------
    for(i=0;i<N;i++)
    {
        for(j=0;j<N;j++)
        {
            t[i][j] = 0;
            for(k=0;k<N;k++)
            {
                for(l=0;l<N;l++)
                {
                    t[i][j] = t[i][j] + w[i][j][k][l];
                }
            }
            t[i][j] = t[i][j] / 2;
        }
    }

    // -------------------- ALGORITMO DE MONTECARLO --------------------
    iter = 100*N2;
    PMC = 0; // Inicializo los PMC
    for(n=1;n<=iter;n++)
    {
        // ------------------- ELIJO (n,m) ALEATORIO -------------------
        x=rand()%N;
        y=rand()%N;

        // --------------------- CÁLCULO DE deltaE ---------------------
        //deltaE=DeltaEnergia(s, w, x, y);
        deltaE=0;

        // ------------ EVALUAR p = min(1, exp(−deltaE/T)) -------------
        expo=exp(-1.0*deltaE/T);
        if(expo<1) p=expo;
        else p=1;

        // ---------- GENERAR NÚMERO ALEATORIO xi ENTRE [0,1] ----------
        // ------------- SI xi<p, CAMBIAR EL SIGNO DEL SPIN ------------
        xi=Random_double(tau);
        if(xi<p) s[x][y]=1-s[x][y];

        // ----------------- ESCRIBO LOS NUEVOS SPINES -----------------
        if(n%N2==0) // Sólo lo escribo en cada paso de Montecarlo
        {
            for(i=0;i<N;i++)
            {
                for(j=0;j<N-1;j++)
                {
                    datos << s[i][j]<<", ";
                }
                datos << s[i][N-1] << endl;
            }
            datos << endl;
            PMC++;
        }
    }

    // Cerrar el fichero y finalizar
    patron.close();
    datos.close();
    return 0;
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

/*---------------------------------------------------------------------
|              FUNCIONES PARA EL ALGORITMO DE MONTECARLO              |
---------------------------------------------------------------------*/

// Función que devuelve deltaE a partir de una posición (x,y) aleatoria
// en la red y aplicando las CONDICIONES DE CONTORNO PERIÓDICAS:
// s[0][i]=s[N][i], s[N+1][i]=s[1][i], s[i][0]=s[i][N], s[i][N+1]=s[i][1]

double DeltaEnergia(int s[N][N], double w[N][N][N][N], int x, int y)
{
    // x, y son las posiciones aleatorias en la red
    int k,l;
    double deltaE;

    deltaE = 0;
    for(k=0;k<N;k++)
    {
        for(l=0;l<N;l++)
        {
            deltaE = deltaE + w[x][y][k][l]*s[x][y]*s[k][l];
        }
    }
    return deltaE*2;
}