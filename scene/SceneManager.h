#pragma once
#include<map>
#include"GameScene.h"
class SceneManager
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	SceneManager(ID3D12Device* arg_device, ID3D12GraphicsCommandList* arg_cmdList);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~SceneManager();
	
	/// <summary>
	/// シーン追加
	/// </summary>
	/// <param name="name">シーン名</param>
	/// <param name="scene">シーンオブジェクト</param>
	/// <param name="camera">カメラ</param>
	void Add(const Scene& name, GameScene* scene,Camera* camera);


	/// <summary>
	/// シーンチェンジ
	/// </summary>
	/// <param name="name">シーン名</param>
	void ChangeScene(const Scene& name);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(DebugText* debugText);

	/// <summary>
	/// ポスト描画前処理
	/// </summary>
	void DrawRenderTexture(DebugText* debugText);

	/// <summary>
	/// ポスト描画処理
	/// </summary>
	void PostEffectDraw();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw(DebugText* debugText);

private:
	std::map<Scene, GameScene*>scenes;
	//現在のシーン
	GameScene* currentScene = nullptr;

	ID3D12Device* device = nullptr;
	ID3D12GraphicsCommandList* cmdList = nullptr;

	unsigned int score = 0;
public:
	bool IsCurrentScene(const Scene& name);
};

