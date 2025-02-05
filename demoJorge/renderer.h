#pragma once

#include <SFML/Graphics.hpp>

class Renderer
{
public:
    Renderer(sf::RenderTarget &target);

    // Método para dibujar una textura en una posición y tamaño especificados, con un recorte opcional.
    void DrawCut(const sf::Texture &texture, const sf::Vector2f &position, const sf::IntRect &cut);

    // Método para dibujar una textura en una posición y tamaño especificados, sin recorte.
    void Draw(const sf::Texture &texture, const sf::Vector2f &position);

private:
    sf::RenderTarget &target; // Referencia al objetivo de renderizado.
};