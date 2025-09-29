#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

class Lighter {
private:

public:
    int shadow_size = 10;
    bool ambient_enabled = true;
    Color ambient_color;
    struct light {
        Vector2f position;
        float radius;
        Color color;
        bool is_on;
    };
    vector<light> lights;

    Lighter();
    void setAmbientColor(const Color& color);
    void draw(RenderWindow& window, View& view);
};

