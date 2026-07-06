#include "Menu.h"
#include "GameHUD.h"
#include <SFML/Graphics.hpp>
#include <optional>
#include <cmath>
#include <memory>
#include <utility>
using namespace sf;

// 全局常量
const float SAFE_LANE_LIMIT = 1.0f;// 玩家车道安全边界
const int OBSTACLE_SPAWN_STEP = 80;// 每多少段生成障碍车
const int MAX_OBSTACLE_COUNT = 8;// 同时存在最大障碍车数量

int width = 1024;
int height = 768;
int roadW = 2000;
int segL = 200;// segment length
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

struct Line
{
    float x, y, z;
    float X, Y, W;
    float curve, spriteX, clip, scale;
    int spriteId;

    Line()
    {
        spriteX = curve = x = y = z = 0.f;
        X = Y = W = clip = scale = 0.f;
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

        destX += destW * spriteX;
        destY += destH * (-1.f);

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
    float gameTime = 0.f;
    sf::Clock gameClock;

    // 路边贴图
    Texture t[50];
    for (int i = 1; i <= 7; i++)
    {
        t[i].loadFromFile("images/" + std::to_string(i) + ".png");
        t[i].setSmooth(true);
    }

    // 远景背景
    Texture bg;
    bg.loadFromFile("images/bg.png");
    bg.setRepeated(true);
    Sprite sBackground(bg);
    sf::IntRect rect(sf::Vector2i(0.f, 0.f), sf::Vector2i(5000.f, 411.f));
    sBackground.setTextureRect(rect);
    sBackground.setPosition(sf::Vector2f(-2000.f, 0.f));

    // 玩家小车
    Texture carTex;
    carTex.loadFromFile("images/mycar.png");
    carTex.setSmooth(true);
    Sprite playerCar(carTex);

    bool gameOver = false;
    std::vector<Line> lines;

    // 生成赛道段
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
            if (const auto* keyEvt = eventOpt->getIf<sf::Event::KeyPressed>())
            {
                if (gameOver && keyEvt->code == sf::Keyboard::Key::R)
                {
                    gameOver = false;
                    pos = 0;
                    playerX = 0.f;
                    gameScore = 0;
                    gameTime = 0.f;
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

        // 越界判定
        if (playerX < -SAFE_LANE_LIMIT || playerX > SAFE_LANE_LIMIT)
        {
            gameOver = true;
        }

        // 小车居中底部
        sf::FloatRect carBounds = playerCar.getLocalBounds();
        float carX = (width - carBounds.size.x) / 2.f;
        float carY = height - carBounds.size.y;
        playerCar.setPosition(sf::Vector2f(carX, carY));

        // 更新HUD数据（2026.7.6早更新）
        hud.UpdateScore(gameScore);
        hud.UpdateTimer(gameTime);
        hud.SetGameOver(gameOver);

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
            Line& p = lines[(n - 1) % N];
            drawQuad(app, grass, 0, p.Y, width, 0, l.Y, width);
            drawQuad(app, rumble, p.X, p.Y, p.W * 1.2, l.X, l.Y, l.W * 1.2);
            drawQuad(app, road, p.X, p.Y, p.W, l.X, l.Y, l.W);
        }
        for (int n = startPos + 300; n > startPos; n--)
            lines[n % N].drawSprite(app, t);

        app.draw(playerCar);
        hud.Render(app);
        app.display();
    }
    return 0;
}
