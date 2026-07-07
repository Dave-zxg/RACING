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

// 障碍物生成参数
extern const int OBSTACLE_SPAWN_STEP;
extern const int MAX_OBSTACLE_COUNT;

#endif
