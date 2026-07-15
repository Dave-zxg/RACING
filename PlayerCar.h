#ifndef PLAYERCAR_H
#define PLAYERCAR_H
#include <sfml/Graphics.hpp>
#include "Config.h"
#include "Obstacle.h"
#include "GameObject.h"
// 类名完全沿用你代码里的 PlayerCar，不改名
class PlayerCar : public GameObject
{
public:
    // 对外提供成员（保持你原有变量名，外部直接访问不改动业务代码）
    float playerX;
    int speed;
    int pos;
    int H;
    bool controlLock = false;

    // 所有函数声明+实现全部放在类内部，不要写到 }; 外面
    // 加载玩家小车贴图，原样复制你的加载逻辑

    // 处理所有玩家按键输入，传入dt，修改内部speed/playerX/H
    void HandleInput(float dt, int& pos, float& nitroTime, float& flyTime, bool& nitroPending, bool& flyPending)
    {
        speed = 0;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
            playerX += 0.1f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
            playerX -= 0.1f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
            speed = 200.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
            speed = -200.f;
        // 先处理按键触发 pending -> 启动计时器的逻辑（如果有pending）
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Tab))
        {
            if (nitroTime <= 0.f && nitroPending)
            {
                nitroTime = ITEM_DURATION; // 启动计时器
                nitroPending = false;
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
        {
            if (flyTime <= 0.f && flyPending)
            {
                flyTime = ITEM_DURATION; // 启动计时器
                flyPending = false;
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Tab) && nitroTime > 0.f)
            speed *= 2.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) && flyTime > 0.f)
            H += 100.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) && H > 1500)
            H -= 100.f;
        pos += speed;
    }

    // 判断是否驶出安全车道，原样复制你的越界逻辑
    bool IsOutOfLane()
    {
        if (playerX < -SAFE_LANE_LIMIT || playerX > SAFE_LANE_LIMIT)
        {
            return true;
        }
        return false;
    }
    void ResetPlayer(int& pos)
    {
        playerX = 0.f;
        pos = 0;
        H = 1500.f;
        speed = 0;
        x = 0;
        y = 0;
        z = 0;
        screenX = screenY = screenW = scale = 0;
    }
    virtual void project(int camX, int camY, int camZ) override;
    virtual void render(sf::RenderWindow& win) override;
    virtual void update(float dt) override;
};

#endif