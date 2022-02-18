#pragma once

#include <DirectXMath.h>
#include"MineMath.h"
#include"WindowSize.h"
/// <summary>
/// カメラ基本機能
/// </summary>
class Camera
{
public: // エイリアス
	// DirectX::を省略
	using XMMATRIX = DirectX::XMMATRIX;

public: // メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="window_width">スクリーン横幅</param>
	/// <param name="window_height">スクリーン縦幅</param>
	Camera(int window_width, int window_height);

	/// <summary>
	/// 仮想デストラクタ
	/// </summary>
	virtual ~Camera() = default;

	/// <summary>
	/// ビュー行列のアップデート
	/// </summary>
	void UpdateViewMatrix();

	/// <summary>
	/// プロジェクション行列のアップデート
	/// </summary>
	void UpdateProjectionMatrix();

	/// <summary>
	/// ビュー行列の取得
	/// </summary>
	/// <returns>ビュー行列</returns>
	const XMMATRIX& GetViewMatrix() {return matView;}

	/// <summary>
	/// プロジェクション行列の取得
	/// </summary>
	/// <returns>プロジェクション行列</returns>
	const XMMATRIX& GetProjectionMatrix() {return matProjection;}

	/// <summary>
	/// ビュープロジェクション行列の取得
	/// </summary>
	/// <returns>ビュープロジェクション行列</returns>
	const XMMATRIX& GetViewProjectionMatrix() {return matViewProjection;}
	
	/// <summary>
	/// 視点座標の取得
	/// </summary>
	/// <returns>視点座標</returns>
	const Vector3& GetEye() {return eye;}
	/// <summary>
	/// 視点座標のセット
	/// </summary>
	/// <param name="eye">視点座標</param>
	void SetEye(const Vector3& eye) {this->eye = eye;}
	/// <summary>
	/// 注視点座標の取得
	/// </summary>
	/// <returns>注視点座標</returns>
	const Vector3& GetTarget() {return target;}
	/// <summary>
	/// 注視点座標のセット
	/// </summary>
	/// <param name="target">注視点座標</param>
	void SetTarget(const Vector3& target) {this->target = target;}
	/// <summary>
	/// 上方向ベクトルの取得
	/// </summary>
	/// <returns>上方向ベクトル</returns>
	const Vector3& GetUp() {return up;}
	/// <summary>
	/// 上方向ベクトル
	/// </summary>
	/// <param name="up">上方向ベクトル</param>
	void SetUp(const Vector3& up) {this->up = up;}
	/// <summary>
	/// 視点座標のみの移動処理
	/// </summary>
	/// <param name="move">1フレームの移動量</param>
	void MoveEyeVector(const Vector3& move);
	/// <summary>
	/// 視点座標と注視点座標の移動処理
	/// </summary>
	/// <param name="movePosition">1フレームの移動処理</param>
	void MoveEyeTarget(const Vector3& movePosition);
	//void MoveEyeTarget(const XMVECTOR movePosition);

private: // メンバ変数
	//ビュー行列
	XMMATRIX matView = DirectX::XMMatrixIdentity();
	//ビルボード行列
	XMMATRIX matBillboard = DirectX::XMMatrixIdentity();
	//Y軸回りビルボード行列
	XMMATRIX matBillboardY = DirectX::XMMatrixIdentity();
	//射影行列
	XMMATRIX matProjection = DirectX::XMMatrixIdentity();
	//ビュー射影行列
	XMMATRIX matViewProjection = DirectX::XMMatrixIdentity();
	//視点座標
	Vector3 eye = {0, 0, -20};
	//注視点座標
	Vector3 target = {0, 0, 0};
	//上方向ベクトル
	Vector3 up = {0, 1, 0};

};

