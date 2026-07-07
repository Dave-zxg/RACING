#include "SpeedParticle.h"
#include <SFML/Graphics.hpp>
#include <algorithm>

SpeedParticle::SpeedParticle()
{
    // 父类GameObject坐标初始化
    x = y = z = 0.f;
    screenX = screenY = screenW = 0.f;
    scale = 1.f;
    texId = 0;
}

void SpeedParticle::spawnTrailParticle(float carScreenX, float carScreenY, float carZ, float speedFactor)
{
    Particle p;
    // 生成在车辆车尾，横向轻微随机偏移，更自然
    p.screenX = carScreenX + (rand() % 12 - 6);
    p.screenY = carScreenY + 8.f;
    p.worldZ = carZ;
    // 速度越快，粒子存活时间越长，拖尾更长
    p.lifeTime = m_maxLife * std::clamp(speedFactor, 0.3f, 1.2f);
    // 速度越快，粒子尺寸越大
    p.size = m_baseSize * std::clamp(speedFactor, 0.4f, 1.3f);
    p.alpha = 220.f;

    m_particles.push_back(p);
}

void SpeedParticle::update(float dt)
{
    // 遍历所有粒子，反向遍历方便删除失效粒子
    for (auto it = m_particles.rbegin(); it != m_particles.rend();)
    {
        Particle &p = *it;
        // 生命周期衰减
        p.lifeTime -= dt;

        // 生命耗尽，移除粒子
        if (p.lifeTime <= 0.f)
        {
            // 反向迭代器删除逻辑
            it = std::reverse_iterator(m_particles.erase(std::next(it).base()));
            continue;
        }

        // 透明度随生命周期降低，逐渐淡出
        p.alpha = 220.f * (p.lifeTime / m_maxLife);
        // 粒子缓慢向下偏移，模拟向后拖尾
        p.screenY += 120.f * dt;
        // 尺寸随生命衰减变小
        p.size = m_baseSize * (p.lifeTime / m_maxLife);
        ++it;
    }
}

void SpeedParticle::render(sf::RenderWindow &window)
{
    for (const auto &p : m_particles)
    {
        // SFML3.1 圆形粒子尾气
        sf::CircleShape particleShape(p.size);
        // 尾气浅灰色，带透明度
        sf::Color smokeColor(160, 160, 160, static_cast<std::uint8_t>(p.alpha));
        particleShape.setFillColor(smokeColor);
        // 粒子中心对齐坐标
        particleShape.setOrigin(sf::Vector2f({p.size, p.size}));
        particleShape.setPosition(sf::Vector2f({p.screenX, p.screenY}));

        window.draw(particleShape);
    }
}

void SpeedParticle::clearAll()
{
    m_particles.clear();
}
