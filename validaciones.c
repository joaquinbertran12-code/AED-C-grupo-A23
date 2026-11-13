#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "validaciones.h"

/* Despues de un scanf puede quedar basura en la linea (por ejemplo "abc"
 * o "3,5"). Esta funcion la descarta hasta el Enter y devuelve 1 si solo
 * habia espacios, o 0 si habia otros caracteres. */
int terminar_linea(void)
{
    int c;
    int limpia = 1;

    while ((c = getchar()) != '\n' && c != EOF) {
        if (c != ' ' && c != '\t') {
            limpia = 0;
        }
    }
    return limpia;
}

void salir_por_fin_de_entrada(void)
{
    printf("\n[AVISO] Se cerro la entrada de datos. Fin del programa.\n");
    exit(0);
}

/* Version para cajas de texto. Aplica las MISMAS reglas que leer_decimal:
 *  - tiene que ser un numero (la coma "3,5" se rechaza porque deja basura)
 *  - tiene que estar entre min y max ("nan" tambien se rechaza) */
int validar_decimal(const char *texto, double min, double max, double *valor, char *mensaje)
{
    char *fin;
    double v;

    v = strtod(texto, &fin);
    if (fin == texto) {             /* no se pudo leer ningun numero */
        strcpy(mensaje, "Entrada invalida: use un numero con PUNTO decimal (ej: 0.5).");
        return 0;
    }
    while (*fin == ' ' || *fin == '\t') {
        fin++;                      /* los espacios al final no molestan */
    }

    if (*fin != '\0') {            /* quedo basura despues del numero (ej: "3,5") */
        strcpy(mensaje, "Entrada invalida: use un numero con PUNTO decimal (ej: 0.5).");
        return 0;
    }
    if (!(v >= min && v <= max)) {
        snprintf(mensaje, LARGO_MENSAJE, "El valor debe estar entre %g y %g.", min, max);
        return 0;
    }

    *valor = v;
    return 1;
}

void mostrar_error(const char *texto)
{
    printf("  [ERROR] %s\n", texto);
}

void mostrar_aviso(const char *texto)
{
    printf("  [AVISO] %s\n", texto);
}

int leer_entero(const char *mensaje, int min, int max)
{
    int valor;
    int leidos;
    int linea_limpia;

    while (1) {
        printf("%s", mensaje);
        fflush(stdout);

        leidos = scanf("%d", &valor);
        if (leidos == EOF) {
            salir_por_fin_de_entrada();
        }
        linea_limpia = terminar_linea();

        if (leidos != 1 || !linea_limpia) {
            mostrar_error("Entrada invalida: escriba un numero entero.");
        } else if (valor < min || valor > max) {
            printf("  [ERROR] El valor debe estar entre %d y %d.\n", min, max);
        } else {
            return valor;
        }
    }
}

double leer_decimal(const char *mensaje, double min, double max)
{
    double valor;
    int leidos;
    int linea_limpia;

    while (1) {
        printf("%s", mensaje);
        fflush(stdout);

        leidos = scanf("%lf", &valor);
        if (leidos == EOF) {
            salir_por_fin_de_entrada();
        }
        linea_limpia = terminar_linea();

        if (leidos != 1 || !linea_limpia) {
            /* "3,5" cae aca: la coma deja basura en la linea */
            mostrar_error("Entrada invalida: escriba un numero con PUNTO decimal (ej: 0.5).");
        } else if (!(valor >= min && valor <= max)) {
            /* Escrito con "!(...)" para que "nan" tambien se rechace */
            printf("  [ERROR] El valor debe estar entre %g y %g.\n", min, max);
        } else {
            return valor;
        }
    }
}

char leer_si_no(const char *mensaje)
{
    char c;
    int leidos;
    int linea_limpia;

    while (1) {
        printf("%s", mensaje);
        fflush(stdout);

        leidos = scanf(" %c", &c);
        if (leidos == EOF) {
            salir_por_fin_de_entrada();
        }
        linea_limpia = terminar_linea();

        if (leidos == 1 && linea_limpia) {
            if (c == 's' || c == 'S') {
                return 's';
            }
            if (c == 'n' || c == 'N') {
                return 'n';
            }
        }
        mostrar_error("Responda con 's' o 'n'.");
    }
}

void pausar(void)
{
    printf("\nPresione Enter para continuar...");
    fflush(stdout);
    getchar();
}
