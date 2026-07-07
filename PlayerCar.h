#ifndef PLAYERCAR_H
#define PLAYERCAR_H
#include <SFML/Graphics.hpp>
#include "Config.h"
#include "Obstacle.h"

// 类名完全沿用你代码里的 PlayerCar，不改名
class PlayerCar
{
public:
    // 对外提供成员（保持你原有变量名，外部直接访问不改动业务代码）
    float playerX;
    int speed;
    int pos;
    int H;
 
    // 所有函数声明+实现全部放在类内部，不要写到 }; 外面
    // 加载玩家小车贴图，原样复制你的加载逻辑
 

    // 处理所有玩家按键输入，传入dt，修改内部speed/playerX/H
    void HandleInput(float dt, int& pos)
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
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Tab))
            speed *= 3.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
            H += 100.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
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

    // 设置小车屏幕位置（底部居中），原样复制你的定位代码
    //void SetScreenPos()
    //{
    //    sf::FloatRect carBounds = playerCar.getLocalBounds();
    //    float carX = (width - carBounds.size.x) / 2.f;
    //    float carY = height - carBounds.size.y;
    //    playerCar.setPosition(sf::Vector2f(carX, carY));
    //}

    //// 绘制玩家小车到窗口
    //void DrawPlayerCar(sf::RenderWindow& app)
    //{
    //    app.draw(playerCar);
    //}

    // R键重置玩家所有数据（playerX、speed、H、pos）
    void ResetPlayer(int& pos)
    {
        playerX = 0.f;
        pos = 0;
        H = 1500.f;
        speed = 0;
    }
};

#endif