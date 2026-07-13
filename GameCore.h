#ifndef GAMECORE_H
#define GAMECORE_H
#include <SFML/Graphics.hpp>
#include <optional>
#include <cmath>
#include <vector>
#include "Config.h"
#include "Line.h"
#include "Obstacle.h"
#include "PlayerCar.h"
#include "GameHUD.h"
#include "GameObject.h"
#include "Item.h"
using namespace sf;
// 全局碰撞函数声明
bool CheckSpriteCollision(const sf::Sprite& sprPlayer, const sf::Sprite& sprObs);

class GameCore
{
public:
    // 构造仅接收窗口，贴图外部传入
    GameCore(sf::RenderWindow& win);
    // 主循环，所有贴图参数由main调用时传入
    void Run(
        sf::Texture t[50],
        sf::Texture& bg,
        sf::Sprite& sBackground,
        sf::Texture& carTex,
        sf::Sprite& playerCar,
        sf::Texture& obsCarTex,
        sf::Texture& nitroTex,
        sf::Texture&flyTex
    );
    sf::RenderWindow& app;
    std::vector<Line> lines;


    int N;
    std::vector<obstacle> obstacleList;
    // 新增道具列表，和障碍车格式对齐
    std::vector<Item> itemList;
    PlayerCar myplayer;
    GameHUD hud;

    float nitroTimer;           // 氮气剩余时间
    float flyTimer;             // 飞行剩余时间

    // 新增道具pending标志：表示已拾取但尚未激活计时
    bool hasNitroPending;
    bool hasFlyPending;

    int gameScore;
	int bestScore;
    float gameTime;
    sf::Clock gameClock;
    bool gameOver;
	bool isNewRecord;

    // 内部逻辑函数，贴图全部作为参数传入
    void GenerateTrack();
    void SpawnObstacles(sf::Texture& obsCarTex);
    // 生成道具，传入两张道具纹理
    void RefreshSpawnItems(sf::Texture& nitroTex, sf::Texture& flyTex,float playerPos);
    // 每帧更新道具倒计时
    void UpdateItemTimer(float dt);
    void ResetFullGame(sf::Texture& obsCarTex, sf::Texture& nitroTex, sf::Texture& flyTex);
    void HandleEventLoop(sf::Texture& obsCarTex, sf::Texture& nitroTex, sf::Texture& flyTex);
    void UpdateGameLogic(sf::Sprite& playerCar,float dt);
    void RenderScene(
        sf::Texture t[50],
        sf::Texture& bg,
        sf::Sprite& sBackground,
        sf::Sprite& playerCar,
        sf::Texture& obsCarTex,sf::Texture& nitroTex,
        sf::Texture& flyTex
    );
};
#endif