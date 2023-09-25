#include "reihe.h"
#include "lookup.h"
#include "main.h"
#include <time.h>
#include <stdio.h>
#include <math.h>

float lookup[2000000];

void performance_test(float n){

    createSqrtLookupTablefull(lookup);

    struct timespec start, end;
    double cpu_time, avg_time;

    unsigned long num_iterations = 1000000;  // Anzahl der Wiederholungen

    printf("Perfomzmessungen für alle Versionen der Wurzelberechnung \n");
    printf("Für alle Versionen wird 10 mal die CPU-Zeit für 1000000 Wiederholungen gemessen und daraus eine Durchschnittszeit ermittelt: \n");

    for(int z = 0; z < 6; z++) { // Durchlaufen jeder der 6 Versionen die Wurzel zu berechnen
        avg_time = 0;

        for (int j = 0; j < 10; j++) { // Jede Version wird zusätzlich 10 mal berechnet um ein Durchschnittswert zu erreichen

            clock_gettime(CLOCK_MONOTONIC, &start);

                //Durchlaufen aller Versionen mit 1000000 Iterationen
                switch(z){
                    case 0: for (unsigned long i = 0; i < num_iterations; i++) {sqrtf(n);} break;
                    case 1: for (unsigned long i = 0; i < num_iterations; i++) {sqrtTaylor(2, n, "linear");} break;
                    case 2: for (unsigned long i = 0; i < num_iterations; i++) {sqrtTaylor(2, n, "binary");} break;
                    case 3: for (unsigned long i = 0; i < num_iterations; i++) {sqrt_lookup_iterative(n, lookup);} break;
                    case 4: for (unsigned long i = 0; i < num_iterations; i++) {sqrt_lookup_binarySearch(n, lookup);} break;
                    case 5: for (unsigned long i = 0; i < num_iterations; i++) {sqrt_lookup_simd(n, lookup);} break;
                    default: break;
                }
            

            clock_gettime(CLOCK_MONOTONIC, &end);

            cpu_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

            avg_time += cpu_time;

        }

        avg_time = avg_time / 10;

        //Ausgabe aller gemessenen Durchschnittslaufzeiten
         switch(z){
                    case 0: printf("Durchschlittliche CPU-Zeit für n = %lu Wurzelberechnung aus glibc: %f Sekunden\n", (unsigned long)n, cpu_time); break;
                    case 1: printf("Durchschlittliche CPU-Zeit für n = %lu Taylor-Reihe: %f Sekunden\n", (unsigned long)n, cpu_time); break;
                    case 2: printf("Durchschlittliche CPU-Zeit für n = %lu Optimierte Version der Taylor-Reihe: %f Sekunden\n", (unsigned long)n, cpu_time); break;
                    case 3: printf("Durchschlittliche CPU-Zeit für n = %lu Lookup: %f Sekunden\n", (unsigned long)n, cpu_time); break;
                    case 4: printf("Durchschlittliche CPU-Zeit für n = %lu Optimierte Version des Durchlaufens des Lookup Tables mit Binärer Suche: %f Sekunden\n", (unsigned long)n, cpu_time); break;
                    case 5: printf("Durchschlittliche CPU-Zeit für n = %lu Optimierte Version des Durchlaufens des Lookup Tables mit SIMD Operationen: %f Sekunden\n", (unsigned long)n, cpu_time); break;
                    default: break;
                }
    }
}
