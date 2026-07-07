#pragma once
#include "../GameObject.h"
#include <vector>

// 单个粒子数据结构
struct Particle
{
    // 3D赛道Z坐标、屏幕投影XY
    float worldZ;
    float screenX;
    float screenY;
    // 存活剩余时间
    float lifeTime;
    // 粒子尺寸
    float size;
    // 透明度
    float alpha;
};

class SpeedParticle : public GameObject
{
public:
    SpeedParticle();
    // 每一帧更新粒子生命周期、位置
    void update(float dt) override;
    // 绘制所有存活粒子
    void render(sf::RenderWindow &window) override;

    // 在玩家车尾生成一个新粒子
    void spawnTrailParticle(float carScreenX, float carScreenY, float carZ, float speedFactor);
    // 清空全部粒子（游戏重置/GameOver调用）
    void clearAll();

private:
    std::vector<Particle> m_particles;
    // 粒子最大存活时长
    const float m_maxLife = 0.6f;
    // 粒子基础尺寸
    const float m_baseSize = 12.f;
};