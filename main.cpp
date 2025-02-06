#include <SFML/Window.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include <cmath>
#include <iostream>
#include <algorithm>

using namespace std;

bool gEnMovimiento { false };
constexpr int escala { 1 };
unsigned int gWindowWidth{ 800 * escala };
unsigned int gWindowHeight{ 600 * escala };
float max_zoom { 10 };
float min_zoom { 0.7 };

float randomFloat (float min, float max)
{
    return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
}



int main(){
    std::string nombreVentana { "Rebote simple" };
    sf::RenderWindow window(sf::VideoMode({gWindowWidth, gWindowHeight}), nombreVentana, sf::Style::Default);
    window.setVerticalSyncEnabled(true);
    
    srand(static_cast<unsigned int>(time(0)));

    sf::Image simonImage;
    if (!simonImage.loadFromFile("./assets/sprites/player/simonBelmont.png"))
    {
        std::cerr << "Error cargando la imagen" << std::endl;
        return -1;
    }
    simonImage.createMaskFromColor(sf::Color(0x74, 0x74, 0x74));    // color key
    sf::Texture simonTexture;
    if (!simonTexture.loadFromImage(simonImage))
    {
        std::cerr << "Error cargando la textura" << std::endl;
        return -1;
    }
    sf::Sprite simonSprite(simonTexture);
    simonSprite.setTextureRect(sf::IntRect({1, 21}, {16, 32}));
    
    float zoom = 2.5;

    simonSprite.setScale(sf::Vector2f(zoom, zoom));

    sf::SoundBuffer buffer;
    if(!buffer.loadFromFile("./assets/sounds/08.wav"))
    {
        std::cerr << "Error cargando el audio" << std::endl;
        return -1;
    }
    sf::Sound sound(buffer);

    // posicion, velocidad y direccion random
    auto resetSprite = [&](){
        simonSprite.setPosition({randomFloat(0, gWindowWidth - 16 * zoom), randomFloat(0, gWindowHeight - 32 * zoom)});
        float speed = randomFloat(0.1f, 0.5f);

        float angle = randomFloat(0.f, 2.f * 3.14159f);
    
        float dx = speed * cos(angle);
        float dy = speed * sin(angle);

        return std::make_pair(dx, dy);
    };
    /*
    auto centerSprite = [&](){
        simonSprite.setPosition({gWindowWidth / 2.f, gWindowHeight / 2.f});
    };*/

    auto [dx, dy] = resetSprite();
    sf::Vector2u initialWindowSize = window.getSize();
    sf::View view(window.getDefaultView());
    sf::Clock clock;    
    while (window.isOpen()){

        while (const std::optional event  = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            if (event->is<sf::Event::KeyPressed>())
            {
                if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
                {
                    if(keyPressed->scancode == sf::Keyboard::Scancode::Enter)
                    {
                        auto [newDx, newDy] = resetSprite();
                        dx = newDx;
                        dy = newDy;
                    }
                    if(keyPressed->scancode == sf::Keyboard::Scancode::Up && zoom < max_zoom) {
                        // Verifica si no se sobrepasarán los límites de la ventana
                        if (simonSprite.getPosition().x + 16 * zoom <= window.getSize().x && 
                            simonSprite.getPosition().y + 32 * zoom <= gWindowHeight) {
                            zoom += 0.1f;
                            
                        }
}
                    if(keyPressed->scancode == sf::Keyboard::Scancode::Down && zoom > min_zoom )
                    {
                        zoom = zoom - 0.1;
                    }
                }

            }
            if (const auto* resized = event->getIf<sf::Event::Resized>())
            {
                float width = static_cast<float>(resized->size.x);
                float height = static_cast<float>(resized->size.y);
                view.setSize(sf::Vector2f(width, height));
                view.setCenter(sf::Vector2f(width / 2, height / 2));
                float scaleX = width / static_cast<float>(initialWindowSize.x);
                float scaleY = height / static_cast<float>(initialWindowSize.y);
                simonSprite.setScale(sf::Vector2f(scaleX, scaleY));
                sf::FloatRect spriteBounds = simonSprite.getGlobalBounds();
                // Ensure the sprite stays within the window bounds
                sf::Vector2f spritePos = simonSprite.getPosition();
                spritePos.x = std::min(std::max(spritePos.x, 0.f), static_cast<float>(width) - spriteBounds.size.x);
                spritePos.y = std::min(std::max(spritePos.y, 0.f), height - spriteBounds.size.y);
                simonSprite.setPosition(spritePos);
                
                /*
                sf::Vector2u newSize = resized->size;

                gWindowWidth = std::max(static_cast<unsigned int>(zoom * 16 * 2), newSize.x);
                gWindowHeight = std::max(static_cast<unsigned int>(zoom * 32 * 2), newSize.y);
                window.setSize({gWindowWidth, gWindowHeight});

                std::cout << "new width: " << gWindowWidth << std::endl;
                std::cout << "new height: " << gWindowHeight << std::endl;
                
                float aspectRatio = static_cast<float>(gWindowWidth) / static_cast<float>(gWindowHeight);
                sf::View view = window.getView();
                view.setSize(sf::Vector2f(gWindowWidth, gWindowHeight));
                // Se divide por dos ya que el centro está justo en la mitad
                view.setCenter(sf::Vector2f(gWindowWidth / 2.f, gWindowHeight / 2.f));

                window.setView(view);

                float spriteWidth = 16 * zoom / aspectRatio;
                float spriteHeight = 32 * zoom / aspectRatio;
                sf::Vector2f pos = simonSprite.getPosition();

                if (pos.x + spriteWidth >= gWindowWidth || pos.y + spriteHeight >= gWindowHeight || pos.x <= 0 || pos.y <= 0)
                {
                    cout << "Centrado" << endl;
                    centerSprite();
                }*/
            }



        }

        // Detectar si la ventana ha sido movida
        /*
        sf::Vector2i currentPosition = window.getPosition();
        if (currentPosition != lastPosition)
        {
            std::cout << "La ventana ha sido movida a: (" << currentPosition.x << ", " << currentPosition.y << ")" << std::endl;
            lastPosition = currentPosition;
        }
        */
        simonSprite.setScale(sf::Vector2f(zoom, zoom));
        float deltaTime = clock.restart().asMilliseconds();

        simonSprite.move(sf::Vector2f(dx*deltaTime, dy*deltaTime));

        // Obtener las dimensiones actuales del sprite
        float spriteWidth = 16 * zoom;
        float spriteHeight = 32 * zoom;

        //Hits
        if (simonSprite.getPosition().x <= 0 || simonSprite.getPosition().x + spriteWidth >= gWindowWidth)
        {
            dx = -dx;
            sound.play();
        }

        if (simonSprite.getPosition().y <= 0 || simonSprite.getPosition().y + spriteHeight >= gWindowHeight)
        {
            dy = -dy;
            sound.play();
        }

        window.clear(sf::Color::Black);
        window.draw(simonSprite);
        window.display();
    }

}