#pragma once
#include"GameScene.h"
#include"GameObjectManager.h"

class ResultScene : public GameScene
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	ResultScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~ResultScene();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(unsigned int arg_score)override;

	/// <summary>
	/// 全てのオブジェクトの生成処理
	/// </summary>
	/// <param name="device">デバイス</param>
	void CreateAllObject(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList)override;

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="input">インプットクラス</param>
	void Update(DebugText* debugText)override;

	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="cmdList">コマンド処理</param>
	void DrawRenderTexture(ID3D12GraphicsCommandList* cmdList, DebugText* debugText)override;

	/// <summary>
	/// ポスト描画処理
	/// </summary>
	/// <param name="cmdList">コマンド処理</param>
	void PostEffectDraw(ID3D12GraphicsCommandList* cmdList)override;

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="cmdList">コマンド処理</param>
	void Draw(ID3D12GraphicsCommandList* cmdList, DebugText* debugText)override;

	void SceneProcessing(DebugText* debugText);

	void Scene1();

	void Scene2();

	void Scene3(DebugText* debugText);


	/// <summary>
	/// 全てのオブジェクトの削除処理
	/// </summary>
	void DeleteAllObject()override;

	/// <summary>
	/// 次のシーンへ
	/// </summary>
	/// <returns>シーン列挙型</returns>
	Scene Next()override;
private:
	GameObjectManager* gameObjectManager = nullptr;
	Sprite* windowTexture = nullptr;
	Sprite* stage_clearTexture = nullptr;
	Bloom* bloom = nullptr;
	OBJHighModel* playerModel = nullptr;
	OBJHighCharacter* playerObject = nullptr;
	int delayTime = 0;
	bool blurFlag = false;
	bool beginFlag = true;
	bool pushButtonFlag = false;
	int sceneWave = 1;
	float easingTime = 0.0f;
	float windowTextureAlpha = 0.0f;
	int blinkCount = 0;
public:
	/// <summary>
	/// ポストエフェクトの生成
	/// </summary>
	void CreatePostEffect();
	/// <summary>
	/// レーザーゲージ処理
	/// </summary>
	void LaserGaugeProcessing();
	/// <summary>
	/// モデルのロード
	/// </summary>
	void LoadAllModel();
};

