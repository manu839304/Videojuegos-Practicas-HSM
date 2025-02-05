# Variables
CC=g++
CFLAGS=-I"../SFML/include" -g -Wall -std=c++20
LDFLAGS= -L"../SFML/lib" -lSFML-audio -lSFML-graphics -lSFML-window -lSFML-system
SRC=main.cpp demo.cpp camera.cpp renderer.o resources.o map.o
OBJ_DIR=build
OBJ=$(OBJ_DIR)/main.o $(OBJ_DIR)/demo.o $(OBJ_DIR)/camera.o $(OBJ_DIR)/renderer.o $(OBJ_DIR)/resources.o $(OBJ_DIR)/map.o
EXEC=$(OBJ_DIR)/game.exe

# Regla por defecto (compilar todo)
all: $(EXEC)

# Enlazar el ejecutable
$(EXEC): $(OBJ) | $(OBJ_DIR)
	$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS)
	
# --------------------------
# Compilar main.cpp a main.o
$(OBJ_DIR)/main.o: main.cpp demo.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c main.cpp -o $(OBJ_DIR)/main.o

# Compilar demo.cpp a demo.o
$(OBJ_DIR)/demo.o: demo.cpp demo.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c demo.cpp -o $(OBJ_DIR)/demo.o

# Compilar camera.cpp a camera.o
$(OBJ_DIR)/camera.o: camera.cpp camera.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c camera.cpp -o $(OBJ_DIR)/camera.o

# Compilar renderer.cpp a renderer.o
$(OBJ_DIR)/renderer.o: renderer.cpp renderer.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c renderer.cpp -o $(OBJ_DIR)/renderer.o

# Compilar map.cpp a map.o
$(OBJ_DIR)/map.o: map.cpp map.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c map.cpp -o $(OBJ_DIR)/map.o

# Compilar resources.cpp a resources.o
$(OBJ_DIR)/resources.o: resources.cpp resources.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c resources.cpp -o $(OBJ_DIR)/resources.o

# --------------------------

# Limpiar archivos generados
clean:
	@rm -f $(OBJ) $(EXEC) || del -f $(OBJ) $(EXEC)
