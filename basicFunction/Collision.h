#pragma once

#include "CollisionPrimitive.h"

/// <summary>
/// 当たり判定ヘルパークラス
/// </summary>
class Collision
{
public:
	/// <summary>
	/// 球同士の当たり判定
	/// </summary>
	/// <param name="sphere">1つ目の球</param>
	/// <param name="sphere_2">2つ目の球</param>
	/// <returns>当たっているかの判定</returns>
	static bool CheckSphereToSphere(const Sphere& sphere, const Sphere& sphere_2);

	/// <summary>
	/// 球と平面の当たり判定
	/// </summary>
	/// <param name="sphere">球</param>
	/// <param name="plane">平面</param>
	/// <param name="inter">擬似交点を書き込むためのポインタ</param>
	/// <returns>当たっているかの判定</returns>
	static bool CheckSphereToPlane(const Sphere& sphere, const Plane& plane, Vector3*inter = nullptr);

	/// <summary>
	/// 点と三角形の当たり判定(計算のみ)
	/// </summary>
	/// <param name="point">点</param>
	/// <param name="triangle">三角形</param>
	/// <param name="closest">最近傍を書き込むためのポインタ</param>
	static void ClosestPtPointToTriangle(const Vector3& point, const Triangle& triangle, Vector3* closest);

	/// <summary>
	/// 球と三角形の当たり判定
	/// </summary>
	/// <param name="sphere">球</param>
	/// <param name="triangle">三角形</param>
	/// <param name="inter">擬似交点を書き込むためのポインタ</param>
	/// <returns>当たっているかの判定</returns>
	static bool CheckSphereToTriangle(const Sphere& sphere, const Triangle& triangle, Vector3* inter = nullptr);

	/// <summary>
	/// レイと平面の当たり判定
	/// </summary>
	/// <param name="lay">レイ</param>
	/// <param name="plane">平面</param>
	/// <param name="distance">距離を書き込むためのポインタ</param>
	/// <param name="inter">擬似交点を書き込むためのポインタ</param>
	/// <returns>当たっているかの判定</returns>
	static bool CheckLayToPlane(const Lay& lay, const Plane& plane, float* distance = nullptr, Vector3* inter = nullptr);

	/// <summary>
	/// レイと三角形の当たり判定
	/// </summary>
	/// <param name="lay">レイ</param>
	/// <param name="triangle">三角形</param>
	/// <param name="distance">距離を書き込むためのポインタ</param>
	/// <param name="inter">擬似交点を書き込むためのポインタ</param>
	/// <returns>当たっているかの判定</returns>
	static bool CheckLayToTriangle(const Lay& lay, const Triangle& triangle, float* distance = nullptr, Vector3* inter = nullptr);
	
	/// <summary>
	/// レイと球の当たり判定
	/// </summary>
	/// <param name="lay">レイ</param>
	/// <param name="sphere">球</param>
	/// <param name="distance">距離を書き込むためのポインタ</param>
	/// <param name="inter">擬似交点を書き込むためのポインタ</param>
	/// <returns>当たっているかの判定</returns>
	static bool CheckLayToSphere(const Lay& lay, const Sphere& sphere, float* distance = nullptr, Vector3* inter = nullptr);
};

