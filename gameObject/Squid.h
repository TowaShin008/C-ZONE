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
#include"SquidEye.h"
#define BULLETMAXNUM 10
#define MAXENEMYLUGTIME 60
#define LANCHMAXTIME 800
#define TENTACLESNUM 40
//ボスクラス
class Squid :public GameObject
{
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	using XMMATRIX = DirectX::XMMATRIX;

public:
	//コンストラクタ
	Squid(ID3D12GraphicsCommandList* arg_cmdList);

	//デストラクタ
	~Squid();

	/// <summary>
	/// オブジェクトの生成処理
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="position">ポジション</param>
	/// <returns>キャラクターオブジェクト</returns>
	static Squid* Create(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList, Vector3 position = { 0.0f,0.0f,0.0f });


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
	void SetOBJModel(OBJModel* eyeModel, OBJModel* bodyModel);

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

	void SetTentaclesModel(OBJModel* tentacleModel);

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
	static void SetCamera(Camera* arg_camera) { Squid::camera = arg_camera; }

	/// <summary>
	/// 体をリサイズし生成する
	/// </summary>
	void AttachBody(ID3D12Device* device);

	/// <summary>
	/// 触手をリサイズし生成する
	/// </summary>
	void AttachTentacles(ID3D12Device* device);

	/// <summary>
	/// 目をリサイズし生成する
	/// </summary>
	void AttachEye(ID3D12Device* device);
private:
	UINT descpriptorSize;
	static ID3D12GraphicsCommandList* cmdList;
	ComPtr<ID3D12Resource>constBuffB0;
	ComPtr<ID3D12Resource>constBuffB1;

	//カメラ関係
	// 
	//ビュー行列
	static XMMATRIX matView;
	//プロジェクション行列
	static XMMATRIX matProjection;
	//カメラクラス
	static Camera* camera;

	//親クラス
	Squid* parent = nullptr;

	//動き出すための時間(複数の敵で隊列を組みたいときに使用する)
	int moveLugTime = 0;

	//現在の動きの段階を格納する変数
	MOVEPHASE currentPhase = MOVEPHASE::PHASE1;

	//現在の動きの種類を格納する変数
	MOVETYPE  currentType = MOVETYPE::TRIANGLE;

	OBJModel* bodyModel = nullptr;
	std::vector<std::vector<Block*>>bodyBlocks;
	std::vector<Block*>tentaclesBlocks;
	SquidEye* leftEye = nullptr;
	SquidEye* rightEye = nullptr;

	bool moveEndFlag = false;
	float rotateX = 0.0f;
	bool shotFlag = true;
	float centerPosition = 0.0f;
	int hp = 400;

	std::unique_ptr<ParticleManager> deathParticle;
	bool deathParticleFlag = false;

	std::vector<std::vector<short>> map;

	MOVEPHASE attackPhase = MOVEPHASE::PHASE1;

	float attackFlag = false;
	int rnd = 0;
	float tentacleAngle = 0.0f;
	float bodyAngle = 0.0f;
	float homePositionX = 20.0f;
	bool upTentacleAttackFlag = false;
	float eyeRotationCount = 0.0f;
	float eyeScaleCount = 0;
public:
	/// <summary>
	/// 定数バッファの転送
	/// </summary>
	void TransferConstBuff();

	/// <summary>
	/// 移動処理
	/// </summary>
	/// <param name="playerPosition">プレイヤーポジション</param>
	void Move(const Vector3& playerPosition);

	/// <summary>
	/// ムーブフェイズのセット
	/// </summary>
	/// <param name="movePhase">ムーブフェイズ</param>
	void SetMovePhase(const MOVEPHASE& arg_currentPhase) { currentPhase = arg_currentPhase; }

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
	/// ある程度進んで停止する処理処理
	/// </summary>
	void StayMove(const Vector3& playerPosition);

	/// <summary>
	/// ムーブラグタイムのセット
	/// </summary>
	/// <param name="moveLugTime">ムーブラグタイム</param>
	void SetMoveLugTime(int arg_moveLugTime) { moveLugTime = arg_moveLugTime; }

	/// <summary>
	/// ムーブラグタイムの取得
	/// </summary>
	/// <returns>ムーブラグタイム</returns>
	int GetMoveLugTime() { return moveLugTime; }

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

	SquidEye* GetLeftSquidEye() { return leftEye; }

	SquidEye* GetRightSquidEye() { return rightEye; }

	std::vector<Block*> GetTentacle() { return tentaclesBlocks; }

	/// <summary>
	/// 上からの触手攻撃
	/// </summary>
	void UpTentacleAttack();

	/// <summary>
	/// 下からの触手攻撃
	/// </summary>
	void DownTentacleAttack();

	/// <summary>
	/// アタックフラグのセット
	/// </summary>
	/// <param name="arg_attackFlag">アタックフラグ</param>
	void SetAttackFlag(bool arg_attackFlag) { attackFlag = arg_attackFlag; }

	/// <summary>
	/// 触手のうねり処理
	/// </summary>
	void TentacleMove();

	/// <summary>
	/// 水に浮いているような演出の処理
	/// </summary>
	void FloatMove();

	/// <summary>
	/// 呼吸の演出処理
	/// </summary>
	void BreathMove();
};

