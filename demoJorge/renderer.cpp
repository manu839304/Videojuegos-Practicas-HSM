#include "renderer.h"
#include <vector>

Renderer::Renderer(sf::RenderTarget &target) : target(target)
{
}

// Método para dibujar una textura en una posición y tamaño especificados, con un recorte opcional.
void Renderer::DrawCut(const sf::Texture &texture, const sf::Vector2f &position, const sf::IntRect &cut)
{
    sf::Sprite sprite(texture);
    sprite.setTextureRect(cut);
    sprite.setPosition(position);

    target.draw(sprite);
}

// Método para dibujar una textura en una posición y tamaño especificados, sin recorte.
void Renderer::Draw(const sf::Texture &texture, const sf::Vector2f &position)
{
    sf::Sprite sprite(texture);
    sprite.setTexture(texture);
    sprite.setPosition(position);

    target.draw(sprite);
}
