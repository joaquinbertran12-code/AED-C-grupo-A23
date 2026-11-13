#ifndef ARTEMIS_H
#define ARTEMIS_H

/* Constantes fisicas (SI) */
#define G_CONST        6.674e-11     // N*m^2/kg^2 
#define MASA_TIERRA    5.972e24      // kg
#define RADIO_TIERRA   6371000.0     // m
#define PI             3.14159265358979
#define GRAVEDAD       9.80          // m/s^2

// Valores por defecto y rangos de entrada 
#define ALTITUD_DEFECTO  192000.0    // m 
#define MASA_DEFECTO     33000.0     // kg
#define ALTITUD_MIN_KM   160.0
#define ALTITUD_MAX_KM   2000.0
#define MASA_MIN         1000.0
#define MASA_MAX         100000.0

// Rangos del pulso / maniobra 
#define ACEL_MIN    0.01            
#define ACEL_MAX    5.0
#define TIEMPO_MIN  0.1              
#define TIEMPO_MAX  120.0

// Reglas del juego 
#define MAX_TURNOS            5
#define FRACCION_COMBUSTIBLE  0.01      /* del peso total de la nave */
#define VELOCIDAD_GASES       3000.0    // m/s
#define TOLERANCIA            10000.0   // m: orbita estable |error| <= 10 km 
#define ALTITUD_REINGRESO     120000.0  // m 
#define PUNTOS_POR_TURNO      150
#define PUNTOS_BONUS_COMBUSTIBLE 60     // maximo, si no se gasta /
#define MISION_EN_CURSO    0
#define MISION_COMPLETADA  1
#define MISION_REINGRESO   2

typedef struct {
    double altitud;   // m
    double masa;      // kg 
} Nave;

typedef struct {
    double r;
    double fuerza_grav;
    double fuerza_cent;
    double v;
    double periodo;
    double ac;
    double ac_en_g;
} ResultadoMCU;

// Resultados del pulso lateral 
typedef struct {
    double v;               // velocidad orbital actual, m/s
    double dv;              // velocidad lateral ganada, m/s 
    double desplazamiento;  // desviacion lateral, m 
    double v_final;         // modulo de la velocidad final, m/s
    double angulo;          // giro de la direccion, grados 
} ResultadoPulso;

typedef struct {
    int    turno;
    char   evento[30];
    double combustible_usado;   
    double altitud_final;       
    int    estable;             
} RegistroTurno;

typedef struct {
    Nave          nave;
    double        altitud_objetivo;
    double        combustible;          
    double        combustible_inicial; 
    int           turnos_jugados;
    int           puntaje;
    int           estado;
    RegistroTurno historial[MAX_TURNOS];
} Mision;

/* ---- Calculos basicos ---- */
double velocidad_orbital(double altitud);
double combustible_para(const Nave *n, double dv);
double dv_maximo(const Nave *n, double combustible);
double metros_por_dv(const Nave *n);   

/* ---- Opcion 1: configurar ---- */
void configurar_nave(Nave *n, double altitud_km, double masa);

/* ---- Opcion 2: analisis orbital ---- */
void calcular_mcu(const Nave *n, ResultadoMCU *res);

// Opcion 3: pulso lateral 
void calcular_pulso(const Nave *n, double a, double dt, ResultadoPulso *res);

// Opcion 4: mision (un turno = sortear_evento > aplicar_maniobra > cerrar_turno) 
void   iniciar_mision(Mision *m, const Nave *n);
int    sortear_evento(Mision *m, int turno);
double aplicar_maniobra(Mision *m, int signo, double dv);
void   cerrar_turno(Mision *m, int turno, double combustible_usado);
void   finalizar_mision(Mision *m);
const char *calificacion(const Mision *m);

#endif
