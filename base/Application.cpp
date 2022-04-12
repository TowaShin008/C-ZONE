#include "Application.h"
#include <cassert>
#include <sstream>
#include <iomanip>
#include"FbxLoader.h"
#include <iostream>

//コンストラクタ
Application::Application()
{
}

//デストラクタ
Application::~Application()
{
	//終了処理
	directXBase->EndWindow();
	//削除処理
	Input::Destroy();
	delete directXBase;
	Audio::Destroy();

	delete sceneManager;

	FbxLoader::GetInstance()->Finalize();

	delete camera;
}

//初期化処理
void Application::Initialize()
{
	//インプットクラス生成処理
	Input::Create();
	//オーディオクラス生成処理
	Audio::Create();
	//DirectXBaseクラス生成処理
	directXBase = new DirectXBase;

	directXBase->CreateAllObject();

	//インプットクラス初期化処理
	Input::GetInstance()->Initialize(directXBase->GetWndClass(), directXBase->Gethwnd());
	//オーディオクラス初期化処理
	Audio::GetInstance()->Initialize();

	ID3D12Device* device = directXBase->GetDevice().Get();

	//各パイプラインの生成
	CreatePipeline();

	//FBXLoader初期化処理
	FbxLoader::GetInstance()->Create(device);
	//カメラクラス生成処理
	camera = new Camera(WindowSize::window_width, WindowSize::window_height);
	//スプライトクラスのデスクリプターヒープを生成
	Sprite::CreateDescriptorHeap(device);
	ObjFileModel::SetDevice(device);

	if (!Sprite::LoadTexture(L"Resources/debugfont_digital_L.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(L"Resources/title.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(L"Resources/stageclear.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(L"Resources/gameclear.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(L"Resources/gameover.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(L"Resources/warning.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(L"Resources/warningBar.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(L"Resources/white_hexagon_s.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(L"Resources/push_space.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(L"Resources/stage1.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(L"Resources/stage2.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(L"Resources/stage3.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(L"Resources/window.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(L"Resources/player_texture.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(L"Resources/score.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(L"Resources/titleicon.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(L"Resources/gauge_frame.png")) {
		assert(0);
		return;
	}

	//デバッグテキストの初期化処理
	debugText.Initialize(debugTextTexNumber);

	//カメラターゲットセット
	camera->SetTarget({ 0,0,0 });
	camera->SetEye({ 0,0,-20.2f });

	//ゲームオブジェクトの静的初期化処理
	CameraSetAllObject();

	//シーンの生成(各シーンはシーンマネージャクラスでデリートされるためデリート処理は記述しなくてよい)
	title.reset(new Title());
	stage.reset(new Stage1());
	stage2.reset(new Stage2());
	stage3.reset(new Stage3());
	bossScene1.reset(new BossScene1());
	bossScene2.reset(new BossScene2());
	bossScene3.reset(new BossScene3());
	gameClear.reset(new GameClear());
	gameOver.reset(new GameOver());
	resultScene.reset(new ResultScene());

	//シーンマネージャクラスの生成
	sceneManager = new SceneManager(device,directXBase->GetCmdList().Get());
	//シーンマネージャーへの追加(追加するにはGameScene.h内のenum Sceneに追加すること)
	AddScene();
}
//全てのオブジェクトに共通のカメラをセット(新しくオブジェクトクラスを実装した場合はここでカメラをセットするのを忘れずに)
void Application::CameraSetAllObject()
{
	ObjFileCharacter::SetCamera(camera);

	Player::SetCamera(camera);
	UnionCharacter::SetCamera(camera);
	Bullet::SetCamera(camera);
	Laser::SetCamera(camera);
	Missile::SetCamera(camera);

	NormalEnemy::SetCamera(camera);
	TankEnemy::SetCamera(camera);
	Alien::SetCamera(camera);
	AircraftCarrier::SetCamera(camera);
	Boss::SetCamera(camera);
	Squid::SetCamera(camera);
	SquidEye::SetCamera(camera);
	FinalBoss::SetCamera(camera);
	FinalBossEye::SetCamera(camera);
	HomingBullet::SetCamera(camera);

	Block::SetCamera(camera);
}

void Application::CreatePipeline()
{
	ID3D12Device* device = directXBase->GetDevice().Get();
	//影なしのオブジェクト用のパイプライン生成
	RootSignature::InitializeSimpleRootSignature(device);
	PipelineState::InitializeSimplePipelineState(device);
	//通常オブジェクト用のパイプライン生成
	RootSignature::InitializeBasicRootSignature(device);
	PipelineState::InitializeBasicPipelineState(device);
	//スプライト用のパイプライン生成
	RootSignature::InitializeSpriteRootSignature(device);
	PipelineState::InitializeSpritePipelineState(device);
	//スライドするスプライト用のパイプライン生成
	RootSignature::InitializeSlideSpriteRootSignature(device);
	PipelineState::InitializeSlideSpritePipelineState(device);
	//ブルームスプライト用のパイプライン生成
	RootSignature::InitializeBloomSpriteRootSignature(device);
	PipelineState::InitializeBloomSpritePipelineState(device);
	//FBX用のパイプライン生成
	RootSignature::InitializeFBXRootSignature(device);
	PipelineState::InitializeFBXPipelineState(device);
	//ライトありオブジェクト用のパイプライン生成
	RootSignature::InitializeLightObjectRootSignature(device);
	PipelineState::InitializeLightObjectPipelineState(device);
	//弾丸用のパイプライン生成
	RootSignature::InitializeBulletRootSignature(device);
	PipelineState::InitializeBulletPipelineState(device);
	//パーティクル用のパイプライン生成
	RootSignature::InitializeParticleRootSignature(device);
	PipelineState::InitializeParticlePipelineState(device);
	//縮小バッファ用のパイプライン生成
	RootSignature::InitializeShrinkTextureRootSignature(device);
	PipelineState::InitializeShrinkTexturePipelineState(device);
	//ブルーム用のパイプライン生成
	RootSignature::InitializeBloomPostEffectRootSignature(device);
	PipelineState::InitializeBloomPostEffectPipelineState(device);
	//ガウシアンブラー用のパイプライン生成
	PipelineState::InitializeGaussianPostEffectPipelineState(device);
	//フェードアウト用のパイプライン生成
	RootSignature::InitializeFadeOutPostEffectRootSignature(device);
	PipelineState::InitializeFadeOutPostEffectPipelineState(device);
	//水滴用のパイプライン生成
	RootSignature::InitializeDropNormalMapPostEffectRootSignature(device);
	PipelineState::InitializeDropNormalMapPostEffectPipelineState(device);
	//ひび割れ用のパイプライン生成
	RootSignature::InitializeGlassNormalMapPostEffectRootSignature(device);
	PipelineState::InitializeGlassNormalMapPostEffectPipelineState(device);
	//スキャンノイズ用のパイプライン生成
	RootSignature::InitializeNoisePostEffectRootSignature(device);
	PipelineState::InitializeNoisePostEffectPipelineState(device);
	//ポストエフェクト用のパイプライン生成
	RootSignature::InitializePostEffectRootSignature(device);
	PipelineState::InitializePostEffectPipelineState(device);
	//ディゾルブ用のパイプライン生成
	RootSignature::InitializeDissolvePostEffectRootSignature(device);
	PipelineState::InitializeDissolvePostEffectPipelineState(device);
}

void Application::AddScene()
{
	sceneManager->Add(Scene::TITLE    , title.get()      , camera);
	sceneManager->Add(Scene::STAGE1   , stage.get()      , camera);
	sceneManager->Add(Scene::BOSS1    , bossScene1.get() , camera);
	sceneManager->Add(Scene::STAGE2   , stage2.get()     , camera);
	sceneManager->Add(Scene::BOSS2    , bossScene2.get() , camera);
	sceneManager->Add(Scene::STAGE3   , stage3.get()     , camera);
	sceneManager->Add(Scene::BOSS3    , bossScene3.get() , camera);
	sceneManager->Add(Scene::GAMECLEAR, gameClear.get()  , camera);
	sceneManager->Add(Scene::GAMEOVER , gameOver.get()   , camera);
	sceneManager->Add(Scene::RESULT   , resultScene.get(), camera);
}

//更新処理
void Application::Update()
{
	//インプット更新処理
	Input::GetInstance()->Update();

	//プログラム終了処理
	if (directXBase->CreateMessage()|| Input::GetInstance()->KeyTriggerState(DIK_ESCAPE))
	{
		endFlag = true;
	}

	//シーンマネージャーの更新処理
	sceneManager->Update(&debugText);
}

//描画処理
void Application::Draw()
{
	sceneManager->DrawRenderTexture(&debugText);


	//描画の開始
	directXBase->BeginDraw();
	//深度バッファのクリア
	directXBase->DepthClear();

	sceneManager->PostEffectDraw();

	directXBase->DepthClear();

	sceneManager->Draw(&debugText);


	//描画の終了
	directXBase->EndDraw();

}

void Application::DrawUI()
{
	//ImGui::Begin("Metarial");
	//ImGui::SetWindowPos(ImVec2(0, 0));
	//ImGui::SetWindowSize(ImVec2(300, 130));
	//ImGui::ColorEdit3("baseColor", baseColor, ImGuiColorEditFlags_Float);
	//ImGui::SliderFloat("metalness", &metalness, 0, 1);
	//ImGui::SliderFloat("specular", &specular, 0, 1);
	//ImGui::SliderFloat("roughness", &roughness, 0, 1);
	//ImGui::End();
}
