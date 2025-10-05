#pragma once
#include <vector>
#include <cmath>
#include <SFML/Graphics.hpp>


class Lighter {
private:

public:
    int shadow_size = 10;
    bool ambient_enabled = true;
    sf::Color ambient_color;
    struct light {
        sf::Vector2f position;
        float radius;
        sf::Color color;
        bool is_on;
    };
    std::vector<light> lights;

    Lighter();
    void setAmbientColor(const sf::Color& color);
    void draw(sf::RenderWindow& window, sf::View& view);
};

