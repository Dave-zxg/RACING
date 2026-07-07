#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H
#include "Config.h"
#include "Line.h"
#include <SFML/Graphics.hpp>

// 所有赛道实体抽象父类：玩家车、障碍车
class GameObject
{
public:
    // 3D世界坐标（赛道空间）
    float x;
    float y;
    float z;

    // 透视投影后屏幕数据（和Line投影字段命名统一）
    float screenX;
    float screenY;
    float screenW;
    float scale;

    // 通用构造：初始化全部坐标归零
    GameObject()
        : x(0), y(0), z(0), screenX(0), screenY(0), screenW(0), scale(0)
    {
    }

    // 纯虚函数：子类必须实现透视投影计算
    virtual void project(int camX, int camY, int camZ) = 0;
    // 纯虚函数：子类实现自身渲染
    virtual void render(sf::RenderWindow& win) = 0;
    // 纯虚函数：帧更新逻辑
    virtual void update(float dt) = 0;

    // 虚析构，保证子类析构正常调用
    virtual ~GameObject() = default;
};

#endif
