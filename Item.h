#ifndef ITEM_H
#define ITEM_H
#include <SFML/Graphics.hpp>
#include "Line.h"
#include "Config.h"
#include "GameObject.h"

class Item : public GameObject
{
public:
    ItemType type;
    float zPos;
    float laneOffset;
    bool active;
    sf::Sprite itemSprite;

    // 构造：同时传入氮气贴图、飞行贴图（main加载的纹理）
    Item(const sf::Texture& nitroTex, const sf::Texture& flyTex);

    // 两套纹理存储
     const sf::Texture* m_nitroTex;
     const sf::Texture* m_flyTex;

    // 设置道具赛道位置、类型，自动切换贴图
    void setTrackPos(float z, float offset, ItemType t);

    // 透视渲染（和Obstacle.render参数统一）
    void render(sf::RenderWindow& app, Line& seg, int camH, int winW, int winH, float campos);

    // 判断道具是否超出视野，标记失效
    void checkLifetime(float playerPosZ, int totalRoadLength);

    // 实现GameObject纯虚接口
    virtual void project(int camX, int camY, int camZ) override;
    virtual void render(sf::RenderWindow& win) override;
    virtual void update(float dt) override;
};

// 道具拾取碰撞检测（仅判断是否拾取，不触发GameOver）
bool CheckItemPickCollision(const sf::Sprite& playerSpr, const sf::Sprite& itemSpr);

#endif
