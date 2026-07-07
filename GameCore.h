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
        sf::Texture& obsCarTex
    );
    sf::RenderWindow& app;

    // 仅赛道、实体、游戏状态，无任何贴图
    std::vector<Line> lines;
    int N;
    std::vector<obstacle> obstacleList;
    PlayerCar myplayer;
    GameHUD hud;

    int gameScore;
	int bestScore;
    float gameTime;
    sf::Clock gameClock;
    bool gameOver;
	bool isNewRecord;

    // 内部逻辑函数，贴图全部作为参数传入
    void GenerateTrack();
    void SpawnObstacles(sf::Texture& obsCarTex);
    void ResetFullGame(sf::Texture& obsCarTex);
    void HandleEventLoop(sf::Texture& obsCarTex);
    void UpdateGameLogic(sf::Sprite& playerCar,float dt);
    void RenderScene(
        sf::Texture t[50],
        sf::Texture& bg,
        sf::Sprite& sBackground,
        sf::Sprite& playerCar,
        sf::Texture& obsCarTex
    );
};
#endif
