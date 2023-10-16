#pragma once
#include <stdint.h>
#include "Matrix4x4.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

struct VertexData
{
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct TriangleData 
{
	Vector4 position[3];
	Vector4 material;
};

struct SpriteData 
{
	Vector4 LeftTop[2];
	Vector4 RightDown[2];
	Vector4 material;
};

struct TransformationMatrix
{
	Matrix4x4 WVP;
	Matrix4x4 World;
};

struct Material 
{
	Vector4 color;
	int32_t enableLighting;
};

struct DirectionalLight
{
	Vector4 color;
	Vector3 direction;
	float intensity;
};