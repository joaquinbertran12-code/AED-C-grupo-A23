#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "artemis.h"
#include "interfaz.h"

int main(void)
{
    Nave nave;
    Mision ultima;
    int hay_ultima = 0;

    srand((unsigned int)time(NULL));

    nave.altitud = ALTITUD_DEFECTO;
    nave.masa = MASA_DEFECTO;
    memset(&ultima, 0, sizeof ultima);

    iniciar_interfaz();

    /* Cada vuelta del while dibuja un cuadro de la ventana (unas 60 por segundo) */
    while (dibujar_cuadro(&nave, &ultima, &hay_ultima)) {
    }

    cerrar_interfaz();
    return 0;
}
