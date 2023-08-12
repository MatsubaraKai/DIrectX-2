#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "VertexData.h"
#include "MyEngine.h"
#include "Triangle.h"
#include "Sprite.h"
#include "Sphere.h"

class GameScene {
public:
	void Initialize(MyEngine* engine, DirectXCommon* dxCommon);
	
	void Update();
	
	void Draw();
	
	void Finalize();

private:
	MyEngine* engine_;
	DirectXCommon* dxCommon_;

	Triangle* triangle_[2];
	TriangleData triangleData_[2];
	Transform transform_;
	Matrix4x4 worldMatrix_;

	Sprite* sprite_[2];
	SpriteData spriteData_;
	Transform spriteTransform_;

	Sphere* sphere_;
	Transform sphereTransform_;
	Vector4 sphereMaterial_;
	Matrix4x4 sphereMatrix_;

	Transform cameraTransform_;
};