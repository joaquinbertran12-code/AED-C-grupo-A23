#include <stdio.h>
#include <string.h>
#include "artemis.h"       
#include "validaciones.h"
#include "cohete.h"
#include "raylib.h"
#include "interfaz.h"

// Constantes y variables de la interfaz                               

#define ANCHO_VENTANA 1000
#define ALTO_VENTANA   700

// Panel para el texto 
#define PANEL_X       410
#define PANEL_Y        20
#define PANEL_ANCHO   570
#define PANEL_ALTO    610
#define TEXTO_X       430
#define LARGO_CAMPO    16

// Pantallas 
#define PANTALLA_MENU        0
#define PANTALLA_CONFIG      1
#define PANTALLA_SIMULACION  2

#define FASE_SUBIENDO   0      // sube
#define FASE_EVENTO     1      // se mueve 
#define FASE_MANIOBRA   2      // maniobra del usuario 
#define FASE_RESULTADO  3      // el cohete se mueve por la maniobra 
#define FASE_FIN        4      // resultado final
#define FASE_LATERAL    5      // opcion 3 (d): movimiento de costado 

static int  pantalla = PANTALLA_MENU;
static int  opcion = 1;             // opcion elegida en el menu del 1 al 5 
static int  salir = 0;

static char campo1[LARGO_CAMPO];    
static char campo2[LARGO_CAMPO];    
static int  campo_foco = 1;         
static char campo_error[30];        
static char mensaje[LARGO_MENSAJE]; 
static char aviso[LARGO_MENSAJE];   

static Cohete        cohete;
static Mision        mision;        
static ResultadoMCU  res_mcu;
static ResultadoPulso res_pulso;
static double pulso_a, pulso_dt;    // datos ingresados en la opcion 3 
static int   fase = FASE_SUBIENDO;
static int   turno = 1;
static int   tipo_maniobra = 0;     // 1 = adelante, 2 = atras, 0 = ninguna

static const char *TEXTO_OPCION[6] = {
    "",
    "1) Configurar altitud y masa",
    "2) Analisis orbital (velocidad, periodo, a_c)",
    "3) Pulso del propulsor lateral",
    "4) JUGAR: mision de estabilizacion orbital",
    "5) Ver ultimo reporte"
};

static const char *DESCRIPCION[6] = {
    "",
    "Cambia la altitud y la masa de la nave Orion.",
    "Calcula velocidad, periodo y aceleracion centripeta.",
    "Un impulso lateral desvia la nave de su orbita.",
    "5 vueltas con eventos al azar: mantenga la orbita.",
    "Muestra el reporte de la ultima mision jugada."
};

//Elementos

// Dibuja un boton. Devuelve 1 si el usuario hizo clic en el (y esta activo). 
static int boton(int x, int y, int ancho, int alto, const char *texto, int activo)
{
    Rectangle r = { (float)x, (float)y, (float)ancho, (float)alto };
    int   encima = CheckCollisionPointRec(GetMousePosition(), r);
    Color fondo = GetColor(0x2A4D8FFF);
    Color letra = WHITE;
    int   ancho_texto = MeasureText(texto, 20);

    if (!activo) {
        fondo = GetColor(0x3A3A44FF);
        letra = GRAY;
    } else if (encima) {
        fondo = GetColor(0x3F78D8FF);
    }

    DrawRectangleRec(r, fondo);
    DrawRectangleLines(x, y, ancho, alto, SKYBLUE);
    DrawText(texto, x + (ancho - ancho_texto) / 2, y + (alto - 20) / 2, 20, letra);

    return activo && encima && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

// Dibuja una caja de texto. Si esta "enfocada" tambien recibe lo que se escribe.

static int caja_texto(int x, int y, int ancho, int alto, char *texto, int enfocada)
{
    Rectangle r = { (float)x, (float)y, (float)ancho, (float)alto };
    int largo = (int)strlen(texto);
    int tecla;
    int clic = CheckCollisionPointRec(GetMousePosition(), r)
               && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    if (enfocada) {
        tecla = GetCharPressed();           
        while (tecla > 0) {
            if (tecla >= 32 && tecla <= 126 && largo < LARGO_CAMPO - 1) {
                texto[largo] = (char)tecla;
                largo++;
                texto[largo] = '\0';
            }
            tecla = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && largo > 0) {
            texto[largo - 1] = '\0';
        }
    }

    DrawRectangleRec(r, GetColor(0x10141FFF));
    DrawRectangleLines(x, y, ancho, alto, enfocada ? GOLD : GRAY);
    DrawText(texto, x + 8, y + (alto - 20) / 2, 20, WHITE);
    if (enfocada && ((int)(GetTime() * 2.0)) % 2 == 0) {
        DrawText("_", x + 8 + MeasureText(texto, 20), y + (alto - 20) / 2, 20, GOLD);
    }
    return clic;
}

static void dibujar_panel(const char *titulo)
{
    DrawRectangle(PANEL_X, PANEL_Y, PANEL_ANCHO, PANEL_ALTO, Fade(GetColor(0x0B1228FF), 0.92f));
    DrawRectangleLines(PANEL_X, PANEL_Y, PANEL_ANCHO, PANEL_ALTO, SKYBLUE);
    DrawText(titulo, TEXTO_X, PANEL_Y + 15, 24, GOLD);
}

// Si hay un error guardado, lo escribe en rojo (nombre y motivo
static void dibujar_error(int y)
{
    if (mensaje[0] != '\0') {
        if (campo_error[0] != '\0') {
            DrawText(TextFormat("[ERROR] %s", campo_error), TEXTO_X, y, 16, RED);
            y += 20;
        }
        DrawText(mensaje, TEXTO_X, y, 16, RED);
    }
}

static void vaciar_teclado(void)
{
    while (GetCharPressed() > 0) {
    }
}

static int validar_campo(const char *texto, double min, double max, double *valor, const char *nombre)
{
    char error[LARGO_MENSAJE];

    if (validar_decimal(texto, min, max, valor, error)) {
        return 1;
    }
    strcpy(campo_error, nombre);
    strcpy(mensaje, error);
    return 0;
}

static void borrar_error(void)
{
    mensaje[0] = '\0';
    campo_error[0] = '\0';
}

// Cambios de pantalla                                                 

static void ir_al_menu(void)
{
    pantalla = PANTALLA_MENU;
    aviso[0] = '\0';
    iniciar_cohete(&cohete);
}

static void entrar_config(const Nave *nave)
{
    pantalla = PANTALLA_CONFIG;
    campo1[0] = '\0';
    campo2[0] = '\0';
    campo_foco = 1;
    borrar_error();
    vaciar_teclado();
    iniciar_cohete(&cohete);

    if (opcion == 1) {                      //datos actuales 
        snprintf(campo1, LARGO_CAMPO, "%g", nave->altitud / 1000.0);
        snprintf(campo2, LARGO_CAMPO, "%g", nave->masa);
    }
    if (opcion == 4) {                      
        iniciar_mision(&mision, nave);
    }
}

static void entrar_simulacion(const Nave *nave)
{
    pantalla = PANTALLA_SIMULACION;
    borrar_error();
    iniciar_cohete(&cohete);
    fase = FASE_SUBIENDO;
    turno = 1;
    tipo_maniobra = 0;

    if (opcion == 2 || opcion == 3) {
        cohete.destino_y = y_por_altitud(nave->altitud);
    }
    if (opcion == 4) {
        cohete.destino_y = y_por_error(0.0);        
    }
    if (opcion == 5) {                              
        fase = FASE_FIN;
        if (mision.estado == MISION_COMPLETADA) {
            cohete.y = Y_TOPE;
        } else {
            cohete.y = Y_SUELO;
        }
        cohete.destino_y = cohete.y;
    }
}

/* ================================================================== */
/* Pantalla 1: menu                                                    */
/* ================================================================== */

static void pantalla_menu(const Nave *nave, int hay_ultima)
{
    int i;
    int y;

    DrawText("ARTEMIS", 450, 25, 64, GOLD);
    DrawText("Simulador de orbita terrestre baja", 450, 95, 20, SKYBLUE);
    DrawText(TextFormat("Datos actuales: h = %.1f km | m = %.1f kg",
                        nave->altitud / 1000.0, nave->masa), 450, 130, 18, WHITE);

    /* Una opcion por boton; la elegida lleva un borde dorado */
    for (i = 1; i <= 5; i++) {
        y = 165 + (i - 1) * 56;
        if (boton(450, y, 480, 48, TEXTO_OPCION[i], 1)) {
            opcion = i;
            aviso[0] = '\0';
        }
        if (i == opcion) {
            DrawRectangleLines(447, y - 3, 486, 54, GOLD);
        }
    }

    DrawText(DESCRIPCION[opcion], 450, 455, 18, SKYBLUE);
    DrawText(aviso, 450, 490, 18, GOLD);

    if (boton(450, 560, 230, 55, "COMENZAR", 1)) {
        if (opcion == 5 && !hay_ultima) {
            strcpy(aviso, "Todavia no jugaste ninguna mision.");
        } else {
            entrar_config(nave);
        }
    }
    if (boton(700, 560, 230, 55, "SALIR", 1)) {
        salir = 1;
    }

    dibujar_cohete(&cohete, 0);
}

// Pantalla 2: configuracion                                          

static void pantalla_config(Nave *nave, const Mision *ultima)
{
    static const char *TITULO[6] = {
        "", "CONFIGURAR ALTITUD Y MASA", "ANALISIS ORBITAL (MCU)",
        "PULSO LATERAL DEL PROPULSOR", "MISION DE ESTABILIDAD EN LEO", "ULTIMO REPORTE"
    };
    static const char *TEXTO_BOTON[6] = {
        "", "GUARDAR", "EJECUTAR", "EJECUTAR", "COMENZAR MISION", "VER REPORTE"
    };
    int    con_campos = (opcion == 1 || opcion == 3);
    int    ejecutar;
    double valor1, valor2;

    dibujar_panel(TITULO[opcion]);
    dibujar_cohete(&cohete, 0);

    if (opcion == 1) {
        DrawText(TextFormat("Altitud orbital h [km] (%g a %g):", ALTITUD_MIN_KM, ALTITUD_MAX_KM),
                 TEXTO_X, 100, 18, WHITE);
        DrawText(TextFormat("Masa de Orion [kg] (%g a %g):", MASA_MIN, MASA_MAX),
                 TEXTO_X, 190, 18, WHITE);
    } else if (opcion == 3) {
        DrawText(TextFormat("Velocidad orbital actual: v = %.2f m/s", velocidad_orbital(nave->altitud)),
                 TEXTO_X, 70, 18, SKYBLUE);
        DrawText(TextFormat("Aceleracion del pulso a [m/s^2] (%g a %g):", ACEL_MIN, ACEL_MAX),
                 TEXTO_X, 100, 18, WHITE);
        DrawText(TextFormat("Duracion del pulso dt [s] (%g a %g):", TIEMPO_MIN, TIEMPO_MAX),
                 TEXTO_X, 190, 18, WHITE);
    } else if (opcion == 4) {
        DrawText(TextFormat("Orion orbita a %.1f km. Habra %d vueltas con eventos.",
                            nave->altitud / 1000.0, MAX_TURNOS), TEXTO_X, 80, 18, WHITE);
        DrawText(TextFormat("OBJETIVO: cada vuelta a +-%.0f km de la altitud original.",
                            TOLERANCIA / 1000.0), TEXTO_X, 115, 18, WHITE);
        DrawText(TextFormat("Si la altitud baja de %.0f km, Orion reingresa y se pierde.",
                            ALTITUD_REINGRESO / 1000.0), TEXTO_X, 150, 18, WHITE);
        DrawText(TextFormat("Combustible: %.1f kg (alcanza para dv = %.1f m/s)",
                            mision.combustible, dv_maximo(&mision.nave, mision.combustible)),
                 TEXTO_X, 185, 18, WHITE);
        DrawText("dv = aceleracion * tiempo", TEXTO_X, 220, 18, WHITE);
        DrawText("PISTA: un impulso dv cambia la altitud en 2*r*dv/v.", TEXTO_X, 270, 18, GOLD);
        DrawText(TextFormat("Con tus datos, cada 1 m/s de dv cambia ~%.2f km.",
                            metros_por_dv(nave) / 1000.0), TEXTO_X, 305, 18, GOLD);
    } else {
        DrawText(TextFormat("Datos de la nave: h = %.1f km | m = %.1f kg",
                            nave->altitud / 1000.0, nave->masa), TEXTO_X, 80, 18, WHITE);
        if (opcion == 2) {
            DrawText("Se calcularan velocidad, periodo y aceleracion", TEXTO_X, 130, 18, SKYBLUE);
            DrawText("centripeta con estos datos.", TEXTO_X, 160, 18, SKYBLUE);
        } else {
            DrawText(TextFormat("Resultado: %s", ultima->estado == MISION_COMPLETADA
                                ? "MISION COMPLETADA" : "MISION FALLIDA"), TEXTO_X, 130, 18, SKYBLUE);
        }
    }

    if (con_campos) {
        if (caja_texto(TEXTO_X, 125, 250, 40, campo1, campo_foco == 1)) {
            campo_foco = 1;
        }
        if (caja_texto(TEXTO_X, 215, 250, 40, campo2, campo_foco == 2)) {
            campo_foco = 2;
        }
        if (IsKeyPressed(KEY_TAB)) {
            campo_foco = 3 - campo_foco;        
        }
        DrawText("Clic en una caja o Tab para cambiar. Enter = aceptar.", TEXTO_X, 280, 16, GRAY);
        dibujar_error(320);
    }

    //Botones 
    ejecutar = boton(TEXTO_X, 565, 250, 50, TEXTO_BOTON[opcion], 1);
    if (con_campos && IsKeyPressed(KEY_ENTER)) {
        ejecutar = 1;
    }
    if (boton(700, 565, 250, 50, "VOLVER AL MENU", 1)) {
        ir_al_menu();
        return;
    }
    if (!ejecutar) {
        return;
    }

    // Ejecutar segun la opcion 
    borrar_error();
    if (opcion == 1) {
        if (validar_campo(campo1, ALTITUD_MIN_KM, ALTITUD_MAX_KM, &valor1, "Altitud")
            && validar_campo(campo2, MASA_MIN, MASA_MAX, &valor2, "Masa")) {
            configurar_nave(nave, valor1, valor2);
            ir_al_menu();
            snprintf(aviso, LARGO_MENSAJE, "Listo: h = %.1f km, m = %.1f kg",
                     nave->altitud / 1000.0, nave->masa);
        }
    } else if (opcion == 2) {
        calcular_mcu(nave, &res_mcu);
        entrar_simulacion(nave);
    } else if (opcion == 3) {
        if (validar_campo(campo1, ACEL_MIN, ACEL_MAX, &valor1, "Aceleracion")
            && validar_campo(campo2, TIEMPO_MIN, TIEMPO_MAX, &valor2, "Duracion")) {
            pulso_a = valor1;
            pulso_dt = valor2;
            calcular_pulso(nave, pulso_a, pulso_dt, &res_pulso);
            entrar_simulacion(nave);
        }
    } else if (opcion == 4) {
        entrar_simulacion(nave);
    } else {
        mision = *ultima;
        entrar_simulacion(nave);
    }
}

// Pantalla 3: simulacion 
// Opcion 2: analisis orbital
static void simulacion_mcu(const Nave *nave)
{
    int y = 110;

    dibujar_panel("ANALISIS ORBITAL (MCU)");
    DrawText(TextFormat("Datos: h = %.1f km | m = %.1f kg", nave->altitud / 1000.0, nave->masa),
             TEXTO_X, 65, 18, WHITE);

    if (cohete_en_movimiento(&cohete)) {
        DrawText("El cohete asciende hasta la altitud orbital...", TEXTO_X, y, 18, SKYBLUE);
        return;
    }

    DrawText(TextFormat("Radio orbital: r = R_T + h = %.1f km", res_mcu.r / 1000.0), TEXTO_X, y, 18, WHITE);
    DrawText("a) La gravedad actua como fuerza centripeta", TEXTO_X, y + 40, 18, GOLD);
    DrawText(TextFormat("   Fg = G*M*m / r^2 = %.4e N", res_mcu.fuerza_grav), TEXTO_X, y + 68, 18, WHITE);
    DrawText("   Fg = Fc  ->  v = raiz(G*M/r)  (m se cancela)", TEXTO_X, y + 96, 18, WHITE);
    DrawText(TextFormat("   Velocidad orbital: v = %.2f m/s (%.3f km/s)", res_mcu.v, res_mcu.v / 1000.0),
             TEXTO_X, y + 124, 18, WHITE);
    DrawText(TextFormat("   Comprobacion: Fc = m*v^2/r = %.4e N", res_mcu.fuerza_cent),
             TEXTO_X, y + 152, 18, WHITE);
    DrawText("b) Periodo orbital", TEXTO_X, y + 192, 18, GOLD);
    DrawText(TextFormat("   T = 2*pi*r / v = %.2f s = %.2f min", res_mcu.periodo, res_mcu.periodo / 60.0),
             TEXTO_X, y + 220, 18, WHITE);
    DrawText("c) Aceleracion centripeta", TEXTO_X, y + 260, 18, GOLD);
    DrawText(TextFormat("   a_c = v^2 / r = %.4f m/s^2 (%.3f g)", res_mcu.ac, res_mcu.ac_en_g),
             TEXTO_X, y + 288, 18, WHITE);
}

//Opcion 3: pulso lateral
static void simulacion_pulso(const Nave *nave)
{
    int y = 110;

    dibujar_panel("PULSO LATERAL DEL PROPULSOR");
    DrawText(TextFormat("h = %.1f km | v orbital = %.2f m/s", nave->altitud / 1000.0, res_pulso.v),
             TEXTO_X, 65, 18, WHITE);
    DrawText(TextFormat("Pulso: a = %g m/s^2 durante dt = %g s", pulso_a, pulso_dt),
             TEXTO_X, 90, 18, WHITE);

    /* Primero sube; cuando llega, el cohete se mueve de costado; cuando
     * termina, se muestran los resultados. */
    if (fase == FASE_SUBIENDO) {
        DrawText("El cohete asciende hasta la altitud orbital...", TEXTO_X, y + 30, 18, SKYBLUE);
        if (!cohete_en_movimiento(&cohete)) {
            cohete.destino_x = x_por_desplazamiento(res_pulso.desplazamiento);
            fase = FASE_LATERAL;
        }
        return;
    }
    if (fase == FASE_LATERAL) {
        DrawText("Pulso lateral en marcha...", TEXTO_X, y + 30, 18, SKYBLUE);
        if (!cohete_en_movimiento(&cohete)) {
            fase = FASE_FIN;
        }
        return;
    }

    DrawText("Velocidad lateral ganada:", TEXTO_X, y + 20, 18, GOLD);
    DrawText(TextFormat("   dv = a*dt = %.3f m/s", res_pulso.dv), TEXTO_X, y + 48, 18, WHITE);
    DrawText("Desviacion lateral:", TEXTO_X, y + 88, 18, GOLD);
    DrawText(TextFormat("   s = 1/2*a*dt^2 = %.3f m", res_pulso.desplazamiento), TEXTO_X, y + 116, 18, WHITE);
    DrawText("Velocidad final (modulo):", TEXTO_X, y + 156, 18, GOLD);
    DrawText(TextFormat("   v' = raiz(v^2 + dv^2) = %.4f m/s", res_pulso.v_final), TEXTO_X, y + 184, 18, WHITE);
    DrawText("Giro de la direccion:", TEXTO_X, y + 224, 18, GOLD);
    DrawText(TextFormat("   %.5f grados", res_pulso.angulo), TEXTO_X, y + 252, 18, WHITE);
    DrawText("Como el empuje es perpendicular a la velocidad, el", TEXTO_X, y + 300, 16, SKYBLUE);
    DrawText("modulo casi no cambia, pero la direccion si: la nave", TEXTO_X, y + 322, 16, SKYBLUE);
    DrawText("se desvia de la orbita circular original.", TEXTO_X, y + 344, 16, SKYBLUE);
}

// Opcion 4 y 5)

// Reporte final: se usa al terminar la mision y en la opcion 5
static void dibujar_reporte(void)
{
    int i;
    int y = 160;

    DrawText(TextFormat("Altitud objetivo: %.1f km | Masa: %.1f kg",
                        mision.altitud_objetivo / 1000.0, mision.nave.masa), TEXTO_X, 70, 18, WHITE);
    if (mision.estado == MISION_COMPLETADA) {
        DrawText("Resultado: MISION COMPLETADA", TEXTO_X, 100, 22, GREEN);
    } else {
        DrawText("Resultado: MISION FALLIDA (reingreso)", TEXTO_X, 100, 22, RED);
    }

    DrawText("Turno", TEXTO_X, y, 16, SKYBLUE);
    DrawText("Evento", TEXTO_X + 60, y, 16, SKYBLUE);
    DrawText("Comb[kg]", TEXTO_X + 300, y, 16, SKYBLUE);
    DrawText("Alt[km]", TEXTO_X + 390, y, 16, SKYBLUE);
    DrawText("Estable", TEXTO_X + 470, y, 16, SKYBLUE);

    for (i = 0; i < mision.turnos_jugados; i++) {
        y = 190 + i * 26;
        DrawText(TextFormat("%d", mision.historial[i].turno), TEXTO_X + 10, y, 16, WHITE);
        DrawText(mision.historial[i].evento, TEXTO_X + 60, y, 16, WHITE);
        DrawText(TextFormat("%.1f", mision.historial[i].combustible_usado), TEXTO_X + 300, y, 16, WHITE);
        DrawText(TextFormat("%.2f", mision.historial[i].altitud_final / 1000.0), TEXTO_X + 390, y, 16, WHITE);
        DrawText(mision.historial[i].estable ? "SI" : "NO", TEXTO_X + 490, y, 16,
                 mision.historial[i].estable ? GREEN : ORANGE);
    }

    DrawText(TextFormat("Combustible restante: %.1f kg de %.1f kg",
                        mision.combustible, mision.combustible_inicial), TEXTO_X, 350, 18, WHITE);
    DrawText(TextFormat("Puntaje final: %d", mision.puntaje), TEXTO_X, 390, 24, GOLD);
    DrawText(TextFormat("Calificacion: %s", calificacion(&mision)), TEXTO_X, 425, 24, GOLD);
}

// sorteo del evento (artemis.c) y manda el cohete a la altura calculada
static void empezar_turno(void)
{
    sortear_evento(&mision, turno);
    cohete.destino_y = y_por_error(mision.nave.altitud - mision.altitud_objetivo);
    fase = FASE_EVENTO;
    tipo_maniobra = 0;
    campo1[0] = '\0';
    campo2[0] = '\0';
    campo_foco = 1;
    borrar_error();
    vaciar_teclado();
}

static void ejecutar_maniobra(int signo, double dv)
{
    double usado = aplicar_maniobra(&mision, signo, dv);

    cerrar_turno(&mision, turno, usado);
    cohete.destino_y = y_por_error(mision.nave.altitud - mision.altitud_objetivo);
    fase = FASE_RESULTADO;
}

static void simulacion_mision(Mision *ultima, int *hay_ultima)
{
    double error = mision.nave.altitud - mision.altitud_objetivo;
    double dv_max, a, t, dv;
    const RegistroTurno *reg;
    int    aplicar;

    dibujar_panel(opcion == 4 ? "MISION: ESTABILIDAD EN LEO" : "ULTIMO REPORTE");

    if (fase == FASE_FIN) {
        dibujar_reporte();
        return;
    }

    DrawText(TextFormat("Turno %d de %d      Puntaje: %d", turno, MAX_TURNOS, mision.puntaje),
             TEXTO_X, 65, 20, WHITE);
    DrawText(TextFormat("Combustible: %.1f kg de %.1f kg", mision.combustible, mision.combustible_inicial),
             TEXTO_X, 95, 18, WHITE);
    DrawText(TextFormat("Altitud: %.2f km (error: %+.2f km)", mision.nave.altitud / 1000.0, error / 1000.0),
             TEXTO_X, 120, 18, WHITE);

    if (fase >= FASE_EVENTO) {
        DrawText(TextFormat("EVENTO: %s", mision.historial[turno - 1].evento), TEXTO_X, 160, 20, ORANGE);
    }

    if (fase == FASE_SUBIENDO) {
        DrawText("Lanzamiento a la orbita objetivo...", TEXTO_X, 160, 20, SKYBLUE);
        if (!cohete_en_movimiento(&cohete)) {
            empezar_turno();
        }

    } else if (fase == FASE_EVENTO) {
        if (!cohete_en_movimiento(&cohete)) {
            fase = FASE_MANIOBRA;
        }

    } else if (fase == FASE_MANIOBRA) {
        dv_max = dv_maximo(&mision.nave, mision.combustible);
        if (dv_max < 0.001) {
            DrawText("Sin combustible: no se puede corregir.", TEXTO_X, 220, 20, GOLD);
            if (boton(TEXTO_X, 270, 200, 50, "CONTINUAR", 1)) {
                ejecutar_maniobra(0, 0.0);
            }
            return;
        }

        DrawText(TextFormat("Maniobra (alcanza para dv = %.2f m/s):", dv_max), TEXTO_X, 205, 18, WHITE);
        if (boton(430, 235, 170, 46, "ADELANTE", 1)) {
            tipo_maniobra = 1;
            borrar_error();
        }
        if (boton(615, 235, 170, 46, "ATRAS", 1)) {
            tipo_maniobra = 2;
            borrar_error();
        }
        if (boton(800, 235, 170, 46, "NO CORREGIR", 1)) {
            ejecutar_maniobra(0, 0.0);
            return;
        }
        if (tipo_maniobra == 1) {
            DrawRectangleLines(427, 232, 176, 52, GOLD);
        }
        if (tipo_maniobra == 2) {
            DrawRectangleLines(612, 232, 176, 52, GOLD);
        }
        DrawText("Adelante: sube la orbita | Atras: baja la orbita", TEXTO_X, 292, 16, GRAY);

        if (tipo_maniobra != 0) {
            DrawText(TextFormat("Aceleracion [m/s^2] (%g a %g):", ACEL_MIN, ACEL_MAX), TEXTO_X, 330, 18, WHITE);
            if (caja_texto(TEXTO_X, 355, 220, 38, campo1, campo_foco == 1)) {
                campo_foco = 1;
            }
            DrawText(TextFormat("Duracion [s] (%g a %g):", TIEMPO_MIN, TIEMPO_MAX), TEXTO_X, 405, 18, WHITE);
            if (caja_texto(TEXTO_X, 430, 220, 38, campo2, campo_foco == 2)) {
                campo_foco = 2;
            }
            if (IsKeyPressed(KEY_TAB)) {
                campo_foco = 3 - campo_foco;
            }
            aplicar = boton(700, 392, 200, 50, "APLICAR", 1);
            if (IsKeyPressed(KEY_ENTER)) {
                aplicar = 1;
            }
            dibujar_error(485);

            if (aplicar) {
                borrar_error();
                if (validar_campo(campo1, ACEL_MIN, ACEL_MAX, &a, "Aceleracion")
                    && validar_campo(campo2, TIEMPO_MIN, TIEMPO_MAX, &t, "Duracion")) {
                    dv = a * t;
                    if (dv > dv_max) {
                        snprintf(mensaje, LARGO_MENSAJE, "Ese impulso gastaria %.1f kg y solo quedan %.1f kg.",
                                 combustible_para(&mision.nave, dv), mision.combustible);
                    } else if (tipo_maniobra == 1) {
                        ejecutar_maniobra(1, dv);
                    } else {
                        ejecutar_maniobra(-1, dv);
                    }
                }
            }
        }

    } else if (fase == FASE_RESULTADO) {
        if (cohete_en_movimiento(&cohete)) {
            DrawText("Aplicando maniobra...", TEXTO_X, 220, 20, SKYBLUE);
            return;
        }

        reg = &mision.historial[turno - 1];
        DrawText(TextFormat("Combustible usado: %.1f kg", reg->combustible_usado), TEXTO_X, 220, 18, WHITE);
        if (mision.estado == MISION_REINGRESO) {
            DrawText(TextFormat("ALERTA: la altitud bajo de %.0f km. REINGRESO!", ALTITUD_REINGRESO / 1000.0),
                     TEXTO_X, 260, 18, RED);
        } else if (reg->estable) {
            DrawText(TextFormat(">> ORBITA ESTABLE (+%d puntos)", PUNTOS_POR_TURNO), TEXTO_X, 260, 20, GREEN);
        } else {
            DrawText(">> Fuera de la tolerancia (sin puntos)", TEXTO_X, 260, 20, GOLD);
        }

        if (mision.estado == MISION_REINGRESO || turno == MAX_TURNOS) {
            if (boton(TEXTO_X, 320, 280, 50, "VER RESULTADO FINAL", 1)) {
                finalizar_mision(&mision);
                *ultima = mision;               
                *hay_ultima = 1;
                fase = FASE_FIN;
                if (mision.estado == MISION_COMPLETADA) {
                    cohete.destino_y = Y_TOPE;  
                } else {
                    cohete.destino_y = Y_SUELO; 
                }
            }
        } else if (boton(TEXTO_X, 320, 280, 50, "SIGUIENTE TURNO", 1)) {
            turno++;
            empezar_turno();
        }
    }
}

static void pantalla_simulacion(const Nave *nave, Mision *ultima, int *hay_ultima)
{
    int reentrando = (opcion >= 4 && mision.estado == MISION_REINGRESO && fase == FASE_FIN);

    mover_cohete(&cohete);

    if (opcion >= 4) {
        dibujar_escala_mision(mision.altitud_objetivo);
    } else {
        dibujar_escala_orbita();
    }
    if (reentrando && !cohete_en_movimiento(&cohete)) {
        dibujar_explosion(cohete.x, Y_SUELO + 20);
    } else {
        dibujar_cohete(&cohete, reentrando);
    }

    if (opcion == 2) {
        simulacion_mcu(nave);
    } else if (opcion == 3) {
        simulacion_pulso(nave);
    } else {
        simulacion_mision(ultima, hay_ultima);
    }

    if (boton(TEXTO_X, 565, 250, 50, "VOLVER AL MENU", 1)) {
        ir_al_menu();
    }
}

// funciones para main.c

void iniciar_interfaz(void)
{
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(ANCHO_VENTANA, ALTO_VENTANA, "ARTEMIS - Simulador de Orbita Terrestre Baja");
    SetTargetFPS(60);
    iniciar_estrellas();
    iniciar_cohete(&cohete);
    aviso[0] = '\0';
    mensaje[0] = '\0';
    campo_error[0] = '\0';
}

int dibujar_cuadro(Nave *nave, Mision *ultima, int *hay_ultima)
{
    if (WindowShouldClose() || salir) {
        return 0;
    }

    BeginDrawing();
    dibujar_fondo();

    if (pantalla == PANTALLA_MENU) {
        pantalla_menu(nave, *hay_ultima);
    } else if (pantalla == PANTALLA_CONFIG) {
        pantalla_config(nave, ultima);
    } else {
        pantalla_simulacion(nave, ultima, hay_ultima);
    }

    EndDrawing();
    return 1;
}

void cerrar_interfaz(void)
{
    CloseWindow();
}
