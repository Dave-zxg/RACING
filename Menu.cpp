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
    m_textQuit(nullptr),
    m_textInstructions(nullptr),
    m_textInstructionContent(nullptr),
    m_showInstructions(false)
{
    auto size = m_win.getSize();
    winW = static_cast<float>(size.x);
    winH = static_cast<float>(size.y);

    // 加载菜单背景图 1024*768 无文字红色赛车
    std::optional<bool> bgLoad = m_bgTex.loadFromFile("E:/vs/2025/小学期1/racing/images/menu_bg.png");
    if (!bgLoad.has_value() || !bgLoad.value())
    {
        MessageBoxW(NULL, L"pictures/menu_bg.png 缺失，请把res文件夹放到exe同级目录", L"资源错误", MB_OK | MB_ICONERROR);
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
    if (!m_font.openFromFile("E:/vs/2025/小学期1/racing/images/font.ttf"))
    {
        // 如果加载失败，弹窗提示（可选）
        MessageBoxW(NULL, L"自定义字体加载失败，请检查 E: / vs / 2025 / 小学期1 / racing / images / font.ttf 是否存在", L"字体错误", MB_OK | MB_ICONERROR);
        exit(-1);
    }

    // 创建菜单文字按钮
    m_textStart = std::make_unique<sf::Text>(m_font, L"Start Game", 100);
    m_textQuit = std::make_unique<sf::Text>(m_font, L"Quit", 100);
    m_textInstructions = std::make_unique<sf::Text>(m_font, L"Instructions", 100);
    
    // 开始按钮居中
    auto startRect = m_textStart->getLocalBounds();
    m_textStart->setOrigin(sf::Vector2f{ startRect.size.x / 2.f, startRect.size.y / 2.f });
    m_textStart->setPosition(sf::Vector2f{ winW / 2.f, winH / 2.f - 120.f });
    m_textStart->setFillColor(sf::Color::White);

    // 退出按钮居中
    auto quitRect = m_textQuit->getLocalBounds();
    m_textQuit->setOrigin(sf::Vector2f{ quitRect.size.x / 2.f, quitRect.size.y / 2.f });
    m_textQuit->setPosition(sf::Vector2f{ winW / 2.f, winH / 2.f });
    m_textQuit->setFillColor(sf::Color(180, 180, 180));

    // 说明按钮（在退出下方）
    auto instRect = m_textInstructions->getLocalBounds();
    m_textInstructions->setOrigin(sf::Vector2f{ instRect.size.x / 2.f, instRect.size.y / 2.f });
    m_textInstructions->setPosition(sf::Vector2f{ winW / 2.f, winH / 2.f + 120.f });
    m_textInstructions->setFillColor(sf::Color(180, 180, 180));

    // 创建说明内容（多行文字）
    m_textInstructionContent = std::make_unique<sf::Text>(m_font,
        L"Game Instructions:\n"
        L"Arrow keys: Move\n"
        L"Up: Speed up\n"
        L"Down: Slow down\n"
        L"Tab: Use Nitro (if collected)\n"
        L"W: Fly (if collected)\n"
        L"Avoid obstacles and stay in lane!\n"
        L"Press any key to return.",
        50);
    m_textInstructionContent->setOrigin(sf::Vector2f{
        m_textInstructionContent->getLocalBounds().size.x / 2.f,
        m_textInstructionContent->getLocalBounds().size.y / 2.f
        });
    m_textInstructionContent->setPosition(sf::Vector2f{ winW / 2.f, winH / 2.f });
    m_textInstructionContent->setFillColor(sf::Color::White);
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
                if (m_showInstructions)
                {
                    // 说明界面：按任意键返回菜单
                    m_showInstructions = false;
                    m_selected = 2; // 光标停在“说明”上
                    continue;
                }
                if (key->code == sf::Keyboard::Key::Up)
                    m_selected = (m_selected - 1 + 3) % 3;
                if (key->code == sf::Keyboard::Key::Down)
                    m_selected = (m_selected + 1) % 3;
                if (key->code == sf::Keyboard::Key::Enter)
                {
                    if (m_selected == 0)
                        return MENU_START_GAME;
                    else if (m_selected == 1)
                        return MENU_QUIT;
                    else if (m_selected == 2)
                    {
                        // 进入说明界面
                        m_showInstructions = true;
                    }
                }
            }
        }

        // 统一设置高亮颜色（先全部置灰，再高亮当前选中）
        m_textStart->setFillColor(sf::Color(100, 100, 100));
        m_textQuit->setFillColor(sf::Color(100, 100, 100));
        m_textInstructions->setFillColor(sf::Color(100, 100, 100));
        if (m_selected == 0)
            m_textStart->setFillColor(sf::Color::White);
        else if (m_selected == 1)
            m_textQuit->setFillColor(sf::Color::White);
        else if (m_selected == 2)
            m_textInstructions->setFillColor(sf::Color::White);

        // 渲染菜单画面
        m_win.clear(sf::Color(8, 8, 20));
        m_win.draw(*m_bgSprite);
        if (m_showInstructions)
        {
            // 显示说明内容
            m_win.draw(*m_textInstructionContent);
        }
        else
        {
            // 显示菜单按钮
            m_win.draw(*m_textStart);
            m_win.draw(*m_textQuit);
            m_win.draw(*m_textInstructions);
        }
        m_win.display();
    }
    return MENU_QUIT;
}
