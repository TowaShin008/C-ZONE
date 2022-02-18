#pragma once
#include<wrl.h>
#include<d3d12.h>
#include<DirectXMath.h>
#include<string>
#include"OBJModel.h"
#include<d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")
#include"GameObject.h"
#include"Camera.h"
#include"Input.h"

//弾丸クラス
class Bullet :public GameObject
{
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	using XMMATRIX = DirectX::XMMATRIX;

public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="arg_cmdList">コマンドリスト</param>
	Bullet(ID3D12GraphicsCommandList* arg_cmdList);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Bullet();

	/// <summary>
	/// オブジェクトの生成処理
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="position">ポジション</param>
	/// <returns>キャラクターオブジェクト</returns>
	static Bullet* Create(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList,Vector3 position = { 0.0f,0.0f,0.0f });


	/// <summary>
	/// 定数バッファの生成
	/// </summary>
	void CreateConstBuffer(ID3D12Device* device);

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(const Vector3& incrementValue);

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	void Draw()override;

	/// <summary>
	/// モデルのセット
	/// </summary>
	/// <param name="objModel">モデル</param>
	void SetOBJModel(OBJModel* arg_objModel) { objModel = arg_objModel; }

	/// <summary>
	/// 視点座標のセット
	/// </summary>
	/// <param name="eye">視点座標</param>
	static void SetEye(const Vector3& eye);

	/// <summary>
	/// 注視点座標のセット
	/// </summary>
	/// <param name="target">注視点座標</param>
	static void SetTarget(const Vector3& target);

	/// <summary>
	/// ビュー行列の更新処理
	/// </summary>
	static void UpdateViewMatrix();

	/// <summary>
	/// カメラのセット
	/// </summary>
	/// <param name="camera">カメラ</param>
	static void SetCamera(Camera* arg_camera) { Bullet::camera = arg_camera; }

	/// <summary>
	/// ポジションの移動
	/// </summary>
	/// <param name="incrementValue">ポジションの増加量</param>
	void SetScrollIncrement(const Vector3& incrementValue);
	/// <summary>
	/// 当たり判定
	/// </summary>
	/// <param name="otherObject">相手のオブジェクト</param>
	/// <returns>当たったかどうか</returns>
	bool IsCollision(GameObject* arg_otherObject);
private:
	UINT descpriptorSize;
	static ID3D12GraphicsCommandList* cmdList;
	ComPtr<ID3D12Resource>constBuffB0;
	ComPtr<ID3D12Resource>constBuffB1;

	//カメラ関係
	static XMMATRIX matView;
	static XMMATRIX matProjection;
	static Camera* camera;

	OBJModel* objModel = nullptr;
	Bullet* parent = nullptr;
public:
	/// <summary>
	/// 定数バッファの転送
	/// </summary>
	void TransferConstBuff();
	/// <summary>
	/// 移動処理
	/// </summary>
	void Move();
};

