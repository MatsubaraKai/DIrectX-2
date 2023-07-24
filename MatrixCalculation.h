#pragma once
#include "Matrix4x4.h"
#include "Vector3.h"
#include "Vector4.h"
#include <assert.h>
#include <cmath>

struct Transform
{
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

//X����]�s��
Matrix4x4 MakeRotateXmatrix(float radian);

//Y����]�s��
Matrix4x4 MakeRotateYmatrix(float radian);

//Z����]�s��
Matrix4x4 MakeRotateZmatrix(float radian);

//���s�ړ�
Matrix4x4 MakeTranslateMatrix(Vector3 translate);

//�g��k��
Matrix4x4 MakeScaleMatrix(const Vector3& scale);

// �A�t�B���ϊ�
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

//�s��̉��@
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2);

//�s��̌��@
Matrix4x4 Sub(const Matrix4x4& m1, const Matrix4x4& m2);

//�s��̐�
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

//�t�s��
Matrix4x4 Inverse(const Matrix4x4& m1);

//�]�u�s��
Matrix4x4 Transpose(const Matrix4x4& m);

//�P�ʍs��
Matrix4x4 MakeIdentity4x4();

//�������e�s��
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRadio, float nearClip, float farClip);