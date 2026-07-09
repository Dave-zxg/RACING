#include "Item.h"
#include <algorithm>

// 构造接收两张纹理，存入成员引用
Item::Item(const sf::Texture& nitroTex, const sf::Texture& flyTex)
    : m_nitroTex(&nitroTex), m_flyTex(&flyTex),itemSprite(nitroTex),
    zPos(0), laneOffset(0), active(false), type(ITEM_NONE)
{
}

// 设置道具类型，自动绑定对应贴图
void Item::setTrackPos(float z, float offset, ItemType t)
{
    zPos = z;
    laneOffset = offset;
    type = t;
    active = true;

    // 根据类型切换贴图
    if (type == ITEM_NITRO)
        itemSprite.setTexture(*m_nitroTex);
    else if (type == ITEM_FLY)
        itemSprite.setTexture(*m_flyTex);
}

void Item::render(sf::RenderWindow& app, Line& seg, int camH, int winW, int winH, float campos)
{
    if (seg.scale <= 0.f || !active)
        return;

    sf::FloatRect texBounds = itemSprite.getLocalBounds();
    float w = texBounds.size.x;
    float h = texBounds.size.y;

    float destW = std::abs(w * seg.W / 400.f);
    float destH = std::abs(h * seg.W / 400.f);
    float screenX = seg.X + seg.scale * laneOffset * winW / 2.f - destW / 2.f;
    float screenY = seg.Y ;

    if (screenY + destH < 0.f || screenY > winH)
        return;

    itemSprite.setScale(sf::Vector2f(destW / w, destH / h));
    itemSprite.setPosition(sf::Vector2f(screenX, screenY));
    app.draw(itemSprite);
}

// 剩余project、update、checkLifetime、碰撞函数完全不变，无需改动
void Item::checkLifetime(float playerPosZ, int totalRoadLength)
{
    float relativeZ = zPos - playerPosZ;
    // 道具跑到玩家身后很远，失效删除
    if (relativeZ < -totalRoadLength * 0.5f)
        active
        = false;
}

// 父类虚接口
void Item::project(int camX, int camY, int camZ)
{
    scale
        = camD / (z - camZ);
    screenX
        = (1 + scale * (x - camX)) * width / 2.f;
    screenY
        = (1 - scale * (y - camY)) * height / 2.f;
    screenW
        = scale * roadW * width / 2.f;
}

void Item::render(sf::RenderWindow& win)
{
    // 空实现，使用带赛道参数的重载render
}

void Item::update(float dt)
{
    // 预留帧更新（闪烁、动画等后续扩展）
}

// 道具拾取碰撞检测（碰撞框缩小，碰到即拾取）
bool CheckItemPickCollision(const sf::Sprite& playerSpr, const sf::Sprite& itemSpr)
{
    sf
        ::FloatRect playerBox = playerSpr.getGlobalBounds();
    sf
        ::FloatRect itemBox = itemSpr.getGlobalBounds();

    // 收缩碰撞盒，避免误拾取
    playerBox
        .position.x += 80;
    playerBox
        .position.y += 80;
    playerBox
        .size.x -= 120;
    playerBox
        .size.y -= 120;

    itemBox
        .position.x += 10;
    itemBox
        .position.y += 10;
    itemBox
        .size.x -= 20;
    itemBox
        .size.y -= 20;

    return playerBox.findIntersection(itemBox).has_value();
}