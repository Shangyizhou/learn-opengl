#!/bin/bash

echo "=== 构建投光物演示 ==="
cd src/pratice
mkdir -p build
cd build

echo "运行CMake..."
cmake ..

echo "编译light_casters_demo..."
make light_casters_demo

if [ $? -eq 0 ]; then
    echo "=== 运行投光物演示 ==="
    echo "控制说明："
    echo "- WASD: 移动相机"
    echo "- 鼠标: 旋转视角"
    echo "- 滚轮: 缩放"
    echo "- 空格: 切换光源移动"
    echo "- 1: 平行光 (太阳光效果)"
    echo "- 2: 点光源 (灯泡效果)"
    echo "- 3: 聚光 (手电筒效果)"
    echo "- 4/5: 调整聚光内角"
    echo "- 6/7: 调整聚光外角"
    echo "- R: 重置参数"
    echo "- ESC: 退出"
    echo "==================="
    echo "效果说明："
    echo "- 平行光: 均匀光照，无衰减"
    echo "- 点光源: 距离衰减，真实感强"
    echo "- 聚光: 锥形光束，边缘平滑"
    echo "==================="
    
    ./light_casters_demo
else
    echo "编译失败！"
    exit 1
fi

echo "Demo运行完成！" 