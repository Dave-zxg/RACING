#ifndef CONFIG_H
#define CONFIG_H
#include <SFML/Graphics.hpp>

// 窗口尺寸
extern const int width;
extern const int height;

// 赛道基础参数
extern const int roadW;
extern const int segL;
extern const float camD;

// 玩家车道限制
extern const float SAFE_LANE_LIMIT;

//1.0预警边界、超时5秒判定
extern const float LANE_WARN_LIMIT;
extern const float LANE_TIMEOUT_SECONDS;

// 障碍物生成参数
extern const int OBSTACLE_SPAWN_STEP;
extern const int MAX_OBSTACLE_COUNT;

enum ItemType
{
    ITEM_NONE,
    ITEM_NITRO,    // Tab加速道具
    ITEM_FLY       // W飞行道具
};
extern const float ITEM_DURATION;
extern const int ITEM_SPAWN_INTERVAL;


#endif
