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

// Random float generator
float randomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

// Sprite structure
struct SpriteData {
    sf::Sprite sprite;
    float dx, dy;
    float zoom;
    int depth; // Depth layer (lower values are drawn first)
    int backgroundLayer; // Background layer (0, 1, or 2)
};

// Function to reset sprite position and velocity
auto resetSprite = [](sf::Sprite& sprite, float zoom) {
    sprite.setPosition({randomFloat(0, gWindowWidth - 16 * zoom), randomFloat(0, gWindowHeight - 32 * zoom)});
    float speed = randomFloat(0.1f, 0.5f);
    float angle = randomFloat(0.f, 2.f * 3.14159f);
    float dx = speed * cos(angle);
    float dy = speed * sin(angle);
    return std::make_pair(dx, dy);
};

// Function to update sprite positions
void updateSprite(SpriteData& spriteData, sf::Sound& sound, std::atomic<bool>& isResizing) {
    sf::Clock clock;
    while (true) {
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

            // Elastic collision
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
        }
        //std::this_thread::sleep_for(std::chrono::milliseconds(20)); // Simulate 100 FPS
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
    for (int i = 0; i < 3; ++i) {  // Create 9 sprites, divided into 3 backgrounds
        sf::Sprite sprite(simonTexture);
        sprite.setTextureRect(sf::IntRect({1, 21}, {16, 32}));
        float zoom = randomFloat(1.0f, 3.0f);
        sprite.setScale(sf::Vector2f(zoom, zoom));
        auto [dx, dy] = resetSprite(sprite, zoom);

        int backgroundLayer = i / 3;  // Group sprites into 3 backgrounds
        sprites.push_back({sprite, dx, dy, zoom, i % 3, backgroundLayer});
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
        threads.emplace_back(updateSprite, std::ref(spriteData), std::ref(sound), std::ref(isResizing));
    }

    // Main loop
    sf::Clock clock;
    while (window.isOpen()) {
        // Handle events
        while (const std::optional event  = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
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

        // Sort by depth (for layering)
        std::sort(sprites.begin(), sprites.end(), [](const SpriteData& a, const SpriteData& b) {
            return a.depth < b.depth;
        });

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
    }

    // Join threads
    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}
