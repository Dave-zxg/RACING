#include "GameHUD.h"
#include "Menu.h"
#include <SFML/Graphics.hpp>
#include <optional>
#include <cmath>
#include <memory>
#include <utility>
#include "Config.h"
#include "Line.h"
#include "Obstacle.h"
#include "PlayerCar.h"
#include "GameCore.h"
using namespace sf;


int main()
{
    RenderWindow app(sf::VideoMode(sf::Vector2u(width, height)), "Outrun Racing!");
    app.setFramerateLimit(60);

    // 启动菜单
    GameMenu menu(app);
    GameMenu::MenuState menuResult = menu.RunMenu();
    // 菜单选择退出，直接结束程序
    if (menuResult == GameMenu::MENU_QUIT)
    {
        return 0;
    }
    // 选择开始游戏，往下执行赛车逻辑

    // 【关键修复】进入游戏前强制清空窗口，彻底消除菜单残留画面（2026.7.6早）
    app.clear(sf::Color(105, 205, 4));
    app.display();


    //玩家实例
    PlayerCar myplayer;
    myplayer.playerX = 0.f;
    myplayer.speed = 0;
    myplayer.H = 1500.f;

    //路边贴图
    Texture t[50];
    for (int i = 1; i <= 7; i++)
    {
        bool loadOk = t[i].loadFromFile("images/" + std::to_string(i) + ".png");
        t[i].setSmooth(true);
    }

    //远景背景贴图
    Texture bg;
    bool bgOk = bg.loadFromFile("images/bg.png");
    bg.setRepeated(true);
    Sprite sBackground(bg);
    sf::IntRect rect(sf::Vector2i(0.f, 0.f), sf::Vector2i(5000.f, 411.f));
    sBackground.setTextureRect(rect);
    sBackground.setPosition(sf::Vector2f(-2000.f, 0.f));

    // 玩家小车贴图
    Texture carTex;
    bool carLoadOk = carTex.loadFromFile("images/mycar.png");
    carTex.setSmooth(true);
    Sprite playerCar(carTex);

    // 障碍物小车贴图
    sf::Texture obsCarTex;
    bool obsTexLoad = obsCarTex.loadFromFile("images/othercar.png");
    obsCarTex.setSmooth(true);


        // 实例游戏核心，窗口传入，所有贴图引用传入内部循环
    GameCore game(app);
    game.Run(t, bg, sBackground, carTex, playerCar, obsCarTex);

    return 0;
}