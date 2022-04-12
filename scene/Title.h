#pragma once
#include"GameScene.h"
#include"GameObjectManager.h"

class Title : public GameScene
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	Title();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Title();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="arg_score">スコア</param>
	void Initialize(unsigned int arg_score)override;

	/// <summary>
	/// 全てのオブジェクトの生成処理
	/// </summary>
	/// <param name="arg_device">デバイス</param>
	/// <param name="arg_cmdList">コマンドリスト</param>
	void CreateAllObject(ID3D12Device* arg_device, ID3D12GraphicsCommandList* arg_cmdList)override;

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="arg_debugText">デバッグテキスト</param>
	void Update(DebugText* arg_debugText)override;

	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="arg_cmdList">コマンドリスト</param>
	/// <param name="arg_debugText">デバッグテキスト</param>
	void DrawRenderTexture(ID3D12GraphicsCommandList* arg_cmdList, DebugText* arg_debugText)override;

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="arg_cmdList">コマンドリスト</param>
	void PostEffectDraw(ID3D12GraphicsCommandList* arg_cmdList)override;

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="arg_cmdList">コマンドリスト</param>
	/// <param name="arg_debugText">デバッグテキスト</param>
	void Draw(ID3D12GraphicsCommandList* arg_cmdList, DebugText* arg_debugText)override;

	/// <summary>
	/// 次のシーンへ
	/// </summary>
	/// <returns>シーン列挙型</returns>
	Scene Next()override;
private:
	std::unique_ptr<GameObjectManager> gameObjectManager;
	std::unique_ptr<Sprite> titleTexture;
	std::unique_ptr<Sprite> titleIcon;
	std::unique_ptr<Sprite> titleButtomTexture;
	std::unique_ptr<Bloom> bloom;
	std::vector<std::vector<std::unique_ptr<Sprite>>>bgHexagon;
	std::vector<std::vector<int>> gradationMap;
	Vector4 blue = { 0.0f,0.0f,1.0f,1.0f };
	Vector4 yellow = { 1.0f,1.0f,0.0f,1.0f };
	Vector4 red = { 1.0f,0.0f,0.0f,1.0f };
	int delayTime = 0;
	bool blurFlag = false;
	bool beginFlag = true;
	bool pushButtonFlag = false;
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
};

