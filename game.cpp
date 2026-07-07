#include "GameHUD.h"
#include "Menu.h"
#include <SFML/Graphics.hpp>
#include <optional>
#include <cmath>
#include <memory>
#include <utility>

using namespace sf;

// 全局常量（插入在using namespace sf; 下方）
const float SAFE_LANE_LIMIT = 1.5f;    // 玩家车道安全边界
const int OBSTACLE_SPAWN_STEP = 250;     // 每多少段生成障碍车
const int MAX_OBSTACLE_COUNT = 8;       // 同时存在最大障碍车数量

int width = 1024;
int height = 768;
int roadW = 2000;
int segL = 200;    // segment length
float camD = 0.84; // camera depth

void drawQuad(RenderWindow& w, Color c, int x1, int y1, int w1, int x2, int y2, int w2)
{
    ConvexShape shape(4);
    shape.setFillColor(c);
    shape.setPoint(0, Vector2f(x1 - w1, y1));
    shape.setPoint(1, Vector2f(x2 - w2, y2));
    shape.setPoint(2, Vector2f(x2 + w2, y2));
    shape.setPoint(3, Vector2f(x1 + w1, y1));
    w.draw(shape);
}

// 碰撞检测：两个精灵包围盒相交返回true
bool CheckSpriteCollision(const sf::Sprite& sprPlayer, const sf::Sprite& sprObs)
{
    // 获取两个车的屏幕包围矩形
    sf::FloatRect playerBox = sprPlayer.getGlobalBounds();
    sf::FloatRect obsBox = sprObs.getGlobalBounds();

    // 缩小碰撞盒，剔除图片空白边缘，避免误撞 SFML3.1写法
    playerBox.position.x += 70;
    playerBox.position.y += 70;
    playerBox.size.x -= 100;
    playerBox.size.y -= 100;

    obsBox.position.x += 50;
    obsBox.position.y += 50;
    obsBox.size.x -= 80;
    obsBox.size.y -= 80;

    // 判断矩形是否重叠
     // ==========手动实现矩形相交判断==========
    float pLeft = playerBox.position.x;
    float pTop = playerBox.position.y;
    float pRight = playerBox.position.x + playerBox.size.x;
    float pBottom = playerBox.position.y + playerBox.size.y;

    float oLeft = obsBox.position.x;
    float oTop = obsBox.position.y;
    float oRight = obsBox.position.x + obsBox.size.x;
    float oBottom = obsBox.position.y + obsBox.size.y;

    // AABB碰撞规则：不满足任意一条无重叠，则判定碰撞
    bool noOverlap = (pRight < oLeft) || (pLeft > oRight) || (pBottom < oTop) || (pTop > oBottom);
    if (noOverlap) return false;

    // 计算重叠宽度、重叠高度，必须重叠达到一定尺寸才算相撞
    float overlapX = std::min(pRight, oRight) - std::max(pLeft, oLeft);
    float overlapY = std::min(pBottom, oBottom) - std::max(pTop, oTop);
    // 横向、纵向重叠都超过15像素才判定碰撞
    return (overlapX > 100.f) && (overlapY > 100.f);
}

struct Line
{
    float x, y, z; // 3d center of line
    float X, Y, W; // screen coord
    float curve, spriteX, clip, scale;
    int spriteId;

    Line()
    {
        spriteX = curve = x = y = z = 0.f;
        spriteId = 0;
    }

    void project(int camX, int camY, int camZ)
    {
        scale = camD / (z - camZ);
        X = (1 + scale * (x - camX)) * width / 2.f;
        Y = (1 - scale * (y - camY)) * height / 2.f;
        W = scale * roadW * width / 2.f;
    }

    void drawSprite(RenderWindow& app, Texture texArr[50])
    {
        if (spriteId == 0)
            return;
        sf::Sprite s(texArr[spriteId]);
        sf::IntRect texRect = s.getTextureRect();
        int w = static_cast<int>(texRect.size.x);
        int h = static_cast<int>(texRect.size.y);

        float destX = X + scale * spriteX * width / 2.f;
        float destY = Y + 4.f;
        float destW = w * W / 266.f;
        float destH = h * W / 266.f;

        destX += destW * spriteX; // offsetX
        destY += destH * (-1.f);  // offsetY

        float clipH = destY + destH - clip;
        if (clipH < 0.f)
            clipH = 0.f;

        if (clipH >= destH)
            return;
        sf::IntRect rect(sf::Vector2i(0, 0), sf::Vector2i(w, static_cast<int>(h - h * clipH / destH)));
        s.setTextureRect(rect);
        s.setScale(sf::Vector2f(destW / float(w), destH / float(h)));
        s.setPosition(sf::Vector2f(destX, destY));
        app.draw(s);
    }
};

//玩家小车类（封装玩家位置、出界判定）
class PlayerCar
{

};

//障碍物类（障碍小车和其他障碍物的出现和消失、玩家碰撞判定）
class obstacle
{
public:
    // 赛道逻辑坐标
    float zPos;        // 障碍物在赛道的深度z
    float laneOffset;  // 横向车道偏移（和playerX同单位）
    bool active;       // 是否有效，失效后删除
    sf::Sprite obsSprite;

    // 构造函数：传入障碍物贴图
    obstacle(const sf::Texture& tex) :obsSprite(tex)
    {
        zPos = 0.f;
        laneOffset = 0.f;
        active = false;
    }

    // 设置障碍物初始赛道位置
    void setTrackPos(float z, float offset)
    {
        zPos = z;
        laneOffset = offset;
        active = true;
    }

    // 复用赛道透视投影，计算并渲染障碍物
    void render(sf::RenderWindow& app, Line& seg, int camH, int winW, int winH, float campos)
    {
        if (seg.scale <= 0.f)
        {
            return;
        }

        sf::FloatRect texBounds = obsSprite.getLocalBounds();
        float w = texBounds.size.x;
        float h = texBounds.size.y;
        // 和路边道具缩放公式完全一致，不会变形变窄
        float destW = std::abs(w * seg.W / 266.f);
        float destH = std::abs(h * seg.W / 266.f);
        // 横向偏移算法照搬drawSprite
        float screenX = seg.X + seg.scale * laneOffset * winW / 2.f - destW / 2.f;
        float screenY = seg.Y + 4.f;


        // 小型裁剪：如果完全在屏幕外则不绘制
        if (screenY + destH < 0.f) return; // 完全在上方
        if (screenY > winH) return;        // 完全在下方

        obsSprite.setScale(sf::Vector2f(destW / w, destH / h));
        obsSprite.setPosition(sf::Vector2f(screenX, screenY));

        // 简易裁切，和道路渲染统一
         // 简易裁切，和树木对齐



        if (screenY < winH)
        {
            app.draw(obsSprite);
        }
    }

    // 判断障碍物是否跑到玩家身后，标记失效
    void checkLifetime(int playerPosZ, int totalRoadLength)
    {
        float relativeZ = zPos - playerPosZ;
        // 障碍落后玩家完整一圈才标记失效，中途永久存在路面
        if (relativeZ < -totalRoadLength)
        {
            active = false;
        }
    }
};

//游戏HUD类（显示速度、分数、时间等信息）


//特效类（爆炸、烟雾、加速等特效的实现和管理）
class SpeedParticle
{
};

//游戏主类，将窗口、循环、事件处理、渲染等整合在一起
class GameCore
{
};

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

    // 初始化HUD
    GameHUD hud;
    int gameScore = 0;
    int bestScore = 0; // 新增：历史最高分
    float gameTime = 0.f;
    bool isNewRecord = false; // 新增：破纪录标记
    sf::Clock gameClock;


    //路边贴图
    Texture t[50];
    for (int i = 1; i <= 7; i++)
    {
        bool loadOk = t[i].loadFromFile("images/" + std::to_string(i) + ".png");
        t[i].setSmooth(true);
    }

    //远景背景
    Texture bg;
    bool bgOk = bg.loadFromFile("images/bg.png");
    bg.setRepeated(true);
    Sprite sBackground(bg);
    sf::IntRect rect(sf::Vector2i(0.f, 0.f), sf::Vector2i(5000.f, 411.f));
    sBackground.setTextureRect(rect);
    sBackground.setPosition(sf::Vector2f(-2000.f, 0.f));

    // 新增：玩家小车
    Texture carTex;
    bool carLoadOk = carTex.loadFromFile("images/mycar.png");
    carTex.setSmooth(true);
    Sprite playerCar(carTex);

    // 新增：障碍物小车贴图资源
    sf::Texture obsCarTex;
    bool obsTexLoad = obsCarTex.loadFromFile("images/othercar.png");
    obsCarTex.setSmooth(true);
    // 障碍物管理容器，存储所有激活障碍车
    std::vector<obstacle> obstacleList;

    // 游戏失败状态
    bool gameOver = false;


    std::vector<Line> lines;


    //生成赛道线段
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
            line.spriteId = 6;
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

    int N = lines.size();

    // 一次性在赛道固定坐标生成障碍，永久附着路面
    for (int i = 200; i < N; i += OBSTACLE_SPAWN_STEP)
    {
        obstacle newObs(obsCarTex);
        float fixedZ = i * segL; // 固定全局深度，不会随pos变化
        int laneIndex = i % 3;
        float offset = 0.f;
        if (laneIndex == 0) offset = -0.8f;
        else if (laneIndex == 1) offset = 0.8f;
        newObs.setTrackPos(fixedZ, offset);
        obstacleList.push_back(newObs);
    }

    float playerX = 0.f;
    int pos = 0.f;
    int H = 1500.f;

    while (app.isOpen())
    {
        float dt = gameClock.restart().asSeconds();
        std::optional<sf::Event> eventOpt;
        while ((eventOpt = app.pollEvent()))
        {
            auto& e = *eventOpt;
            if (e.is<sf::Event::Closed>())
            {
                app.close();
            }

            // 失败后按R重置游戏
           // 检测按键按下事件，游戏结束按R重置
            if (const auto* keyEvt = eventOpt->getIf<sf::Event::KeyPressed>())
            {
                if (gameOver && keyEvt->code == sf::Keyboard::Key::R)
                {
                    gameOver = false;
                    pos = 0;
                    playerX = 0.f;
                    gameScore = 0;
                    gameTime = 0.f;
                    isNewRecord = false; // 新增重置破纪录标记
                    obstacleList.clear();
                    // 一次性在赛道固定坐标生成障碍，永久附着路面
                    for (int i = 200; i < N; i += OBSTACLE_SPAWN_STEP)
                    {
                        obstacle newObs(obsCarTex);
                        float fixedZ = i * segL; // 固定全局深度，不会随pos变化
                        int laneIndex = i % 3;
                        float offset = 0.f;
                        if (laneIndex == 0) offset = -0.8f;
                        else if (laneIndex == 1) offset = 0.8f;
                        newObs.setTrackPos(fixedZ, offset);
                        obstacleList.push_back(newObs);
                    }
                }
            }

        }
        int speed = 0;
        if (!gameOver)
        {
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
            gameTime += dt;
            if (speed > 0)
            {
                float addScore = speed * dt * 0.5f;
                gameScore += static_cast<int>(addScore);
            }
        }
        while (pos >= N * segL)
            pos -= N * segL;
        while (pos < 0)
            pos += N * segL;

        // 车道越界失败判定
        // 车道越界失败判定
        if ((playerX < -SAFE_LANE_LIMIT || playerX > SAFE_LANE_LIMIT) && !gameOver)
        {
            gameOver = true;
            // 对比最高分，判断是否破纪录
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


        // 新增：固定小车在窗口底部居中
        sf::FloatRect carBounds = playerCar.getLocalBounds();
        float carX = (width - carBounds.size.x) / 2.f;
        float carY = height - carBounds.size.y; // 贴窗口最底边
        playerCar.setPosition(sf::Vector2f(carX, carY));

        // 更新HUD数据（2026.7.6早更新）
               // 更新HUD数据（新增最高分、破纪录标记）
        hud.UpdateScore(gameScore);
        hud.UpdateBest(bestScore); // 刷新最高分文字
        hud.UpdateTimer(gameTime);
        hud.SetGameOver(gameOver, isNewRecord); // 传入破纪录标记

        app.clear(Color(105, 205, 4));
        app.draw(sBackground);
        int startPos = pos / segL;
        int camH = lines[startPos].y + H;
        if (speed > 0)
        {
            float offsetX = -lines[startPos].curve * 2.f;
            sBackground.move(sf::Vector2f(offsetX, 0.f));
        }
        if (speed < 0)
        {
            float offsetX = lines[startPos].curve * 2.f;
            sBackground.move(sf::Vector2f(offsetX, 0.f));
        }

        int maxy = height;
        float x = 0, dx = 0;

        ///////draw road////////
        for (int n = startPos; n < startPos + 300; n++)
        {
            Line& l = lines[n % N];
            l.project(playerX * roadW - x, camH, startPos * segL - (n >= N ? N * segL : 0));
            x += dx;
            dx += l.curve;

            l.clip = maxy;
            if (l.Y >= maxy)
                continue;
            maxy = l.Y;

            Color grass = (n / 3) % 2 ? Color(16, 200, 16) : Color(0, 154, 0);
            Color rumble = (n / 3) % 2 ? Color(255, 255, 255) : Color(0, 0, 0);
            Color road = (n / 3) % 2 ? Color(107, 107, 107) : Color(105, 105, 105);

            Line& p = lines[(n - 1) % N]; // previous line

            drawQuad(app, grass, 0, p.Y, width, 0, l.Y, width);
            drawQuad(app, rumble, p.X, p.Y, p.W * 1.2, l.X, l.Y, l.W * 1.2);
            drawQuad(app, road, p.X, p.Y, p.W, l.X, l.Y, l.W);
        }

        ////////draw objects////////
        for (int n = startPos + 300; n > startPos; n--)
            lines[n % N].drawSprite(app, t);

        // 新增：渲染所有赛道障碍物
        int totalRoadLength = N * segL;
        for (auto& obs : obstacleList)
        {
            if (!obs.active)
            {
                printf("跳过：障碍车已失效\n");
                continue;
            }

            // 计算障碍物相对玩家的深度，循环赛道取对应分段
            float relZ = obs.zPos - pos;
            while (relZ < 0) relZ += totalRoadLength;
            while (relZ > totalRoadLength) relZ -= totalRoadLength;

            int segIndex = static_cast<int>(relZ / segL) % N;
            Line& targetSeg = lines[segIndex];
            int currentCamH = lines[startPos].y + H;


            // 复刻路面的x/dx曲线累加逻辑：算出目标分段的总弯道偏移
            float curveX = 0.f;
            float curveDx = 0.f;

            // 从当前视野起点，遍历到障碍所在分段，累加所有curve
            for (int n = startPos; n <= segIndex; n++)
            {
                int idx = n % N;
                curveDx += lines[idx].curve;
                curveX += curveDx;
            }

            // 重点：project第一个参数减去累计弯道偏移curveX，和路面 l.project(playerX * roadW - x,...) 完全对齐
            float segZ = segIndex * segL;
            targetSeg.project(playerX * roadW - curveX, currentCamH, pos);
            obs.render(app, targetSeg, currentCamH, width, height, pos);

            // 碰撞判断：游戏未结束时才检测
            if (!gameOver)
            {
                bool isCrash = CheckSpriteCollision(playerCar, obs.obsSprite);
                if (isCrash && !gameOver)
                {
                    gameOver = true; // 碰撞触发，游戏结束
                    // 对比最高分，判断是否破纪录
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

            // 检测生命周期，超出视野标记失效
            obs.checkLifetime(pos, totalRoadLength);
        }
        // 清理失效障碍物
        for (auto iter = obstacleList.begin(); iter != obstacleList.end();)
        {
            if (!iter->active)
                iter = obstacleList.erase(iter);
            else
                ++iter;
        }

        // ===== 新增：渲染底部小车 =====
        app.draw(playerCar);
        hud.Render(app);
        app.display();

    }
    return 0;
}
