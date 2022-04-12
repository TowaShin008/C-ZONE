#pragma once
#include"GameScene.h"
#include"GameObjectManager.h"
#include"MotionBlur.h"
#define ENEMYNUM 10
#define MAXWAVE 5
#define MAXNEXTWAVECOUNT 900
class Stage2 : public GameScene
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Stage2();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Stage2();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="score">スコア</param>
	void Initialize(unsigned int score)override;

	/// <summary>
	/// 全てのオブジェクトの生成処理
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="cmdList">コマンドリスト</param>
	void CreateAllObject(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList)override;

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="input">インプットクラス</param>
	void Update(DebugText* debugText)override;

	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	/// <param name="debugText">デバッグテキスト</param>
	void DrawRenderTexture(ID3D12GraphicsCommandList* cmdList, DebugText* debugText)override;

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	void PostEffectDraw(ID3D12GraphicsCommandList* cmdList)override;

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	/// <param name="debugText">デバッグテキスト</param>
	void Draw(ID3D12GraphicsCommandList* cmdList, DebugText* debugText)override;

	/// <summary>
	/// ウェーブ切り替え処理
	/// </summary>
	void WaveProcessing();

	void StageStartProcessing();

	void Wave1();

	void Wave2();

	void Wave3();

	void Wave4();

	void Wave5();

	/// <summary>
	/// 次のシーンへ
	/// </summary>
	/// <returns>シーン列挙型</returns>
	Scene Next()override;

private:
	std::unique_ptr<GameObjectManager> gameObjectManager;

	std::unique_ptr<ObjFileModel> planeModel;
	std::unique_ptr<ObjFileModel> aircraftCarrierPlaneModel;
	std::unique_ptr<ObjFileModel> enemyPlaneModel;
	std::unique_ptr<ObjFileModel> sphereModel;
	std::unique_ptr<ObjFileModel> missileModel;

	Player* playerObject = nullptr;
	std::vector<NormalEnemy*> normalEnemyObjects;
	std::vector<std::vector<Block*>>blocks;
	std::vector<std::vector<Block*>>blocks_2;
	std::vector<TankEnemy*>tankEnemys;
	AircraftCarrier* aircraftCarrier = nullptr;

	std::unique_ptr<ObjFileModel> yellowBulletModel;
	std::unique_ptr<ObjFileModel> redBulletModel;
	std::unique_ptr<ObjFileModel> tankBulletModel;
	std::unique_ptr<ObjFileModel> blockModel;
	std::unique_ptr<ObjFileModel> scoreModel;
	std::unique_ptr<ObjFileModel> unionModel;
	std::unique_ptr<ObjFileModel> alienModel;
	std::unique_ptr<ObjFileModel> laserModel;

	std::vector<std::vector<short>> map;
	std::vector<std::vector<short>> map_2;

	std::unique_ptr<MotionBlur> motionBlur;
	std::unique_ptr<FadeOut> fadeOut;
	std::unique_ptr<Bloom> bloom;

	std::unique_ptr<Sprite> stage2Texture;
	std::unique_ptr<Sprite> lifeTexture;
	std::unique_ptr<Sprite> gaugeFrame;

	float rotateY = 0.0f;
	int delayTime = 0;
	bool blurFlag = false;
	bool beginFlag = true;
	float incrementValue = 0.02f;
	float scrollPos = incrementValue;
	short wave = 0;
	bool allEnemyDeadFlag = false;
	unsigned int nextWaveCount = 0;
	int chargeCount = 1;
	int particleLugtime = 0;
	int alphaCount = 0;
public:
	/// <summary>
	/// ポストエフェクトの生成
	/// </summary>
	void CreatePostEffect();

	/// <summary>
	/// モデルのロード
	/// </summary>
	void LoadAllModel();

	/// <summary>
	/// マップデータのセット
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="arg_cmdList">コマンドリスト</param>
	void SetMapData(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList);

	/// <summary>
	/// ゲームオブジェクトの生成
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="arg_cmdList">コマンドリスト</param>
	void CreateGameObject(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList);
};

