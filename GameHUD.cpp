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

    // 最高纪录UI（分数下方）新增
    m_bestText = std::make_unique<sf::Text>(m_hudFont);
    m_bestText->setCharacterSize(28);
    m_bestText->setFillColor(sf::Color::Black);
    m_bestText->setPosition(sf::Vector2f{ 10.f, 40.f });
    UpdateBest(0);

    // 计时文本（黑色）
    m_timerText = std::make_unique<sf::Text>(m_hudFont);
    m_timerText->setCharacterSize(32);
    m_timerText->setFillColor(sf::Color::Black);
    m_timerText->setPosition(sf::Vector2f{ static_cast<float>(width) - 160.f, 10.f });
    UpdateTimer(0.f);

    // 氮气道具文字
    m_nitroText = std::make_unique<sf::Text>(m_hudFont);
    m_nitroText->setCharacterSize(28);
    m_nitroText->setFillColor(sf::Color::Blue);
    m_nitroText->setPosition(sf::Vector2f{ 10.f, 70.f });

    // 飞行道具文字
    m_flyText = std::make_unique<sf::Text>(m_hudFont);
    m_flyText->setCharacterSize(32);
    m_flyText->setFillColor(sf::Color(200, 0, 200));
    m_flyText->setPosition(sf::Vector2f{ 10.f, 100.f });

    // GameOver文字
    m_gameOverText = std::make_unique<sf::Text>(m_hudFont);
    m_gameOverText->setCharacterSize(75);
    m_gameOverText->setFillColor(sf::Color::Red);
    m_gameOverText->setString("GAME OVER!\nPress R to Restart");
    sf::FloatRect textBounds = m_gameOverText->getLocalBounds();
    m_gameOverText->setOrigin(sf::Vector2f{ textBounds.size.x / 2.f, textBounds.size.y / 2.f });
    m_gameOverText->setPosition(sf::Vector2f{ width / 2.f, height / 2.f });

    // ========== 车道偏离倒计时文字 ==========
    m_deviateText = std::make_unique<sf::Text>(m_hudFont);
    m_deviateText->setCharacterSize(30);
    m_deviateText->setFillColor(sf::Color::Red);
    m_deviateText->setPosition(sf::Vector2f{ 10.f, 130.f });
    // 默认透明不显示
    m_deviateText->setFillColor(sf::Color::Transparent);
}

void GameHUD::UpdateScore(int score)
{
    if (!m_fontValid) return;
    std::stringstream ss;
    ss << "Score: " << score;
    m_scoreText->setString(ss.str());
}

//新增
void GameHUD::UpdateBest(int bestScore)
{
    if (!m_fontValid) return;
    std::stringstream ss;
    ss << "Best: " << bestScore;
    m_bestText->setString(ss.str());
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

void GameHUD::UpdateItemStatus(float nitro, float fly, bool nitroPending, bool flyPending)
{
    if (!m_fontValid) return;
    std::stringstream ss;
    // 更新氮气显示
    ss.str("");
    ss.clear();
    if (nitro > 0.f)
    {
        ss << "Nitro: " << std::fixed << std::setprecision(1) << nitro;
    }
    else
    {
        ss << "Nitro: Unavailable";
    }
    m_nitroText->setString(ss.str());

    // 更新飞行显示
    ss.str("");
    ss.clear(); 
    if (fly > 0.f)
    {
        ss << "Fly: " << std::fixed << std::setprecision(1) << fly;
    }
    else
    {
        ss << "Fly: Unavailable";
    }
    m_flyText->setString(ss.str());
}

//void GameHUD::SetTimerColor(sf::Color color)
//{
//    if (!m_fontValid) return;
//    m_timerText->setFillColor(color);
//}

//调整
void GameHUD::SetGameOver(bool isOver, bool newRecord)
{
    if (!m_fontValid) return;
    if (isOver)
    {
        if (newRecord)
            m_gameOverText->setString("NEW RECORD!\nGAME OVER!\nPress R to Restart");
        else
            m_gameOverText->setString("GAME OVER!\nPress R to Restart");
        m_gameOverText->setFillColor(sf::Color::Red);
    }
    else
    {
        m_gameOverText->setFillColor(sf::Color::Transparent);
    }
}

void GameHUD::UpdateDeviateCountdown(float devTime)
{
    if (!m_fontValid) return;
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1);
    // 剩余危险时间 = 总超时5秒 - 当前累计偏离时间
    float remain = LANE_TIMEOUT_SECONDS - devTime;
    if (remain < 0.f) remain = 0.f;
    ss << "Out of lane! Remain:" << remain << "s";
    m_deviateText->setString(ss.str());
    // devTime>0 显示红色文字，否则透明隐藏
    if (devTime > 0.01f)
        m_deviateText->setFillColor(sf::Color::Red);
    else
        m_deviateText->setFillColor(sf::Color::Transparent);
}


void GameHUD::Render(sf::RenderWindow& win)
{
    if (!m_fontValid) return;
    win.draw(*m_scoreText);
    win.draw(*m_bestText); // 新增
    win.draw(*m_timerText);
    win.draw(*m_gameOverText);

    // 新增绘制道具状态文字
    win.draw(*m_nitroText);
    win.draw(*m_flyText);
    win.draw(*m_deviateText);
}