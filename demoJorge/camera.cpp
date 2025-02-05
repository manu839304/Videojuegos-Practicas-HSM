#include "camera.h"

// Constructor de la clase Camera, inicializa el nivel de zoom
Camera::Camera(float zoomLevel) : zoomLevel(zoomLevel) {}

// Devuelve una vista ajustada al tamaño de la ventana
sf::View Camera::GetView(sf::Vector2u windowSize)
{

    float aspect = (float)windowSize.x / (float)windowSize.y; // Calcula la relación de aspecto

    sf::Vector2f size{};

    // Ajusta el tamaño de la vista según la relación de aspecto
    if (aspect < 1.0f)
    {
        size = sf::Vector2f(zoomLevel, zoomLevel / aspect);
    }
    else
    {
        size = sf::Vector2f(zoomLevel * aspect, zoomLevel);
    }

    return sf::View(position, size);
}