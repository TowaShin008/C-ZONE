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
#include"Bullet.h"

//ブロック(障害物)クラス
class Block:public GameObject
{
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	using XMMATRIX = DirectX::XMMATRIX;

public:
	//コンストラクタ
	Block(ID3D12GraphicsCommandList* arg_cmdList);

	//デストラクタ
	~Block();

	/// <summary>
	/// オブジェクトの生成処理
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="position">ポジション</param>
	/// <returns>キャラクターオブジェクト</returns>
	static Block* Create(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList,Vector3 position = { 0.0f,0.0f,0.0f });


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
	void Update(const Vector3& incrementValue)override;

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	void Draw();

	/// <summary>
	/// 定数バッファの転送
	/// </summary>
	void TransferConstBuff();

	/// <summary>
	/// モデルのセット
	/// </summary>
	/// <param name="objModel">モデル</param>
	void SetOBJModel(OBJModel* arg_objModel);


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
	static void SetCamera(Camera* arg_camera) { Block::camera = arg_camera; }

	void SetStageBlockFlag(bool arg_stageblockFlag) { stageblockFlag = arg_stageblockFlag; }

	void SetParent(Block* arg_parent) { parent = arg_parent; }

private:
	UINT descpriptorSize;
	static ID3D12GraphicsCommandList* cmdList;
	ComPtr<ID3D12Resource>constBuffB0;
	ComPtr<ID3D12Resource>constBuffB1;

	//ビュー行列
	static XMMATRIX matView;
	//プロジェクション行列
	static XMMATRIX matProjection;
	//カメラクラス
	static Camera* camera;

	//モデルデータ格納用変数
	OBJModel* objModel = nullptr;
	//親クラス
	Block* parent = nullptr;

	float centerPos = 0.0f;
	bool stageblockFlag = true;

public:

	/// <summary>
	/// isDeadFlagをtrueにする
	/// </summary>
	void IsDead();

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
	bool IsCollision(GameObject* arg_otherObject)override;
};

