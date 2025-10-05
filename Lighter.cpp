#include "Lighter.h"

Lighter::Lighter() {
    flare_texture.loadFromFile("src/flare.png");
    flare.setTexture(flare_texture);
    flare.setScale(0.2f, 0.2f);
    flare.setOrigin(flare_texture.getSize().x / 2.f, flare_texture.getSize().y / 2.f);
    flare.setColor(sf::Color(255, 255, 255, 255));
}

void Lighter::setAmbientColor(const sf::Color& color) {
    ambient_color = color;
}

void Lighter::draw(sf::RenderWindow& window, sf::View& view) {

    sf::Vector2f view_center = view.getCenter();
    sf::Vector2f view_size = view.getSize();

    float left = view_center.x - view_size.x / 2;
    float top = view_center.y - view_size.y / 2;

    int gridX = ceil(view_size.x / shadow_size);
    int gridY = ceil(view_size.y / shadow_size);

    // Draw ambient light
    if (ambient_enabled) {
        sf::RectangleShape ambient(view_size);
        ambient.setPosition(left, top);
        ambient.setFillColor(ambient_color);
        window.draw(ambient);
    }

    // Update lights
    std::vector<light> light_sources;
    for (auto& light : lights) {
        light_sources.push_back(light);
        if (flare_enabled && light.is_on) {
            flare.setPosition(light.position);
            window.draw(flare);
        }
    }

    // Draw brightness shadows
    sf::VertexArray shadowVerts(sf::Quads, gridX * gridY * 4);
    for (int i = 0; i < gridX; i++) {
        for (int j = 0; j < gridY; j++) {
            float posX = left + i * shadow_size;
            float posY = top + j * shadow_size;
            sf::Vector2f cellPos(posX + shadow_size / 2.f, posY + shadow_size / 2.f);

            float brightness = min_brightness;

            // Calculate shadow brightness
            for (const auto& light : light_sources) {
                if (light.is_on) {
                    float dx = light.position.x - cellPos.x;
                    float dy = light.position.y - cellPos.y;
                    float distance = sqrt(dx * dx + dy * dy);

                    float shadow_radius = light.radius * 1.2f;
                    if (distance < shadow_radius) {
                        float localBrightness = 1.0f - (distance / shadow_radius);
                        brightness = std::max(brightness, localBrightness);
                    }
                }
            }

            float alpha = 255 * (1.0f - brightness);
            alpha = std::clamp(alpha, 0.f, 255.f);

            sf::Color shadowColor(0, 0, 0, static_cast<sf::Uint8>(alpha));

            int vertexIndex = (i + j * gridX) * 4;
            shadowVerts[vertexIndex + 0].position = sf::Vector2f(posX, posY);
            shadowVerts[vertexIndex + 1].position = sf::Vector2f(posX + shadow_size, posY);
            shadowVerts[vertexIndex + 2].position = sf::Vector2f(posX + shadow_size, posY + shadow_size);
            shadowVerts[vertexIndex + 3].position = sf::Vector2f(posX, posY + shadow_size);

            for (int k = 0; k < 4; ++k)
                shadowVerts[vertexIndex + k].color = shadowColor;
        }
    }
    window.draw(shadowVerts);

    // Draw colored lights with additive blending
    sf::VertexArray lightVerts(sf::Quads, gridX * gridY * 4);
    for (int i = 0; i < gridX; i++) {
        for (int j = 0; j < gridY; j++) {
            float posX = left + i * shadow_size;
            float posY = top + j * shadow_size;
            sf::Vector2f cellPos(posX + shadow_size / 2.f, posY + shadow_size / 2.f);

            float totalRed = 0.0f, totalGreen = 0.0f, totalBlue = 0.0f;

            // Add light contributions
            for (const auto& light : light_sources) {
                if (light.is_on) {
                    float dx = light.position.x - cellPos.x;
                    float dy = light.position.y - cellPos.y;
                    float distance = sqrt(dx * dx + dy * dy);

                    if (distance < light.radius) {
                        float intensity = 1.0f - (distance / light.radius);
                        intensity = intensity * intensity;

                        totalRed += (light.color.r / 255.0f) * intensity;
                        totalGreen += (light.color.g / 255.0f) * intensity;
                        totalBlue += (light.color.b / 255.0f) * intensity;
                    }
                }
            }

            // Clamp and scale light values
            totalRed = std::min(totalRed * 128, 255.0f);
            totalGreen = std::min(totalGreen * 128, 255.0f);
            totalBlue = std::min(totalBlue * 128, 255.0f);

            sf::Color lightColor(static_cast<sf::Uint8>(totalRed), static_cast<sf::Uint8>(totalGreen), static_cast<sf::Uint8>(totalBlue), 255);

            int vertexIndex = (i + j * gridX) * 4;
            lightVerts[vertexIndex + 0].position = sf::Vector2f(posX, posY);
            lightVerts[vertexIndex + 1].position = sf::Vector2f(posX + shadow_size, posY);
            lightVerts[vertexIndex + 2].position = sf::Vector2f(posX + shadow_size, posY + shadow_size);
            lightVerts[vertexIndex + 3].position = sf::Vector2f(posX, posY + shadow_size);

            for (int k = 0; k < 4; ++k)
                lightVerts[vertexIndex + k].color = lightColor;
        }
    }

    // Draw lights with additive blending
    sf::RenderStates lightStates;
    lightStates.blendMode = sf::BlendAdd;
    window.draw(lightVerts, lightStates);
}