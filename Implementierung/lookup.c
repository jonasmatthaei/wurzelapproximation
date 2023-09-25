#include <stdio.h>
#include <emmintrin.h>


/*Wurzelberechnung -> Durchlaufen des gesamten Lookup Tables mit Binary Search*/
float sqrt_lookup_binarySearch(float n, float* lookup) {
    /*Werte initialisieren und bestimmen für low und high -> Anfangswerte*/
    long long size = 2000000;
    float sqrtres;
    int low = 1;
    int high = size;

    /** Implementierung Binary Search, suche nach dem nächstgelegenen Wert im Lookup*/
    while (high >= low) {

        int mid = low + (high - low) / 2;

        if (lookup[mid] < n) {
            low = mid + 1;
        }

        if (lookup[mid] > n) {
            high = mid - 1;
        }

        if (lookup[mid] >= n && lookup[mid - 1] <= n) { //Wert gefunden

            //Vorbereitung für lineare Interpolation
            float  x1 = lookup[mid - 1];
            float x2 = lookup[mid];
            float y1;
            float y2;

            if (mid > 1000000){
                /*Wert y1 und y2 müssen angepasst werden aufgrund der Befüllung des Lookup Tables ab Index 1000000*/
                 y1 = (mid - 1) + ((((float)mid) - 1000000) * 10113);
                 y2 = mid + ((((float)mid) - 1000000) * 10113);
            }
            else{
                y1 = mid - 1;
                y2 = mid;
            }

            // Formel für lineare Interpolation
            sqrtres = y1 + (y2 - y1) / (x2 - x1) * (n - x1);
            break;
        }

    }
    return sqrtres; // Rückgabewert: Approximierte Wurzel zu n
}


/*Wurzelberechnung -> Durchlaufen des Lookup Tables iterativ*/
float sqrt_lookup_iterative(float n, float* lookup){
    float sqrtres;
    long long size = 2000000;

    /*Suchen nach dem nächstgelegenen Wert im Lookup*/

    for (long long i = 0; i < size; i++) {

        if (lookup[i] > n) {

            /*Vorbereitung für lineare Interpolation*/
            float x1 = lookup[i - 1];
            float x2 = lookup[i];
            float y1;
            float y2;

            if (i > 1000000){
                /*Wert y1 und y2 müssen angepasst werden aufgrund der Befüllung des Lookup Tables ab Index 1000000*/
                 y1 = (i - 1) + ((((float)i) - 1000000) * 10113);
                 y2 = i + ((((float)i) - 1000000) * 10113);
            }
            else{
                y1 = i - 1;
                y2 = i;
            }

            /**Formel für lineare Interpolation*/
            sqrtres = y1 + (y2 - y1) / (x2 - x1) * (n - x1);
            break;
        }
    }
    return sqrtres; // Rückgabewert: Approximierte Wurzel zu n
}


/**Wurzelberechnung -> Durchlaufen des Lookup Tables mit SIMD Instruktionen (zwei 64 bit long long Werte in ein 128 bit XMM)**/

float sqrt_lookup_simd(float x, float* lookup) {
    /*Falls der übergebene Wert 0 ist, kann dirket 0 zurückgegeben werden*/
    if (x == 0) return 0;

    /*Zuweisung von Laufvariable, Prüfvariable und Erstellung eines 128 bit XMM Registers mit vier gleichen float Werten zum späteren Vergleichen*/
    long long size = 2000000;
    long long i = 0;
    int found = 0;
    __m128 xs = _mm_set1_ps(x);

    while (size > 4) {

        __m128 load = _mm_load_ps(&lookup[i]); // Laden der Werte aus dem Lookup Table in ein Register

        /*Vergleichsoperationen zwischen dem übergebenen Wert und vier Werten aus dem Lookup Table + Erstellung von Masken zur Trefferprüfung*/
        __m128 cmpgt = _mm_cmpgt_ps(load,xs);
        int mask = _mm_movemask_ps(cmpgt);

        if (mask != 0) {//Trefferprüfung
            found = 1;
            break;
        }

        i += 4;
        size -= 4;
    }

    /*Falls kein Treffer gefunden wird, wird in den letzten vier Werten iterativ gesucht*/
    if(found == 0) {

        for (; i < size; i++) {

            if (lookup[i] > x) {

                /**Vorbereitung für lineare Interpolation*/

                float x1 = lookup[i - 1];
                float x2 = lookup[i];
                float y1;
                float y2;

                if (i > 1000000) {
                    /*Wert y1 und y2 müssen angepasst werden aufgrund der Befüllung des Lookup Tables ab Index 1000000*/
                    y1 = (i - 1) + ((((float) i) - 1000000) * 10113);
                    y2 = i + ((((float) i) - 1000000) * 10113);
                }

                else {
                    y1 = i - 1;
                    y2 = i;
                }

                return  (float) (y1 + (y2 - y1) / (x2 - x1) * (x - x1)); // Rückgabewert: Approximierte Wurzel zu x
            }
        }
    }

    else{

        for (; i < (i + 4); i++) {

            if (lookup[i] > x) {

                /**Vorbereitung für lineare Interpolation*/

                float x1 = lookup[i - 1];
                float x2 = lookup[i];
                float y1;
                float y2;

                if (i > 1000000) {
                    /*Wert y1 und y2 müssen angepasst werden aufgrund der Befüllung des Lookup Tables ab Index 1000000*/
                    y1 = (i - 1) + ((((float) i) - 1000000) * 10113);
                    y2 = i + ((((float) i) - 1000000) * 10113);

                } else {
                    y1 = i - 1;
                    y2 = i;
                }

                return  (float) (y1 + (y2 - y1) / (x2 - x1) * (x - x1));// Rückgabewert: Approximierte Wurzel zu x
            }
        }
    }
}
