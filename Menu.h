#ifndef MENU_H
#define MENU_H
#include <SFML/Graphics.hpp>
#include <memory>

class GameMenu
{
public:
    enum MenuState
    {
        MENU_QUIT,
        MENU_START_GAME
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

    int m_selected;
    float winW;
    float winH;
};
#endif