#include "GameCore.h"
#include <cstdio>
#include <algorithm>
#include <SFML/Graphics.hpp>
using namespace sf;
GameCore::GameCore(sf::RenderWindow& win)
	: app(win), gameScore(0), gameTime(0.f), gameOver(false), bestScore(0), isNewRecord(false)
{
    // 玩家初始值不变
    myplayer.playerX = 0.f;
    myplayer.speed = 0;
    myplayer.H = 1500.f;
    GenerateTrack();
}

// 生成赛道（无贴图，不变）
void GameCore::GenerateTrack()
{
    lines.clear();
    for (int i = 0; i < 1600; i++)
    {
        Line line;
        line.z = i * segL;
        if (i > 300 && i < 700)
            line.curve = 0.5;
        if (i > 1100.f)
            line.curve = -0.7;
        if (i < 300 && i % 20 == 0)
        {
            line.spriteX = -2.5f;
            line.spriteId = 5;
        }
        if (i % 17 == 0)
        {
            line.spriteX = 2.0f;
        }
        if (i > 300 && i % 20 == 0)
        {
            line.spriteX = -0.7f;
            line.spriteId = 4;
        }
        if (i > 800 && i % 20 == 0)
        {
            line.spriteX = -1.2f;
            line.spriteId = 1;
        }
        if (i == 400)
        {
            line.spriteX = -1.2f;
            line.spriteId = 7;
        }
        if (i > 750)
            line.y = sin(i / 30.0f) * 1500.f;
        lines.push_back(std::move(line));
    }
    N = lines.size();
}

// 生成障碍，obsCarTex由外部main传入
void GameCore::SpawnObstacles(sf::Texture& obsCarTex)
{
    obstacleList.clear();
    for (int i = 200; i < N; i += OBSTACLE_SPAWN_STEP)
    {
        obstacle newObs(obsCarTex);
        float fixedZ = i * segL;
        int laneIndex = i % 3;
        float offset = 0.f;
        if (laneIndex == 0) offset = -0.8f;
        else if (laneIndex == 1) offset = 0.8f;
        newObs.setTrackPos(fixedZ, offset);
        obstacleList.push_back(newObs);
    }
}

// R键重置，贴图参数外部传入
void GameCore::ResetFullGame(sf::Texture& obsCarTex)
{
    gameOver = false;
    gameScore = 0;
	isNewRecord = false;
    gameTime = 0.f;
    myplayer.ResetPlayer(myplayer.pos);
    obstacleList.clear();
    SpawnObstacles(obsCarTex);
}

// 事件循环，无贴图
void GameCore::HandleEventLoop(sf::Texture& obsCarTex)
{
    std::optional<sf::Event> eventOpt;
    while ((eventOpt = app.pollEvent()))
    {
        auto& e = *eventOpt;
        if (e.is<sf::Event::Closed>())
        {
            app.close();
        }
        if (const auto* keyEvt = eventOpt->getIf<sf::Event::KeyPressed>())
        {
            if (gameOver && keyEvt->code == sf::Keyboard::Key::R)
            {
                
                ResetFullGame(obsCarTex);
                
            }
        }
    }
}

// 更新逻辑，仅playerCar作为外部精灵传入
void GameCore::UpdateGameLogic(sf::Sprite& playerCar,float dt)
{
    if (!gameOver)
    {
        myplayer.HandleInput(dt, myplayer.pos);
        gameTime += dt;
        if (myplayer.speed > 0)
        {
            float addScore = myplayer.speed * dt* 0.5f;
            gameScore += static_cast<int>(addScore);
        }
    }
    while (myplayer.pos >= N * segL)
        myplayer.pos -= N * segL;
    while (myplayer.pos < 0)
        myplayer.pos += N * segL;

    if (myplayer.IsOutOfLane())
    {
        gameOver = true;
        if (gameScore > bestScore)
        {
            bestScore = gameScore;
            isNewRecord = true;
        }
        else
        {
            isNewRecord = false;
        }
    }

    // 小车屏幕定位，playerCar外部传入
    sf::FloatRect carBounds = playerCar.getLocalBounds();
    float carX = (width - carBounds.size.x) / 2.f;
    float carY = height - carBounds.size.y;
    playerCar.setPosition(sf::Vector2f(carX, carY));

    hud.UpdateScore(gameScore);
    hud.UpdateTimer(gameTime);
    hud.UpdateBest(bestScore);
    hud.SetGameOver(gameOver, isNewRecord);
}

// 渲染：全部贴图、精灵从参数传入，内部不持有
void GameCore::RenderScene(
    sf::Texture t[50],
    sf::Texture& bg,
    sf::Sprite& sBackground,
    sf::Sprite& playerCar,
    sf::Texture& obsCarTex
)
{
    app.clear(Color(105, 205, 4));
    app.draw(sBackground);
    int startPos = myplayer.pos / segL;
    int camH = lines[startPos].y + myplayer.H;

    if (myplayer.speed > 0)
    {
        float offsetX = -lines[startPos].curve * 2.f;
        sBackground.move(sf::Vector2f(offsetX, 0.f));
    }
    if (myplayer.speed < 0)
    {
        float offsetX = lines[startPos].curve * 2.f;
        sBackground.move(sf::Vector2f(offsetX, 0.f));
    }

    int maxy = height;
    float x = 0, dx = 0;
    for (int n = startPos; n < startPos + 300; n++)
    {
        Line& l = lines[n % N];
        l.project(myplayer.playerX * roadW - x, camH, startPos * segL - (n >= N ? N * segL : 0));
        x += dx;
        dx += l.curve;
        l.clip = maxy;
        if (l.Y >= maxy)
            continue;
        maxy = l.Y;
        Color grass = (n / 3) % 2 ? Color(16, 200, 16) : Color(0, 154, 0);
        Color rumble = (n / 3) % 2 ? Color(255, 255, 255) : Color(0, 0, 0);
        Color road = (n / 3) % 2 ? Color(107, 107, 107) : Color(105, 105, 105);
        Line& p = lines[(n - 1) % N];
        drawQuad(app, grass, 0, p.Y, width, 0, l.Y, width);
        drawQuad(app, rumble, p.X, p.Y, p.W * 1.2, l.X, l.Y, l.W * 1.2);
        drawQuad(app, road, p.X, p.Y, p.W, l.X,l.Y, l.W);
    }

    for (int n = startPos + 300; n > startPos; n--)
        lines[n % N].drawSprite(app, t);

    int totalRoadLength = N * segL;
    for (auto& obs : obstacleList)
    {
        if (!obs.active)
        {
            printf("跳过：障碍车已失效\n");
            continue;
        }
        float relZ = obs.zPos - myplayer.pos;
        while (relZ < 0) relZ += totalRoadLength;
        while (relZ > totalRoadLength) relZ -= totalRoadLength;
        int segIndex = static_cast<int>(relZ / segL) % N;
        Line& targetSeg = lines[segIndex];
        int currentCamH = lines[startPos].y + myplayer.H;
        float curveX = 0.f;
        float curveDx = 0.f;
        for (int n = startPos; n <= segIndex; n++)
        {
            int idx = n % N;
            curveDx += lines[idx].curve;
            curveX += curveDx;
        }
        targetSeg.project(myplayer.playerX * roadW - curveX, currentCamH, myplayer.pos);
        obs.render(app, targetSeg, currentCamH, width, height, myplayer.pos);

        if (!gameOver)
        {
            bool isCrash = CheckSpriteCollision(playerCar, obs.obsSprite);
            if (isCrash)
            {
                gameOver = true;
                if (gameScore > bestScore)
                {
                    bestScore = gameScore;
                    isNewRecord = true;
                }
                else
                {
                    isNewRecord = false;
                }
            }
        }
        obs.checkLifetime(myplayer.pos, totalRoadLength);
    }

    for (auto iter = obstacleList.begin(); iter != obstacleList.end();)
    {
        if (!iter->active) iter = obstacleList.erase(iter);
        else ++iter;
    }

    app.draw(playerCar);
    hud.Render(app);
    app.display();
}

// 主循环：所有贴图从main传入，贴图变量全部留在main
void GameCore::Run(
    sf::Texture t[50],
    sf::Texture& bg,
    sf::Sprite& sBackground,
    sf::Texture& carTex,
    sf::Sprite& playerCar,
    sf::Texture& obsCarTex
)
{
    // 首次生成初始障碍（obsCarTex外部）
    SpawnObstacles(obsCarTex);

    while (app.isOpen())
    {
        float dt = gameClock.restart().asSeconds();
        HandleEventLoop(obsCarTex);

        UpdateGameLogic(playerCar,dt);
        RenderScene(t, bg, sBackground, playerCar, obsCarTex);
    }
}
