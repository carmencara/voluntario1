Este repositorio contiene los archivos necesarios para la resolución del problema voluntario del tema 2 (Modelo de Ising: una
introducción al método Monte Carlo): modelo de Hopfield de red neuronal. Los objetivos de este problema son:
1. Ver cómo la red es capaz de recordar un solo patrón almacenado partiendo de (i) una condición inicial aleatoria y (ii) del patrón
   deformado. Calcular el solapamiento en función del tiempo.
2. Hacer lo anterior para diferentes temperaturas, de manera que se pueda obtener la curva de solapamiento frente a la temperatura.
3. Repetir los puntos 1 y 2 para varios patrones.
4. Calcular cómo decae la recuperación de la memoria en función del número de patrones almacenados.

Se han realizado dos programas en C++:
- hopfield.cpp (para los apartados 1-3)
- memoria_patrones.cpp (para el apartado 4)
En ambos se ha necesitado la librería GSL para la generación de números aleatorios (gsl_rng.h).

Los ficheros con los datos que se extraen de las simulaciones se encuentran en las carpetas:
- "Resultados para 1 patrón": resultados para los apartados 1 y 2.
- "Resultados para 4 patrones": resultados para el apartado 3.
- "Memoria en función del número de patrones": resultados para el apartado 4.
Además, el script "animacion_hopfield.py" ha servido para realizar todas las animaciones e imágenes de la red neuronal y la carpeta
"N50_pollito" contiene pruebas realizadas para una red más pequeña, de N=50 (en el informe se ha utilizado la red de N=128).
