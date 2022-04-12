#pragma once
#include<wrl.h>
#include<d3d12.h>
#include<DirectXMath.h>
#include<string>
#include<d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")
#include"GameObject.h"
#include"Camera.h"
#include"Input.h"
#include"HomingBullet.h"
#include"NormalEnemy.h"
#include"ParticleManager.h"
#define BULLETMAXNUM 10
#define MAXENEMYLUGTIME 60
#define LANCHMAXTIME 800
//ボスクラス
class Boss :public GameObject
{
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	using XMMATRIX = DirectX::XMMATRIX;

public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="arg_cmdList">コマンドリスト</param>
	/// <param name="arg_device">デバイス</param>
	Boss(ID3D12GraphicsCommandList* arg_cmdList, ID3D12Device* arg_device);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Boss();

	/// <summary>
	/// オブジェクトの生成処理
	/// </summary>
	/// <param name="arg_device">デバイス</param>
	/// <param name="arg_cmdList">コマンドリスト</param>
	/// <param name="arg_position">ポジション</param>
	/// <returns>キャラクターオブジェクト</returns>
	static Boss* Create(ID3D12Device* arg_device, ID3D12GraphicsCommandList* arg_cmdList,const Vector3& arg_position = { 0.0f,0.0f,0.0f });

	/// <summary>
	/// 定数バッファの生成
	/// </summary>
	void CreateConstBuffer();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="arg_incrementValue">スクロール量</param>
	/// <param name="arg_playerPosition">プレイヤーポジション</param>
	void Update(const Vector3& arg_incrementValue, const Vector3& arg_playerPosition);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// モデルのセット
	/// </summary>
	/// <param name="arg_objModel">モデル</param>
	void SetOBJModel(ObjFileModel* arg_objModel, ObjFileModel* arg_bulletModel);

	/// <summary>
	/// 弾丸モデルのセット
	/// </summary>
	/// <param name="arg_bulletModel">モデル</param>
	void SetBulletModel(ObjFileModel* arg_bulletModel);

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
	static void SetCamera(Camera* arg_camera) { Boss::camera = arg_camera; }

	/// <summary>
	/// 弾をリサイズし生成する
	/// </summary>
	void AttachBullet();

private:
	UINT descpriptorSize;
	static ID3D12GraphicsCommandList* cmdList;
	ComPtr<ID3D12Resource>constBuffB0;
	ComPtr<ID3D12Resource>constBuffB1;

	//カメラ関係
	//ビュー行列
	static XMMATRIX matView;
	//プロジェクション行列
	static XMMATRIX matProjection;
	//カメラクラス
	static Camera* camera;

	static ID3D12Device* device;

	//モデルデータ格納用変数
	ObjFileModel* objModel = nullptr;
	//親クラス
	Boss* parent = nullptr;

	//再発射するまでの時間
	int shotLugTime = MAXENEMYLUGTIME;

	//動き出すための時間(複数の敵で隊列を組みたいときに使用する)
	int moveLugTime = 0;

	//現在の動きの段階を格納する変数
	MOVEPHASE currentPhase = MOVEPHASE::PHASE1;

	//現在の動きの種類を格納する変数
	MOVETYPE  currentType = MOVETYPE::TRIANGLE;

	bool moveEndFlag = false;
	float curveTime = 0.0f;
	float rotateX = 0.0f;
	bool shotFlag = true;
	float centerPosition = 0.0f;
	bool lanchFlag = false;
	int lanchLugTime = 0;
	int hp = 400;

	std::unique_ptr<ParticleManager> deathParticle;
	HomingBullet* bigBullet = nullptr;
	bool deathParticleFlag = false;
public:

	/// <summary>
	/// 定数バッファの転送
	/// </summary>
	void TransferConstBuff();

	/// <summary>
	/// チャージ弾の取得
	/// </summary>
	/// <returns>チャージ弾</returns>
	HomingBullet* GetBigBullet() { return bigBullet; }

	/// <summary>
	/// 移動処理
	/// </summary>
	void Move();

	/// <summary>
	/// ムーブフェイズのセット
	/// </summary>
	/// <param name="arg_movePhase">ムーブフェイズ</param>
	void SetMovePhase(const MOVEPHASE& arg_currentPhase) { currentPhase = arg_currentPhase; }

	/// <summary>
	/// ムーブフェイズの取得
	/// </summary>
	/// <returns>ムーブフェイズ</returns>
	MOVEPHASE GetMovePhase() { return currentPhase; }

	/// <summary>
	/// ムーブタイプのセット
	/// </summary>
	/// <param name="arg_moveType">ムーブタイプ</param>
	void SetMoveType(const MOVETYPE& arg_moveType);

	/// <summary>
	/// ムーブタイプの取得
	/// </summary>
	/// <returns>ムーブタイプ</returns>
	MOVETYPE GetMoveType() { return currentType; }

	/// <summary>
	/// ある程度進んで停止する処理処理
	/// </summary>
	void StayMove();

	/// <summary>
	/// ムーブラグタイムのセット
	/// </summary>
	/// <param name="arg_moveLugTime">ムーブラグタイム</param>
	void SetMoveLugTime(int arg_moveLugTime) { moveLugTime = arg_moveLugTime; }

	/// <summary>
	/// ムーブラグタイムの取得
	/// </summary>
	/// <returns>ムーブラグタイム</returns>
	int GetMoveLugTime() { return moveLugTime; }

	/// <summary>
	/// ムーブエンドフラグのセット
	/// </summary>
	/// <param name="arg_moveEndFlag">ムーブエンドフラグ</param>
	void SetMoveEndFlag(bool arg_moveEndFlag) { moveEndFlag = arg_moveEndFlag; }

	/// <summary>
	/// ムーブエンドフラグの所得
	/// </summary>
	/// <returns>ムーブエンドフラグ</returns>
	bool GetMoveEndFlag() { return moveEndFlag; }

	/// <summary>
	/// isDeadFlagをtrueにする
	/// </summary>
	void IsDead();

	/// <summary>
	/// 射撃できるかのフラグをセットする
	/// </summary>
	/// <param name="arg_shotFlag">射撃できるかどうかのフラグ</param>
	void SetShotFlag(bool arg_shotFlag) { shotFlag = arg_shotFlag; }

	/// <summary>
	/// 発射までのラグタイム
	/// </summary>
	/// <param name="arg_shotLugTime">ラグタイム</param>
	void SetShotLugTime(int arg_shotLugTime) { shotLugTime = arg_shotLugTime; }

	/// <summary>
	/// ポジションの移動
	/// </summary>
	/// <param name="arg_incrementValue">ポジションの増加量</param>
	void SetScrollIncrement(const Vector3& arg_incrementValue);

	/// <summary>
	/// センターポジションのセット
	/// </summary>
	/// <param name="arg_centerPosition"></param>
	void SetCenterPos(float arg_centerPosition) { centerPosition = arg_centerPosition; }

	/// <summary>
	/// 敵の発射フラグの取得
	/// </summary>
	/// <returns>発射フラグ</returns>
	bool GetLanchFlag() { return lanchFlag; }

	/// <summary>
	/// 敵の発射フラグのセット
	/// </summary>
	/// <param name="arg_lanchFlag">敵の発射フラグのセット</param>
	void SetLanchFlag(bool arg_lanchFlag) { lanchFlag = arg_lanchFlag; }

	/// <summary>
	/// HPのセット
	/// </summary>
	/// <param name="arg_hp">HP</param>
	void SetHP(int arg_hp) { hp = arg_hp; }

	/// <summary>
	/// HPの取得
	/// </summary>
	/// <returns>HP</returns>
	int GetHP() { return hp; }

	/// <summary>
	/// ダメージ処理
	/// </summary>
	void Damage()override;

	/// <summary>
	/// 大ダメージ処理
	/// </summary>
	void CriticalDamage()override;

	/// <summary>
	/// ダメージ演出
	/// </summary>
	void DamageEffect();

	/// <summary>
	/// 死亡演出用のパーティクルのセット
	/// </summary>
	/// <param name="arg_deathParticle">パーティクル</param>
	void SetDeathParticleManager(ParticleManager* arg_deathParticle) { deathParticle.reset(arg_deathParticle); }

	/// <summary>
	/// 死亡演出の発生フラグ
	/// </summary>
	/// <param name="arg_deathParticle">死亡フラグの発生フラグ</param>
	void SetDeathParticleFlag(bool arg_deathParticleFlag) { deathParticleFlag = arg_deathParticleFlag; }

	/// <summary>
	/// 死亡パーティクル処理
	/// </summary>
	void DeathParticleProcessing();

	/// <summary>
	/// 当たり判定
	/// </summary>
	/// <param name="arg_otherObject">相手のオブジェクト</param>
	/// <returns>当たったかどうか</returns>
	bool IsCollision(GameObject* arg_otherObject);

	/// <summary>
	/// 弾の発射処理
	/// </summary>
	/// <param name="arg_incrementValue">ポジションの増加量</param>
	/// <param name="arg_playerPosition">プレイヤーのポジション</param>
	void ShotBullet(const Vector3& arg_incrementValue, const Vector3& arg_playerPosition);
};

