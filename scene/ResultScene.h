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
	/// <param name="debugText">デバッグテキスト</param>
	void SceneProcessing(DebugText* debugText);

	void Scene1();

	void Scene2();

	void Scene3(DebugText* debugText);

	/// <summary>
	/// 次のシーンへ
	/// </summary>
	/// <returns>シーン列挙型</returns>
	Scene Next()override;
private:
	std::unique_ptr<GameObjectManager> gameObjectManager;
	std::unique_ptr<Sprite> windowTexture;
	std::unique_ptr<Sprite> stage_clearTexture;
	std::unique_ptr<Bloom> bloom;
	std::unique_ptr<ObjFileModel> playerModel;
	ObjFileCharacter* playerObject = nullptr;
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
	/// モデルのロード
	/// </summary>
	void LoadAllModel();
};

