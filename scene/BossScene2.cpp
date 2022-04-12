#include "BossScene2.h"
#include <iomanip>

BossScene2::BossScene2()
{
}

BossScene2::~BossScene2()
{
}

void BossScene2::Initialize(unsigned int arg_score)
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

void BossScene2::CreateAllObject(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList)
{
	gameObjectManager.reset(new GameObjectManager());
	gameObjectManager->Initialize(score);
	
	//�|�X�g�G�t�F�N�g�̐���
	CreatePostEffect();
	
	//���f���̃��[�h
	LoadAllModel();
	
	//�Q�[���I�u�W�F�N�g�̐���
	CreateGameObject(device, arg_cmdList);
}

void BossScene2::CreatePostEffect()
{
	motionBlur.reset(new MotionBlur());
	motionBlur->Initialize();

	dropNormalMap.reset(new DropNormalMap());
	dropNormalMap->Initialize();
	dropNormalMap->SetFadeOutFlag(false);

	bloom.reset(new Bloom());
	bloom->Initialize();
	bloom->SetFadeOutFlag(false);
	bloom->SetFadeOutCount(1.0f);

	fadeOut.reset(new FadeOut());
	fadeOut->Initialize();
	fadeOut->SetFadeOutFlag(false);
}

void BossScene2::LoadAllModel()
{
	//���f���f�[�^1�̃��[�h(���@)
	planeModel.reset(ObjFileModel::CreateFromOBJ("Plane_2"));
	missileModel.reset(ObjFileModel::CreateFromOBJ("Missile"));
	unionModel.reset(ObjFileModel::CreateFromOBJ("UnionCharacter"));

	//���f���f�[�^1�̃��[�h(���F�e�e)
	yellowBulletModel.reset(ObjFileModel::CreateFromOBJ("bullet"));

	//���f���f�[�^1�̃��[�h(�u���b�N)
	eyeModel.reset(ObjFileModel::CreateFromOBJ("Eye"));
	blockBodyModel.reset(ObjFileModel::CreateFromOBJ("Squid"));
	seaModel.reset(ObjFileModel::CreateFromOBJ("Sea"));

	//���f���f�[�^1�̃��[�h(�u���b�N)
	laserModel.reset(ObjFileModel::CreateFromOBJ("red_lay"));
}

void BossScene2::Update(DebugText* debugText)
{
	//���[�V�����u���[�̃A�b�v�f�[�g
	motionBlur->Update();

	sea->Update({ 0,0,0 });

	squid->Update({ incrementValue,0.0f,0.0f }, playerObject->GetPosition());

	gameObjectManager->AllObjectCollision();


	gameObjectManager->UpdateAllObject({ incrementValue,0.0f,0.0f });

	if (playerObject->GetIsDeadFlag() || squid->GetIsDeadFlag())
	{
		score += 100;
		endFlag = true;
	}

	//�E�F�[�u�؂�ւ�����
	SceneProcessing();

	//�X�R�A�̕\��
	gameObjectManager->DisPlayScore(debugText);

	std::ostringstream hpText;

	hpText << ":" << std::fixed << playerObject->GetHP();

	debugText->Print(hpText.str(), 150.0f, 150.0f, 1.25f);

	debugText->Print("BEAM:", 200.0f, WindowSize::window_height - 60.0f, 0.5f);
}

void BossScene2::DrawRenderTexture(ID3D12GraphicsCommandList* cmdList, DebugText* debugText)
{
	const int buttleScene = 3;
	if (sceneWave >= buttleScene)
	{
		bloom->PreDrawScene(cmdList);
		sea->Draw();

		squid->Draw();

		gameObjectManager->DrawAllObject();

		Sprite::BeginDraw(cmdList, false);

		lifeTexture->Draw();
		gaugeFrame->Draw();

		Sprite::EndDraw();

		debugText->DrawAll(cmdList);
		bloom->PostDrawScene(cmdList);
	}
	else if (sceneWave == 1)
	{
		dropNormalMap->PreDrawScene(cmdList);
		sea->Draw();

		gameObjectManager->DrawAllObject();
		squid->Draw();


		dropNormalMap->PostDrawScene(cmdList);
	}
	else
	{
		fadeOut->PreDrawScene(cmdList);

		sea->Draw();

		gameObjectManager->DrawAllObject();
		squid->Draw();


		fadeOut->PostDrawScene(cmdList);
	}
}

void BossScene2::PostEffectDraw(ID3D12GraphicsCommandList* cmdList)
{
	const int buttleScene = 3;
	if (sceneWave >= buttleScene)
	{
	}
}

void BossScene2::Draw(ID3D12GraphicsCommandList* cmdList, DebugText* debugText)
{
	const int buttleScene = 3;
	if (sceneWave >= buttleScene)
	{//�o�g���V�[���ł����UI��\��
		bloom->Draw(cmdList);

	}
	else if (sceneWave == 1)
	{
		dropNormalMap->Draw(cmdList);

		//WARNING�̓_�ŏ���
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
		//�J�E���^�[����p�x�����߂�
		const float angle = (angleCounter / 180.0f) * XM_PI;
		//sin���g���ăA���t�@�l���v�Z
		warningTextureAlpha = sinf(angle);
		warningTexture->SetColor({ 1.0f,1.0f,1.0f, warningTextureAlpha });

		const float stopPosition = 0.0f;
		//WARNING�̏㉺�ɂ���o�[�̈ړ�����
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
	else
	{
		fadeOut->Draw(cmdList);

		//WARNING�̓_�ŏ���
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
		//�J�E���^�[����p�x�����߂�
		const float angle = (angleCounter / 180.0f) * XM_PI;
		//sin���g���ăA���t�@�l���v�Z
		warningTextureAlpha = sinf(angle);
		warningTexture->SetColor({ 1.0f,1.0f,1.0f, warningTextureAlpha });

		const float stopPosition = 0.0f;
		//WARNING�̏㉺�ɂ���o�[�̈ړ�����
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

void BossScene2::SceneProcessing()
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

void BossScene2::Scene1()
{
	playerObject->SetDisPlayLaserGaugeFlag(false);
	squid->SetAttackFlag(false);

	const float easingMaxTime = 1.0f;
	if (easingTime < easingMaxTime)
	{
		if (squid->GetPosition().y < 0.0f)
		{
			Vector3 bossPosition = squid->GetPosition();
			const Vector2 startPosition = { -100.0f,0.0f };
			const Vector2 endPosition = { 0.0f,0.0f };

			const float easingTimeIncrementSize = 0.001f;
			easingTime += easingTimeIncrementSize;
			float rotationEasing = easeOut(startPosition, endPosition, easingTime).x;
			bossPosition.y = rotationEasing;
			squid->SetPosition(bossPosition);
		}
		else
		{
			easingTime = easingMaxTime;
		}

		const float changeSceneTime = 0.6f;
		if (easingTime > changeSceneTime)
		{
			dropNormalMap->SetFadeOutFlag(true);
		}
	}
	else
	{
		sceneWave++;
		dropNormalMap->SetFadeOutFlag(false);
		easingTime = 0;
	}
}

void BossScene2::Scene2()
{
	squid->SetAttackFlag(false);
	Vector3 playerPosition = playerObject->GetPosition();

	const float incrementPlayerPosition = 0.05f;
	const float playerEndPosition = 0.0f;
	if (playerPosition.z < playerEndPosition)
	{
		playerPosition.z += incrementPlayerPosition;
	}

	playerObject->SetPosition(playerPosition);
	//�J�����^�[�Q�b�g�Z�b�g
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

void BossScene2::Scene3()
{

}

void BossScene2::Scene4()
{
}

void BossScene2::Scene5()
{

}

Scene BossScene2::Next()
{
	if (playerObject->GetIsDeadFlag() == true)
	{
		return Scene::GAMEOVER;
	}

	return Scene::RESULT;
}

void BossScene2::CreateGameObject(ID3D12Device* arg_device, ID3D12GraphicsCommandList* arg_cmdList)
{
	sea.reset(ObjFileCharacter::Create(arg_device, arg_cmdList, { 0.0f,-100.0f,50.0f }));
	sea->SetOBJModel(seaModel.get());
	sea->SetScale({ 1000.0f,50.0f,1000.0f });

	//�L�����N�^�[1�̐���
	playerObject = Player::Create(arg_device, arg_cmdList, { 0.0f,0.0f,-40.0f });

	warningTexture.reset(Sprite::Create(L"Resources/warning.png", { (float)WindowSize::window_width / 8,(float)WindowSize::window_height / 2 - 50.0f }));
	warningBarTexture_1.reset(Sprite::Create(L"Resources/warningBar.png", { WindowSize::window_width,(float)WindowSize::window_height / 3 * 1.0f }));
	warningBarTexture_2.reset(Sprite::Create(L"Resources/warningBar.png", { -WindowSize::window_width,(float)WindowSize::window_height / 3 * 2.0f }));


	//�L�����N�^�[1�̐���
	squid.reset(Squid::Create(arg_device, arg_cmdList, { scrollPos + 5.0f + 25.0f,0.0f,0.0f }));
	//���f��1�̃Z�b�g
	squid->SetOBJModel(eyeModel.get(), blockBodyModel.get());
	squid->SetIsDeadFlag(true);
	squid->SetCenterPos(scrollPos);
	squid->Update({ incrementValue,0.0f,0.0f }, playerObject->GetPosition());

	squid->Initialize();
	squid->SetMoveLugTime(0);
	squid->SetPosition({ 0.0f,-50.0f,50.0f });
	squid->SetShotFlag(false);

	for (int i = 0; i < squid->GetTentacle().size(); i++)
	{
		gameObjectManager->AddGameObject(squid->GetTentacle()[i]);
	}


	playerObject->SetBossSceneFlag(true);
	gameObjectManager->AddGameObject(squid->GetLeftSquidEye());
	gameObjectManager->AddGameObject(squid->GetRightSquidEye());

	playerObject->SetBossSceneFlag(true);
	playerObject->SetMoveFlag(false);
	playerObject->SetShotFlag(false);

	//���f��1�̃Z�b�g
	playerObject->SetOBJModel(planeModel.get(), yellowBulletModel.get(), unionModel.get(), yellowBulletModel.get(), laserModel.get(), laserModel.get(), missileModel.get());

	//�J�����^�[�Q�b�g�Z�b�g
	camera->SetTarget({ 0,0,0 });
	camera->SetEye({ -40,-60,-20.2f });
	//�}�l�[�W���[�N���X�ɃI�u�W�F�N�g�̃A�h���X��S�Ēǉ�
	gameObjectManager->AddGameObject(playerObject);

	lifeTexture.reset(Sprite::Create(L"Resources/player_texture.png", { 100.0f, 190.0f }));
	lifeTexture->SetAnchorPoint({ 0.5f,0.5f });
	lifeTexture->SetScale({ 104.0f,68.0f });

	gaugeFrame.reset(Sprite::Create(L"Resources/gauge_frame.png", { (float)WindowSize::window_width / 2 + 8.0f,(float)WindowSize::window_height - 50.0f }));
	gaugeFrame->SetScale({ 675.0f,70.0f });
	gaugeFrame->SetAnchorPoint({ 0.5f,0.5f });

	camera->SetTarget({ -5.0f, -11.0f, 50.0f });
}
