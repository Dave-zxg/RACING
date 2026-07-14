#ifndef MENU_H
#define MENU_H
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

class GameMenu
{
public:
    enum MenuState
    {
        MENU_QUIT,
        MENU_START_GAME,
        MENU_INSTRUCTIONS
    };

    explicit GameMenu(sf::RenderWindow& win);
    MenuState RunMenu();

private:
    sf::RenderWindow& m_win;

    sf::Texture m_bgTex;
    std::unique_ptr<sf::Sprite> m_bgSprite;

    sf::Font m_font;
    std::unique_ptr<sf::Text> m_textStart;
    std::unique_ptr<sf::Text> m_textQuit;
    std::unique_ptr<sf::Text> m_textInstructions; // 新增：游戏说明按钮
    std::unique_ptr<sf::Text> m_textInstructionContent; // 新增：说明内容

    int m_selected;
    float winW, winH;
    bool m_showInstructions;
};
#endif
