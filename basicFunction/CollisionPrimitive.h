#pragma once

#include "MineMath.h"

//球構造体
struct Sphere
{
	// 中心座標
	Vector3 center = {0,0,0};
	// 半径
	float radius = 1.0f;
};
///平面構造体
struct Plane
{
	// 法線ベクトル
	Vector3 normal = { 0,1,0 };
	// 原点(0,0,0)からの距離
	float distance = 0.0f;
};
//三角形構造体
class Triangle
{
public:
	//頂点座標３つ
	Vector3 p0;
	Vector3 p1;
	Vector3 p2;
	//法線ベクトル
	Vector3 normal;
	///<summary>
	///法線の計算
	///</summary>
	void ComputeNormal();
};
//レイ構造体
struct Lay
{
	//視点座標
	Vector3 start = { 0,0,0 };
	//方向
	Vector3 dir = { 1,0,0 };
};