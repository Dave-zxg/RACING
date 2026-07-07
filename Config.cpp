#include "Config.h"

// 常量定义
const int width = 1024;
const int height = 768;
const int roadW = 2000;
const int segL = 200;
const float camD = 0.84f;

const float SAFE_LANE_LIMIT = 1.5f;// 玩家车道安全边界
const int OBSTACLE_SPAWN_STEP = 250;// 每多少段生成障碍车
const int MAX_OBSTACLE_COUNT = 8;// 同时存在最大障碍车数量