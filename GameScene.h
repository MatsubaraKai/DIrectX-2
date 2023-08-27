#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "VertexData.h"
#include "MyEngine.h"
#include "Triangle.h"
#include "Sprite.h"

class GameScene {
public:
	void Initialize(MyEngine* engine, DirectXCommon* direct);
	
	void Update();
	
	void Draw();
	
	void Finalize();

private:
	MyEngine* engine_;
	DirectXCommon* dxCommon_;

	//三角形
	Triangle* triangle_[2];
	Vector4 data1_[2];
	Vector4 data2_[2];
	Vector4 data3_[2];
	Vector4 material_[2];
	Transform transform_;
	Matrix4x4 worldMatrix_;

	//スプライト
	Sprite* sprite_[2];
	Vector4 spriteDataLeftTop_[2];
	Vector4 spriteDataRightDown_[2];
	Transform spriteTransform_[1];
	Vector4 spriteMaterial[2];
};