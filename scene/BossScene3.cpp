#include "BossScene3.h"
#include <iomanip>

BossScene3::BossScene3()
{
}

BossScene3::~BossScene3()
{
}

void BossScene3::Initialize(unsigned int arg_score)
{
	endFlag = false;
	rotateY = 0.0f;
	score = arg_score;
	beginFlag = true;
	sceneWave = 1;
	allEnemyDeadFlag = false;
	scrollPos = 0.1f;
	easingTime = 0.0f;
	warningTextureAlpha = 1.0f;
	incrementFlag = false;
	angleCounter = 0.0f;
}

void BossScene3::CreateAllObject(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList)
{
	gameObjectManager.reset(new GameObjectManager());
	gameObjectManager->Initialize(score);

	//ポストエフェクトの生成
	CreatePostEffect();
	
	//モデルのロード
	LoadAllModel();
	
	//ゲームオブジェクトの生成
	CreateGameObject(device, arg_cmdList);
}

void BossScene3::CreatePostEffect()
{
	motionBlur.reset(new MotionBlur());
	motionBlur->Initialize();

	bloom.reset(new Bloom());
	bloom->Initialize();
	bloom->SetFadeOutFlag(false);
	bloom->SetFadeOutCount(1.0f);

	fadeOut.reset(new FadeOut());
	fadeOut->Initialize();
	fadeOut->SetFadeOutFlag(false);
}

void BossScene3::LoadAllModel()
{
	//モデルデータ1のロード(自機)
	planeModel.reset(ObjFileModel::CreateFromOBJ("Plane_2"));
	missileModel.reset(ObjFileModel::CreateFromOBJ("Missile"));
	unionModel.reset(ObjFileModel::CreateFromOBJ("UnionCharacter"));

	//モデルデータ1のロード(黄色銃弾)
	yellowBulletModel.reset(ObjFileModel::CreateFromOBJ("bullet"));

	//モデルデータ1のロード(赤銃弾)
	fragmentBulletModel.reset(ObjFileModel::CreateFromOBJ("fragment_bullet"));

	//モデルデータ1のロード(ブロック)
	bossBodyModel.reset(ObjFileModel::CreateFromOBJ("Final_Boss"));
	eyeModel.reset(ObjFileModel::CreateFromOBJ("FinalBoss_Eye"));
	wingModel.reset(ObjFileModel::CreateFromOBJ("Wing"));

	//モデルデータ1のロード(ブロック)
	laserModel.reset(ObjFileModel::CreateFromOBJ("red_lay"));
}

void BossScene3::Update(DebugText* debugText)
{
	//モーションブラーのアップデート
	motionBlur->Update();

	camera->MoveEyeTarget({ incrementValue,0.0f,0.0f });


	boss->Update({ incrementValue,0.0f,0.0f }, playerObject->GetPosition());

	//スクロールの移動量
	scrollPos += incrementValue;


	gameObjectManager->AllObjectCollision();


	gameObjectManager->UpdateAllObject({ incrementValue,0.0f,0.0f });

	if (playerObject->GetIsDeadFlag() || boss->GetIsDeadFlag())
	{
		score += 100;
		endFlag = true;
	}

	//ウェーブ切り替え処理
	SceneProcessing();

	//スコアの表示
	gameObjectManager->DisPlayScore(debugText);

	std::ostringstream hpText;

	hpText << ":" << std::fixed << playerObject->GetHP();

	debugText->Print(hpText.str(), 150.0f, 150.0f, 1.25f);

	debugText->Print("BEAM:", 200.0f, WindowSize::window_height - 60.0f, 0.5f);
}

void BossScene3::DrawRenderTexture(ID3D12GraphicsCommandList* cmdList, DebugText* debugText)
{
	const int buttleScene = 3;
	if (sceneWave >= buttleScene)
	{
		bloom->PreDrawScene(cmdList);

		gameObjectManager->DrawAllObject();

		Sprite::BeginDraw(cmdList, false);

		lifeTexture->Draw();
		gaugeFrame->Draw();

		Sprite::EndDraw();

		debugText->DrawAll(cmdList);

		bloom->PostDrawScene(cmdList);
	}
	else
	{
		fadeOut->PreDrawScene(cmdList);

		gameObjectManager->DrawAllObject();

		fadeOut->PostDrawScene(cmdList);
	}
}

void BossScene3::PostEffectDraw(ID3D12GraphicsCommandList* cmdList)
{
	const int buttleScene = 3;
	if (sceneWave >= buttleScene)
	{//バトルシーンであればモーションブラーを適用
		motionBlur->Draw(cmdList);
	}
}

void BossScene3::Draw(ID3D12GraphicsCommandList* cmdList, DebugText* debugText)
{
	const int buttleScene = 3;
	if (sceneWave >= buttleScene)
	{//バトルシーンであればUIを表示
		bloom->Draw(cmdList);
	}
	else
	{
		fadeOut->Draw(cmdList);

		//WARNINGの点滅処理
		if (incrementFlag)
		{
			const float alphaIncrementSize = 1.0f;
			angleCounter += alphaIncrementSize;
		}
		else
		{
			const float alphaDecrementSize = 1.0f;
			angleCounter -= alphaDecrementSize;
		}
		const float minAngle = 0.0f;
		const float maxAngle = 180.0f;
		if (angleCounter < minAngle)
		{
			incrementFlag = true;
		}
		else if (angleCounter > maxAngle)
		{
			incrementFlag = false;
		}
		//カウンターから角度を求める
		const float angle = (angleCounter / 180.0f) * XM_PI;
		//sinを使ってアルファ値を計算
		warningTextureAlpha = sinf(angle);
		warningTexture->SetColor({ 1.0f,1.0f,1.0f, warningTextureAlpha });

		const float stopPosition = 0.0f;
		//WARNINGの上下にあるバーの移動処理
		Vector2 barPosition = warningBarTexture_1->GetPosition();
		if (barPosition.x > stopPosition)
		{
			const float barTextureDecrementPosition = 3.0f;
			barPosition.x -= barTextureDecrementPosition;
			warningBarTexture_1->SetPosition(barPosition);
		}
		else
		{
			warningBarTexture_1->SetSlideTextureFlag(true, true);
		}

		Vector2 barPosition_2 = warningBarTexture_2->GetPosition();
		if (barPosition_2.x < stopPosition)
		{
			const float barTextureIncrementPosition = 3.0f;
			barPosition_2.x += barTextureIncrementPosition;
			warningBarTexture_2->SetPosition(barPosition_2);
		}
		else
		{
			warningBarTexture_2->SetSlideTextureFlag(true, false);
		}

		Sprite::BeginDraw(cmdList, false);
		warningTexture->Draw();
		warningBarTexture_1->Draw();
		warningBarTexture_2->Draw();
		Sprite::EndDraw();
	}
}

void BossScene3::SceneProcessing()
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

		Scene3();

		break;

	case 4:

		Scene4();

		break;

	case 5:

		Scene5();

		break;

	default:
		break;
	}
}

void BossScene3::Scene1()
{
	playerObject->SetDisPlayLaserGaugeFlag(false);
	boss->SetShotFlag(false);
	boss->SetBodyAttackFlag(false);

	const float easingMaxTime = 1.0f;
	if (easingTime < easingMaxTime)
	{
		const Vector2 startPosition = { 0.0f,0.0f };
		const Vector2 endPosition = { 630.0f,0.0f };
		const float easingTimeIncrementSize = 0.001f;
		easingTime += easingTimeIncrementSize;
		float rotationEasing = easeOut(startPosition, endPosition, easingTime).x;
		camera->SetEye({ cos(rotationEasing / 180 * XM_PI) * (bossPosition.z),0,sin(rotationEasing / 180 * XM_PI) * (bossPosition.z) + (bossPosition.z) });

		const float changeSceneTime = 0.6f;
		if (easingTime > changeSceneTime)
		{
			fadeOut->SetFadeOutFlag(true);
		}
	}
	else
	{
		sceneWave++;
		fadeOut->SetFadeOutFlag(false);
		easingTime = 0;
	}
}

void BossScene3::Scene2()
{
	boss->SetShotFlag(false);
	boss->SetBodyAttackFlag(false);

	Vector3 playerPosition = playerObject->GetPosition();

	const float playerEndPosition = 0.0f;
	if (playerPosition.z < playerEndPosition)
	{
		const float incrementPlayerPosition = 0.05f;
		playerPosition.z += incrementPlayerPosition;
	}

	playerObject->SetPosition(playerPosition);
	//カメラターゲットセット
	camera->SetTarget({ 0,0,playerPosition.z });

	const float easingMaxTime = 1.0f;
	if (playerPosition.z < playerEndPosition || easingTime < easingMaxTime)
	{
		const float incrementEasingTime = 0.0015f;
		if (easingTime < easingMaxTime)
		{
			easingTime += incrementEasingTime;
		}

		float rotationEasing = easeOut({ 5.0f,0.0f }, { 0.0f,0.0f }, easingTime).x;
		camera->SetEye({ rotationEasing,rotationEasing,-20.2f });
	}
	else
	{
		sceneWave++;
		easingTime = 0;
		camera->SetTarget({ 0,0,0 });
		camera->SetEye({ 0,0,-20.2f });
		playerObject->SetMoveFlag(true);
		playerObject->SetShotFlag(true);
		playerObject->SetDisPlayLaserGaugeFlag(true);
	}
}

void BossScene3::Scene3()
{

}

void BossScene3::Scene4()
{
}

void BossScene3::Scene5()
{

}

Scene BossScene3::Next()
{
	if (playerObject->GetIsDeadFlag() == true)
	{
		return Scene::GAMEOVER;
	}

	return Scene::RESULT;
}

void BossScene3::CreateGameObject(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList)
{
	//キャラクター1の生成
	playerObject = Player::Create(device, arg_cmdList, { 0.0f,0.0f,-40.0f });

	warningTexture.reset(Sprite::Create(L"Resources/warning.png", { (float)WindowSize::window_width / 8,(float)WindowSize::window_height / 2 - 50.0f }));
	warningBarTexture_1.reset(Sprite::Create(L"Resources/warningBar.png", { WindowSize::window_width,(float)WindowSize::window_height / 3 * 1.0f }));
	warningBarTexture_2.reset(Sprite::Create(L"Resources/warningBar.png", { -WindowSize::window_width,(float)WindowSize::window_height / 3 * 2.0f }));


	//キャラクター1の生成
	boss = FinalBoss::Create(device, arg_cmdList, { 0.0f,0.0f,0.0f });
	//モデル1のセット
	boss->SetOBJModel(eyeModel.get(), bossBodyModel.get(), wingModel.get(), fragmentBulletModel.get());
	boss->SetIsDeadFlag(true);
	boss->SetCenterPos(scrollPos);
	boss->Update({ incrementValue,0.0f,0.0f }, playerObject->GetPosition());

	boss->Initialize();
	boss->SetPosition({ 0.0f,0.0f,50.0f });
	boss->SetShotFlag(false);

	playerObject->SetBossSceneFlag(true);
	gameObjectManager->AddGameObject(boss);

	playerObject->SetBossSceneFlag(true);
	playerObject->SetMoveFlag(false);
	playerObject->SetShotFlag(false);

	//モデル1のセット
	playerObject->SetOBJModel(planeModel.get(), yellowBulletModel.get(), unionModel.get(), yellowBulletModel.get(), laserModel.get(), laserModel.get(), missileModel.get());

	//カメラターゲットセット
	camera->SetTarget({ 0,0,0 });
	camera->SetEye({ 5,5,-20.2f });
	//マネージャークラスにオブジェクトのアドレスを全て追加
	gameObjectManager->AddGameObject(playerObject);

	gameObjectManager->AddGameObject(boss->GetCollisionEye());

	lifeTexture.reset(Sprite::Create(L"Resources/player_texture.png", { 100.0f, 190.0f }));
	lifeTexture->SetAnchorPoint({ 0.5f,0.5f });
	lifeTexture->SetScale({ 104.0f,68.0f });

	gaugeFrame.reset(Sprite::Create(L"Resources/gauge_frame.png", { (float)WindowSize::window_width / 2 + 8.0f,(float)WindowSize::window_height - 50.0f }));
	gaugeFrame->SetScale({ 675.0f,70.0f });
	gaugeFrame->SetAnchorPoint({ 0.5f,0.5f });

	bossPosition = boss->GetPosition();

	camera->SetTarget(bossPosition);
}
