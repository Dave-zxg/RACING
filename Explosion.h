namespace sf
{
    class RenderWindow;
}
#ifndef EXPLOSION_H
#define EXPLOSION_H
#include <SFML/Graphics.hpp>
#include "Config.h"
#include "Line.h"

// 爆炸贴图动画实体
struct Explosion
{
    Explosion(float trackX, float trackZ, sf::Texture &tex, int totalFrame);

    sf::Sprite spr;
    float animTimer;    // 动画计时器
    float animDuration; // 总动画时长
    int frameCount;     // 总帧数
    int curFrame;       // 当前帧
    float x, z;         // 赛道3D坐标
    float screenX, screenY;
    float scale;

    // 初始化爆炸
    void Spawn(float trackX, float trackZ, sf::Texture &tex, int totalFrame);
    // 更新动画+透视
    void Update(float dt, int camX, int camY, int camZ);
    // 投影计算
    void Project(int camX, int camY, int camZ);
    void Render(sf::RenderWindow &win);
    bool IsDead() const { return animTimer >= animDuration; }
};
#endif