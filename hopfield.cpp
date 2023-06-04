//---------------------------------------------------------------------
//                                                                    |
//         ALGORITMO DE METROPOLIS PARA EL MODELO DE HOPFIELD         |
//                                                                    |
//  Estudio de un solo patrón:                                        |
//    - Capacidad de la red de recordar distintos patrones            |
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

#define N 50          // Tamaño de la matriz de spines
#define P 1           // Número de patrones

static double w[N*N][N*N];    // Pesos sinápticos, w[N*i+j][k*N+l] es la interacción entre (i,j) y (k,l)

using namespace std;

// FUNCIONES QUE SE VAN A UTILIZAR
void LeerFichero(int q[][N*N]);
void Calcular_w(int q[][N*N], double w[N*N][N*N], double a[]);
void Calcular_t(double t[][N], int q[][N*N], double a[], double w[N*N][N*N]);
double Calcular_deltaE(int s[][N], int q[][N*N], double t[][N], double a[], double w[N*N][N*N], int x, int y);
double Calcula_solap(int s[][N], int q[][N*N], double a[], int patron);
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
    int q[3][N*N];         // Matriz del patrón inicial
    int x,y;               // Coordenadas aleatorias en la red
    double deformacion;    // Porcentaje de neuronas deformadas del patrón inicial
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
    ofstream datos;        // Fichero para guardar los spines
    ofstream solapamiento; // Fichero para guardar el solapamiento
    ifstream patron;       // Fichero con el patrón inicial a recordar "pollitos.txt"
    gsl_rng *tau;          // Para números aleatorios con gsl
    int semilla;           // Para números aleatorios con gsl


    // ------------------ CONFIGURACIONES INICIALES -------------------
    // Guardar el(los) patrón(es) inicial(es) en la matriz q[P][N][N]
    LeerFichero(q);
    // Elegir si los elementos de la red son aleatorios (true) o los del patrón (false)
    aleatorio=true;
    // Paso de Montecarlo
    N2=N*N;
    // Temperatura inicial
    T=1E-4;
    // Establecer la semilla para generar números aleatorios
    semilla=SemillaTiempo();
    tau = gsl_rng_alloc(gsl_rng_taus);
    gsl_rng_set(tau,semilla);

    // Abrir ficheros para guardar resultados
    datos.open("datos_1patron.txt");
    solapamiento.open("solapamiento_1patron.txt");

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
                s[i][j]=q[0][N*i+j]; // La red es igual que el patrón inicial
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
    for(m=0;m<4;m++){
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
    iter = 100*N2;
    for(n=0;n<iter;n++)
    {
        // ------------------- ELIJO (n,m) ALEATORIO -------------------
        x=rand()%N;
        y=rand()%N;

        // --------------------- CÁLCULO DE deltaE ---------------------
        deltaE = Calcular_deltaE(s, q, t, a, w, x, y);

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
            
            // Guardo el solapamiento para cada paso de Montecarlo
            solapamiento << n/N2 << "\t";
            for(i=0;i<P;i++)
            {
                solap[i] = Calcula_solap(s, q, a, i);
                solapamiento << solap[i] << "\t";
            }
            solapamiento << endl;
        }
    }

    // Cerrar el fichero y finalizar
    patron.close();
    datos.close();
    return 0;
}

/*---------------------------------------------------------------------
|               FUNCIONES PARA TRATAMIENTO DE FICHEROS                |
---------------------------------------------------------------------*/
void LeerFichero(int q[][N*N])
{
    int i,j;            // Contadores
    ifstream patron1;   // Fichero donde está el patrón 1
    ifstream patron2;   // Fichero donde está el patrón 2
    ifstream patron3;   // Fichero donde está el patrón 3

    // Abrir los ficheros
    if(P==1)
    {
        patron1.open("pollito.txt");
    }
    else if (P==3)
    {
        patron1.open("imagen1.txt");
        patron2.open("imagen2.txt");
        patron3.open("imagen3.txt");
    }
    else cout << "Introduzca 1 o 3 patrones" << endl;

    // Guardar el contenido de los ficheros en la matriz de los patrones
    for(i=0;i<N;i++)
    {
        for(j=0;j<N;j++)
        {
            if (P==1)
            {
                patron1 >> q[0][i*N+j];
            }
            if (P==3)
            {
                patron1 >> q[0][i*N+j];
                patron2 >> q[1][i*N+j];
                patron3 >> q[2][i*N+j];
            }
        }
    }

    // Cerrar los ficheros
    if(P==1)
    {
        patron1.close();
    }
    else if (P==3)
    {
        patron1.close();
        patron2.close();
        patron3.close();
    }
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
double Calcula_solap(int s[][N], int q[][N*N], double a[], int patron)
{
    int i, j;       // Contadores
    double solap;   // Solapamiento
    
    solap = 0.0;
    for(i=0;i<N;i++)
    {
        for(j=0;j<N;j++)
        {
            solap = solap + (q[patron][i*N+j]-a[patron])*(s[i][j]-a[patron]);
        }
    }
    solap = 1.0*solap/(a[patron]*(1-a[patron])*N*N);

    return solap;
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