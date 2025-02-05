#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include <unordered_map>
#include <vector>
#include "camera.h"
#include "animationManager.hpp"

// Variables globales de configuración
bool gEnMovimiento { false };
constexpr int escala { 1 };
constexpr int gWindowWidth { 768 * escala };
constexpr int gWindowHeight { 250 * escala };
constexpr float gMovementSpeed { 50.0f };

// Constantes para la física
constexpr float GRAVITY { 980.0f };       // aceleración en píxeles/segundo²
constexpr float JUMP_FORCE { 350.0f };    // velocidad inicial del salto (píxeles/segundo)

// Cámara
Camera camera(sf::FloatRect({0.f, 0.f}, {gWindowWidth, gWindowHeight}));

// Contenedores para sprites y texturas
std::vector<sf::Sprite> gSprites;
std::unordered_map<std::string, sf::Texture> gTextures;

// Puntero global para acceder al sprite de Simon
sf::Sprite* gSimonSprite { nullptr };
bool isOnGround { true };       // indica si Simon está en el suelo
float verticalSpeed { 0.0f };   // velocidad vertical actual

// Definir el suelo y la pared para colisiones
sf::RectangleShape gFloor;
sf::RectangleShape gWall;

AnimationManager* gAnimationManager { nullptr };

// DEPURACION
std::string currentAnimation;

// Función de colisiones
void CheckCollisions(sf::FloatRect simonBounds, sf::FloatRect objectBounds)
{
    // Si esto da true, es porque la hitbox de Simon ha penetrado el objeto <objectBounds>
    if (const std::optional<sf::FloatRect> intersection = simonBounds.findIntersection(objectBounds))
    {
        float overlapX { intersection->size.x };
        float overlapY { intersection->size.y };

        if (overlapX < overlapY)    // Colisión horizontal
        {
            if ((simonBounds.position.x + simonBounds.size.x * 0.5f) < (objectBounds.position.x + objectBounds.size.x * 0.5f))
            {
                //std::cout << "Colision con borde lateral izquierdo de objeto." << std::endl;
                gSimonSprite->move({-overlapX, 0.f});
            }
            else
            {
                //std::cout << "Colision con borde lateral derecho de objeto." << std::endl;
                gSimonSprite->move({overlapX, 0.f});
            }
        }
        else    // Colisión vertical
        {
            if ((simonBounds.position.y + simonBounds.size.y * 0.5f) < (objectBounds.position.y + objectBounds.size.y * 0.5f))
            {
                //std::cout << "Colision con borde superior de objeto." << std::endl;
                gSimonSprite->move({0.f, -overlapY});
                verticalSpeed = 0.0f;   // Simon deja de caere
                isOnGround = true;      // Indicamos que Simon está en el suelo
            }
            else
            {
                //std::cout << "Colision con borde inferior de objeto." << std::endl;
                gSimonSprite->move({0.f, overlapY});
                verticalSpeed = 0.0f;   // Simon pasará a estar cayendo
            }
        }
    }
}


void CheckAllCollisions()
{
    sf::FloatRect simonBounds = gSimonSprite->getGlobalBounds();
    sf::FloatRect floorBounds = gFloor.getGlobalBounds();
    sf::FloatRect wallBounds = gWall.getGlobalBounds();

    CheckCollisions(simonBounds, floorBounds);
    CheckCollisions(simonBounds, wallBounds);
}

// Inicializa Simon, las animaciones, el suelo y las paredes
bool init()
{
    // Mundo
    if (!gTextures["bgEntrada"].loadFromFile("./assets/maps/level1Entrance.png", false))
    {
        std::cerr << "Error cargando la textura de fondo" << std::endl;
        return false;
    }
    sf::Sprite bgSprite(gTextures["bgEntrada"]);
    bgSprite.setTextureRect(sf::IntRect({1, 11}, {768, 192}));
    gSprites.push_back(bgSprite);

    // Simon
    sf::Image simonImage;
    if (!simonImage.loadFromFile("./assets/sprites/player/simonBelmont.png"))
    {
        std::cerr << "Error cargando la imagen de Simon" << std::endl;
        return false;
    }
    simonImage.createMaskFromColor(sf::Color(0x74, 0x74, 0x74)); // color key
    gTextures["simon"] = sf::Texture(simonImage, false);

    sf::Sprite simonSprite(gTextures["simon"]);
    simonSprite.setTextureRect(sf::IntRect({1, 21}, {16, 32}));
    simonSprite.setPosition({245.f, 171.f});
    
    sf::FloatRect bounds = simonSprite.getLocalBounds();
    simonSprite.setOrigin({bounds.size.x / 2.f, bounds.size.y});
    gSprites.push_back(simonSprite);
    gSimonSprite = &gSprites.back();

    // Inicializar AnimationManager
    gAnimationManager = new AnimationManager(*gSimonSprite);

    // Definir animaciones
    std::vector<AnimationManager::Frame> idleFrames {
        AnimationManager::Frame{sf::IntRect(sf::Vector2(1, 21), sf::Vector2(16, 32)), 0.2f}
    };
    std::vector<AnimationManager::Frame> jumpFrames {
        AnimationManager::Frame{sf::IntRect(sf::Vector2(84, 21), sf::Vector2(16, 32)), 0.1f}
    };
    std::vector<AnimationManager::Frame> walkFrames {
        AnimationManager::Frame{sf::IntRect(sf::Vector2(29, 21), sf::Vector2(16, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(46, 21), sf::Vector2(16, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(63, 21), sf::Vector2(16, 32)), 0.1f}
    };

    gAnimationManager->addAnimation("idle", idleFrames);
    gAnimationManager->addAnimation("jump", jumpFrames);
    gAnimationManager->addAnimation("walk", walkFrames);

    gAnimationManager->playAnimation("idle");
    currentAnimation = "idle";

    // Suelo
    gFloor.setSize(sf::Vector2f(static_cast<float>(gWindowWidth), 50.f)); // 50 píxeles de alto (puedes ajustar este valor)
    gFloor.setFillColor(sf::Color(139, 69, 19)); // Color marrón
    gFloor.setPosition({0.f, 171.f}); // Posicionado a lo largo del ancho, justo a los pies de Simon

    // Pared
    gWall.setSize(sf::Vector2f(50.f, 20.f));
    gWall.setFillColor(sf::Color(139, 69, 19)); // Color marrón
    gWall.setPosition({518.f, 75.f});

    return true;
}

// Movimiento y animación de Simon
bool updateMovement(const float deltaTime, bool haciaArriba, bool haciaIzquierda, bool haciaDerecha)
{
    if (!gSimonSprite)
    {
        std::cerr << "Error: sprite de Simon no inicializado" << std::endl;
        return false;
    }

    // Aplicar la gravedad solo si Simon no está en el suelo
    if (!isOnGround)
    {
        verticalSpeed += GRAVITY * deltaTime;
        gSimonSprite->move({0.f, verticalSpeed * deltaTime});
    }

    // Saltar solo si Simon está en el suelo
    if (haciaArriba && isOnGround)
    {
        verticalSpeed = -JUMP_FORCE;  // Aplicar la fuerza de salto
        isOnGround = false;          // Simon ya no está en el suelo
    }

    // Movimiento horizontal
    if (haciaIzquierda)
    {
        gSimonSprite->move({-1.5f * deltaTime * gMovementSpeed, 0.f});
        gSimonSprite->setScale({1.f, 1.f});
    }
    else if (haciaDerecha)
    {
        gSimonSprite->move({1.5f * deltaTime * gMovementSpeed, 0.f});
        gSimonSprite->setScale({-1.f, 1.f});
    }

    gAnimationManager->update(deltaTime);
    return true;
}

// Bucle principal del juego
int main()
{
    sf::RenderWindow window(sf::VideoMode({gWindowWidth, gWindowHeight}), "Castlevania", sf::Style::Default);
    window.setVerticalSyncEnabled(true);
    sf::Clock deltaClock;

    if (!init())
    {
        return -1;
    }

    bool haciaIzquierda { false };
    bool haciaDerecha { false };
    bool haciaArriba { false };

    while (window.isOpen())
    {
        float deltaTime = deltaClock.restart().asSeconds();

        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
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
                        if (isOnGround)
                        {
                            verticalSpeed = -JUMP_FORCE;
                            isOnGround = false;
                        }
                        break;
                    case sf::Keyboard::Scancode::Down:
                        gSimonSprite->move({0.f, 3.0f});
                        break;
                    case sf::Keyboard::Scancode::Left:
                        haciaIzquierda = true;
                        haciaDerecha = false;
                        gSimonSprite->setScale({1.f, 1.f});
                        break;
                    case sf::Keyboard::Scancode::Right:
                        haciaDerecha = true;
                        haciaIzquierda = false;
                        gSimonSprite->setScale({-1.f, 1.f});
                        break;
                    default:
                        break;
                }
            }
            else if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>())
            {
                switch (keyReleased->scancode)
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
        std::cout << currentAnimation << std::endl;
        // Verificar colisiones
        CheckAllCollisions();

        if (!updateMovement(deltaTime, haciaArriba, haciaIzquierda, haciaDerecha))
        {
            return -1;
        }
        gAnimationManager->updateAnimation(isOnGround, haciaDerecha, haciaIzquierda, currentAnimation);
        window.clear();
        for (const auto& sprite : gSprites)
        {
            window.draw(sprite);
        }

        window.draw(gFloor);
        window.draw(gWall);
        window.draw(*gSimonSprite);
        window.display();
    }

    delete gAnimationManager;
    return 0;
}
