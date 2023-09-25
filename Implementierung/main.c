#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <errno.h>
#include <string.h>

#include "reihe.h"
#include "lookup.h"
#include "performance.h"


const char* usage_msg =
  "Benutzung: %s [Optionen]\n"
  "   schreibe: %s -h, --help     (Zeige Hilfemeldung an und beende das Programm)\n";

const char* help_msg =
  "Hilfemeldung:\n"
  "  birthday_eq   Eine Funktion, die die Anzahl der Elemente k berechnet, für eine Wahrscheinlichkeit von mindestens 50 Prozent einer Kollision in einer Grundmenge n. \n"
  "\n"
  "Optionale Argumente:\n"
  "  -n <Zahl>    Größe der Grundmenge (default: N = 365)\n"
  "  -V <Zahl>    Wähle Version der Funktion birthday_eq (0-5) -> (default: V = 0)\n"
  "      -V0 = Hauptimplementierung (sqrt mit TaylorSeries + Binary Search) \n"
  "      -V1 = sqrt mit TaylorSeries Lineare Suche \n"
  "      -V2 = sqrt mit Lookup-Tabelle \n"
  "      -V3 = sqrt mit Lookup-Tabelle + durchlaufen mit SIMD Instruktionen \n"
  "      -V4 = sqrt mit Lookup-Tabelle + Suche mit Binary Search \n"
  "      -V5 = sqrtf aus C-Bibliothek\n"
  "  -B <Zahl>    Laufzeitmessung, <Zahl> = Optional (Anzahl an Wiederholungen des Funktionsaufrufs, Beispiel: -B100 für 100 Iterationen) \n"
  "  -P <Zahl>    Performanztests der Wurzelfunktionen, Zahl = Wert unter der Wurzel (Zahl nicht optional)\n"
  "  -h, --help   Zeige Hilfemeldung an (dieser Text) und beende das Programm\n";


void print_usage(const char* progname) {
  fprintf(stderr, usage_msg, progname, progname, progname);
}

void print_help(const char* progname) {
  fprintf(stderr, "\n%s", help_msg);
}

float lookupTable[2000000];
const float ln_two = 0.69314718055994530941723212145818F;

float birthday_eq_v0(unsigned long n) {
  int ableitungen = 2;
  float zähler = 1 + sqrtTaylor(ableitungen, 1 + (8.0f * (float)n) * ln_two, "binary");
  float nenner = 2;
  float ergebnis = zähler / nenner;
  return ergebnis;
}

float birthday_eq_v1(unsigned long n) {
  int ableitungen = 2;
  float zähler = 1 + sqrtTaylor(ableitungen,1 + (8.0f * (float)n) * ln_two, "linear");
  float nenner = 2;
  float ergebnis = zähler / nenner;
  return ergebnis;
}

float birthday_eq_v2(unsigned long n) {
  float zähler = 1 + sqrt_lookup_iterative(1 + (8.0f * (float)n) * ln_two, lookupTable);
  float nenner = 2;
  float ergebnis = zähler / nenner;
  return ergebnis;
}

float birthday_eq_v3(unsigned long n) {
  float zähler = 1 + sqrt_lookup_simd(1 + (8.0f * (float)n) * ln_two, lookupTable);
  float nenner = 2;
  float ergebnis = zähler / nenner;
  return ergebnis;
}

float birthday_eq_v4(unsigned long n) {
  float zähler = 1 + sqrt_lookup_binarySearch(1 + (8.0f * (float)n) * ln_two, lookupTable);
  float nenner = 2;
  float ergebnis = zähler / nenner;
  return ergebnis;
}

float birthday_eq_v5(unsigned long n) {
  float zähler = 1 + sqrtf(1 + (8.0f * (float)n) * ln_two);
  float nenner = 2.0f;
  float ergebnis = zähler / nenner;
  return ergebnis;
}

typedef float (*BirthdayEqFunction)(unsigned long);

BirthdayEqFunction functions[] = {
  birthday_eq_v0,
  birthday_eq_v1,
  birthday_eq_v2,
  birthday_eq_v3,
  birthday_eq_v4,
  birthday_eq_v5,
};

BirthdayEqFunction get_birthday_eq(int version) {
  return functions[version];
}

//Misst die Laufzeit einer Funktion abhängig von Iterationen und gibt die CPU-Zeit zurück
double getRuntime(int functionIterations, BirthdayEqFunction birthday_eq, unsigned long n) {
  struct timespec start, end;
  double cpu_time;
  clock_gettime(CLOCK_MONOTONIC, &start);
  for (int i = 0; i < functionIterations; i++) {
    float result = birthday_eq(n);
  }
  clock_gettime(CLOCK_MONOTONIC, &end);
  cpu_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
  return cpu_time;
}


//Returned 0 wenn arg ein kein valider unsigned long und ansonsten parsed Long
unsigned long parse_ulong(const char* arg, char ArgType) {
  char* endptr;
  errno = 0;
  long checkForNegative = (long)strtol(arg, &endptr, 10);
  if(checkForNegative < 0) {
    fprintf(stderr, "Fehler: Die Eingabe für %c muss größer als 0 sein\n", ArgType);
    return 0;
  }
  unsigned long result = (unsigned long)strtoul(arg, &endptr, 10);
  if (result <= 0) {
    fprintf(stderr, "Fehler: Die Eingabe für %c muss größer als 0 sein und darf keine char Zeichen enthalten\n", ArgType);
    return 0;
  } else if (endptr == arg || *endptr != '\0') {
    fprintf(stderr, "Fehler: Ungültige Eingabe für Argument -%c, Eingabe darf nur Ganzzahlen enthalten\n", ArgType);
    return 0;
  } else if (errno == ERANGE) {
    fprintf(stderr, "Fehler: Eingabe des Arguments -%c überschreitet Wertebereich von Typ: unsigned long\n", ArgType);
    return 0;
  }
  return result;
}

//Returned parsed Int wenn arg ein valider int ist, ansonsten -1
int parse_int(const char* arg, char ArgType) {
  char* endptr;
  errno = 0;
  long result = (long)strtol(arg, &endptr, 10);

  if(result > INT_MAX || result < INT_MIN || errno == ERANGE) {
    fprintf(stderr, "Fehler: Eingabe des Arguments -%c überschreitet Wertebereich von Typ: int\n", ArgType);
    return -1;
  } else if(result < 0) {
    fprintf(stderr, "Fehler: Ungültige Eingabe für Argument -%c, Zahl ist negativ und muss positiv sein\n", ArgType);
    return -1;
  } else if (endptr == arg || *endptr != '\0') {
    fprintf(stderr, "Fehler: Ungültige Eingabe für Argument -%c, Eingabe darf nur Ganzzahlen enthalten\n", ArgType);
    return -1;
  }
  return (int)result;
}

//Lookup Table befüllen mit float Werten, um bestimmte Werte mit großer Größe zu berechnen.
void createSqrtLookupTablefull(float lookupTable[]) {
    for (long long i = 0; i < 1000000; i++) {
        lookupTable[i] = (float)i * (float)i;
    }
    long long x = 1000000;
    for (long long i = 1000000; i < 2000000; i++) {
        lookupTable[i] = (float)x * (float)x;
        x = x + 10113;
     }
}


int main(int argc, char** argv) {
  const char* progname = argv[0];
  const char* optstring = "V:n:P:B::h";

  if (argc == 1) {
    print_usage(progname);
    return EXIT_FAILURE;
  }

  unsigned long n = 365;
  unsigned long p;
  BirthdayEqFunction birthday_eq = get_birthday_eq(0); //0 (default) = Hauptimplementierung
  int numberIterationsTestingOptional = -1;
  int opt, functionVersion;

  for (int i = 1; i < argc; i++) { //not clean yet (search for --help in entire argv[], could be reduced to argv[1] but then not exhaustive
    if (strcmp(argv[i], "--help") == 0) { //are we allowed to use strcmp?
      print_help(progname);
      return EXIT_SUCCESS;
    }
  }

  while (( opt = getopt ( argc , argv , optstring) ) != -1) {
    switch(opt) {
      case 'P':
        if ((p = parse_ulong(optarg, 'P')) == 0) {
          return EXIT_FAILURE;
        } else {
          performance_test(p);
        }
        return EXIT_SUCCESS;
      case 'n':
        if ((n = parse_ulong(optarg, 'n')) == 0) {
          return EXIT_FAILURE;
        }
        break;
      case 'V':
        if ((functionVersion = parse_int(optarg, 'V')) == -1) {
          return EXIT_FAILURE;
        }
        if (functionVersion > 5) {
          fprintf(stderr, "Invalide Funktionsversion, wähle eine Version zwischen 0-5\n");
          return EXIT_FAILURE;
        } else {
          birthday_eq = get_birthday_eq(functionVersion);
        }
        break;
      case 'B':
        if(optarg != NULL) {
          if ((numberIterationsTestingOptional = parse_int(optarg, 'B')) == -1) {
            return EXIT_FAILURE;
          }
          if (numberIterationsTestingOptional == 0){
            fprintf(stderr, "Argument -B darf nicht 0 sein\n");
            return EXIT_FAILURE;
          }
        } else {
          numberIterationsTestingOptional = 1;
        }
        break;
      case 'h': print_help(progname); return EXIT_SUCCESS;
      default: print_usage(progname); return EXIT_FAILURE;
      }
  }

  if(functionVersion >= 2 && functionVersion <=4) {
    createSqrtLookupTablefull(lookupTable);
  }

  printf("\nbirthday_eq(birthday_eq_v%d, n = %lu) = %f (k)\n\n", functionVersion, n, birthday_eq(n));

  if(numberIterationsTestingOptional > 0) {
    double cpu_time = getRuntime(numberIterationsTestingOptional, birthday_eq, n);
    double cpu_time_comparison = getRuntime(numberIterationsTestingOptional, birthday_eq_v5, n);
    printf("CPU-Zeit birthday_eq_v%d(n = %lu), %d Iteration(en) = %f Sekunden\n\n", functionVersion, n ,numberIterationsTestingOptional ,cpu_time);
    printf("Zum Vergleich, C-Bibliothek: birthday_eq_v5(n = %lu), %d Iteration(en) = %f Sekunden (evtl. genauer als unsere Impl.)\n", n ,numberIterationsTestingOptional ,cpu_time_comparison);
  }

  return EXIT_SUCCESS;
}
