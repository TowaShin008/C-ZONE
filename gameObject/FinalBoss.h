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
#include"Block.h"
#include"FinalBossEye.h"
#define BULLETMAXNUM 10
#define MAXENEMYLUGTIME 60
#define LANCHMAXTIME 800
#define TENTACLESNUM 40
#define BIGBULLETMAXNUM 3
//ボスクラス
class FinalBoss :public GameObject
{
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	using XMMATRIX = DirectX::XMMATRIX;

public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="arg_cmdList">コマンドリスト</param>
	FinalBoss(ID3D12GraphicsCommandList* arg_cmdList);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~FinalBoss();

	/// <summary>
	/// オブジェクトの生成処理
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="position">ポジション</param>
	/// <returns>キャラクターオブジェクト</returns>
	static FinalBoss* Create(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList, Vector3 position = { 0.0f,0.0f,0.0f });


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
	void Update(const Vector3& incrementValue, const Vector3& playerPosition);

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	void Draw();

	/// <summary>
	/// モデルのセット
	/// </summary>
	/// <param name="objModel">モデル</param>
	void SetOBJModel(OBJModel* eyeModel, OBJModel* bodyModel, OBJModel* wingModel, OBJHighModel* bulletModel);

	/// <summary>
	/// 体のモデルのモデルのセット
	/// </summary>
	/// <param name="bulletModel">モデル</param>
	void SetBodyModel(OBJModel* bodyModel);

	/// <summary>
	/// 目のモデルのモデルのセット
	/// </summary>
	/// <param name="bulletModel">モデル</param>
	void SetEyeModel(OBJModel* eyeModel);

	/// <summary>
	/// 翼のモデルのセット
	/// </summary>
	/// <param name="wingModel"></param>
	void SetWingModel(OBJModel* wingModel);

	/// <summary>
	/// 弾丸モデルのセット
	/// </summary>
	/// <param name="bulletModel">モデル</param>
	void SetBulletModel(OBJHighModel* bulletModel);

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
	static void SetCamera(Camera* camera) { FinalBoss::camera = camera; }

	/// <summary>
	/// 体をリサイズし生成する
	/// </summary>
	void AttachBody(ID3D12Device* device);

	/// <summary>
	/// 目をリサイズし生成する
	/// </summary>
	void AttachEye(ID3D12Device* device);

	/// <summary>
	/// 左翼をリサイズし生成する
	/// </summary>
	void AttachLeftWing(ID3D12Device* device);

	/// <summary>
	/// 右翼をリサイズし生成する
	/// </summary>
	void AttachRightWing(ID3D12Device* device);

	/// <summary>
	/// 弾をリサイズし生成する
	/// </summary>
	void AttachBullet(ID3D12Device* device);
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

	//親クラス
	FinalBoss* parent = nullptr;


	//再発射するまでの時間
	int shotLugTime = 60;

	//現在の動きの段階を格納する変数
	MOVEPHASE currentPhase = MOVEPHASE::PHASE1;

	//現在の動きの種類を格納する変数
	MOVETYPE  currentType = MOVETYPE::TRIANGLE;

	OBJModel* bodyModel = nullptr;
	Block* bodyBlock = nullptr;

	FinalBossEye* finalBossEye = nullptr;

	Block* parentLeftWing = nullptr;
	std::vector<Block*>leftWing;

	Block* parentRightWing = nullptr;
	std::vector<Block*>rightWing;

	bool moveEndFlag = false;
	float curveTime = 0.0f;
	bool shotFlag = true;
	float centerPosition = 0.0f;
	int hp = 400;

	std::unique_ptr<ParticleManager> deathParticle;
	bool deathParticleFlag = false;

	MOVEPHASE bodyAttackPhase = MOVEPHASE::PHASE1;
	Vector3 previousParentLeftWingRotation = { 0,0,0 };
	Vector3 previousParentRightWingRotation = { 0,0,0 };

	float attackFlag = false;
	int rnd = 0;
	float tentacleAngle = 0.0f;
	float bodyAngle = 0.0f;
	float homePositionX = 20.0f;
	bool blinkFlag = false;
	float blinkAngle = 0.0f;
	bool entryFlag = true;
	bool bodyAttackFlag = false;

	std::vector<HomingBullet*> bigBullets;
public:
	/// <summary>
	/// 定数バッファの転送
	/// </summary>
	void TransferConstBuff();

	/// <summary>
	/// チャージ弾の取得
	/// </summary>
	/// <returns>チャージ弾</returns>
	std::vector<HomingBullet*> GetBigBullet() { return bigBullets; }

	/// <summary>
	///　左翼の親オブジェクトの取得
	/// </summary>
	Block* GetLeftParentWing() { return parentLeftWing; }

	/// <summary>
	/// 右翼の親オブジェクトの取得
	/// </summary>
	Block* GetRightParentWing() { return parentRightWing; }
	/// <summary>
	/// ムーブフェイズのセット
	/// </summary>
	/// <param name="movePhase">ムーブフェイズ</param>
	void SetMovePhase(const MOVEPHASE& movePhase) { currentPhase = movePhase; }

	/// <summary>
	/// ムーブフェイズの取得
	/// </summary>
	/// <returns>ムーブフェイズ</returns>
	MOVEPHASE GetMovePhase() { return currentPhase; }

	/// <summary>
	/// ムーブタイプの取得
	/// </summary>
	/// <returns>ムーブタイプ</returns>
	MOVETYPE GetMoveType() { return currentType; }

	/// <summary>
	/// 移動処理
	/// </summary>
	void Move();

	/// <summary>
	/// ある程度進んで停止する処理処理
	/// </summary>
	/// <param name="incrementValue">スクロール量</param>
	/// <param name="playerPosition">プレイヤーポジション</param>
	void StayMove();

	/// <summary>
	/// ムーブエンドフラグのセット
	/// </summary>
	/// <param name="moveEndFlag">ムーブエンドフラグ</param>
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
	/// <param name="shotFlag">射撃できるかどうかのフラグ</param>
	void SetShotFlag(bool arg_shotFlag) { shotFlag = arg_shotFlag; }

	/// <summary>
	/// 発射までのラグタイム
	/// </summary>
	/// <param name="arg_shotLugTime">ラグタイム</param>
	void SetShotLugTime(int arg_shotLugTime) { shotLugTime = arg_shotLugTime; }

	/// <summary>
	/// 体当たりフラグのセット
	/// </summary>
	/// <param name="arg_bodyAttackFlag">体当たりフラグ</param>
	void SetBodyAttackFlag(bool arg_bodyAttackFlag) { bodyAttackFlag = arg_bodyAttackFlag; }

	/// <summary>
	/// ポジションの移動
	/// </summary>
	/// <param name="incrementValue">ポジションの増加量</param>
	void SetScrollIncrement(const Vector3& incrementValue);

	/// <summary>
	/// センターポジションのセット
	/// </summary>
	/// <param name="centerPosition"></param>
	void SetCenterPos(float arg_centerPosition) { centerPosition = arg_centerPosition; }

	/// <summary>
	/// HPのセット
	/// </summary>
	/// <param name="hp">HP</param>
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
	/// <param name="deathParticle">パーティクル</param>
	void SetDeathParticleManager(ParticleManager* arg_deathParticle) { deathParticle.reset(arg_deathParticle); }

	/// <summary>
	/// 死亡演出の発生フラグ
	/// </summary>
	/// <param name="deathParticle">死亡フラグの発生フラグ</param>
	void SetDeathParticleFlag(bool arg_deathParticleFlag) { deathParticleFlag = arg_deathParticleFlag; }

	/// <summary>
	/// 死亡パーティクル処理
	/// </summary>
	void DeathParticleProcessing();

	/// <summary>
	/// 当たり判定
	/// </summary>
	/// <param name="otherObject">相手のオブジェクト</param>
	/// <returns>当たったかどうか</returns>
	bool IsCollision(GameObject* arg_otherObject);

	FinalBossEye* GetCollisionEye() { return finalBossEye; }

	/// <summary>
	/// 攻撃フラグのセット
	/// </summary>
	/// <param name="arg_attackFlag"></param>
	void SetAttackFlag(bool arg_attackFlag) { attackFlag = arg_attackFlag; }

	/// <summary>
	/// 呼吸演出
	/// </summary>
	void BreathMove();

	/// <summary>
	/// 射撃処理
	/// </summary>
	void ShotBullet(const Vector3& incrementValue, const Vector3& playerPosition);

	/// <summary>
	/// 瞬き処理
	/// </summary>
	void Blink();

	/// <summary>
	/// 登場演出
	/// </summary>
	void EntryPerformance();

	/// <summary>
	/// 初期姿勢
	/// </summary>
	void InitialPosture();

	/// <summary>
	/// 通常姿勢
	/// </summary>
	void NormalPosture();

	/// <summary>
	/// 死亡演出
	/// </summary>
	void EndPerformance();

	/// <summary>
	/// 体当たり攻撃処理
	/// </summary>
	void BodyAttack();
};

