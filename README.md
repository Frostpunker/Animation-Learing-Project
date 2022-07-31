# Animation-Learning-Project
This is a student project aimed to learn animation algorithms and techniques.

项目背景:
本项目用于本人深入学习动画技术的底层原理。整个工程的搭建参考了Gabor Szauer
的著作《Hands-On C++ Game Animation Programming》，并基于作者提供的代码框架进行部分
改动，着重实现了人物的脚步IK动画。通过OpenGL以及部分第三方库，实现了一条完整的动画管线，
搭建了一个简单的动画引擎。

项目介绍:
 
主程序:
提供了一个控制台调试窗口以及一个Win32窗口程序，用nuklear来实现UI接口

数学引擎:
实现了2D，3D，4D向量，四元数，对偶四元数，矩阵及其相关操作
将变换信息封装到SQT（缩放、旋转、平移）结构中
提供了Bezier和Hermite两种插值样条
在CPU中实现了光线与三角形求交算法

资产管道:
利用cgltf将gltf文件数据加载到引擎中，完成动画数据和模型数据的提取。
利用stb_image读取纹理数据到OpenGL并绑定到模型。

渲染框架:
基于OpenGL搭建了一个简易渲染框架，通过编写一系列API将Shader文件操作封装到
一个单独的对象中

动画系统:

核心：实现了动画帧、动画轨迹、动画片段、骨骼、姿势等数据结构。从GLTF文件中读取
的动画数据将会写入这些结构，引擎会依照固定的帧率对上述部分数据进行采样，根据当前
播放时间对动画帧进行插值以更新模型动画。

骨骼蒙皮：
获取相关变换数据后，在CPU中计算矩阵调色板并将其传送到Shader文件，在GPU中完成
顶点混合和视口变换。

IK：利用CCD和FABRIK两种解算器实现人物模型的脚步IK，并可以通过窗口程序的UI接口在
运行时实时切换这两种解算器来对比它们的效果。
