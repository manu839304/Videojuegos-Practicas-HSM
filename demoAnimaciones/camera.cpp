#include "camera.h"


Camera::Camera(sf::Vector2f _viewSize) : startVertex(0.f, 0.f), viewSize(_viewSize) {}

Camera::Camera(sf::FloatRect _viewRect) : startVertex(_viewRect.position), viewSize(_viewRect.size) {}

Camera::Camera(sf::Vector2f _startVertex, sf::Vector2f _viewSize) :
                startVertex(_startVertex), viewSize(_viewSize) {}


sf::View Camera::GetView(const sf::Vector2u& windowSize) const
{
    const float windowAspect = static_cast<float>(windowSize.x) / windowSize.y;
    const float viewAspect   = viewSize.x / viewSize.y;
    sf::Vector2f adjustedSize;

    if (windowAspect > viewAspect)
    {
        // La ventana es más ancha que la vista original: fijamos la altura
        adjustedSize.y = viewSize.y;
        adjustedSize.x = viewSize.y * windowAspect;
    }
    else
    {
        // La ventana es más estrecha: fijamos el ancho
        adjustedSize.x = viewSize.x;
        adjustedSize.y = viewSize.x / windowAspect;
    }

    return sf::View(sf::FloatRect(startVertex, adjustedSize));
}
