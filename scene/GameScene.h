#pragma once
#include<d3d12.h>
#include"Sprite.h"
#include<vector>
#include"Input.h"
#include"ParticleManager.h"
#include"DirectXBase.h"
#include"OBJCharacter.h"
#include"OBJLoader.h"
#include"Camera.h"
#include"Player.h"
#include"NormalEnemy.h"
#include"Alien.h"
#include"Block.h"
#include"UnionCharacter.h"
#include"DebugText.h"
#include"AircraftCarrier.h"
#include"Boss.h"
#include"Squid.h"
#include"FinalBoss.h"
#include"FinalBossEye.h"
#include"FadeOut.h"
#include"Bloom.h"
#include"GlassNormalMap.h"
#include"DropNormalMap.h"
#include"NoisePostEffect.h"
#include"Model.h"
#include"object3d.h"
#include"FbxLoader.h"
#include"OBJHighModel.h"
#include"OBJHighCharacter.h"
#include"TankEnemy.h"
#include<memory>
using namespace Microsoft::WRL;
enum class Scene
{
	TITLE,
	STAGE1,
	BOSS1,
	STAGE2,
	BOSS2,
	STAGE3,
	BOSS3,
	GAMECLEAR,
	GAMEOVER,
	RESULT,
};

class GameScene
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	GameScene();

	/// <summary>
	/// 仮想デストラクタ
	/// </summary>
	virtual ~GameScene();

	/// <summary>
	/// 初期化処理
	/// </summary>
	virtual void Initialize(unsigned int arg_score);

	/// <summary>
	/// 全てのオブジェクトの生成処理
	/// </summary>
	/// <param name="device"></param>
	virtual void CreateAllObject(ID3D12Device* device,ID3D12GraphicsCommandList* arg_cmdList);

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="input">インプットクラス</param>
	virtual void Update(DebugText* debugText);

	/// <summary>
	/// ポスト描画前処理
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	virtual void DrawRenderTexture(ID3D12GraphicsCommandList* cmdList, DebugText* debugText);

	/// <summary>
	/// ポスト描画処理
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	virtual void PostEffectDraw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	virtual void Draw(ID3D12GraphicsCommandList* cmdList, DebugText* debugText);

	/// <summary>
	/// 全てのオブジェクトの削除処理
	/// </summary>
	virtual void DeleteAllObject();

	/// <summary>
	/// カメラのセット
	/// </summary>
	/// <param name="camera">カメラオブジェクト</param>
	void SetCamera(Camera* camera) { this->camera = camera; }

	/// <summary>
	/// シーン終了フラグのセット
	/// </summary>
	/// <param name="endFlag">シーン終了フラグ</param>
	void SetEndFlag(bool endFlag) { this->endFlag = endFlag; }

	/// <summary>
	/// シーン終了フラグの取得
	/// </summary>
	/// <returns>シーン終了フラグ</returns>
	bool GetEndFlag() { return endFlag; }

	/// <summary>
	/// スコアを取得
	/// </summary>
	/// <returns></returns>
	unsigned int GetScore() { return score; }

	/// <summary>
	/// 次のシーンへ
	/// </summary>
	/// <returns>シーン列挙型</returns>
	virtual Scene Next();

	Scene GetPreviousScene() { return previousScene; }

	void SetPreviousScene(Scene arg_previousScene) { previousScene = arg_previousScene; }

	Scene GetSceneName() { return sceneName; }

	void SetSceneName(Scene arg_sceneName) { sceneName = arg_sceneName; }

protected:
	Camera* camera = nullptr;
	bool endFlag = false;
	unsigned int score = 0;
	Scene previousScene = Scene::TITLE;
	Scene sceneName = Scene::TITLE;
};

