#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include <sstream>
#include <optional>
#include <iomanip>
#include <iostream>
#include <unordered_map>
#include <vector>
#include "camera.h"

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

sf::RectangleShape gFloor;
sf::RectangleShape gWallUp;
sf::RectangleShape gWallDown;


sf::RectangleShape FloatRectToRectShape(const sf::FloatRect& floatRect)
{
    sf::RectangleShape rectShape(floatRect.size);
    rectShape.setPosition(floatRect.position);

    rectShape.setFillColor(sf::Color::Transparent);
    rectShape.setOutlineColor(sf::Color::Red);
    rectShape.setOutlineThickness(2.f);

    return rectShape;
}

void CheckCollisions(sf::FloatRect simonBounds, sf::FloatRect objectBounds, const bool debug = false)
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
                if (debug) std::cout << "Colision con borde lateral izquierdo de objeto." << std::endl;
                gSimonSprite->move({-overlapX, 0.f});
            }
            else
            {
                if (debug) std::cout << "Colision con borde lateral derecho de objeto." << std::endl;
                gSimonSprite->move({overlapX, 0.f});
            }
        }
        else    // Colisión vertical
        {
            if ((simonBounds.position.y + simonBounds.size.y * 0.5f) < (objectBounds.position.y + objectBounds.size.y * 0.5f))
            {
                if (debug) std::cout << "Colision con borde superior de objeto." << std::endl;
                gSimonSprite->move({0.f, -overlapY});
                verticalSpeed = 0.0f;   // Simon deja de caere
                isOnGround = true;      // Indicamos que Simon está en el suelo
            }
            else
            {
                if (debug) std::cout << "Colision con borde inferior de objeto." << std::endl;
                gSimonSprite->move({0.f, overlapY});
                verticalSpeed = 0.0f;   // Simon pasará a estar cayendo
            }
        }
    }
}

void CheckAllCollisions(const bool debug = false)
{
    sf::FloatRect simonBounds = gSimonSprite->getGlobalBounds();
    sf::FloatRect floorBounds = gFloor.getGlobalBounds();
    sf::FloatRect wallUpBounds = gWallUp.getGlobalBounds();
    sf::FloatRect wallDownBounds = gWallDown.getGlobalBounds();

    CheckCollisions(simonBounds, floorBounds, debug);
    CheckCollisions(simonBounds, wallUpBounds, debug);
    CheckCollisions(simonBounds, wallDownBounds, debug);
}

std::string formatFPSandTime(float deltaTime)
{
    std::ostringstream oss;

    // Formatea FPS con 2 decimales
    oss << std::fixed << std::setprecision(2) << (1.f / deltaTime);
    std::string fps = oss.str();

    // Limpia el flujo
    oss.str("");
    oss.clear();

    // Formatea el tiempo en milisegundos con 2 decimales
    oss << std::fixed << std::setprecision(2) << (deltaTime * 1000);
    std::string ms = oss.str();

    return fps + " FPS\n" + ms + " ms";
}

// Prototipos de funciones
bool init();
bool UpdateMovement(float deltaTime, bool haciaArriba, bool haciaIzquierda, bool haciaDerecha);
void render(sf::RenderWindow& window, const sf::Text& text);


bool init()
{
    // Fondo ----------------------------------------------------------------------------
    if (!gTextures["bgEntrada"].loadFromFile("./assets/maps/level1Entrance.png", false))
    {
        std::cerr << "Error cargando la textura de fondo" << std::endl;
        return false;
    }
    sf::Sprite bgSprite(gTextures["bgEntrada"]);
    bgSprite.setTextureRect(sf::IntRect({1, 11}, {768, 192}));
    gSprites.push_back(bgSprite);

    // Suelo ----------------------------------------------------------------------------

    gFloor.setSize(sf::Vector2f(static_cast<float>(gWindowWidth), 50.f)); // 50 píxeles de alto (puedes ajustar este valor)
    gFloor.setFillColor(sf::Color(139, 69, 19)); // Color marrón
    gFloor.setPosition({0.f, 171.f}); // Posicionado a lo largo del ancho, justo a los pies de Simon

    // Pared Arriba ----------------------------------------------------------------------------

    gWallUp.setSize(sf::Vector2f(50.f, 20.f));
    gWallUp.setFillColor(sf::Color(139, 69, 19)); // Color marrón
    gWallUp.setPosition({518.f, 75.f});

    // Pared Abajo ----------------------------------------------------------------------------

    gWallDown.setSize(sf::Vector2f(50.f, 20.f));
    gWallDown.setFillColor(sf::Color(139, 69, 19)); // Color marrón
    gWallDown.setPosition({465.f, 125.f});

    // Simon ----------------------------------------------------------------------------

    // Cargar imagen y configurar textura de Simon (aplicando color key)
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
    
    // Ajusta el origen de las transformaciones al centro inferior
    simonSprite.setOrigin({bounds.size.x / 2.f, bounds.size.y});
    gSprites.push_back(simonSprite);
    gSimonSprite = &gSprites.back();

    return true;
}

bool updateMovement(const float deltaTime, const bool haciaArriba, const bool haciaIzquierda, const bool haciaDerecha)
{
    if (!gSimonSprite)
    {
        std::cerr << "Error: sprite de Simon no inicializado" << std::endl;
        return false;
    }

    // Movimiento vertical con gravedad
    verticalSpeed += GRAVITY * deltaTime;
    gSimonSprite->move({0.f, verticalSpeed * deltaTime});

    if (haciaArriba)
    {
        if (haciaDerecha)
        {
            //gSimonSprite->move({1.f * deltaTime * gMovementSpeed, -1.5f * deltaTime * gMovementSpeed});
            gSimonSprite->move({1.f * deltaTime * gMovementSpeed, 0.f});
        }
        else if (haciaIzquierda)
        {
            //gSimonSprite->move({-1.f * deltaTime * gMovementSpeed, -1.5f * deltaTime * gMovementSpeed});
            gSimonSprite->move({-1.f * deltaTime * gMovementSpeed, 0.f});
        }
    }
    else if (haciaIzquierda)
    {
        gSimonSprite->move({-1.5f * deltaTime * gMovementSpeed, 0.f});
    }
    else if (haciaDerecha)
    {
        gSimonSprite->move({1.5f * deltaTime * gMovementSpeed, 0.f});
    }

    return true;
}

void render(sf::RenderWindow& window, const sf::Text& text)
{
    window.clear(sf::Color::Black);
    for (const auto& sprite : gSprites)
    {
        window.draw(sprite);
    }
    window.draw(gFloor);
    window.draw(FloatRectToRectShape(gFloor.getGlobalBounds()));
    window.draw(gWallUp);
    window.draw(FloatRectToRectShape(gWallUp.getGlobalBounds()));
    window.draw(gWallDown);
    window.draw(FloatRectToRectShape(gWallDown.getGlobalBounds()));
    if (gSimonSprite)
    {
        window.draw(*gSimonSprite);
        window.draw(FloatRectToRectShape(gSimonSprite->getGlobalBounds()));
    }
    window.draw(text);
    window.display();
}

int main()
{
    std::string nombreVentana { "Castlevania: En busca de la Eduardomena Pose" };
    sf::RenderWindow window(sf::VideoMode({gWindowWidth, gWindowHeight}), nombreVentana, sf::Style::Default);
    window.setVerticalSyncEnabled(true);
    sf::Clock deltaClock;

    if (!init())
    {
        std::cerr << "Error en la inicialización" << std::endl;
        return -1;
    }
    
    // Variables para controlar el movimiento
    bool haciaIzquierda { false };
    bool haciaDerecha { false };
    bool haciaArriba { false };

    sf::Font font;
    if (!font.openFromFile("./assets/fonts/NESfonts/nintendo-nes-font.ttf"))
    {
        std::cerr << "Error en la carga de la fuente de texto" << std::endl;
        return -1;
    }
    
    sf::Text text(font);
    text.setString("60 FPS\n16.67 ms");
    text.setCharacterSize(12);
    text.setFillColor(sf::Color::White);

    // Bucle principal del juego
    while (window.isOpen())
    {
        float deltaTime = deltaClock.restart().asSeconds(); // tiempo transcurrido entre fotograma
        text.setString(formatFPSandTime(deltaTime));

        // Procesar eventos
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
                        verticalSpeed = JUMP_FORCE * 0.5f;
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

        window.setView(camera.GetView(window.getSize())); // Actualiza la vista de la cámara

        if (!updateMovement(deltaTime, haciaArriba, haciaIzquierda, haciaDerecha))
        {
            std::cerr << "Error en el update" << std::endl;
            return -1;
        }

        CheckAllCollisions();  // check collisions and correct Simon's position

        render(window, text);
    }

    return 0;
}
