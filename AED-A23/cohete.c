#include <math.h>
#include "artemis.h"     
#include "cohete.h"
#include "raylib.h"

#define CANT_ESTRELLAS 90
#define ANCHO_ESCENA   380     
#define Y_TIERRA       640      

static int estrella_x[CANT_ESTRELLAS];
static int estrella_y[CANT_ESTRELLAS];
static int estrella_r[CANT_ESTRELLAS];

static Vector2 punto(float x, float y)
{
    Vector2 p;

    p.x = x;
    p.y = y;
    return p;
}

static float limitar(float valor, float minimo, float maximo)
{
    if (valor < minimo) {
        return minimo;
    }
    if (valor > maximo) {
        return maximo;
    }
    return valor;
}

static float acercar(float actual, float destino, float paso)
{
    if (actual < destino) {
        actual += paso;
        if (actual > destino) {
            actual = destino;
        }
    } else if (actual > destino) {
        actual -= paso;
        if (actual < destino) {
            actual = destino;
        }
    }
    return actual;
}

// Fondo                                                              

void iniciar_estrellas(void)
{
    int i;

    for (i = 0; i < CANT_ESTRELLAS; i++) {
        estrella_x[i] = GetRandomValue(0, 999);
        estrella_y[i] = GetRandomValue(0, Y_TIERRA - 5);
        estrella_r[i] = GetRandomValue(1, 2);
    }
}

void dibujar_fondo(void)
{
    int i;

    ClearBackground(GetColor(0x070B1AFF));
    for (i = 0; i < CANT_ESTRELLAS; i++) {
        DrawCircle(estrella_x[i], estrella_y[i], (float)estrella_r[i], Fade(WHITE, 0.8f));
    }

    /* La Tierra: una franja azul abajo */
    DrawRectangle(0, Y_TIERRA, 1000, 700 - Y_TIERRA, GetColor(0x1D4E89FF));
    DrawRectangle(0, Y_TIERRA, 1000, 4, GetColor(0x4FA3E0FF));
    DrawText("TIERRA", 10, Y_TIERRA + 20, 20, WHITE);
}

// Cohete                                                             

void iniciar_cohete(Cohete *c)
{
    c->x = X_COHETE;
    c->y = Y_SUELO;
    c->destino_x = X_COHETE;
    c->destino_y = Y_SUELO;
}

void mover_cohete(Cohete *c)
{
    float paso = (float)(VELOCIDAD_COHETE * GetFrameTime());

    c->x = acercar(c->x, c->destino_x, paso);
    c->y = acercar(c->y, c->destino_y, paso);
}

int cohete_en_movimiento(const Cohete *c)
{
    return (c->x != c->destino_x || c->y != c->destino_y);
}

void dibujar_cohete(const Cohete *c, int reentrando)
{
    float x = c->x;
    float y = c->y;
    float largo;
    int   llama = 0;       

    if (reentrando) {
        llama = 2;
    } else if (cohete_en_movimiento(c) && c->destino_y <= c->y) {
        llama = 1;         
    }

    if (llama > 0) {
        largo = 30.0f + 10.0f * (float)sin(GetTime() * 30.0);
        if (llama == 1) {
            DrawTriangle(punto(x - 12, y + 40), punto(x, y + 40 + largo), punto(x + 12, y + 40), ORANGE);
            DrawTriangle(punto(x - 6, y + 40), punto(x, y + 40 + largo * 0.6f), punto(x + 6, y + 40), YELLOW);
        } else {
            DrawTriangle(punto(x - 12, y + 40), punto(x, y + 40 + largo), punto(x + 12, y + 40), RED);
            DrawTriangle(punto(x - 6, y + 40), punto(x, y + 40 + largo * 0.6f), punto(x + 6, y + 40), ORANGE);
        }
    }

    // Aletas 
    DrawTriangle(punto(x - 15, y + 10), punto(x - 30, y + 40), punto(x - 15, y + 40), RED);
    DrawTriangle(punto(x + 15, y + 10), punto(x + 15, y + 40), punto(x + 30, y + 40), RED);

    // Cuerpo
    DrawRectangle((int)(x - 15), (int)(y - 40), 30, 80, LIGHTGRAY);
    DrawRectangle((int)(x - 15), (int)(y + 22), 30, 8, DARKGRAY);

    // Punta
    DrawTriangle(punto(x, y - 70), punto(x - 15, y - 40), punto(x + 15, y - 40), RED);

    // Ventana
    DrawCircle((int)x, (int)(y - 12), 9.0f, SKYBLUE);
    DrawCircleLines((int)x, (int)(y - 12), 9.0f, DARKGRAY);
}

void dibujar_explosion(float x, float y)
{
    float latido = (float)sin(GetTime() * 12.0);

    DrawCircle((int)x, (int)y, 55.0f + 6.0f * latido, Fade(RED, 0.8f));
    DrawCircle((int)x, (int)y, 38.0f + 5.0f * latido, ORANGE);
    DrawCircle((int)x, (int)y, 20.0f + 4.0f * latido, YELLOW);
}

// Opciones 2 y 3: 0 km abajo, 2000 km (ALTITUD_MAX_KM) arriba.
float y_por_altitud(double altitud)
{
    double fraccion = altitud / (ALTITUD_MAX_KM * 1000.0);
    float  y = (float)(Y_SUELO - fraccion * (Y_SUELO - Y_TOPE));

    return limitar(y, Y_TOPE, Y_SUELO);
}

float y_por_error(double error)
{
    float y = (float)(Y_CENTRO - (error / 1000.0) * PX_POR_KM);

    return limitar(y, Y_TOPE, Y_SUELO);
}

// Opcion 3: el desplazamiento maximo posible es el de a = ACEL_MAX y dt = TIEMPO_MAX; ese valor corresponde a 120 pixeles hacia la derecha, creo 
float x_por_desplazamiento(double desplazamiento)
{
    double maximo = 0.5 * ACEL_MAX * TIEMPO_MAX * TIEMPO_MAX;
    double fraccion = desplazamiento / maximo;

    if (fraccion > 1.0) {
        fraccion = 1.0;
    }
    return (float)(X_COHETE + fraccion * 120.0);
}


void dibujar_escala_orbita(void)
{
    int   km;
    float y;

    DrawText("Altura en pantalla = altitud orbital", 10, 10, 16, GRAY);
    for (km = 500; km <= 2000; km += 500) {
        y = y_por_altitud(km * 1000.0);
        DrawLine(0, (int)y, ANCHO_ESCENA, (int)y, Fade(SKYBLUE, 0.25f));
        DrawText(TextFormat("%d km", km), 10, (int)y - 18, 16, SKYBLUE);
    }
}

void dibujar_escala_mision(double altitud_objetivo)
{
    int   km;
    float y, y_sup, y_inf, y_reingreso;

    DrawText("Franja verde: tolerancia +-10 km", 10, 10, 16, GRAY);

    // orbita estable
    y_sup = y_por_error(TOLERANCIA);
    y_inf = y_por_error(-TOLERANCIA);
    DrawRectangle(0, (int)y_sup, ANCHO_ESCENA, (int)(y_inf - y_sup), Fade(GREEN, 0.25f));

    // Lineas cada 20 km alrededor del objetivo 
    for (km = -60; km <= 60; km += 20) {
        y = y_por_error(km * 1000.0);
        DrawLine(0, (int)y, ANCHO_ESCENA, (int)y, Fade(SKYBLUE, 0.25f));
        if (km == 0) {
            DrawText("OBJETIVO", 10, (int)y - 18, 16, GREEN);
        } else {
            DrawText(TextFormat("%+d km", km), 10, (int)y - 18, 16, SKYBLUE);
        }
    }

    // Linea roja de reingreso, solo si cae dentro de la zona visible 
    y_reingreso = (float)(Y_CENTRO - ((ALTITUD_REINGRESO - altitud_objetivo) / 1000.0) * PX_POR_KM);
    if (y_reingreso >= Y_TOPE && y_reingreso <= Y_SUELO) {
        DrawLine(0, (int)y_reingreso, ANCHO_ESCENA, (int)y_reingreso, RED);
        DrawText("REINGRESO", 10, (int)y_reingreso - 18, 16, RED);
    }
}
