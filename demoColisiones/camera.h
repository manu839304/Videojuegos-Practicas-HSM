#pragma once

#include <SFML/Graphics.hpp>

// Clase que representa una cámara en SFML
class Camera
{
public:
    sf::Vector2f viewSize;
    sf::Vector2f startVertex;

    Camera(sf::Vector2f _viewSize);
    Camera(sf::FloatRect _viewRect);
    Camera(sf::Vector2f _startVertex, sf::Vector2f _viewSize);

    // Devuelve la vista ajustada según el tamaño de la ventana
    sf::View GetView(const sf::Vector2u& windowSize) const;
};