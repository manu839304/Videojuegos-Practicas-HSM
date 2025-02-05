#include "demo.h"
#include "resources.h"
#include "map.h"
#include <iostream>

Map map;
Camera camera(320.0f);

const float movementSpeed = 75.0f;

// Inicializa los recursos y configuraciones necesarias
void Init(const sf::Window &window)
{
    // En este caso primero se extrae en formato imagen para realizar las conversiones primero, después se almacena en la clase de recursos
    sf::Image simonImage;
    if (!simonImage.loadFromFile("../assets/sprites/player/simonBelmont.png"))
        exit(-1);

    simonImage.createMaskFromColor(sf::Color(0x74, 0x74, 0x74));

    if (!Resources::textures["simon"].loadFromImage(simonImage))
        exit(-1);

    // Creación textura de ejemplo cuadrado
    sf::Image squareImage;
    if (!squareImage.loadFromFile("../assets/others/square.png"))
        exit(-1);

    squareImage.createMaskFromColor(sf::Color(0x74, 0x74, 0x74));

    if (!Resources::textures["square"].loadFromImage(squareImage))
        exit(-1);

    if (!Resources::textures["background"].loadFromFile("../assets/maps/level1Entrance.png"))
        exit(-1);

    // Incializacion del grid del mapa
    sf::Image image;
    if (!image.loadFromFile("../assets/others/demoMap.png"))
        exit(-1);

    map.CreateFromImage(image);

    // Posición inicial de la camara
    camera.position = sf::Vector2f(150.0f, 87.5f);
}

// Actualiza la lógica del programa en función del tiempo transcurrido
void Update(float deltaTime)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
    {
        camera.position.x += movementSpeed * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
    {
        camera.position.x -= movementSpeed * deltaTime;
    }
}

// Renderiza los elementos en la ventana
void Render(Renderer &renderer)
{
    map.DrawBackground(renderer);
    map.Draw(renderer);
    // renderer.DrawCut(Resources::textures["simon"], sf::Vector2f(), sf::Vector2f(7.5, 5.5), sf::IntRect({1, 21}, {16, 32}));
}
