#include "PlayerCar.h"
#include "Config.h"
#include <SFML/Graphics.hpp>
using namespace sf;
void PlayerCar::update(float dt)
{
    HandleInput(dt, this->pos);
}
void PlayerCar::project(int camX, int camY, int camZ)
{
    // 和 Line::project 透视算法完全统一
    scale = camD / (z - camZ);
    screenX = (1 + scale * (x - camX)) * width / 2.f;
    screenY = (1 - scale * (y - camY)) * height / 2.f;
    screenW = scale * roadW * width / 2.f;
}
void PlayerCar::render(sf::RenderWindow& win)
{
    // 预留统一渲染接口，当前玩家精灵由外部main传入GameCore
    // 后续如果把Sprite移入PlayerCar，再在这里写绘制代码
}