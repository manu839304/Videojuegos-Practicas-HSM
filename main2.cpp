#include <SFML/Window.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>

using namespace std;

// Constants
const unsigned int originalWindowWidth = 800;
const unsigned int originalWindowHeight = 600;
unsigned int gWindowWidth = originalWindowWidth;
unsigned int gWindowHeight = originalWindowHeight;
unsigned int minWindowWidth = 200;
unsigned int minWindowHeight = 150;
float max_zoom = 10;
float min_zoom = 0.7;
float windowScaleFactor = 1.0f;

sf::Vector2u lastWindowSize = {gWindowWidth, gWindowHeight};

// Thread synchronization
std::mutex mtx;
std::atomic<bool> isResizing(false);
std::atomic<bool> isRunning(true); // Add this line

// Random float generator
float randomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

// Sprite structure
struct SpriteData {
    sf::Sprite sprite;
    float dx, dy;
    float zoom;
    int backgroundLayer; // Background layer (0, 1, or 2)
};

// Function to reset sprite position and velocity
auto resetSprite = [](sf::Sprite& sprite, float zoom, bool quieto = false) {
    if (!quieto)
    {
        sprite.setPosition({randomFloat(0, gWindowWidth - 16 * zoom), randomFloat(0, gWindowHeight - 32 * zoom)});
        float speed = randomFloat(0.1f, 0.6f);
        float angle = randomFloat(0.f, 2.f * 3.14159f);
        float dx = speed * cos(angle);
        float dy = speed * sin(angle);
        return std::make_pair(dx, dy);
    }
    else{
        sprite.setPosition({randomFloat(0, gWindowWidth - 16 * zoom), randomFloat(0, gWindowHeight - 32 * zoom)});

        float dx = 0.0f;
        float dy = 0.0f;
        return std::make_pair(dx, dy);
    }
    
    
};

// Function to check if two sprites collide based on position and size
bool checkCollision(const sf::Sprite& sprite1, const sf::Sprite& sprite2, const sf::Vector2f& velocity1, const sf::Vector2f& velocity2) {
    // Get the position of both sprites
    sf::Vector2f pos1 = sprite1.getPosition();
    sf::Vector2f pos2 = sprite2.getPosition();

    // Get the dimensions of both sprites (scaled by zoom factor)
    float width1 = 16 * sprite1.getScale().x;  // Assuming base width of 16 pixels
    float height1 = 32 * sprite1.getScale().y; // Assuming base height of 32 pixels

    float width2 = 16 * sprite2.getScale().x;  // Same here for sprite2
    float height2 = 32 * sprite2.getScale().y;

    // Calculate the predicted positions based on velocity
    sf::Vector2f predictedPos1 = pos1 + velocity1;
    sf::Vector2f predictedPos2 = pos2 + velocity2;

    // Check for overlap in the X and Y directions using predicted positions
    bool collisionX = predictedPos1.x + width1 > predictedPos2.x && predictedPos1.x < predictedPos2.x + width2;
    bool collisionY = predictedPos1.y + height1 > predictedPos2.y && predictedPos1.y < predictedPos2.y + height2;

    // If both X and Y overlap, the sprites are colliding
    return collisionX && collisionY;
}


// Function to update sprite positions
void updateSprite(SpriteData& spriteData, sf::Sound& sound, std::atomic<bool>& isResizing, std::vector<SpriteData>& sprites, std::atomic<bool>& isRunning) {
    sf::Clock clock;
    while (isRunning) {
        if (!isResizing) {
            // Update sprite position
            float deltaTime = clock.restart().asMilliseconds();  // Now deltaTime is in milliseconds

            spriteData.sprite.move(sf::Vector2f(spriteData.dx * deltaTime, spriteData.dy * deltaTime));

            // Check for collisions with window borders
            std::lock_guard<std::mutex> lock(mtx);

            spriteData.sprite.setScale(sf::Vector2f(spriteData.zoom * windowScaleFactor, spriteData.zoom * windowScaleFactor));
            sf::Vector2f pos = spriteData.sprite.getPosition();
            pos.x = std::min(pos.x, static_cast<float>(gWindowWidth) - 16 * spriteData.zoom * windowScaleFactor);
            pos.y = std::min(pos.y, static_cast<float>(gWindowHeight) - 32 * spriteData.zoom * windowScaleFactor);
            spriteData.sprite.setPosition(pos);

            float spriteWidth = 16 * spriteData.zoom * windowScaleFactor;
            float spriteHeight = 32 * spriteData.zoom * windowScaleFactor;

            // Elastic collision with window borders
            if (pos.x <= 0 || pos.x + spriteWidth >= gWindowWidth) {
                spriteData.dx = -spriteData.dx;  // Reverse direction horizontally
                sound.play();
                pos.x = std::max(0.f, std::min(pos.x, gWindowWidth - spriteWidth));
            }
            if (pos.y <= 0 || pos.y + spriteHeight >= gWindowHeight) {
                spriteData.dy = -spriteData.dy;  // Reverse direction vertically
                sound.play();
                pos.y = std::max(0.f, std::min(pos.y, gWindowHeight - spriteHeight));
            }

            spriteData.sprite.setPosition(pos);

            // Check for collisions with other sprites on the same depth
            for (auto& otherSpriteData : sprites) {
                if (&otherSpriteData != &spriteData && otherSpriteData.backgroundLayer == spriteData.backgroundLayer) {
                    if (checkCollision(spriteData.sprite, otherSpriteData.sprite,
                        sf::Vector2f(spriteData.dx * deltaTime, spriteData.dy * deltaTime),sf::Vector2f(otherSpriteData.dx * deltaTime, spriteData.dy * deltaTime))) {
                        // Swap velocities to simulate a bounce
                
                        // Casos para el eje X
                    if (spriteData.dx > 0 && otherSpriteData.dx > 0) {
                        if (spriteData.dx < otherSpriteData.dx) {
                            otherSpriteData.dx = -otherSpriteData.dx;
                        } else {
                            spriteData.dx = -spriteData.dx;
                        }
                    } else if (spriteData.dx < 0 && otherSpriteData.dx < 0) {
                        if (spriteData.dx < otherSpriteData.dx) {
                            spriteData.dx = -spriteData.dx;
                        } else {
                            otherSpriteData.dx = -otherSpriteData.dx;
                        }
                    } else if ((spriteData.dx < 0 && otherSpriteData.dx > 0) || (spriteData.dx > 0 && otherSpriteData.dx < 0)) {
                        spriteData.dx = -spriteData.dx;
                        otherSpriteData.dx = -otherSpriteData.dx;
                    }

                    // Casos para el eje Y
                    if (spriteData.dy > 0 && otherSpriteData.dy > 0) {
                        if (spriteData.dy < otherSpriteData.dy) {
                            otherSpriteData.dy = -otherSpriteData.dy;
                        } else {
                            spriteData.dy = -spriteData.dy;
                        }
                    } else if (spriteData.dy < 0 && otherSpriteData.dy < 0) {
                        if (spriteData.dy < otherSpriteData.dy) {
                            spriteData.dy = -spriteData.dy;
                        } else {
                            otherSpriteData.dy = -otherSpriteData.dy;
                        }
                    } else if ((spriteData.dy < 0 && otherSpriteData.dy > 0) || (spriteData.dy > 0 && otherSpriteData.dy < 0)) {
                        spriteData.dy = -spriteData.dy;
                        otherSpriteData.dy = -otherSpriteData.dy;
                    }            
                    else{
                        spriteData.dx = -spriteData.dx;
                        otherSpriteData.dx = -otherSpriteData.dx;
                        spriteData.dy = -spriteData.dy;
                        otherSpriteData.dy = -otherSpriteData.dy;
                    }    

                        
                        sound.play();
                    }
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20)); // Simulate 100 FPS
    }
}

int main() {
    std::string nombreVentana{"Rebote simple"};
    sf::RenderWindow window(sf::VideoMode({gWindowWidth, gWindowHeight}), nombreVentana, sf::Style::Default);
    //window.setVerticalSyncEnabled(true);
    window.setVerticalSyncEnabled(false);
    window.setFramerateLimit(120);  // Limit to 120 FPS

    srand(static_cast<unsigned int>(time(0)));

    // Load texture
    sf::Image simonImage;
    if (!simonImage.loadFromFile("./assets/sprites/player/simonBelmont.png")) {
        std::cerr << "Error loading the image" << std::endl;
        return -1;
    }
    simonImage.createMaskFromColor(sf::Color(0x74, 0x74, 0x74)); // Color key
    sf::Texture simonTexture;
    if (!simonTexture.loadFromImage(simonImage)) {
        std::cerr << "Error loading the texture" << std::endl;
        return -1;
    }

    // Create multiple sprites
    std::vector<SpriteData> sprites;
    for (int i = 0; i < 6; ++i) {  // Create 9 sprites, divided into 3 backgrounds
        sf::Sprite sprite(simonTexture);
        sprite.setTextureRect(sf::IntRect({1, 21}, {16, 32}));
        float zoom = randomFloat(3.0f, 6.0f);
        sprite.setScale(sf::Vector2f(zoom, zoom));
        bool quieto = false;
        if (i>3)
        {
            quieto = true;
        }
        else{
            quieto = false;
        }
        
        auto [dx, dy] = resetSprite(sprite, zoom,quieto);

        int backgroundLayer = i % 3;  // Group sprites into 3 backgrounds
        sprites.push_back({sprite, dx, dy, zoom, backgroundLayer});
    }

    // Load sound
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("./assets/sounds/08.wav")) {
        std::cerr << "Error loading the audio" << std::endl;
        return -1;
    }
    sf::Sound sound(buffer);

    // Create threads for each sprite
    std::vector<std::thread> threads;
    for (auto& spriteData : sprites) {
        threads.emplace_back(updateSprite, std::ref(spriteData), std::ref(sound), std::ref(isResizing), std::ref(sprites), std::ref(isRunning));
    }

    // Main loop
    sf::Clock clock;
    while (window.isOpen()) {
        // Handle events
        while (const std::optional event  = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                isRunning = false; // Set isRunning to false
                window.close();
            }
            if (const auto* resized = event->getIf<sf::Event::Resized>()) {
                isResizing = true;
                gWindowWidth = std::max(resized->size.x, minWindowWidth);
                gWindowHeight = std::max(resized->size.y, minWindowHeight);
                windowScaleFactor = std::min(
                    static_cast<float>(gWindowWidth) / originalWindowWidth,
                    static_cast<float>(gWindowHeight) / originalWindowHeight
                );
                sf::View view = window.getView();
                view.setSize(sf::Vector2f(static_cast<float>(gWindowWidth), static_cast<float>(gWindowHeight)));
                view.setCenter(sf::Vector2f(gWindowWidth / 2.f, gWindowHeight / 2.f));
                window.setView(view);
                isResizing = false;
            }
        }
        
        // Draw sprites in order of depth
        window.clear(sf::Color::Black);
        std::lock_guard<std::mutex> lock(mtx);

        

        // Paint sprites based on their background layer
        for (auto& spriteData : sprites) {
            sf::Color spriteColor;
            if (spriteData.backgroundLayer == 0) {
                spriteColor = sf::Color::Red;
            } else if (spriteData.backgroundLayer == 1) {
                spriteColor = sf::Color::Green;
            } else {
                spriteColor = sf::Color::Blue;
            }

            spriteData.sprite.setColor(spriteColor);
            window.draw(spriteData.sprite);
        }

        window.display();
        // Join threads
        // Esperar que todos los hilos terminen antes de cerrar el programa
    
        
    }
    for (auto& thread : threads) {
        thread.join();
    }

    

    return 0;
}