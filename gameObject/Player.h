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
#include"Missile.h"
#include"Laser.h"
#include"UnionCharacter.h"
#include"ParticleManager.h"

#define BULLETMAXNUM 10
#define MISSILEMAXNUM 4
#define MAXPLAYERLUGTIME 30
#define SPAWN 10
#define CHARGEMAXTIME 200
#define CHARGELUTIME 5
//プレイヤークラス
class Player :public GameObject
{
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	using XMMATRIX = DirectX::XMMATRIX;

public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="arg_cmdList">コマンドリスト</param>
	Player(ID3D12GraphicsCommandList* arg_cmdList);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player();

	/// <summary>
	/// オブジェクトの生成処理
	/// </summary>
	/// <param name="arg_device">デバイス</param>
	/// <param name="arg_cmdList">コマンドリスト</param>
	/// <param name="arg_position">ポジション</param>
	/// <returns>キャラクターオブジェクト</returns>
	static Player* Create(ID3D12Device* arg_device,ID3D12GraphicsCommandList* arg_cmdList,const Vector3& arg_position = { 0.0f,0.0f,0.0f });


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
	void Update(const Vector3& arg_incrementValue);

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	void Draw();

	/// <summary>
	/// モデルのセット
	/// </summary>
	/// <param name="arg_objModel">自機のモデル</param>
	/// <param name="arg_bulletModel">自機から出る弾のモデル</param>
	/// <param name="arg_unionModel">子機のモデル</param>
	/// <param name="arg_uBulletmodel">子機から出る弾のモデル</param>
	/// <param name="arg_laserModel">レーザーのモデル</param>
	void SetOBJModel(OBJHighModel* arg_objModel, OBJModel* arg_bulletModel,OBJHighModel* arg_unionModel,OBJModel* arg_uBulletmodel,OBJHighModel* arg_laserModel,OBJHighModel* arg_missileModel);

	/// <summary>
	/// 弾丸モデルのセット
	/// </summary>
	/// <param name="arg_bulletModel">モデル</param>
	void SetBulletModel(OBJModel* arg_bulletModel);

	/// <summary>
	/// レーザーモデルのセット
	/// </summary>
	/// <param name="arg_bulletModel">モデル</param>
	void SetLaserModel(OBJHighModel* arg_laserModel);

	/// <summary>
	/// ミサイルモデルのセット
	/// </summary>
	/// <param name="arg_bulletModel">モデル</param>
	void SetMissileModel(OBJHighModel* arg_missileModel);

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
	static void SetCamera(Camera* arg_camera) { Player::camera = arg_camera; }

	/// <summary>
	/// 弾をリサイズし生成する
	/// </summary>
	/// <param name="arg_device">デバイス</param>
	void AttachBullet(ID3D12Device* arg_device);

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
	Player* parent = nullptr;
	std::vector<Bullet*>bullets;
	std::unique_ptr<Bullet> chargeBullet;
	std::unique_ptr<Laser> laser;
	std::vector<Missile*>missiles;
	int shotLugTime = MAXPLAYERLUGTIME;
	int shotMissileLugTime = MAXPLAYERLUGTIME;
	std::unique_ptr<UnionCharacter> unionCharacter;
	int spawnTime = 0;
	std::unique_ptr<ParticleManager> deathParticle;
	std::unique_ptr<ParticleManager> chargeParticleManager;
	std::unique_ptr<ParticleManager> chargeMaxParticleManager;
	std::unique_ptr<ParticleManager> chargeBulletParticleManager;
	std::unique_ptr<ParticleManager> jetParticleManager;
	Vector3 deathPosition = { 0,0,0 };
	bool deathParticleFlag = false;
	int life = 0;
	int chargeCount = 1;
	int particleLugtime = 0;
	float centerPosition = 0.0f;
	bool bossSceneFlag = false;
	int laserEnergy = 0;
	bool isAbleToChargeLaser = true;
	int chargeLugTime = 0;
	bool shotFlag = true;
	bool moveFlag = true;
	bool spawnFlag = false;
public:
	/// <summary>
	/// 銃弾の配列の取得
	/// </summary>
	/// <returns>銃弾の配列</returns>
	std::vector<Bullet*> GetBullets() { return bullets; }

	/// <summary>
	/// チャージ弾の取得
	/// </summary>
	/// <returns>チャージ弾</returns>
	Bullet* GetChargeBullet() { return chargeBullet.get(); }

	/// <summary>
	/// ミサイルの配列の取得
	/// </summary>
	/// <returns>銃弾の配列</returns>
	std::vector<Missile*> GetMissiles() { return missiles; }

	/// <summary>
	/// ユニオンの銃弾の配列の取得
	/// </summary>
	/// <returns>ユニオンの銃弾の配列</returns>
	std::vector<Bullet*> GetUnionBullets() { return unionCharacter->GetBullets(); }

	/// <summary>
	/// ユニオンのセット
	/// </summary>
	/// <param name="arg_unionCharacter">ユニオンのポインタ</param>
	void SetUnionCharacter(UnionCharacter* arg_unionCharacter) { unionCharacter.reset(arg_unionCharacter); }

	/// <summary>
	/// ユニオンの取得
	/// </summary>
	/// <returns>ユニオンのポインタ</returns>
	UnionCharacter* GetUnionCharacter() { return unionCharacter.get(); }

	/// <summary>
	/// パーティクルのセット
	/// </summary>
	/// <param name="arg_particleMan">パーティクルマネージャー</param>
	void SetChargePerticleManager(ParticleManager* arg_particleManager) { chargeParticleManager.reset(arg_particleManager); }

	/// <summary>
	/// パーティクルのセット
	/// </summary>
	/// <param name="arg_particleMan">パーティクルマネージャー</param>
	void SetChargeMaxPerticleManager(ParticleManager* arg_particleMaxManager) { chargeMaxParticleManager.reset(arg_particleMaxManager); }

	/// <summary>
	/// パーティクルのセット
	/// </summary>
	/// <param name="arg_particleMan">パーティクルマネージャー</param>
	void SetChargeBulletPerticleManager(ParticleManager* arg_particleBulletManager) { chargeBulletParticleManager.reset(arg_particleBulletManager); }


	/// <summary>
	/// パーティクルのセット
	/// </summary>
	/// <param name="arg_particleMan">パーティクルマネージャー</param>
	void SetJetPerticleManager(ParticleManager* arg_jetParticletManager) { jetParticleManager.reset(arg_jetParticletManager); }

	/// <summary>
	/// ポジションの移動
	/// </summary>
	/// <param name="arg_incrementValue">ポジションの増加量</param>
	void SetScrollIncrement(const Vector3& arg_incrementValue);

	/// <summary>
	/// moveFlagのセット
	/// </summary>
	/// <param name="arg_moveFlag">ムーブフラグ</param>
	void SetMoveFlag(bool arg_moveFlag) { moveFlag = arg_moveFlag; }

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
	/// ボス戦への切り替え
	/// </summary>
	/// <param name="arg_bossSceneFlag">ボス戦になっているかどうか</param>
	void SetBossSceneFlag(bool arg_bossSceneFlag);

	/// <summary>
	/// ショットフラグのセット
	/// </summary>
	/// <param name="arg_shotFlag">ショットフラグ</param>
	void SetShotFlag(bool arg_shotFlag);

	/// <summary>
	/// ショットフラグの取得
	/// </summary>
	/// <returns></returns>
	bool GetShotFlag() { return shotFlag; }

	/// <summary>
	/// 弾の発射処理
	/// </summary>
	/// <param name="arg_incrementValue">スクロール量</param>
	void ShotBullet(const Vector3& arg_incrementValue);

	/// <summary>
	/// チャージ弾の発射処理
	/// </summary>
	/// <param name="arg_incrementValue">スクロール量</param>
	void ShotChargeBullet(const Vector3& arg_incrementValue);

	/// <summary>
	/// レーザーの発射処理
	/// </summary>
	/// <param name="arg_incrementValue">スクロール量</param>
	void ShotLaser(const Vector3& incrementValue);

	/// <summary>
	/// レーザーの発射処理
	/// </summary>
	/// <param name="arg_incrementValue">スクロール量</param>
	void ShotMissile(const Vector3& arg_incrementValue);

	/// <summary>
	/// レーザーエネルギーの取得
	/// </summary>
	/// <returns>レーザーエネルギー</returns>
	int GetLaserEnergy() { return laserEnergy; }

	/// <summary>
	/// 定数バッファの転送
	/// </summary>
	void TransferConstBuff();

	/// <summary>
	/// 移動処理
	/// </summary>
	/// <param name="arg_incrementValue"></param>
	void Move(const Vector3& arg_incrementValue);

	/// <summary>
	/// HPを１減らす
	/// </summary>
	void Damage()override;

	/// <summary>
	/// 大ダメージ処理
	/// </summary>
	void CriticalDamage()override;

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
	bool IsCollision(GameObject* arg_otherObject)override;

	/// <summary>
	/// レーザーの死亡フラグの取得
	/// </summary>
	/// <returns>死亡判定</returns>
	bool GetLaserIsDead() { return laser->GetIsDeadFlag(); }

	/// <summary>
	/// 出現演出
	/// </summary>
	/// <returns>定位置にたどり着いたか</returns>
	bool SpawnProcessing();

	/// <summary>
	/// スポーンフラグのセット
	/// </summary>
	/// <param name="arg_spawnFlag"></param>
	void SetSpawnFlag(bool arg_spawnFlag) { spawnFlag = arg_spawnFlag; }
	
	/// <summary>
	/// 攻撃手段の更新処理
	/// </summary>
	/// <param name="arg_incrementValue">スクロール量</param>
	void UpdateAttack(const Vector3& arg_incrementValue);
};

