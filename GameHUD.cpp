#include "GameHUD.h"
#include <sstream>
#include <memory>
#include <iomanip>

GameHUD::GameHUD()
    : m_fontValid(false),
    m_scoreText(nullptr),
    m_timerText(nullptr),
    m_gameOverText(nullptr)
{
    // 字体加载兜底
    if (m_hudFont.openFromFile("images/font.ttf"))
        m_fontValid = true;
    else if (m_hudFont.openFromFile("C:/Windows/Fonts/msyh.ttc"))
        m_fontValid = true;
    else if (m_hudFont.openFromFile("C:/Windows/Fonts/simsun.ttc"))
        m_fontValid = true;

    if (!m_fontValid) return;

    // 分数文本
    m_scoreText = std::make_unique<sf::Text>(m_hudFont);
    m_scoreText->setCharacterSize(28);
    m_scoreText->setFillColor(sf::Color::Black);
    m_scoreText->setPosition(sf::Vector2f{ 10.f, 10.f });
    UpdateScore(0);

    // 计时文本（黑色）
    m_timerText = std::make_unique<sf::Text>(m_hudFont);
    m_timerText->setCharacterSize(28);
    m_timerText->setFillColor(sf::Color::Black);
    m_timerText->setPosition(sf::Vector2f{ static_cast<float>(width) - 160.f, 10.f });
    UpdateTimer(0.f);

    // GameOver文字
    m_gameOverText = std::make_unique<sf::Text>(m_hudFont);
    m_gameOverText->setCharacterSize(60);
    m_gameOverText->setFillColor(sf::Color::Red);
    m_gameOverText->setString("GAME OVER!\nPress R to Restart");
    sf::FloatRect textBounds = m_gameOverText->getLocalBounds();
    m_gameOverText->setOrigin(sf::Vector2f{ textBounds.size.x / 2.f, textBounds.size.y / 2.f });
    m_gameOverText->setPosition(sf::Vector2f{ width / 2.f, height / 2.f });
}

void GameHUD::UpdateScore(int score)
{
    if (!m_fontValid) return;
    std::stringstream ss;
    ss << "Score: " << score;
    m_scoreText->setString(ss.str());
}

void GameHUD::UpdateTimer(float second)
{
    if (!m_fontValid) return;
    std::stringstream ss;
    ss << std::fixed;
    ss.precision(1);
    ss << "Time: " << second;
    m_timerText->setString(ss.str());
}

void GameHUD::SetTimerColor(sf::Color color)
{
    if (!m_fontValid) return;
    m_timerText->setFillColor(color);
}

void GameHUD::SetGameOver(bool isOver)
{
    if (!m_fontValid) return;
    if (isOver)
        m_gameOverText->setFillColor(sf::Color::Red);
    else
        m_gameOverText->setFillColor(sf::Color::Transparent);
}

void GameHUD::Render(sf::RenderWindow& win)
{
    if (!m_fontValid) return;
    win.draw(*m_scoreText);
    win.draw(*m_timerText);
    win.draw(*m_gameOverText);
}