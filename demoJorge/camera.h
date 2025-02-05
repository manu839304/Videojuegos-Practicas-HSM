#pragma once

#include <SFML/Graphics.hpp>

// Clase que representa una cámara en SFML
class Camera
{
public:
    // Constructor que inicializa el nivel de zoom con un valor por defecto
    Camera(float zoomLevel = 5.0f);

    // Devuelve la vista ajustada según el tamaño de la ventana
    sf::View GetView(sf::Vector2u windowSize);

    float zoomLevel;
    sf::Vector2f position;
};