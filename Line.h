#ifndef TRACKSEGMENT_H
#define TRACKSEGMENT_H
#include <SFML/Graphics.hpp>
#include "Config.h"

// 原Line结构体，重命名为TrackSegment语义更清晰
struct Line
{
    // 3D赛道坐标
    float x, y, z;
    // 投影后屏幕坐标
    float X, Y, W;
    // 弯道、路边道具参数
    float curve, spriteX, clip, scale;
    int spriteId;

    ItemType spawnItem; // 道具系统新增

    Line();
    // 透视投影计算
    void project(int camX, int camY, int camZ);
    // 绘制路边树木/标牌
    void drawSprite(sf::RenderWindow& app, sf::Texture texArr[50]);
};

// 全局绘制四边形工具函数（赛道专用）
void drawQuad(sf::RenderWindow& w, sf::Color c,int x1, int y1, int w1,int x2, int y2, int w2);

#endif
