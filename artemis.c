#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "artemis.h"


#define CANT_EVENTOS 6

const char NOMBRES[CANT_EVENTOS][30] = {
    "Arrastre atmosferico",
    "Tormenta solar",
    "Venteo de gases",
    "Impulso por micrometeoroide",
    "Residuo espacial cercano",
    "Orbita tranquila"
};

const double EFECTOS[CANT_EVENTOS] = {
    -8000.0, -15000.0, 6000.0, 9000.0, -10000.0, 0.0
};

double velocidad_orbital(double altitud)
{
    double r = RADIO_TIERRA + altitud;
    return sqrt(G_CONST * MASA_TIERRA / r);
}

double combustible_para(const Nave *n, double dv)
{
    return n->masa * dv / VELOCIDAD_GASES;
}

double dv_maximo(const Nave *n, double combustible)
{
    return combustible * VELOCIDAD_GASES / n->masa;
}

double metros_por_dv(const Nave *n)
{
    double r = RADIO_TIERRA + n->altitud;
    double v = velocidad_orbital(n->altitud);

    return 2.0 * r / v;
}

void configurar_nave(Nave *n, double altitud_km, double masa)
{
    n->altitud = 1000.0 * altitud_km;
    n->masa = masa;
}

void calcular_mcu(const Nave *n, ResultadoMCU *res)
{
    res->r = RADIO_TIERRA + n->altitud;
    res->fuerza_grav = G_CONST * MASA_TIERRA * n->masa / (res->r * res->r);
    res->v = velocidad_orbital(n->altitud);
    res->fuerza_cent = n->masa * res->v * res->v / res->r;
    res->periodo = 2.0 * PI * res->r / res->v;
    res->ac = res->v * res->v / res->r;
    res->ac_en_g = res->ac / GRAVEDAD;
}

void calcular_pulso(const Nave *n, double a, double dt, ResultadoPulso *res)
{
    res->v = velocidad_orbital(n->altitud);

    res->dv = a * dt;
    res->desplazamiento = 0.5 * a * dt * dt;
    res->v_final = sqrt(res->v * res->v + res->dv * res->dv);
    res->angulo = atan(res->dv / res->v) * 180.0 / PI;
}

void iniciar_mision(Mision *m, const Nave *n)
{
    m->nave = *n;
    m->altitud_objetivo = n->altitud;
    m->combustible_inicial = FRACCION_COMBUSTIBLE * n->masa;
    m->combustible = m->combustible_inicial;
    m->turnos_jugados = 0;
    m->puntaje = 0;
    m->estado = MISION_EN_CURSO;
}

// Primera parte de jugar_turno: sortea el evento y lo aplica a la altitud.
// Guarda el nombre del evento en el historial. Devuelve el numero de evento. 
int sortear_evento(Mision *m, int turno)
{
    int evento = rand() % CANT_EVENTOS;

    m->nave.altitud += EFECTOS[evento];
    strcpy(m->historial[turno - 1].evento, NOMBRES[evento]);
    return evento;
}

// Un impulso dv cambia la altitud en dh = 2*r*dv/v 
double aplicar_maniobra(Mision *m, int signo, double dv)
{
    double r = RADIO_TIERRA + m->nave.altitud;
    double v = velocidad_orbital(m->nave.altitud);
    double usado = combustible_para(&m->nave, dv);

    m->nave.altitud += signo * 2.0 * r * dv / v;
    m->combustible -= usado;
    if (m->combustible < 0.0) {
        m->combustible = 0.0;   // evita valores negativos por redondeo 
    }
    return usado;
}
// Aca marce
void cerrar_turno(Mision *m, int turno, double combustible_usado)
{
    RegistroTurno *reg = &m->historial[turno - 1];
    double error = m->nave.altitud - m->altitud_objetivo;

    reg->combustible_usado = combustible_usado;
    m->turnos_jugados = turno;
    reg->turno = turno;
    reg->altitud_final = m->nave.altitud;
    reg->estable = (fabs(error) <= TOLERANCIA);

    if (m->nave.altitud < ALTITUD_REINGRESO) {
        m->estado = MISION_REINGRESO;
    } else if (reg->estable) {
        m->puntaje += PUNTOS_POR_TURNO;
    }
}

void finalizar_mision(Mision *m)
{
    if (m->estado == MISION_EN_CURSO) {
        m->estado = MISION_COMPLETADA;
        m->puntaje += (int)(PUNTOS_BONUS_COMBUSTIBLE * m->combustible / m->combustible_inicial);
    }
}

const char *calificacion(const Mision *m)
{
    if (m->estado != MISION_COMPLETADA) {
        return "Reintentar";
    }
    if (m->puntaje >= 700) {
        return "Comandante de mision";
    }
    if (m->puntaje >= 500) {
        return "Piloto";
    }
    if (m->puntaje >= 300) {
        return "Cadete";
    }
    return "Necesita mas entrenamiento";
}
