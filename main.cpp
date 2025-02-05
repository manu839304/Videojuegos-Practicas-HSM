#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>


bool gEnMovimiento { false };
constexpr int escala { 1 };
constexpr int gWindowWidth{ 800 * escala};
constexpr int gWindowHeight{ 250 * escala};

int main()
{
    std::string nombreVentana { "Castlevania: En busca de la Eduardomena Pose" };
    sf::RenderWindow window(sf::VideoMode({gWindowWidth, gWindowHeight}), nombreVentana, sf::Style::Default);
    window.setVerticalSyncEnabled(true);

    sf::Texture bgTexture;
    if (!bgTexture.loadFromFile("../assets/maps/level1Entrance.png", false))
    {
        std::cerr << "Error cargando la textura" << std::endl;
    }
    sf::Sprite bgSprite(bgTexture);
    bgSprite.setTextureRect(sf::IntRect({1, 11}, {768, 192}));

    sf::Image simonImage;
    if (!simonImage.loadFromFile("../assets/sprites/player/simonBelmont.png"))
    {
        std::cerr << "Error cargando la imagen" << std::endl;
    }
    simonImage.createMaskFromColor(sf::Color(0x74, 0x74, 0x74));    // color key
    sf::Texture simonTexture(simonImage, false);
    sf::Sprite simonSprite(simonTexture);
    simonSprite.setTextureRect(sf::IntRect({1, 21}, {16, 32}));
    simonSprite.setPosition({245.f, 139.f});

    bool haciaIzquierda { false };
    bool haciaDerecha { false };
    bool haciaArriba { false };

    // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (const auto* resized = event->getIf<sf::Event::Resized>())
            {
                std::cout << "new width: " << resized->size.x << std::endl;
                std::cout << "new height: " << resized->size.y << std::endl;
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                switch (keyPressed->scancode)
                {
                case sf::Keyboard::Scancode::Escape:
                    window.close();
                    break;
                case sf::Keyboard::Scancode::Up:
                    haciaArriba = true;
                    simonSprite.move({0.f, -1.5f});
                    break;
                case sf::Keyboard::Scancode::Down:
                    simonSprite.move({0.f, 1.5f});
                    break;
                case sf::Keyboard::Scancode::Left:
                    haciaIzquierda = true;
                    haciaDerecha = false;
                    simonSprite.setScale({1.f, 1.f});
                    break;
                case sf::Keyboard::Scancode::Right:
                    haciaDerecha = true;
                    haciaIzquierda = false;
                    simonSprite.setScale({-1.f, 1.f});
                    break;
                default:
                    break;
                }
            } else if (const auto* keyPressed = event->getIf<sf::Event::KeyReleased>())
            {
                switch (keyPressed->scancode)
                {
                case sf::Keyboard::Scancode::Up:
                    haciaArriba = false;
                    break;
                case sf::Keyboard::Scancode::Left:
                    haciaIzquierda = false;
                    break;
                case sf::Keyboard::Scancode::Right:
                    haciaDerecha = false;
                    break;
                default:
                    break;
                }
            }
        }

        if (haciaArriba)
        {
            if (haciaDerecha)
            {
                simonSprite.move({1.f, -1.5f});
            }
            else if (haciaIzquierda)
            {
                simonSprite.move({-1.f, -1.5f});
            }
            else
            {
                simonSprite.move({0.f, -1.5f});
            }
        }
        if (haciaIzquierda)
        {
            simonSprite.move({-1.5f, 0.f});
        }
        else if (haciaDerecha)
        {
            simonSprite.move({1.5f, 0.f});
        }
        
        window.clear(sf::Color::Black); // obligatorio limpiar la ventana antes de dibujar SIEMPRE

        window.draw(bgSprite);
        window.draw(simonSprite);

        window.display();
    }
}