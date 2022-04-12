#pragma once
#include<DirectXMath.h>

using XMFLOAT2 = DirectX::XMFLOAT2;
using XMFLOAT3 = DirectX::XMFLOAT3;
using XMFLOAT4 = DirectX::XMFLOAT4;

#pragma region Vector

//ベクトル
struct Vector4 : public XMFLOAT4
{
	//コンストラクタ
	Vector4();                                //零ベクトルとする
	Vector4(float x, float y, float z, float w);       //x成分,y成分,z成分 を指定しての生成

	//単項演算子のオーバーロード
	Vector4 operator+() const;
	Vector4 operator-() const;

	//代入演算子オーバーロード
	Vector4& operator+=(const Vector4& v);
	Vector4& operator-=(const Vector4& v);
	Vector4& operator*=(float s);
	Vector4& operator/=(float s);
};

//2項演算子オーバーロード
//※いろんな引数のパターンに対応(引数の順序)するため、以下のように準備している
const Vector4 operator+(const Vector4& v1, const Vector4& v2);
const Vector4 operator-(const Vector4& v1, const Vector4& v2);
const Vector4 operator*(const Vector4& v, float s);
const Vector4 operator*(float s, const Vector4& v);
const Vector4 operator/(const Vector4& v, float s);


struct Vector3 : public XMFLOAT3
{

	//コンストラクタ
	Vector3();                                //零ベクトルとする
	Vector3(float x, float y, float z);       //x成分,y成分,z成分 を指定しての生成

	//メンバ関数

	/// <summary>
	/// ノルムを求める
	/// </summary>
	/// <returns>ノルム</returns>
	float length() const;

	/// <summary>
	/// 正規化
	/// </summary>
	/// <returns>計算結果</returns>
	Vector3& normalize();

	/// <summary>
	/// 内積
	/// </summary>
	/// <param name="v">ベクトル</param>
	/// <returns>計算結果</returns>
	float dot(const Vector3& v) const;

	/// <summary>
	/// 外積
	/// </summary>
	/// <param name="v">ベクトル</param>
	/// <returns>計算結果</returns>
	Vector3 cross(const Vector3& v)const;      //外積を求める

	//単項演算子のオーバーロード
	Vector3 operator+() const;
	Vector3 operator-() const;

	//代入演算子オーバーロード
	Vector3& operator+=(const Vector3& v);
	Vector3& operator-=(const Vector3& v);
	Vector3& operator*=(const Vector3& v);
	Vector3& operator*=(float s);
	Vector3& operator/=(float s);
};

//2項演算子オーバーロード
//※いろんな引数のパターンに対応(引数の順序)するため、以下のように準備している
const Vector3 operator+(const Vector3& v1, const Vector3& v2);
const Vector3 operator-(const Vector3& v1, const Vector3& v2);
const Vector3 operator*(const Vector3& v1, const Vector3& v2);
const Vector3 operator*(const Vector3& v, float s);
const Vector3 operator*(float s, const Vector3& v);
const Vector3 operator/(const Vector3& v, float s);

//補間関数

/// <summary>
/// 線形補間(一次関数補間)
/// </summary>
/// <param name="start">スタート位置</param>
/// <param name="end">ゴール位置</param>
/// <param name="t">補間時間</param>
/// <returns>時間に応じた位置</returns>
const Vector3 lerp(const Vector3& start, const Vector3& end, const float t);

/// <summary>
/// ease-In補間
/// </summary>
/// <param name="start">スタート位置</param>
/// <param name="end">ゴール位置</param>
/// <param name="t">補間時間</param>
/// <returns>時間に応じた位置</returns>
const Vector3 easeIn(const Vector3& start, const Vector3& end, const float t);

/// <summary>
/// ease-Out補間
/// </summary>
/// <param name="start">スタート位置</param>
/// <param name="end">ゴール位置</param>
/// <param name="t">補間時間</param>
/// <returns>時間に応じた位置</returns>
const Vector3 easeOut(const Vector3& start, const Vector3& end, const float t);

/// <summary>
/// easeInOut補間
/// </summary>
/// <param name="start">スタート位置</param>
/// <param name="end">ゴール位置</param>
/// <param name="t">補間時間</param>
/// <returns>時間に応じた位置</returns>
const Vector3 easeInOut(const Vector3& start, const Vector3& end, const float t);


struct Vector2 : public XMFLOAT2
{
	//コンストラクタ
	Vector2();                                //零ベクトルとする
	Vector2(float x, float y);       //x成分,y成分,z成分 を指定しての生成

	//メンバ関数
	float length() const;                    //ノルム(長さ)を求める
	Vector2& normalize();                    //正規化する
	float dot(const Vector2& v) const;       //内積を求める
	float cross(const Vector2& v)const;      //外積を求める

	//単項演算子のオーバーロード
	Vector2 operator+() const;
	Vector2 operator-() const;

	//代入演算子オーバーロード
	Vector2& operator+=(const Vector2& v);
	Vector2& operator-=(const Vector2& v);
	Vector2& operator*=(float s);
	Vector2& operator/=(float s);
};

//2項演算子オーバーロード
//※いろんな引数のパターンに対応(引数の順序)するため、以下のように準備している
const Vector2 operator+(const Vector2& v1, const Vector2& v2);
const Vector2 operator-(const Vector2& v1, const Vector2& v2);
const Vector2 operator*(const Vector2& v, float s);
const Vector2 operator*(float s, const Vector2& v);
const Vector2 operator/(const Vector2& v, float s);
//補間関数

/// <summary>
/// 線形補間(一次関数補間)
/// </summary>
/// <param name="start">スタート位置</param>
/// <param name="end">ゴール位置</param>
/// <param name="t">補間時間</param>
/// <returns>時間に応じた位置</returns>
const Vector2 lerp(const Vector2& start, const Vector2& end, const float t);

/// <summary>
/// ease-In補間
/// </summary>
/// <param name="start">スタート位置</param>
/// <param name="end">ゴール位置</param>
/// <param name="t">補間時間</param>
/// <returns>時間に応じた位置</returns>
const Vector2 easeIn(const Vector2& start, const Vector2& end, const float t);

/// <summary>
/// ease-Out補間
/// </summary>
/// <param name="start">スタート位置</param>
/// <param name="end">ゴール位置</param>
/// <param name="t">補間時間</param>
/// <returns>時間に応じた位置</returns>
const Vector2 easeOut(const Vector2& start, const Vector2& end, const float t);

/// <summary>
/// ease-InOut補間
/// </summary>
/// <param name="start">スタート位置</param>
/// <param name="end">ゴール位置</param>
/// <param name="t">補間時間</param>
/// <returns>時間に応じた位置</returns>
const Vector2 easeInOut(const Vector2& start, const Vector2& end, const float t);

#pragma endregion

#pragma region Matrix

//行列
struct Matrix4
{
	float m[4][4];
};

/// <summary>
/// 単位行列の取得
/// </summary>
/// <returns>単位行列</returns>
Matrix4 identity();

/// <summary>
/// 拡大縮小行列の計算
/// </summary>
/// <param name="s">拡大する割合</param>
/// <returns>拡大縮小行列</returns>
Matrix4 scale(const Vector3& s);

/// <summary>
/// 回転行列の計算
/// </summary>
/// <param name="angle">回転角度</param>
/// <returns>回転行列</returns>
Matrix4 rotateX(float angle);
Matrix4 rotateY(float angle);
Matrix4 rotateZ(float angle);

/// <summary>
/// 平行移動行列の計算
/// </summary>
/// <param name="t">移動距離</param>
/// <returns>平行移動行列</returns>
Matrix4 translate(const Vector3& t);

/// <summary>
/// 座標変換(ベクトルと行列の掛け算をする)
/// </summary>
/// <param name="v">移動行列</param>
/// <param name="m">行列</param>
/// <returns>ベクトル</returns>
Vector3 transform(const Vector3& v, const Matrix4& m);

//代入演算子オーバーロード
Matrix4& operator *= (Matrix4& m1, const Matrix4& m2);

//2項演算子オーバーロード
Matrix4 operator * (const Matrix4& m1, const Matrix4& m2);
Vector3 operator * (const Vector3& v, const Matrix4& m);

#pragma endregion

#pragma region Quaternion

//クォータニオン
struct Quaternion
{
	float x;//x成分
	float y;//y成分
	float z;//z成分
	float w;//w成分
};

//クォータニオンを作成
Quaternion quaternion(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f);
Quaternion quaternion(const Vector3& v, float angle);

/// <summary>
/// 内積の計算
/// </summary>
/// <param name="q1">クォータニオン</param>
/// <param name="q2">クォータニオン</param>
/// <returns>計算結果</returns>
float dot(const Quaternion& q1, const Quaternion& q2);

/// <summary>
/// ノルムを求める
/// </summary>
/// <param name="q">クォータニオン</param>
/// <returns>計算結果</returns>
float length(const Quaternion& q);

/// <summary>
/// 正規化する
/// </summary>
/// <param name="q">クォータニオン</param>
/// <returns>計算結果</returns>
Quaternion normalize(const Quaternion& q);

/// <summary>
/// 共役(きょうやく)四元数を求める
/// </summary>
/// <param name="q">クォータニオン</param>
/// <returns>計算結果</returns>
Quaternion conjugate(const Quaternion& q);

//単項演算子オーバーロード
Quaternion operator + (Quaternion& q);
Quaternion operator - (Quaternion& q);
//代入演算子オーバーロード
Quaternion& operator +=(Quaternion& q1, const Quaternion& q2);
Quaternion& operator -=(Quaternion& q1, const Quaternion& q2);
Quaternion& operator *=(Quaternion& q, float s);
Quaternion& operator /=(Quaternion& q, float s);
Quaternion& operator *=(Quaternion& q1, const Quaternion& q2);
//2項演算子オーバーロード
Quaternion operator + (const Quaternion& q1, const Quaternion& q2);
Quaternion operator - (const Quaternion& q1, const Quaternion& q2);
Quaternion operator * (const Quaternion& q1, const Quaternion& q2);
Quaternion operator * (const Quaternion& q, float s);
Quaternion operator * (float s, Quaternion& q);
Quaternion operator / (const Quaternion& q, float s);

/// <summary>
/// 球面線形補間
/// </summary>
/// <param name="q1">クォータニオン</param>
/// <param name="q2">クォータニオン</param>
/// <param name="t">補間時間</param>
/// <returns>クォータニオン</returns>
Quaternion slerp(const Quaternion& q1, const Quaternion& q2, float t);

/// <summary>
/// 線形補間
/// </summary>
/// <param name="q1">クォータニオン</param>
/// <param name="q2">クォータニオン</param>
/// <param name="t">補間時間</param>
/// <returns>クォータニオン</returns>
Quaternion lerp(const Quaternion& q1, const Quaternion& q2, float t);

/// <summary>
/// 回転行列の計算
/// </summary>
/// <param name="q">クォータニオン</param>
/// <returns>回転行列</returns>
Matrix4 rotate(const Quaternion& q);

/// <summary>
/// クォータニオンの計算
/// </summary>
/// <param name="m">行列</param>
/// <returns>計算結果</returns>
Quaternion quaternion(const Matrix4& m);

/// <summary>
/// 3軸の計算
/// </summary>
/// <param name="q">クォータニオン</param>
/// <returns>ベクトル</returns>
Vector3 getAxis(const Quaternion& q);

#pragma endregion
