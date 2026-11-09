#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <random>

void setPolygon(float angle, float b, sf::VertexArray &poly)
{
    angle = fmod(angle + M_PI * 0.25, M_PI * 2) - M_PI * 0.25;

    float vertices[] = {
        -1., -1.,
        1., -1.,
        -1., 1.,
        1., 1.};
    float m = (sin(angle) / cos(angle));
    float y1 = -1. * m + b;
    float y2 = 1. * m + b;
    float x1 = (-1. - b) / m;
    float x2 = (1. - b) / m;
    if (angle < M_PI * 0.25 && angle > -M_PI * 0.25)
    {
        vertices[5] = y1;
        vertices[7] = y2;
    }
    else if (angle < M_PI * 0.75 && angle > M_PI * 0.25)
    {
        vertices[0] = x1;
        vertices[4] = x2;
    }
    else if (angle < M_PI * 1.25 && angle > M_PI * 0.75)
    {
        vertices[1] = y1;
        vertices[3] = y2;
    }
    else if (angle < M_PI * 2 && angle > M_PI * 1.25)
    {
        vertices[2] = x1;
        vertices[6] = x2;
    }

    for (size_t i = 0; i < 4; i++)
    {
        poly[i].position = sf::Vector2f(vertices[i * 2] * 450, vertices[i * 2 + 1] * 450);
        poly[i].texCoords = sf::Vector2f(((vertices[i * 2] + 1) / 2) * 900, ((vertices[i * 2 + 1] + 1) / 2) * 900);
    }
}

sf::View view(sf::Vector2f(0.f, 0.f), sf::Vector2f(900.f, 900.f));
sf::RenderWindow window(sf::VideoMode(900, 900), "split");
sf::VertexArray poly1(sf::TrianglesStrip, 4);
sf::VertexArray poly2(sf::TrianglesStrip, 4);
sf::Texture the_texture;
float angle = 50 * M_PI / 180;
float b = 0;
bool start = true;

void draw(sf::Clock &clock, sf::RenderTexture &renderTarget)
{
    renderTarget.setView(view);
    renderTarget.clear();
    float step = clock.getElapsedTime().asSeconds() * 100.;

    sf::Vector2f translate1(step * cos(angle - M_PI * 0.5), step * sin(angle - M_PI * 0.5));
    sf::RenderStates states1;
    states1.transform.translate(translate1);
    states1.texture = &the_texture;
    renderTarget.draw(poly1, states1);

    sf::Vector2f translate2(step * cos(angle + M_PI * 0.5), step * sin(angle + M_PI * 0.5));
    sf::RenderStates states2;
    states2.transform.translate(translate2);
    states2.texture = &the_texture;
    renderTarget.draw(poly2, states2);

    renderTarget.display();
}

int main()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    view.setViewport(sf::FloatRect(0.f, 0., 1.f, 1.f));

    setPolygon(angle, b, poly1);
    setPolygon(angle + M_PI, b, poly2);

    sf::RenderTexture tempRenderTarget;
    tempRenderTarget.create(window.getSize().x, window.getSize().y);

    sf::RenderTexture renderTarget;
    renderTarget.create(window.getSize().x, window.getSize().y);

    the_texture.create(window.getSize().x, window.getSize().y);

    window.setVerticalSyncEnabled(true);
    sf::Clock clock;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed ||
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
                window.close();
        }

        if (clock.getElapsedTime().asSeconds() > .2 || start)
        {
            start = false;
            draw(clock, tempRenderTarget);

            angle = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * 360.0;
            b = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * 1. - .5;

            clock.restart();
            setPolygon(angle, b, poly1);
            setPolygon(angle + M_PI, b, poly2);

            sf::RectangleShape line(sf::Vector2f(5000, 10.f));
            line.setOrigin(2500, 5);
            line.setPosition(0, b * 450);
            line.rotate(angle * 180 / M_PI);
            tempRenderTarget.draw(line);

            the_texture = tempRenderTarget.getTexture();
        }

        draw(clock, renderTarget);

        sf::Sprite sprite(renderTarget.getTexture());
        window.draw(sprite);

        window.display();
    }
}
