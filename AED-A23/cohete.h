#ifndef COHETE_H
#define COHETE_H


// El cohete con su CENTRO en x, y 
#define X_COHETE    190      
#define Y_SUELO     600      
#define Y_TOPE      110     
#define Y_CENTRO    355      
#define PX_POR_KM   4.0      
#define VELOCIDAD_COHETE 150.0   

typedef struct {
    float x;
    float y;
    float destino_x;
    float destino_y;
} Cohete;

void iniciar_estrellas(void);
void dibujar_fondo(void);                       // espacio + estrellas + Tierra 

void iniciar_cohete(Cohete *c);                 // sw apoya en la Tierra
void mover_cohete(Cohete *c);                   // acerca al destino 
int  cohete_en_movimiento(const Cohete *c);     
void dibujar_cohete(const Cohete *c, int reentrando);
void dibujar_explosion(float x, float y);

// para convertir los datos de la simulacion en posiciones 
float y_por_altitud(double altitud);            // opciones 2 y 3: 0 a 2000 km 
float y_por_error(double error);                // mision: error = altitud - objetivo 
float x_por_desplazamiento(double desplazamiento);   // opcion 3 

void dibujar_escala_orbita(void);
void dibujar_escala_mision(double altitud_objetivo);

#endif
