#pragma once
#include"DirectXBase.h"
#include"Collision.h"
#include"Input.h"
#include"DebugText.h"
#include"Light.h"
#include"LightObject.h"
#include"Audio.h"
#include<time.h>
#include<vector>
#include"Title.h"
#include"Stage1.h"
#include"BossScene1.h"
#include"Stage2.h"
#include"BossScene2.h"
#include"Stage3.h"
#include"BossScene3.h"
#include"GameClear.h"
#include"GameOver.h"
#include"ResultScene.h"
#include"Object3d.h"
#include"MineMath.h"
#include"OBJCharacter.h"
#include"OBJLoader.h"
#include"OBJModel.h"
#include"PostEffect.h"
#include"SceneManager.h"
#include "Bullet.h"
#include"OBJHighModel.h"
#include"OBJHighCharacter.h"
#include"PipelineState.h"
#define ENDTIME 2880
#define UP 2
#define DOWN -2
#define LEFT -1
#define RIGHT 1

class Application
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	Application();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Application();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	/// <summary>
	/// 各パイプラインの生成
	/// </summary>
	void CreatePipeline();

	/// <summary>
	/// オブジェクトの静的初期化
	/// </summary>
	void CameraSetAllObject();

	/// <summary>
	/// シーンの追加
	/// </summary>
	void AddScene();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// Imguiの描画処理
	/// </summary>
	void DrawUI();

	/// <summary>
	/// シーン終了フラグの取得
	/// </summary>
	/// <returns>シーン終了フラグ</returns>
	bool GetEndFlag() { return endFlag; }

	/// <summary>
	/// シーン終了フラグのセット
	/// </summary>
	/// <param name="endFlag">シーン終了フラグ</param>
	void SetEndFlag(bool arg_endFlag) { endFlag = arg_endFlag; }
private:
	DebugText debugText;
	DirectXBase* directXBase = nullptr;
	bool endFlag = false;

	Camera* camera = nullptr;

	SceneManager* sceneManager = nullptr;

	std::unique_ptr<Title> title;
	std::unique_ptr<Stage1> stage;
	std::unique_ptr<BossScene1> bossScene1;
	std::unique_ptr<Stage2> stage2;
	std::unique_ptr<BossScene2> bossScene2;
	std::unique_ptr<Stage3> stage3;
	std::unique_ptr<BossScene3> bossScene3;
	std::unique_ptr<GameClear> gameClear;
	std::unique_ptr<GameOver> gameOver;
	std::unique_ptr<ResultScene> resultScene;
	int score = 0;
private: // 静的メンバ変数
	static const int debugTextTexNumber = 0;
};