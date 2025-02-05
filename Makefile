# Variables
CC=g++
CFLAGS=-I"./SFML/include" -g -Wall -std=c++20
LDFLAGS= -L"./SFML/lib" -lSFML-audio -lSFML-graphics -lSFML-window -lSFML-system
SRC=main.cpp
OBJ_DIR=build
OBJ=$(OBJ_DIR)/main.o
EXEC=$(OBJ_DIR)/game.exe

# Regla por defecto (compilar todo)
all: $(EXEC)

# Enlazar el ejecutable
$(EXEC): $(OBJ) | $(OBJ_DIR)
	$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS)

# Compilar el archivo .cpp a .o
$(OBJ): $(SRC) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(SRC) -o $(OBJ)

# Crear la carpeta build si no existe
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Limpiar archivos generados
clean:
	@rm -f $(OBJ) $(EXEC) || del -f $(OBJ) $(EXEC)
