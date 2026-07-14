#include "Explosion.h"
#include <cmath>
#include <iostream>
extern const float camD;
extern const int width;
extern const int height;
extern const int roadW;

// 带参构造（解决无默认构造报错）
// 构造函数使用初始化列表初始化spr
Explosion::Explosion(float trackX, float trackZ, sf::Texture &tex, int totalFrame)
    : spr(tex)
{
    Spawn(trackX, trackZ, tex, totalFrame);
}

void Explosion::Spawn(float trackX, float trackZ, sf::Texture &tex, int totalFrame)
{
    x = trackX;
    z = trackZ;
    frameCount = totalFrame;
    curFrame = 0;
    animTimer = 0.f;
    animDuration = 2.f;

    spr.setTexture(tex);
    sf::Vector2u texSize = tex.getSize();
    int frameW = static_cast<int>(texSize.x / frameCount);
    int frameH = static_cast<int>(texSize.y);

    // SFML3 合法写法
    sf::Vector2i pos{0, 0};
    sf::Vector2i sz{frameW, frameH};
    spr.setTextureRect(sf::IntRect{pos, sz});

    spr.setOrigin(sf::Vector2f(static_cast<float>(frameW) / 2.f, static_cast<float>(frameH) / 2.f));
}

void Explosion::Render(sf::RenderWindow &win)
{
    std::cout << "got" << spr.getPosition().x << "," << spr.getPosition().y << std::endl;
    win.draw(spr);
}
void Explosion::Project(int camX, int camY, int camZ)
{
    float dist = z - camZ;

    if (dist < 18.f)
        dist = 18.f;

    scale = camD / dist;
    scale *= 15.f;
    if (scale < 0.15f)
        scale = 0.15f;
    // if (scale > 2.2f)
    //   scale = 2.2f;

    screenX = width / 2.f - (x - camX * roadW) * scale;
    screenY = height - 220.f - scale * 100.f;
    std::cout << "dist=" << dist << " scale=" << scale << " X=" << screenX << " Y=" << screenY << std::endl;
}

void Explosion::Update(float dt, int camX, int camY, int camZ)
{
    animTimer += dt;
    float frameStep = animDuration / frameCount;
    curFrame = static_cast<int>(animTimer / frameStep);
    if (curFrame >= frameCount)
        curFrame = frameCount - 1;

    sf::Vector2u texSize = spr.getTexture().getSize();
    int frameW = static_cast<int>(texSize.x / frameCount);
    int frameH = static_cast<int>(texSize.y);

    sf::Vector2i pos{curFrame * frameW, 0};
    sf::Vector2i sz{frameW, frameH};
    spr.setTextureRect(sf::IntRect{pos, sz});

    Project(camX, camY, camZ);
    spr.setScale(sf::Vector2f(scale, scale));
    spr.setPosition(sf::Vector2f(screenX, screenY));
}
