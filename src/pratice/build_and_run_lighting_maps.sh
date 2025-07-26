#!/bin/bash

echo "=== 构建光照贴图演示 ==="
cd src/pratice
mkdir -p build
cd build

echo "运行CMake..."
cmake ..

echo "编译lighting_maps_demo..."
make lighting_maps_demo

if [ $? -eq 0 ]; then
    echo "=== 运行光照贴图演示 ==="
    echo "控制说明："
    echo "- WASD: 移动相机"
    echo "- 鼠标: 旋转视角"
    echo "- 滚轮: 缩放"
    echo "- 空格: 切换光源移动"
    echo "- 1/2: 调整环境光强度"
    echo "- 3/4: 调整漫反射强度"
    echo "- 5/6: 调整镜面反射强度"
    echo "- 7/8: 调整反光度"
    echo "- R: 重置参数"
    echo "- ESC: 退出"
    echo "====================="
    echo "效果说明："
    echo "- 木箱表面有纹理细节"
    echo "- 边框部分有镜面反射"
    echo "- 木头部分不反光"
    echo "====================="
    
    ./lighting_maps_demo
else
    echo "编译失败！"
    exit 1
fi

echo "Demo运行完成！" 