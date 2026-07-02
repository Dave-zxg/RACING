#include "Menu.h"
#include <windows.h>
#include <optional>
#include <algorithm>

GameMenu::GameMenu(sf::RenderWindow& win)
    : m_win(win),
    m_selected(0),
    winW(0.f),
    winH(0.f),
    m_bgSprite(nullptr),
    m_textStart(nullptr),
    m_textQuit(nullptr)
{
    auto size = m_win.getSize();
    winW = static_cast<float>(size.x);
    winH = static_cast<float>(size.y);

    // 加载菜单背景图 1024*768 无文字红色赛车
    std::optional<bool> bgLoad = m_bgTex.loadFromFile("res/menu_bg.png");
    if (!bgLoad.has_value() || !bgLoad.value())
    {
        MessageBoxW(NULL, L"res/menu_bg.png 缺失，请把res文件夹放到exe同级目录", L"资源错误", MB_OK | MB_ICONERROR);
        exit(-1);
    }
    m_bgSprite = std::make_unique<sf::Sprite>(m_bgTex);

    // 等比例适配窗口居中
    auto texSize = m_bgTex.getSize();
    float scaleX = winW / static_cast<float>(texSize.x);
    float scaleY = winH / static_cast<float>(texSize.y);
    float scale = std::min(scaleX, scaleY);
    m_bgSprite->setScale(sf::Vector2f{ scale, scale });
    m_bgSprite->setOrigin(sf::Vector2f{ texSize.x / 2.f, texSize.y / 2.f });
    m_bgSprite->setPosition(sf::Vector2f{ winW / 2.f, winH / 2.f });

    // 加载系统字体兜底
    std::optional<bool> fontLoad = m_font.openFromFile("C:/Windows/Fonts/simsun.ttc");
    if (!fontLoad.has_value() || !fontLoad.value())
    {
        fontLoad = m_font.openFromFile("C:/Windows/Fonts/msyh.ttc");
        if (!fontLoad.has_value() || !fontLoad.value())
        {
            MessageBoxW(NULL, L"宋体、微软雅黑字体加载失败", L"字体错误", MB_OK | MB_ICONERROR);
            exit(-1);
        }
    }

    // 创建菜单文字按钮
    m_textStart = std::make_unique<sf::Text>(m_font, L"开始赛车游戏", 52);
    m_textQuit = std::make_unique<sf::Text>(m_font, L"退出游戏", 52);

    // 开始按钮居中
    auto startRect = m_textStart->getLocalBounds();
    m_textStart->setOrigin(sf::Vector2f{ startRect.size.x / 2.f, startRect.size.y / 2.f });
    m_textStart->setPosition(sf::Vector2f{ winW / 2.f, winH / 2.f - 70.f });
    m_textStart->setFillColor(sf::Color::White);

    // 退出按钮居中
    auto quitRect = m_textQuit->getLocalBounds();
    m_textQuit->setOrigin(sf::Vector2f{ quitRect.size.x / 2.f, quitRect.size.y / 2.f });
    m_textQuit->setPosition(sf::Vector2f{ winW / 2.f, winH / 2.f + 70.f });
    m_textQuit->setFillColor(sf::Color(180, 180, 180));
}

GameMenu::MenuState GameMenu::RunMenu()
{
    sf::Clock clk;
    while (m_win.isOpen())
    {
        clk.restart();

        // SFML3.1 标准事件循环
        std::optional<sf::Event> evtOpt;
        while ((evtOpt = m_win.pollEvent()))
        {
            const auto& evt = evtOpt.value();
            if (evt.is<sf::Event::Closed>())
                return MENU_QUIT;

            if (const auto* key = evt.getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Up)
                    m_selected = (m_selected - 1 + 2) % 2;
                if (key->code == sf::Keyboard::Key::Down)
                    m_selected = (m_selected + 1) % 2;
                if (key->code == sf::Keyboard::Key::Enter)
                {
                    if (m_selected == 0)
                        return MENU_START_GAME;
                    else
                        return MENU_QUIT;
                }
            }
        }

        // 切换高亮颜色
        if (m_selected == 0)
        {
            m_textStart->setFillColor(sf::Color::White);
            m_textQuit->setFillColor(sf::Color(100, 100, 100));
        }
        else
        {
            m_textStart->setFillColor(sf::Color(100, 100, 100));
            m_textQuit->setFillColor(sf::Color::White);
        }

        // 渲染菜单画面
        m_win.clear(sf::Color(8, 8, 20));
        m_win.draw(*m_bgSprite);
        m_win.draw(*m_textStart);
        m_win.draw(*m_textQuit);
        m_win.display();
    }
    return MENU_QUIT;
}