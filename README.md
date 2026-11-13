* ARTEMIS – Simulador de Órbita Terrestre Baja

* Integrantes del grupo

- Almeida Diaz, Marcelo Ariel
- Barrios, Leonel Luis
- Bertran Parodi, Joaquín Alejo
- Martin, Julieta
- Mecozzi, Bianca Lucianna

**Comisión:** K1.1

* Uso de inteligencia artificial

Durante el desarrollo del proyecto se utilizaron las siguientes herramientas de inteligencia artificial como apoyo:

- Claude
- ChatGPT

* Uso crítico de la inteligencia artificial

Durante el desarrollo del programa se utilizó la inteligencia artificial como herramienta de apoyo para la generación y modificación del código, así como para la
incorporación de la biblioteca **raylib** y el desarrollo de la interfaz gráfica, aspectos con los que los integrantes del grupo no estaban familiarizados.

Sin embargo, su utilización requirió un análisis crítico de las respuestas obtenidas y la realización de distintas modificaciones:

- Algunas de las primeras propuestas de la IA incluían fórmulas, cálculos y conceptos de Física que no fueron desarrollados en la materia Física I. Por este
- motivo, fue necesario revisar los cálculos, identificar los contenidos que no habían sido abordados en la asignatura y modificar el código para adaptarlo a los
- temas estudiados.
- Se incorporaron controles para evitar que el usuario ingresara valores fuera de los rangos permitidos, una característica que las primeras versiones no
- contemplaban.
- Se revisaron las relaciones entre la aceleración, el tiempo de impulso, el cambio de velocidad y el consumo de combustible, buscando que las decisiones del
- usuario tuvieran consecuencias dentro de la simulación y no fueran tratadas como datos aislados.
- Las propuestas iniciales de la interfaz gráfica también fueron revisadas y ajustadas de acuerdo con lo solicitado en el escenario asignado. Se modificó la
- forma de ingresar los datos y de presentar los resultados. Se buscó desarrollar una interfaz sencilla, acorde con los conocimientos y posibilidades de
- comprensión del grupo, lo que permitió entender el funcionamiento de raylib e incorporar sus recursos de manera progresiva.

En conclusión, la inteligencia artificial facilitó el desarrollo del trabajo, pero sus respuestas no fueron utilizadas de manera automática. Fue necesario 
verificar, corregir y adaptar las propuestas, tomando decisiones propias para asegurar que el programa respetara los contenidos de la materia y los objetivos de 
la consigna.

* Descripción general del código

ARTEMIS es un simulador de órbita terrestre baja desarrollado en el lenguaje de programación C. Se trata de un programa organizado por módulos, en el que cada 
uno cumple una función específica. Para su ejecución se utiliza la biblioteca raylib, que permite crear una interfaz gráfica y mostrar el contenido del programa
dentro de una ventana, en lugar de utilizar la terminal tradicional.
El programa se ejecuta mediante su compilación y posterior apertura del archivo ejecutable, utilizando las bibliotecas necesarias de raylib.
Cuenta con un menú principal que permite seleccionar distintas opciones: configurar la altitud y la masa de la nave, realizar un análisis orbital, calcular y
ejecutar un pulso del propulsor lateral, iniciar una misión de estabilización orbital y consultar el último reporte obtenido.
Entre sus funciones principales se encuentran el cálculo de la velocidad orbital, el período, la aceleración centrípeta, las fuerzas gravitatorias y el consumo 
de combustible. También incorpora un sistema de juego por turnos, en el que pueden ocurrir diferentes eventos aleatorios que modifican la altitud de la nave. El 
usuario debe realizar maniobras para mantener la órbita estable.
Al finalizar una misión, el programa genera un reporte con los resultados obtenidos, el combustible utilizado y el puntaje alcanzado. Además, cuenta con 
validaciones para controlar los datos ingresados, mensajes de error y avisos que orientan al usuario durante la interacción.
En cuanto a su representación visual, utiliza raylib para crear una ventana gráfica con fondo oscuro, texto organizado en filas, distintos colores para
diferenciar la información y una tipografía que facilita la lectura. La interacción se realiza principalmente mediante el teclado, permitiendo ingresar valores, 
seleccionar opciones y recorrer el contenido de la pantalla mediante el desplazamiento.
De esta manera, ARTEMIS combina cálculos físicos, simulación y elementos de juego en un programa interactivo, modular y organizado.

* Instrucciones de ejecución

Para ejecutar el programa, se deben seguir los siguientes pasos:

1. Descargar y descomprimir el archivo que contiene el código fuente del proyecto.
2. Abrir una terminal en Visual Studio Code y ubicarse en la carpeta donde se encuentran los archivos del proyecto.
3. Compilar el programa utilizando el siguiente comando:  `make `
Este comando utiliza el archivo `Makefile` para compilar los archivos `.c` y enlazarlos con las bibliotecas necesarias de raylib.
4. Una vez finalizada la compilación, ejecutar el programa mediante el siguiente comando: ./simulador.exe
5. Se abrirá una ventana gráfica con el menú principal de ARTEMIS, desde donde se podrá interactuar con las diferentes opciones del simulador.

* Controles e instrucciones del programa

 1. Iniciar el programa
Al ejecutar el programa, se abre la ventana principal de ARTEMIS, donde se muestran los datos actuales de la nave Orion:
- Altitud orbital.
- Masa de la nave.
 2. Seleccionar una opción
En el menú principal se encuentran cinco opciones:
- Configurar altitud y masa: permite modificar la altitud orbital y la masa de la nave.
- Análisis orbital: calcula la velocidad orbital, el período y la aceleración centrípeta.
- Pulso del propulsor lateral: simula un impulso lateral y calcula el desplazamiento y el cambio de dirección de la nave.
- Jugar – Misión de estabilización orbital: inicia una misión interactiva en la que se debe mantener la nave dentro de una órbita estable.
- Ver último reporte: permite consultar los resultados de la última misión realizada.
Una vez seleccionada la opción deseada, se debe presionar el botón COMENZAR.
3. Configurar los parámetros
Dependiendo de la opción seleccionada, el programa puede solicitar diferentes valores para realizar los cálculos o iniciar la simulación correspondiente.
 4. Ejecutar la simulación
Una vez confirmados los parámetros, comienza la simulación correspondiente:
- Análisis orbital: se muestran los cálculos físicos realizados.
- Pulso lateral: se observa el movimiento del cohete y, posteriormente, se presentan los resultados obtenidos.
- Misión de estabilización: se desarrollan diferentes turnos en los que pueden ocurrir eventos aleatorios que afectan la altitud de la nave.
 5. Jugar la misión de estabilización orbital
El objetivo de esta misión es mantener la nave Orion cerca de su altitud orbital inicial durante un total de **cinco turnos**.
Durante la partida pueden ocurrir distintos eventos que modifican las condiciones de la nave. El usuario deberá tomar decisiones y realizar maniobras para
intentar conservar una órbita estable.
 6. Consultar los resultados
Al finalizar la misión, se muestra un reporte que contiene la siguiente información:
- Resultado de la misión.
- Historial de los turnos.
- Eventos ocurridos.
- Combustible utilizado y restante.
- Altitud final de cada turno.
- Puntaje obtenido.
- Calificación final.
 7. Volver al menú o salir
Una vez finalizada la simulación, se puede utilizar el botón **VOLVER AL MENÚ** para realizar otra operación o seleccionar **SALIR** desde la pantalla principal
para cerrar el programa.

* Dificultades encontradas
Durante el desarrollo del proyecto surgieron distintas dificultades, principalmente relacionadas con la falta de experiencia de los integrantes en el lenguaje C
y en el manejo de sus herramientas. Fue necesario familiarizarse con la estructura del código, la organización en distintos archivos y la resolución de errores
de compilación.
Otra dificultad importante fue la incorporación de **raylib**, ya que el grupo no tenía conocimientos previos sobre la creación de interfaces gráficas ni sobre
la configuración y utilización de bibliotecas externas.
Además, se presentaron dificultades para adaptar los cálculos físicos al nivel de los contenidos desarrollados en Física I y lograr que las distintas
funcionalidades del simulador trabajaran correctamente en conjunto.

Estas dificultades requirieron investigación, pruebas, correcciones y el aprendizaje progresivo de nuevas herramientas. A través de este proceso, los integrantes pudieron adquirir nuevos conocimientos sobre programación en C, organización modular del código y desarrollo de interfaces gráficas.
