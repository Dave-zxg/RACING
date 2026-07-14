#ifndef GAMECORE_H
#define GAMECORE_H
#include <sfml/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <optional>
#include <cmath>
#include <vector>
#include "Config.h"
#include "Line.h"
#include "Obstacle.h"
#include "PlayerCar.h"
#include "GameHUD.h"
#include "Explosion.h"
#include "Item.h"
using namespace sf;
// 全局碰撞函数声明
bool CheckSpriteCollision(const sf::Sprite &sprPlayer, const sf::Sprite &sprObs);

class GameCore
{
public:
    // 构造仅接收窗口，贴图外部传入
    GameCore(sf::RenderWindow &win);
    // 新增：设置游戏背景音乐指针
    void setMusic(sf::Music *music) { m_gameMusic = music; }
    // 主循环，所有贴图参数由main调用时传入
    void Run(
        sf::Texture t[50],
        sf::Texture &bg,
        sf::Sprite &sBackground,
        sf::Texture &carTex,
        sf::Sprite &playerCar,
        sf::Texture &obsCarTex,
        sf::Texture &nitroTex,
        sf::Texture &flyTex,
        sf::Texture &explodeTex);
    sf::RenderWindow &app;

    bool waitingCrash = false;
    float crashDelay = 0.f;

    // 仅赛道、实体、游戏状态，无任何贴图
    std::vector<Line> lines;
    int N;
    std::vector<obstacle> obstacleList;
    std::vector<Item> itemList;
    PlayerCar myplayer;
    GameHUD hud;

    float nitroTimer; // 氮气剩余时间
    float flyTimer;
    float laneDeviateTimer; // 偏离1.0f累计计时
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
    void SpawnObstacles(sf::Texture &obsCarTex);
    void RefreshSpawnItems(sf::Texture &nitroTex, sf::Texture &flyTex, float playerPos);
    // 每帧更新道具倒计时
    void UpdateItemTimer(float dt);
    void ResetFullGame(sf::Texture &obsCarTex, sf::Texture &nitroTex, sf::Texture &flyTex);
    void HandleEventLoop(sf::Texture &obsCarTex, sf::Texture &nitroTex, sf::Texture &flyTex);
    void UpdateGameLogic(sf::Sprite &playerCar, float dt, sf::Texture &obsCarTex, sf::Texture &explodeTex);
    void RenderScene(
        sf::Texture t[50],
        sf::Texture &bg,
        sf::Sprite &sBackground,
        sf::Sprite &playerCar,
        sf::Texture &obsCarTex,
        sf::Texture &nitroTex,
        sf::Texture &flyTex,
        sf::Texture &explodeTex,
        float dt);
    void setNitroMusic(sf::Music *music) { m_nitroMusic = music; } // 新增

private:
    // 爆炸相关容器
    std::vector<Explosion> explosionList;
    // 爆炸函数声明
    void SpawnExplosion(float trackX, float trackZ, sf::Texture &expTex);
    void UpdateDrawExplosion(float dt, sf::RenderWindow &app, sf::Texture &expTex);
    sf::Music *m_gameMusic = nullptr;  // 游戏背景音乐指针
    sf::Music *m_nitroMusic = nullptr; // 氮气加速音效
    bool m_prevTabPressed = false;     // 用于检测按键上升沿
};
#endif