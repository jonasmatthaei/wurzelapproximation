#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>



long long factorial(int n) {
    if (n >= 1)
        return n*factorial(n-1);
    else
        return 1;
}

double power(double base, int exponent) {
    double result = 1.0;
    for(int i = 0; i < exponent; i++) {
        result *= base;
    }
    return result;
}

float absolute(float num) {
    int* intPtr = (int*)&num;
    *intPtr &= 0x7FFFFFFF;
    return num;
}




// Sucht a mittels Linearer Suche, wobei n - a^2 minimal sein muss
void approximateSqrtLinear(float n, double* a, float* x) {

    *a = 0;
    while ((*a) * (*a) < n) {
        (*a)++;
    }

    *x = n - (*a) * (*a);
}


// Sucht a mittels Binärer Suche, wobei n - a^2 minimal sein muss
void approximateSqrtBinary(float n, double* a, float* remainder) {
    *a = (double) n;
    double lowerBound = 0, upperBound = n;
    double square = (*a) * (*a);

    while (absolute(upperBound - lowerBound) > 1 && absolute(square - n) > 0.1) {
        *a = (upperBound + lowerBound) / 2.0;
        square = (*a) * (*a);

        if(square > n) {
            upperBound = *a;
        } else {
            lowerBound = *a;
        }
    }

    *remainder = n - square;
}


 // Berechnung der Wurzel von 'n' mittlels Taylorpolynom
float sqrtTaylor(int derivative, float n, char* mode) {

    double root;
    float remainder;
    if (strcmp(mode, "binary") == 0) {
        approximateSqrtBinary(n, &root, &remainder);
    } else if (strcmp(mode, "linear") == 0) {
        approximateSqrtLinear(n, &root, &remainder);
    }

    int index = 0;
    float derivativeResult = 0;
    float finalResult = root;
    int counter = 1;
    while (index <= derivative) {
        int i = 0;
        float product = 1;
        float b = 0.5;
        while (i < index) {
            product *= b;
            b -= 1;
            i++;
        }
        if (index != 0) {
            derivativeResult = product * (1 / power(root, counter));
            counter += 2;
            derivativeResult /= factorial(index);
            finalResult += derivativeResult * power(remainder, index);
        }
        index++;
    }

    return finalResult;

    return finalResult;
}
