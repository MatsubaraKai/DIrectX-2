#pragma once
#include "Vector2.h"
#include "Vector4.h"

struct VertexData
{
	Vector4 position;
	Vector2 texcoord;
};

struct TriangleData 
{
	Vector4 position[3];
	Vector4 material;
};

struct SpriteData
{
	Vector4 positionLeftTop[2];
	Vector4 positionRightDown[2];
	Vector4 material;
};