
CC     = gcc
CFLAGS = -Wall -O2
SRC    = main.c artemis.c validaciones.c interfaz.c cohete.c
OUT    = artemis

ifeq ($(OS),Windows_NT)
    LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm -lm
    OUT  = artemis.exe
else ifeq ($(shell uname),Darwin)
    LIBS = -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -lm
else
    LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif

$(OUT): $(SRC) artemis.h validaciones.h interfaz.h cohete.h
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LIBS)

clean:
	rm -f artemis artemis.exe
