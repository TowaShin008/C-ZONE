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
#include"Bullet.h"
#include"ParticleManager.h"
#include"OBJCharacter.h"
#define BULLETMAXNUM 10
#define BRAKEINTERVAL 30
//地上を走る敵クラス
class TankEnemy :public GameObject
{
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	using XMMATRIX = DirectX::XMMATRIX;

public:
	//コンストラクタ
	TankEnemy(ID3D12GraphicsCommandList* arg_cmdList);

	//デストラクタ
	~TankEnemy();

	/// <summary>
	/// オブジェクトの生成処理
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="position">ポジション</param>
	/// <returns>キャラクターオブジェクト</returns>
	static TankEnemy* Create(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList, Vector3 position = { 0.0f,0.0f,0.0f });


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
	/// モデルのセット
	/// </summary>
	/// <param name="objModel">モデル</param>
	void SetOBJModel(OBJHighModel* arg_objModel, OBJModel* bulletModel,OBJModel* scoreModel);

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
	static void SetCamera(Camera* camera) { TankEnemy::camera = camera; }

	/// <summary>
	/// 敵の弾をリサイズし生成する
	/// </summary>
	void AttachBullet(ID3D12Device* device);

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

	//モデルデータ格納用変数
	OBJHighModel* objModel = nullptr;
	//親クラス
	TankEnemy* parent = nullptr;
	//弾
	std::vector<Bullet*>bullets;
	//再発射するまでの時間
	int shotLugTime = 0;

	//動き出すまでの時間(複数の敵で隊列を組みたいときに使用する)
	int moveLugTime = 0;

	//現在の動きの段階を格納する変数
	MOVEPHASE currentPhase = MOVEPHASE::PHASE1;

	//現在の動きの種類を格納する変数
	MOVETYPE  currentType = MOVETYPE::TRIANGLE;

	bool moveEndFlag = false;
	float curveTime = 0.0f;
	float rotateX = 0.0f;
	float centerPosition = 0.0f;
	int life = 0;
	bool deathParticleFlag = false;
	float brakeCount = 0.0f;

	std::unique_ptr<ParticleManager> deathParticle;
	std::unique_ptr<OBJCharacter> scoreCharacter;
	Vector3 playerPosition = { 0.0f,0.0f,0.0f };

public:
	/// <summary>
	/// 弾丸の配列の取得
	/// </summary>
	/// <returns>弾丸の配列</returns>
	std::vector<Bullet*> GetBullets() { return bullets; }

	/// <summary>
	/// 弾丸の発射処理
	/// </summary>
	void ShotBullet();

	/// <summary>
	/// 定数バッファの転送
	/// </summary>
	void TransferConstBuff();

	/// <summary>
	/// 移動処理
	/// </summary>
	void Move();

	/// <summary>
	/// ブレーキ処理
	/// </summary>
	void Brake();

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
	/// ムーブタイプのセット
	/// </summary>
	/// <param name="moveType">ムーブタイプ</param>
	void SetMoveType(const MOVETYPE& moveType);

	/// <summary>
	/// ムーブタイプの取得
	/// </summary>
	/// <returns>ムーブタイプ</returns>
	MOVETYPE GetMoveType() { return currentType; }

	/// <summary>
	/// 四角形の形に移動する更新処理
	/// </summary>
	void RectAngleMove();

	/// <summary>
	/// 三角形の形に移動する更新処理
	/// </summary>
	void TriAngleMove();


	/// <summary>
	/// 斜めに移動する処理
	/// </summary>
	void LeftDiagonallyMove();

	/// <summary>
	/// 上方向に曲線を描いて移動する処理
	/// </summary>
	void UpCurveMove();

	/// <summary>
	/// 下方向に曲線を描いて移動する処理
	/// </summary>
	void DownCurveMove();

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
	/// <param name="moveEndFlag"></param>
	void SetMoveEndFlag(bool arg_moveEndFlag) { moveEndFlag = arg_moveEndFlag; }

	/// <summary>
	/// ムーブエンドフラグの所得
	/// </summary>
	/// <returns></returns>
	bool GetMoveEndFlag() { return moveEndFlag; }

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
	/// センターポジションのセット
	/// </summary>
	/// <param name="centerPosition">センターポジション</param>
	void SetCenterPos(float arg_centerPosition) { centerPosition = arg_centerPosition; }

	/// <summary>
	/// ダメージ処理
	/// </summary>
	void Damage()override;

	/// <summary>
	/// 大ダメージ処理
	/// </summary>
	void CriticalDamage()override;

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
	bool IsCollision(GameObject* arg_otherObject)override;

	/// <summary>
	/// スコア演出処理
	/// </summary>
	/// <param name="incrementValue">スクロール量</param>
	void ScoreProcessing(Vector3 incrementValue);
};

