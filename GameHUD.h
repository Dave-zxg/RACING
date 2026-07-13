#ifndef GAMEHUD_H
#define GAMEHUD_H
#include <SFML/Graphics.hpp>
#include <string>
#include <memory>

#include "Config.h"

class GameHUD
{
public:
    GameHUD();
    void UpdateScore(int score);
    void UpdateBest(int bestScore); // 新增
    void UpdateTimer(float second);
    void UpdateItemStatus(float nitro, float fly, bool nitroPending, bool flyPending);
    void SetGameOver(bool isOver, bool newRecord); // 修改参数
    void Render(sf::RenderWindow& win);

private:
    sf::Font m_hudFont;
    bool m_fontValid;

    std::unique_ptr<sf::Text> m_scoreText;
    std::unique_ptr<sf::Text> m_bestText; // 新增最高分文字
    std::unique_ptr<sf::Text> m_timerText;
    std::unique_ptr<sf::Text> m_gameOverText;
    std::unique_ptr<sf::Text> m_nitroText;
    std::unique_ptr<sf::Text> m_flyText;
};
#endif