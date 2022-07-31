/*用于将动画数据烘焙到纹理文件中*/

#pragma once
#include "Skeleton.h"
#include "Clip.h"
#include "AnimTexture.h"

void BakeAnimationToTexture(Skeleton& skel, Clip& clip, AnimTexture& tex);