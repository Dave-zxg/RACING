#include "Obstacle.h"
#include <algorithm>
// 构造函数实现
obstacle::obstacle(const sf::Texture& tex) :obsSprite(tex)
{
    zPos = 0.f;
    laneOffset = 0.f;
    active = false;
}
void obstacle::setTrackPos(float z, float offset)
{
    zPos = z;
    laneOffset = offset;
    active = true;
}
void obstacle::render(sf::RenderWindow& app, Line& seg, int camH, int winW, int winH, float campos)
{
    if (seg.scale <= 0.f)
    {
        return;
    }
    sf::FloatRect texBounds = obsSprite.getLocalBounds();
    float w = texBounds.size.x;
    float h = texBounds.size.y;
    float destW = std::abs(w * seg.W / 266.f);
    float destH = std::abs(h * seg.W / 266.f);
    float screenX = seg.X + seg.scale * laneOffset * winW / 2.f - destW / 2.f;
    float screenY = seg.Y + 4.f;
    if (screenY + destH < 0.f) return;
    if (screenY > winH) return;
    obsSprite.setScale(sf::Vector2f(destW / w, destH / h));
    obsSprite.setPosition(sf::Vector2f(screenX, screenY));
    if (screenY < winH)
    {
        app.draw(obsSprite);
    }
}
void obstacle::checkLifetime(int playerPosZ, int totalRoadLength)
{
    float relativeZ = zPos - playerPosZ;
    if (relativeZ < -totalRoadLength)
    {
        active = false;
    }
}

// 全局碰撞函数
bool CheckSpriteCollision(const sf::Sprite& sprPlayer, const sf::Sprite& sprObs)
{
    sf::FloatRect playerBox = sprPlayer.getGlobalBounds();
    sf::FloatRect obsBox = sprObs.getGlobalBounds();
    playerBox.position.x += 70;
    playerBox.position.y += 70;
    playerBox.size.x -= 100;
    playerBox.size.y -= 100;
    obsBox.position.x += 50;
    obsBox.position.y += 50;
    obsBox.size.x -= 80;
    obsBox.size.y -= 80;
    float pLeft = playerBox.position.x;
    float pTop = playerBox.position.y;
    float pRight = playerBox.position.x + playerBox.size.x;
    float pBottom = playerBox.position.y + playerBox.size.y;
    float oLeft = obsBox.position.x;
    float oTop = obsBox.position.y;
    float oRight = obsBox.position.x + obsBox.size.x;
    float oBottom = obsBox.position.y + obsBox.size.y;
    bool noOverlap = (pRight < oLeft) || (pLeft > oRight) || (pBottom < oTop) || (pTop > oBottom);
    if (noOverlap) return false;
    float overlapX = std::min(pRight, oRight) - std::max(pLeft, oLeft);
    float overlapY = std::min(pBottom, oBottom) - std::max(pTop, oTop);
    return (overlapX > 100.f) && (overlapY > 100.f);
}