#include "GameClear.h"


GameClear::GameClear()
{
}

GameClear::~GameClear()
{
}

void GameClear::Initialize(unsigned int arg_score)
{
	endFlag = false;
	rotateY = 0.0f;
	delayTime = 0;
	score = arg_score;
	blurFlag = false;
	beginFlag = true;
	pushButtonFlag = false;
}

void GameClear::CreateAllObject(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList)
{
	gameObjectManager.reset(new GameObjectManager());
	gameObjectManager->Initialize(score);

	CreatePostEffect();

	LoadAllModel();
	//カメラターゲットセット
	camera->SetTarget({ 0,0,0 });
	camera->SetEye({ 0,0,-20.2f });


	//マネージャークラスに入っているオブジェクト全てをアップデート
	gameObjectManager->UpdateAllObject({0,0,0});

	//テクスチャの生成処理
	gameClearTexture.reset(Sprite::Create(L"Resources/gameclear.png", { WindowSize::window_width / 2.0f,WindowSize::window_height / 2.0f }));
	gameClearTexture->SetAnchorPoint({ 0.5f,0.5f });
}

void GameClear::CreatePostEffect()
{
	bloom.reset(new Bloom());
	bloom->Initialize();
	bloom->SetFadeOutFlag(false);
}

void GameClear::LoadAllModel()
{

}

void GameClear::Update(DebugText* debugText)
{
	gameObjectManager->UpdateAllObject({ 0,0,0 });


	if (pushButtonFlag == false && Input::GetInstance()->KeyTriggerState(DIK_SPACE))
	{
		bloom->SetFadeOutFlag(true);
		pushButtonFlag = true;
	}

	if (pushButtonFlag && bloom->GetFadeOutCount() <= 0.0f)
	{
		endFlag = true;
	}
}

void GameClear::DrawRenderTexture(ID3D12GraphicsCommandList* cmdList, DebugText* debugText)
{
	bloom->PreDrawScene(cmdList);

	gameObjectManager->DrawAllObject();

	Sprite::BeginDraw(cmdList, true);
	gameClearTexture->Draw();
	Sprite::EndDraw();

	bloom->PostDrawScene(cmdList);
}

void GameClear::PostEffectDraw(ID3D12GraphicsCommandList* cmdList)
{
	bloom->Draw(cmdList);
}

void GameClear::Draw(ID3D12GraphicsCommandList* cmdList, DebugText* debugText)
{

}

Scene GameClear::Next()
{
	//カメラターゲットセット
	camera->SetTarget({ 0,0,0 });
	camera->SetEye({ 0,0,-20.2f });
	return Scene::TITLE;
}

