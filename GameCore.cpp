#include "GameCore.h"
#include <cstdio>
#include <algorithm>
#include <sfml/Graphics.hpp>
#include <iostream>
#include "Item.h"

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
        line.spawnItem = ITEM_NONE;
        if (i % ITEM_SPAWN_INTERVAL == 0 && i > 50)
        {
            // 计算当前是第几个道具点位
            int seqIndex = i / ITEM_SPAWN_INTERVAL;
            if (seqIndex % 2 == 0)
            {
                line.spawnItem = ITEM_NITRO;
            }
            else
            {
                line.spawnItem = ITEM_FLY;
            }
            line.spriteX = 0.f;
            line.spriteId = 6;
        }
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
        if (laneIndex == 0)
            offset = -0.8f;
        else if (laneIndex == 1)
            offset = 0.8f;
        newObs.setTrackPos(fixedZ, offset);
        obstacleList.push_back(newObs);
    }
}

// R键重置，贴图参数外部传入
void GameCore::RefreshSpawnItems(sf::Texture& nitroTex, sf::Texture& flyTex, float playerPos)
{
    for (int i = 0; i < N; i++)
    {
        Line& line = lines[i];
        if (line.spawnItem == ITEM_NONE)
            continue;

        // 实例化Item，传入两张道具贴图
        Item newItem(nitroTex, flyTex);
        float zPos = i * segL;
        bool exist = false;
        for (auto& it : itemList)
        {
            if (it.active && fabs(it.zPos - zPos) < segL)
            {
                exist = true;
                break;
            }
        }
        if (exist)
            continue;
        newItem.setTrackPos(zPos, 0.f, line.spawnItem);
        itemList.push_back(newItem);
    }
}

void GameCore::UpdateItemTimer(float dt)
{
    if (nitroTimer > 0.f)
        nitroTimer -= dt;
    if (flyTimer > 0.f)
        flyTimer -= dt;
    if (nitroTimer < 0.f)
        nitroTimer = 0.f;
    if (flyTimer < 0.f)
        flyTimer = 0.f;
    if (flyTimer <= 0.f)

    {
        // 当前镜头高于地面基准，平滑下降
        if (myplayer.H > 1500.f)
        {
            // 回落速度，数值越大下落越快
            float fallSpeed = 8000.f;
            myplayer.H -= fallSpeed * dt;

            // 防止低于地面高度，封顶锁定
            if (myplayer.H < 1500.f)
                myplayer.H = 1500.f;
        }
    }
}
void GameCore::ResetFullGame(sf::Texture& obsCarTex, sf::Texture& nitroTex, sf::Texture& flyTex)
{
    gameOver = false;
    gameScore = 0;
    isNewRecord = false;
    gameTime = 0.f;
    nitroTimer = 0.f;
    flyTimer = 0.f;
    laneDeviateTimer = 0.f;  // 重置偏离计时
    hasNitroPending = false; // 清除pending
    hasFlyPending = false;
    waitingCrash = false;
    crashDelay = 0.f;
    myplayer.controlLock = false;
    myplayer.ResetPlayer(myplayer.pos);
    explosionList.clear(); // 清空爆炸动画
    obstacleList.clear();
    SpawnObstacles(obsCarTex);
    RefreshSpawnItems(nitroTex, flyTex, myplayer.pos);
    if (m_gameMusic)
    {
        m_gameMusic->stop();
        m_gameMusic->play();
    }
}

// 事件循环，无贴图
void GameCore::HandleEventLoop(sf::Texture& obsCarTex, sf::Texture& nitroTex, sf::Texture& flyTex)
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

                ResetFullGame(obsCarTex, nitroTex, flyTex);
            }
        }
    }
}

// 更新逻辑，仅playerCar作为外部精灵传入
void GameCore::UpdateGameLogic(sf::Sprite& playerCar, float dt, sf::Texture& obsCarTex, sf::Texture& explodeTex)
{
	bool wasGameOver = gameOver;
    UpdateItemTimer(dt);
    if (!gameOver && !myplayer.controlLock)
    {
        myplayer.HandleInput(dt, myplayer.pos, nitroTimer, flyTimer, hasNitroPending, hasFlyPending);
        gameTime += dt;
        if (myplayer.speed > 0)
        {
            float addScore = myplayer.speed * dt * 0.5f;
            gameScore += static_cast<int>(addScore);
        }
    }

    while (myplayer.pos >= N * segL)
        myplayer.pos -= N * segL;
    while (myplayer.pos < 0)
        myplayer.pos += N * segL;
    float absX = std::abs(myplayer.playerX);
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
        myplayer.controlLock = true;
        SpawnExplosion(myplayer.playerX, myplayer.pos, explodeTex);
    }

    else if (absX > LANE_WARN_LIMIT)
    {
        // 在1.0 ~ 1.3之间，累计偏离时间
        laneDeviateTimer += dt;
        // 持续超过5秒游戏结束
        if (laneDeviateTimer >= LANE_TIMEOUT_SECONDS)
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
    else
    {
        // 回到1.0以内，重置计时
        laneDeviateTimer = 0.f;
    }
    // 小车屏幕定位，playerCar外部传入
    sf::FloatRect carBounds = playerCar.getLocalBounds();
    float carX = (width - carBounds.size.x) / 2.f;
    float carY = height - carBounds.size.y;
    playerCar.setPosition(sf::Vector2f(carX, carY));

    if (waitingCrash)
    {
        crashDelay -= dt;
        if (crashDelay <= 0.f)
        {
            gameOver = true;
           
        }
    }

    hud.UpdateScore(gameScore);
    hud.UpdateTimer(gameTime);
    hud.SetGameOver(gameOver, isNewRecord);
    hud.UpdateBest(bestScore);
    hud.UpdateItemStatus(nitroTimer, flyTimer, hasNitroPending, hasFlyPending);
    hud.UpdateDeviateCountdown(laneDeviateTimer);
    // ---- 新增：检测 Tab 按键按下瞬间 ----
    bool tabPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Tab);
    if (tabPressed && !m_prevTabPressed && nitroTimer > 0.f && m_nitroMusic)
    {
        // 重头播放（如果已经在播放，先停止再播放）
        m_nitroMusic->stop();
        m_nitroMusic->play();
    }
    m_prevTabPressed = tabPressed;
    if (!wasGameOver && gameOver)
    {
        if (m_nitroMusic)
        {
			m_nitroMusic->stop();
        }
    }
}

// 渲染：全部贴图、精灵从参数传入，内部不持有
void GameCore::RenderScene(
    sf::Texture t[50],
    sf::Texture& bg,
    sf::Sprite& sBackground,
    sf::Sprite& playerCar,
    sf::Texture& obsCarTex,
    sf::Texture& nitroTex,
    sf::Texture& flyTex,
    sf::Texture& explodeTex,
    float dt)
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
        drawQuad(app, road, p.X, p.Y, p.W, l.X, l.Y, l.W);
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
        while (relZ < 0)
            relZ += totalRoadLength;
        while (relZ > totalRoadLength)
            relZ -= totalRoadLength;
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
            if (isCrash && !waitingCrash)
            {
                if (m_gameMusic)
                    m_nitroMusic->stop();
                myplayer.controlLock = true;
                SpawnExplosion(myplayer.playerX, myplayer.pos, explodeTex);
                waitingCrash = true;
                crashDelay = 1.0f; // 预留1秒播放爆炸，可自行改成0.8 / 1.2
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
        if (!iter->active)
            iter = obstacleList.erase(iter);
        else
            ++iter;
    }
    int playerSeg = myplayer.pos / segL;

    for (auto& item : itemList)
    {
        if (!item.active)
            continue;
        float relZ = item.zPos - myplayer.pos;
        while (relZ < 0)
            relZ += totalRoadLength;
        while (relZ > totalRoadLength)
            relZ -= totalRoadLength;

        int segIndex = static_cast<int>(relZ / segL) % N;
        Line& targetSeg = lines[segIndex];
        float curveX = 0.f, curveDx = 0.f;
        for (int n = playerSeg; n <= segIndex; n++)
        {
            int idx = n % N;
            curveDx += lines[idx].curve;
            curveX += curveDx;
        }
        targetSeg.project(myplayer.playerX * roadW - curveX, camH, myplayer.pos);
        item.render(app, targetSeg, camH, width, height, myplayer.pos);
        item.checkLifetime(myplayer.pos, totalRoadLength);
        if (!gameOver && CheckItemPickCollision(playerCar, item.itemSprite))
        {
            if (item.type == ITEM_NITRO)
            {
                hasNitroPending = true;
            }
            if (item.type == ITEM_FLY)
            {
                hasFlyPending = true;
            }
            item.active = false;
        }
    }

    // 清理跑出视野的道具
    for (auto iter = itemList.begin(); iter != itemList.end();)
    {
        if (!iter->active)
            iter = itemList.erase(iter);
        else
            ++iter;
    }

    app.draw(playerCar);
    UpdateDrawExplosion(dt, app, explodeTex);
    hud.Render(app);
    app.display();
    // 放在UpdateGameLogic最下方，无条件生成
}

void GameCore::SpawnExplosion(float trackX, float trackZ, sf::Texture& expTex)
{
    
    Explosion exp(trackX, trackZ, expTex, 1);
    explosionList.push_back(exp);
}

void GameCore::UpdateDrawExplosion(float dt, sf::RenderWindow& app, sf::Texture& expTex)
{
    int camH = lines[myplayer.pos / segL % N].y + myplayer.H;
    float camX = myplayer.playerX;
    int camZ = myplayer.pos;

    // 更新爆炸动画
    for (auto& exp : explosionList)
    {
        exp.Update(dt, camX, camH, camZ);
        exp.Project(camX, camH, camZ);
    }

    // 渲染顺序：碎片粒子 → 爆炸贴图（贴图在上层）
    for (auto& exp : explosionList)
        exp.Render(app);

    // 清除生命周期结束的爆zha
    explosionList.erase(std::remove_if(explosionList.begin(), explosionList.end(),
        [](Explosion& e)
        { return e.IsDead(); }),
        explosionList.end());
}
// 主循环：所有贴图从main传入，贴图变量全部留在main
void GameCore::Run(
    sf::Texture t[50],
    sf::Texture& bg,
    sf::Sprite& sBackground,
    sf::Texture& carTex,
    sf::Sprite& playerCar,
    sf::Texture& obsCarTex,
    sf::Texture& nitroTex,
    sf::Texture& flyTex,
    sf::Texture& explodeTex)
{
    if (m_gameMusic && m_gameMusic->getStatus() != sf::Music::Status::Playing)
    {
        m_gameMusic->play();
    }
    // 首次生成初始障碍（obsCarTex外部）
    SpawnObstacles(obsCarTex);
    RefreshSpawnItems(nitroTex, flyTex, myplayer.pos);

    while (app.isOpen())
    {
        float dt = gameClock.restart().asSeconds();
        HandleEventLoop(obsCarTex, nitroTex, flyTex);

        // 检测R键重置
        std::optional<sf::Event> tempEvt;
        while ((tempEvt = app.pollEvent()))
        {
            if (auto* key = tempEvt->getIf<sf::Event::KeyPressed>())
            {
                if (gameOver && key->code == sf::Keyboard::Key::R)
                {
                    ResetFullGame(obsCarTex, nitroTex, flyTex);
                }
            }
        }

        UpdateGameLogic(playerCar, dt, obsCarTex, explodeTex);
        RenderScene(t, bg, sBackground, playerCar, obsCarTex, nitroTex, flyTex, explodeTex, dt);
    }
}