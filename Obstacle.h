#ifndef OBSTACLE_H
#define OBSTACLE_H
#include <SFML/Graphics.hpp>
#include "Line.h"
#include "Config.h"
#include "GameObject.h"
//障碍物类（障碍小车和其他障碍物的出现和消失、玩家碰撞判定）
class obstacle:public GameObject
{
public:
    // 赛道逻辑坐标
    float zPos;        // 障碍物在赛道的深度z
    float laneOffset;  // 横向车道偏移（和playerX同单位）
    bool active;       // 是否有效，失效后删除
    sf::Sprite obsSprite;
    // 构造函数：传入障碍物贴图
    obstacle(const sf::Texture& tex);
    // 设置障碍物初始赛道位置
    void setTrackPos(float z, float offset);
    // 复用赛道透视投影，计算并渲染障碍物
    void render(sf::RenderWindow& app, Line& seg, int camH, int winW, int winH, float campos);
    // 判断障碍物是否跑到玩家身后，标记失效
    void checkLifetime(int playerPosZ, int totalRoadLength);
    // 实现父类GameObject纯虚接口
    virtual void project(int camX, int camY, int camZ) override;
    virtual void render(sf::RenderWindow& win) override;
    virtual void update(float dt) override;
};

// 全局碰撞检测函数声明
bool CheckSpriteCollision(const sf::Sprite& sprPlayer, const sf::Sprite& sprObs);



#endif
