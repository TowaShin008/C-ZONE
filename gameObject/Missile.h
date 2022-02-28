#pragma once
#include<wrl.h>
#include<d3d12.h>
#include<DirectXMath.h>
#include<string>
#include"OBJModel.h"
#include"ParticleManager.h"
#include<d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")
#include"GameObject.h"
#include"Camera.h"
#include"Input.h"
#define MAXMISSILELUGTIME 2

//ミサイルクラス
class Missile :public GameObject
{
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	using XMMATRIX = DirectX::XMMATRIX;

public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="arg_cmdList">コマンドリスト</param>
	Missile(ID3D12GraphicsCommandList* arg_cmdList);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Missile();

	/// <summary>
	/// オブジェクトの生成処理
	/// </summary>
	/// <param name="arg_device">デバイス</param>
	/// <param name="arg_cmdList">コマンドリスト</param>
	/// <param name="arg_position">ポジション</param>
	/// <returns>キャラクターオブジェクト</returns>
	static Missile* Create(ID3D12Device* arg_device, ID3D12GraphicsCommandList* arg_cmdList,const Vector3& arg_position = { 0.0f,0.0f,0.0f });


	/// <summary>
	/// 定数バッファの生成
	/// </summary>
	/// <param name="arg_device">デバイス</param>
	void CreateConstBuffer(ID3D12Device* arg_device);

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="arg_incrementValue">スクロール量</param>
	/// <param name="arg_centerPosition">画面の中心座標</param>
	void Update(const Vector3& arg_incrementValue, float arg_centerPosition);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw()override;

	/// <summary>
	/// モデルのセット
	/// </summary>
	/// <param name="arg_objModel">モデル</param>
	void SetOBJModel(OBJHighModel* arg_objModel) { objModel = arg_objModel; }

	/// <summary>
	/// 視点座標のセット
	/// </summary>
	/// <param name="arg_eye">視点座標</param>
	static void SetEye(const Vector3& arg_eye);

	/// <summary>
	/// 注視点座標のセット
	/// </summary>
	/// <param name="arg_target">注視点座標</param>
	static void SetTarget(const Vector3& arg_target);

	/// <summary>
	/// ビュー行列の更新処理
	/// </summary>
	static void UpdateViewMatrix();

	/// <summary>
	/// カメラのセット
	/// </summary>
	/// <param name="arg_camera">カメラ</param>
	static void SetCamera(Camera* arg_camera) { Missile::camera = arg_camera; }

	/// <summary>
	/// ポジションの移動
	/// </summary>
	/// <param name="arg_incrementValue">ポジションの増加量</param>
	void SetScrollIncrement(const Vector3& arg_incrementValue);

	/// <summary>
	/// 当たり判定
	/// </summary>
	/// <param name="arg_otherObject">相手のオブジェクト</param>
	/// <returns>当たったかどうか</returns>
	bool IsCollision(GameObject* arg_otherObject);

	/// <summary>
	/// 上部からの発射フラグ
	/// </summary>
	/// <param name="arg_upShotFlag">上部からの発射フラグ</param>
	void SetUpShotFlag(bool arg_upShotFlag) { upShotFlag = arg_upShotFlag; }

	/// <summary>
	/// ボスシーンかどうかのフラグ
	/// </summary>
	/// <param name="arg_bossSceneFlag">ボスシーンフラグ</param>
	void SetBossSceneFlag(bool arg_bossSceneFlag) { bossSceneFlag = arg_bossSceneFlag; }

	/// <summary>
	/// パーティクルのセット
	/// </summary>
	/// <param name="arg_particleMan">パーティクルマネージャー</param>
	void SetMissilePerticleMan(ParticleManager* arg_particleMissileMan) { missileParticleMan.reset(arg_particleMissileMan); }
private:
	UINT descpriptorSize;
	static ID3D12GraphicsCommandList* cmdList;
	ComPtr<ID3D12Resource>constBuffB0;
	ComPtr<ID3D12Resource>constBuffB1;

	//カメラ関係
	static XMMATRIX matView;
	static XMMATRIX matProjection;
	static Camera* camera;

	OBJHighModel* objModel = nullptr;
	std::unique_ptr<ParticleManager> missileParticleMan;
	Missile* parent = nullptr;
	float curveTime = 2.0f;
	bool upShotFlag = true;
	int particleLugtime = 0;
	bool bossSceneFlag = false;

public:
	/// <summary>
	/// 定数バッファの転送
	/// </summary>
	void TransferConstBuff();

	/// <summary>
	/// 移動処理
	/// </summary>
	void Move();

	/// <summary>
	/// 煙パーティクル処理
	/// </summary>
	/// <param name="arg_centerPosition">画面の中心点</param>
	void MissileParticleProcessing(float arg_centerPosition);
};