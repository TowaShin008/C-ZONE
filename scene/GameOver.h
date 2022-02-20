#pragma once
#include"GameScene.h"
#include"GameObjectManager.h"
#define ENDINGMOVEMAXTIME 500

class GameOver : public GameScene
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	GameOver();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameOver();

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

	/// <summary>
	/// 次のシーンへ
	/// </summary>
	/// <returns>シーン列挙型</returns>
	Scene Next()override;
private:
	std::unique_ptr<GameObjectManager> gameObjectManager;
	std::unique_ptr<Sprite> gameOverTexture;
	std::unique_ptr<Bloom> bloom;

	std::vector<std::vector<std::unique_ptr<Sprite>>>bgHexagon;
	std::vector<std::vector<int>> gradationMap;

	float rotateY = 0.0f;
	int delayTime = 0;
	bool blurFlag = false;
	bool beginFlag = true;
	int endingMoveTime = 0;
	bool pushButtonFlag = false;
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

