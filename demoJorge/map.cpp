#include "Map.h"

// Constructor que inicializa la cuadrícula vacía
Map::Map() : grid() {}

void Map::CreateFromImage(const sf::Image &image)
{
    grid.clear();

    grid = std::vector(image.getSize().x, std::vector(image.getSize().y, 0));

    for (size_t x = 0; x < grid.size(); x++)
    {
        for (size_t y = 0; y < grid[x].size(); y++)
        {
            sf::Vector2u pixelCoords(static_cast<unsigned int>(x), static_cast<unsigned int>(y));
            sf::Color color = image.getPixel(pixelCoords);
            if (color == sf::Color::Black)
            {
                grid[x][y] = 1;
            }
        }
    }
}

void Map::Draw(Renderer &renderer)
{
    int x = 0;

    // Recorre la cuadrícula y dibuja las celdas activas
    for (const auto &column : grid)
    {
        int y = 0;
        for (const auto &cell : column)
        {
            if (cell)
            {

                // Obtener la textura
                const sf::Texture &texture = Resources::textures["square"];

                // Obtener el tamaño de la textura
                sf::Vector2u textureSize = texture.getSize();

                // Usar el tamaño de la textura para separarlos
                renderer.Draw(texture, sf::Vector2f(textureSize.x * x, textureSize.y * y));
            }
            y++;
        }
        x++;
    }
}

void Map::DrawBackground(Renderer &renderer)
{
    renderer.DrawCut(Resources::textures["background"], sf::Vector2f(0, 0), sf::IntRect({1, 11}, {768, 175}));
}