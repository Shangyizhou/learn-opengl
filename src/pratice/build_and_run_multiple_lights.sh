#!/bin/bash

echo "=== 构建多光源演示 ==="
cd src/pratice
mkdir -p build
cd build

echo "运行CMake..."
cmake ..

echo "编译multiple_lights_demo..."
make multiple_lights_demo

if [ $? -eq 0 ]; then
    echo "=== 运行多光源演示 ==="
    echo "控制说明："
    echo "- WASD: 移动相机"
    echo "- 鼠标: 旋转视角"
    echo "- 滚轮: 缩放"
    echo "- 空格: 切换光源移动"
    echo "- 1/2: 调整聚光内角"
    echo "- 3/4: 调整聚光外角"
    echo "- R: 重置参数"
    echo "- ESC: 退出"
    echo "==================="
    echo "场景包含："
    echo "- 1个平行光 (太阳光，均匀照明)"
    echo "- 4个点光源 (红、绿、蓝、黄彩色灯泡)"
    echo "- 1个聚光 (白色手电筒)"
    echo "==================="
    echo "效果说明："
    echo "- 所有光源同时作用，产生丰富的照明效果"
    echo "- 点光源有距离衰减，颜色混合"
    echo "- 聚光提供局部强光照明"
    echo "- 平行光提供全局环境照明"
    echo "==================="
    
    ./multiple_lights_demo
else
    echo "编译失败！"
    exit 1
fi

echo "Demo运行完成！" 