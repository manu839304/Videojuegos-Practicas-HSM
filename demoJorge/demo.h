#pragma once

#include <SFML/Graphics.hpp>
#include "renderer.h"
#include "camera.h"

extern Camera camera; // Instancia de la cámara

// Inicializa los recursos y configuraciones necesarias
void Init(const sf::Window &window);

// Actualiza la lógica del programa en función del tiempo transcurrido
void Update(float deltaTime);

// Renderiza los elementos en la ventana
void Render(Renderer &renderer);

void createFilledSquareTexture(int size, int alpha);