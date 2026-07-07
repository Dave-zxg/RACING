#include "Line.h"
#include "Config.h"

// Line构造函数原样复制
Line::Line()
{
    spriteX = curve = x = y = z = 0.f;
    spriteId = 0;
}

// Line::project 原样复制，无任何修改
void Line::project(int camX, int camY, int camZ)
{
    scale = camD / (z - camZ);
    X = (1 + scale * (x - camX)) * width / 2.f;
    Y = (1 - scale * (y - camY)) * height / 2.f;
    W = scale * roadW * width / 2.f;
}

// Line::drawSprite 原样复制
void Line::drawSprite(sf::RenderWindow& app, sf::Texture texArr[50])
{
    if (spriteId == 0)
        return;
    sf::Sprite s(texArr[spriteId]);
    sf::IntRect texRect = s.getTextureRect();
    int w = static_cast<int>(texRect.size.x);
    int h = static_cast<int>(texRect.size.y);
    float destX = X + scale * spriteX * width / 2.f;
    float destY = Y + 4.f;
    float destW = w * W / 266.f;
    float destH = h * W / 266.f;
    destX += destW * spriteX; // offsetX
    destY += destH * (-1.f);  // offsetY
    float clipH = destY + destH - clip;
    if (clipH < 0.f)
        clipH = 0.f;
    if (clipH >= destH)
        return;
    sf::IntRect rect(sf::Vector2i(0, 0), sf::Vector2i(w, static_cast<int>(h - h * clipH / destH)));
    s.setTextureRect(rect);
    s.setScale(sf::Vector2f(destW / float(w), destH / float(h)));
    s.setPosition(sf::Vector2f(destX, destY));
    app.draw(s);
}

// 全局drawQuad函数原样复制
void drawQuad(sf::RenderWindow& w, sf::Color c, int x1, int y1, int w1, int x2, int y2, int w2)
{
    sf::ConvexShape shape(4);
    shape.setFillColor(c);
    shape.setPoint(0, sf::Vector2f(x1 - w1, y1));
    shape.setPoint(1, sf::Vector2f(x2 - w2, y2));
    shape.setPoint(2, sf::Vector2f(x2 + w2, y2));
    shape.setPoint(3, sf::Vector2f(x1 + w1, y1));
    w.draw(shape);
}