#include "Config.h"

// 常量定义
const int width = 1024;
const int height = 768;
const int roadW = 2000;
const int segL = 200;
const float camD = 0.84f;

const float SAFE_LANE_LIMIT = 1.3f;// 玩家车道安全边界
const float LANE_WARN_LIMIT = 0.9f; // 计时触发边界
const float LANE_TIMEOUT_SECONDS = 5.0f;// 持续5秒超时失败
const int OBSTACLE_SPAWN_STEP = 350;// 每多少段生成障碍车
const int MAX_OBSTACLE_COUNT = 8;// 同时存在最大障碍车数量

const float ITEM_DURATION = 8.0f;//道具持续时间（秒）
const int ITEM_SPAWN_INTERVAL = 500;//道具生成间隔（段数）