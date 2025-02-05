# Videojuegos by Eduardo Mena

Creando un videojuego 2D divertido que tenga a Eduardo Mena como boss final.

## Librerías usadas:

|      Librería      | Descripción |
|--------------------|-------------|
| SFML               | Gráficos, ventana de juego, inputs, audio, network, etc. |

## Primeros pasos del repositorio para ejecutar el juego

1. Hacer "pull" (ya incluye todas las librerías y .dll)
2. Ejecutar "make clean".
3. Ejecutar comando "make". CUIDADO: si quieres poner las librerías y/o .dll en otro sitio, hay que modificar rutas de las líneas 3 y 4 del MAKEFILE haciendo caso a los comentarios
4. Ejecutar comando "./build/game.exe"

## ANEXO 1: Preparación de entorno e Instalación desde 0 de SFML 3.0 en Windows (29/01/2025)

1. Crear el proyecto en una ruta sin nombres raros (espacios, signos raros, etc...). La carpeta del repositorio del Github Desktop sirve.
2. Descargar del repositorio de SFML versión 3.0: SFML-3.0.0-windows-gcc-14.2.0-mingw-64-bit.zip (para tener la librería)
3. Descomprimir el zip en el directorio del proyecto (lo ideal es tener una carpeta propia para tu mingw). Llamemos a la carpeta descomprimida "SFML".
4. Finalmente, entramos a la carpeta SFML/bin/ y copiamos el ficheros .dll para pegarlos al lado del "main.cpp" del proyecto (de momento por conveniencia).
5. Si se ha hecho todo bien, ya podemos ejecutar el Makefile del repositorio del proyecto y lanzar el ejecutable game.exe de la carpeta ./build
