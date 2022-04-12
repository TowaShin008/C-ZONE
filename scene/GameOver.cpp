#include "GameOver.h"


GameOver::GameOver()
{
}

GameOver::~GameOver()
{
}

void GameOver::Initialize(unsigned int arg_score)
{
	endFlag = false;
	rotateY = 0.0f;
	delayTime = 0;
	score = arg_score;
	blurFlag = false;
	beginFlag = true;
	endingMoveTime = 0;
	pushButtonFlag = false;
}

void GameOver::CreateAllObject(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList)
{
	gameObjectManager.reset(new GameObjectManager());
	gameObjectManager->Initialize(score);

	CreatePostEffect();

	LoadAllModel();

	camera->SetTarget({ 0,0,0 });
	camera->SetEye({ 0,0,-20.2f });

	//マネージャークラスにオブジェクトのアドレスを全て追加

	//マネージャークラスに入っているオブジェクト全てをアップデート
	gameObjectManager->UpdateAllObject({ 0,0,0 });

	//テクスチャの生成処理
	gameOverTexture.reset(Sprite::Create(L"Resources/gameover.png", { WindowSize::window_width / 2.0f, WindowSize::window_height / 2.0f}));
	gameOverTexture->SetAnchorPoint({ 0.5f,0.5f });

	//背景のグラデーションをするための配列
	gradationMap = {
	{10,11,12,13,14,15,16,17},
	{ 9,10,11,12,13,14,15,16},
	{ 8, 9,10,11,12,13,14,15},
	{ 7, 8, 9,10,11,12,13,14},
	{ 6, 7, 8, 9,10,11,12,13},
	{ 5, 6, 7, 8, 9,10,11,12},
	{ 4, 5, 6, 7, 8, 9,10,11},
	{ 3, 4, 5, 6, 7, 8, 9,10},
	{ 2, 3, 4, 5, 6, 7, 8, 9},
	{ 1, 2, 3, 4, 5, 6, 7, 8},
	{ 0, 1, 2, 3, 4, 5, 6, 7},
	};

	bgHexagon.resize(gradationMap.size());
	const Vector4 red = { 1.0f,0.0f,0.0f,1.0f };
	for (int i = 0; i < gradationMap.size(); i++)
	{
		bgHexagon[i].resize(gradationMap[i].size());

		for (int j = 0; j < gradationMap[i].size(); j++)
		{
			//生成位置を一行ずつ互い違いにずらす
			if (j % 2 == 1)
			{
				bgHexagon[i][j].reset(Sprite::Create(L"Resources/white_hexagon_s.png", { 130.0f * i - 35.0f, 115.0f * j - 200.0f }));
				bgHexagon[i][j]->SetLugTime(gradationMap[i][j] * 30);
				bgHexagon[i][j]->SetScale({ 170.0f,170.0f });
			}
			else
			{
				bgHexagon[i][j].reset(Sprite::Create(L"Resources/white_hexagon_s.png", { 130.0f * i - 100.0f, 115.0f * j - 200.0f }));
				bgHexagon[i][j]->SetLugTime(gradationMap[i][j] * 30);
				bgHexagon[i][j]->SetScale({ 170.0f,170.0f });
			}
			bgHexagon[i][j]->SetLugTime(gradationMap[i][j] * 30);
			bgHexagon[i][j]->SetAlphaFlag(false);
			bgHexagon[i][j]->SetColor(red);
		}
	}

}

void GameOver::CreatePostEffect()
{
	//ポストエフェクト生成処理
	bloom.reset(new Bloom());
	bloom->Initialize();
	bloom->SetFadeOutFlag(false);
}

void GameOver::LoadAllModel()
{

}

void GameOver::Update(DebugText* debugText)
{
	gameObjectManager->UpdateAllObject({ 0,0,0 });


	for (int i = 0; i < gradationMap.size(); i++)
	{
		for (int j = 0; j < gradationMap[i].size(); j++)
		{
			bgHexagon[i][j]->GradatioUpdate();
		}
	}

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

void GameOver::DrawRenderTexture(ID3D12GraphicsCommandList* cmdList, DebugText* debugText)
{
	bloom->PreDrawScene(cmdList);
	gameObjectManager->DrawAllObject();

	Sprite::BeginDraw(cmdList,false);
	//背景描画
	for (int i = 0; i < gradationMap.size(); i++)
	{
		for (int j = 0; j < gradationMap[i].size(); j++)
		{
			bgHexagon[i][j]->Draw();
		}
	}

	Sprite::EndDraw();

	Sprite::BeginDraw(cmdList, true);
	
	gameOverTexture->Draw();

	Sprite::EndDraw();

	bloom->PostDrawScene(cmdList);
}

void GameOver::PostEffectDraw(ID3D12GraphicsCommandList* cmdList)
{
	bloom->Draw(cmdList);
}

void GameOver::Draw(ID3D12GraphicsCommandList* cmdList, DebugText* debugText)
{

}

Scene GameOver::Next()
{
	//カメラターゲットセット
	camera->SetTarget({ 0,0,0 });
	camera->SetEye({ 0,0,-20.2f });
	return Scene::TITLE;
}

