#ifndef VALIDACIONES_H
#define VALIDACIONES_H

/* Tamano que debe tener el texto donde validar_decimal deja el error */
#define LARGO_MENSAJE 100

/* ---- Version para la ventana (Raylib): recibe el TEXTO de una caja ---- */

/* Devuelve 1 si el texto es un numero valido entre min y max (y lo deja en
 * *valor). Si no, devuelve 0 y escribe el motivo en "mensaje". */
int validar_decimal(const char *texto, double min, double max, double *valor, char *mensaje);

/* ---- Versiones originales para consola (usan scanf) ---- */
/* Todas repiten la pregunta hasta recibir un dato valido. */

int leer_entero(const char *mensaje, int min, int max);

/* Usar punto decimal (0.5). */
double leer_decimal(const char *mensaje, double min, double max);

/* Devuelve 's' o 'n'. */
char leer_si_no(const char *mensaje);

void mostrar_error(const char *texto);
void mostrar_aviso(const char *texto);
void pausar(void);

#endif
