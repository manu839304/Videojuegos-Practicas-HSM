#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>

const unsigned int originalWindowWidth = 800;
const unsigned int originalWindowHeight = 600;
unsigned int gWindowWidth = originalWindowWidth;
unsigned int gWindowHeight = originalWindowHeight;
const float spawnInterval = 0.05f;
bool isSpacePressed = false;
bool isResizing = false;
float windowScaleFactor = 1.0f;
sf::Vector2u lastWindowSize = {gWindowWidth, gWindowHeight};

struct SpriteInfo {
    sf::Sprite sprite;
    sf::Vector2f velocity;
    float scale;
    float rotationSpeed;
};

float randomFloat (float min, float max)
{
    return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
}


int main() {
    std::string nombreVentana { "Rebote Multiple" };
    sf::RenderWindow window(sf::VideoMode({gWindowWidth, gWindowHeight}), nombreVentana, sf::Style::Default);
    window.setVerticalSyncEnabled(true);
    
    srand(static_cast<unsigned int>(time(0)));

    sf::Image simonImage;
    if (!simonImage.loadFromFile("./assets/sprites/player/simonBelmont.png")) {
        std::cerr << "Error cargando la imagen" << std::endl;
        return -1;
    }
    simonImage.createMaskFromColor(sf::Color(0x74, 0x74, 0x74));
    sf::Texture simonTexture;
    if (!simonTexture.loadFromImage(simonImage)) {
        std::cerr << "Error cargando la textura" << std::endl;
        return -1;
    }

    std::vector<SpriteInfo> sprites;
    sf::Clock clock;
    sf::Clock spawnClock;
    
    
    while (window.isOpen()) {
        while (const std::optional event  = window.pollEvent()){
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (event->is<sf::Event::KeyPressed>())
            {
                if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
                {
                    if(keyPressed->scancode == sf::Keyboard::Scancode::Space){
                        isSpacePressed = true;
                    }
                }
            }
            if (event->is<sf::Event::KeyReleased>())
            {
                if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>())
                {
                    if(keyReleased->scancode == sf::Keyboard::Scancode::Space){
                        isSpacePressed = false;
                    } 
                }
            }

           if (event->is<sf::Event::Resized>()) {
                isResizing = true;

                const auto* resized = event->getIf<sf::Event::Resized>();
                gWindowWidth = resized->size.x;
                gWindowHeight = resized->size.y;

                sf::View view = window.getView();
                view.setSize({static_cast<float>(gWindowWidth), static_cast<float>(gWindowHeight)});
                view.setCenter({static_cast<float>(gWindowWidth)/2.0f, static_cast<float>(gWindowHeight)/2.0f});
                window.setView(view);

                windowScaleFactor = std::min(
                    static_cast<float>(gWindowWidth) / originalWindowWidth,
                    static_cast<float>(gWindowHeight) / originalWindowHeight
                );
            }
        }

        if (isResizing && (window.getSize() != lastWindowSize)) {
            lastWindowSize = window.getSize();
        } else if (isResizing && (window.getSize() == lastWindowSize)) {
            isResizing = false;
        }

        if (isResizing) {
            clock.restart();
        }
        else
        {
            if (spawnClock.getElapsedTime().asSeconds() >= spawnInterval / (isSpacePressed ? 2.0f : 1.0f)) {
                sf::Sprite sprite(simonTexture);
                sprite.setTextureRect(sf::IntRect({1, 21}, {16, 32}));
                sprite.setPosition({gWindowWidth / 2.0f, gWindowHeight / 2.0f});
                
                float angle = randomFloat(0.f, 2.f * 3.14159f);
                float speed = randomFloat(2.0f, 3.5f);
                float rotationSpeed = randomFloat(1.0f, 2.5f); // Velocidad de rotación aleatoria
                sf::Vector2f velocity(std::cos(angle) * speed, std::sin(angle) * speed);
                
                sprites.push_back({sprite, velocity, 0.0f, rotationSpeed});
                spawnClock.restart();
            }
            
            float deltaTime = clock.restart().asSeconds();
            
            for (auto& s : sprites) {
                sf::Vector2f pos = s.sprite.getPosition();
                float distanceFromCenter = std::sqrt(std::pow(pos.x - gWindowWidth / 2.0f, 2) + std::pow(pos.y - gWindowHeight / 2.0f, 2));
                float zoomMultiplier = isSpacePressed ? 2.0f : 1.0f;
                float speedMultiplier = 1.0f + (distanceFromCenter / 400.0f); // Aumenta la velocidad según la distancia

                speedMultiplier = isSpacePressed ? speedMultiplier * 2.0f : speedMultiplier;

                pos += s.velocity * deltaTime * 100.0f * speedMultiplier;
                s.scale += deltaTime * 2.0f * zoomMultiplier; // Incremento progresivo del tamaño
                //float aspectRatio = static_cast<float>(gWindowWidth) / static_cast<float>(gWindowHeight);
                s.sprite.setScale({s.scale * windowScaleFactor, s.scale * windowScaleFactor});
                s.sprite.rotate(sf::degrees(s.rotationSpeed * deltaTime * 100.0f * (isSpacePressed ? 2.0f : 1.0f))); // Aplicar rotación

                s.sprite.setPosition(pos);
            }
        }     

        window.clear();
        for (const auto& s : sprites) {
            window.draw(s.sprite);
        }
        window.display();

    }
    return 0;
}

