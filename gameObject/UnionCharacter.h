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
#define SCREENTOP 10.0f
#define SCREENBUTTOM -10.0f
#define SCREENLEFT -17.0f
#define SCREENRIGHT 17.0f
#define SCREENBACK 50.0f
#define BULLETMAXNUM 10
#define MAXPLAYERLUGTIME 30
//プレイヤーの子機クラス
class UnionCharacter :public GameObject
{
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	using XMMATRIX = DirectX::XMMATRIX;

public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	UnionCharacter(ID3D12GraphicsCommandList* arg_cmdList);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~UnionCharacter();

	/// <summary>
	/// オブジェクトの生成処理
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="position">ポジション</param>
	/// <returns>キャラクターオブジェクト</returns>
	static UnionCharacter* Create(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList,Vector3 position = { 0.0f,0.0f,0.0f });


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
	void Update(GameObject* player, const Vector3& incrementValue);

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	void Draw();

	/// <summary>
	/// モデルのセット
	/// </summary>
	/// <param name="objModel">モデル</param>
	void SetOBJModel(OBJHighModel* arg_objModel, OBJModel* arg_bulletModel);

	/// <summary>
	/// 弾丸モデルのセット
	/// </summary>
	/// <param name="bulletModel">モデル</param>
	void SetBulletModel(OBJModel* bulletModel);

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
	static void SetCamera(Camera* arg_camera) { UnionCharacter::camera = arg_camera; }

	/// <summary>
	/// プレイヤーの弾をリサイズし生成する
	/// </summary>
	void AttachBullet(ID3D12Device* device);

	/// <summary>
	/// ポジションの移動
	/// </summary>
	/// <param name="incrementValue">ポジションの増加量</param>
	void SetScrollIncrement(const Vector3& incrementValue);
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
	UnionCharacter* parent = nullptr;
	std::vector<Bullet*>bullets;
	int shotLugTime = MAXPLAYERLUGTIME;
	Vector3 accel = { 0.0f,0.0f,0.0f };
	bool bossSceneFlag = false;
	bool lanchFlag = false;
	bool playerFrontFlag = false;
	bool playerBackFlag = false;
	int maxWireLength = 4;
	bool shotFlag = true;
public:
	/// <summary>
	/// 銃弾の配列を取得
	/// </summary>
	/// <returns>銃弾の配列</returns>
	std::vector<Bullet*> GetBullets() { return bullets; }
	/// <summary>
	/// ボスシーンに移っているか
	/// </summary>
	/// <param name="bossSceneFlag">ボスフラグ</param>
	void SetBossSceneFlag(bool arg_bossSceneFlag);

	/// <summary>
	/// プレイヤーから発射されたか
	/// </summary>
	/// <param name="lanchFlag">発射フラグ</param>
	void SetLanchFlag(bool arg_lanchFlag);
	bool GetLanchFlag() { return lanchFlag; }

	/// <summary>
	/// プレイヤーに当たっているかどうか
	/// </summary>
	/// <returns>当たっているかどうか</returns>
	bool IsPlayerCollision(const Vector3& otherPosition, float otherRadius);

	/// <summary>
	/// プレイヤーのすぐまえにいるかのセッターとゲッター
	/// </summary>
	void SetPlayerFrontFlag(bool arg_playerFrontFlag) { playerFrontFlag = arg_playerFrontFlag; }
	bool GetPlayerFrontFlag() { return playerFrontFlag; }

	/// <summary>
	/// プレイヤーのすぐ後ろにいるかのセッターとゲッター
	/// </summary>
	void SetPlayerBackFlag(bool arg_playerBackFlag) { playerBackFlag = arg_playerBackFlag; }
	bool GetPlayerBackFlag() { return playerBackFlag; }

	/// <summary>
	/// ショットフラグのセット
	/// </summary>
	/// <param name="arg_shotFlag">ショットフラグ</param>
	void SetShotFlag(bool arg_shotFlag) { shotFlag = arg_shotFlag; }

	/// <summary>
	/// 弾の発射処理
	/// </summary>
	void ShotBullet(const Vector3& incrementValue);

	/// <summary>
	/// 定数バッファの転送
	/// </summary>
	/// <param name="incrementValue">スクロールの移動量</param>
	void TransferConstBuff(const Vector3& incrementValue);

	/// <summary>
	/// 移動処理
	/// </summary>
	/// <param name="incrementValue">スクロールの移動量</param>
	void Move(GameObject* player);

	/// <summary>
	/// 当たり判定
	/// </summary>
	/// <param name="otherObject">相手のオブジェクト</param>
	/// <returns>当たったかどうか</returns>
	bool IsCollision(GameObject* arg_otherObject);

	/// <summary>
	/// 攻撃手段の更新処理
	/// </summary>
	/// <param name="incrementValue">スクロール量</param>
	void UpdateAttack(const Vector3& incrementValue);
};

