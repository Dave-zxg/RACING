#ifndef GAMEHUD_H
#define GAMEHUD_H
#include <SFML/Graphics.hpp>
#include <string>
#include <memory>

extern int width;
extern int height;

class GameHUD
{
public:
    GameHUD();
    void UpdateScore(int score);
    void UpdateTimer(float second);
    void SetGameOver(bool isOver);
    void SetTimerColor(sf::Color color);
    void Render(sf::RenderWindow& win);

private:
    sf::Font m_hudFont;
    bool m_fontValid;

    std::unique_ptr<sf::Text> m_scoreText;
    std::unique_ptr<sf::Text> m_timerText;
    std::unique_ptr<sf::Text> m_gameOverText;
};
#endif