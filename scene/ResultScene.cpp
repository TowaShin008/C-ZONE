#include "ResultScene.h"
#include <iomanip>

ResultScene::ResultScene()
{
}

ResultScene::~ResultScene()
{
}

void ResultScene::Initialize(unsigned int arg_score)
{
	endFlag = false;
	delayTime = 0;
	sceneWave = 1;
	score = arg_score;
	blurFlag = false;
	beginFlag = true;
	pushButtonFlag = false;
	easingTime = 0.0f;
	windowTextureAlpha = 0.0f;
}

void ResultScene::CreateAllObject(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList)
{
	gameObjectManager.reset(new GameObjectManager());
	gameObjectManager->Initialize(score);

	CreatePostEffect();


	LoadAllModel();

	//キャラクター1の生成
	playerObject = OBJHighCharacter::Create(device, arg_cmdList, { 0.0f,0.0f,-40.0f });

	playerObject->SetOBJModel(playerModel.get());
	playerObject->SetRotation({ 270.0f,0.0f,180.0f });
	playerObject->SetScale({ 0.4f,0.4f,0.4f });

	//マネージャークラスにオブジェクトのアドレスを全て追加
	gameObjectManager->AddGameObject(playerObject);

	//カメラターゲットセット
	camera->SetTarget({ 0,0,0 });
	camera->SetEye({ 0,0,-20.2f });
	//マネージャークラスに入っているオブジェクト全てをアップデート
	gameObjectManager->UpdateAllObject({ 0,0,0 });

	//テクスチャの生成処理
	windowTexture.reset(Sprite::Create(L"Resources/window.png", { WindowSize::window_width / 2.0f,WindowSize::window_height / 2.0f }));
	windowTexture->SetAnchorPoint({ 0.5f,0.5f });
	windowTexture->SetScale({ 1000.0f,600.0f });
	windowTexture->SetColor({ 1.0f,1.0f,1.0f,0.0f });

	stage_clearTexture.reset(Sprite::Create(L"Resources/stageclear.png", { WindowSize::window_width / 2.0f,WindowSize::window_height / 2.0f }));
	stage_clearTexture->SetAnchorPoint({ 0.5f,0.5f });
}

void ResultScene::CreatePostEffect()
{
	bloom.reset(new Bloom());
	bloom->Initialize();
	bloom->SetFadeOutFlag(false);
}

void ResultScene::LoadAllModel()
{
	playerModel.reset(OBJHighModel::CreateFromOBJ("Plane_2"));
}

void ResultScene::Update(DebugText* debugText)
{
	gameObjectManager->UpdateAllObject({ 0,0,0 });

	SceneProcessing(debugText);
}

void ResultScene::DrawRenderTexture(ID3D12GraphicsCommandList* cmdList, DebugText* debugText)
{
	bloom->PreDrawScene(cmdList);

	gameObjectManager->DrawAllObject();

	const int finalScene = 3;

	if (sceneWave == finalScene)
	{
		Sprite::BeginDraw(cmdList, false);
		windowTexture->Draw();
		Sprite::EndDraw();

		debugText->DrawAll(cmdList);
	}
	else
	{
		const int drawTime = 40;
		if (blinkCount > drawTime)
		{
			Sprite::BeginDraw(cmdList, true);
			stage_clearTexture->Draw();
			Sprite::EndDraw();
		}
		const int maxTime = 80;
		if (maxTime > blinkCount)
		{
			blinkCount++;
		}
		else
		{
			const int initializeTime = 0;
			blinkCount = initializeTime;
		}
	}

	bloom->PostDrawScene(cmdList);
}

void ResultScene::PostEffectDraw(ID3D12GraphicsCommandList* cmdList)
{
	bloom->Draw(cmdList);
}

void ResultScene::Draw(ID3D12GraphicsCommandList* cmdList, DebugText* debugText)
{

}

void ResultScene::SceneProcessing(DebugText* debugText)
{
	switch (sceneWave)
	{
	case 1:

		Scene1();

		break;

	case 2:

		Scene2();

		break;

	case 3:

		Scene3(debugText);

		break;


	default:
		break;
	}
}

void ResultScene::Scene1()
{
	Vector3 playerPosition = playerObject->GetPosition();

	const float incrementPlayerPosition = 0.05f;
	const float playerEndPosition = 100.0f;
	if (playerPosition.z < playerEndPosition)
		playerPosition.z += incrementPlayerPosition;

	playerObject->SetPosition(playerPosition);
	//カメラターゲットセット
	camera->SetTarget({ 0,0,playerPosition.z });
	//camera->SetEye({ 5,5,-20.2f });

	const float easingMaxTime = 1.0f;
	if (easingTime < easingMaxTime)
	{
		const float incrementEasingTime = 0.0015f;
		if (easingTime < easingMaxTime)
			easingTime += incrementEasingTime;

		float rotationEasing = easeOut({ 5.0f,0.0f }, { 0.0f,0.0f }, easingTime).x;
		camera->SetEye({ rotationEasing,rotationEasing,-20.2f });
	}
	else
	{
		sceneWave++;
		easingTime = 0;
		camera->SetTarget({ 0,0,0 });
		camera->SetEye({ 0,0,-20.2f });
	}
}

void ResultScene::Scene2()
{
	sceneWave++;
}

void ResultScene::Scene3(DebugText* debugText)
{
	const float fixPos = 150.0f;
	debugText->Print("Score", (WindowSize::window_width / 4.0f) + fixPos, WindowSize::window_height / 4.0f, 2.0f);

	std::ostringstream scoreText;

	scoreText << std::setfill('0') << std::setw(8)
		<< std::fixed << gameObjectManager->GetScore() << "00";

	Vector3 playerPosition = playerObject->GetPosition();

	const float incrementPlayerPosition = 0.05f;
	const float playerEndPosition = 100.0f;
	if (playerPosition.z < playerEndPosition)
		playerPosition.z += incrementPlayerPosition;

	playerObject->SetPosition(playerPosition);
	//カメラターゲットセット
	camera->SetTarget({ 0,0,playerPosition.z });

	const float fixPos_2 = 30.0f;
	debugText->Print(scoreText.str(), (WindowSize::window_width / 5.0f) + fixPos_2, WindowSize::window_height / 2.0f, 2.0f);

	if (windowTextureAlpha < 1.0f)
	{
		windowTextureAlpha += 0.01f;
		windowTexture->SetColor({ 1.0f,1.0f,1.0f,windowTextureAlpha });
	}
	else
	{
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
}

Scene ResultScene::Next()
{
	//カメラターゲットセット
	camera->SetTarget({ 0,0,0 });
	camera->SetEye({ 0,0,-20.2f });
	if (previousScene == Scene::BOSS1)
	{
		return Scene::STAGE2;
	}

	if (previousScene == Scene::BOSS2)
	{
		return Scene::STAGE3;
	}
	return Scene::TITLE;
}

