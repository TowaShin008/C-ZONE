#include "BossScene1.h"
#include <iomanip>

BossScene1::BossScene1()
{
}

BossScene1::~BossScene1()
{
}

void BossScene1::Initialize(unsigned int arg_score)
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

void BossScene1::CreateAllObject(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList)
{
	gameObjectManager.reset(new GameObjectManager());
	gameObjectManager->Initialize(score);

	CreatePostEffect();

	LoadAllModel();

	//�L�����N�^�[1�̐���
	playerObject = Player::Create(device, arg_cmdList, { 0.0f,0.0f,-40.0f });

	warningTexture.reset(Sprite::Create(L"Resources/warning.png", { (float)WindowSize::window_width / 8,(float)WindowSize::window_height / 2 - 50.0f }));
	warningBarTexture_1.reset(Sprite::Create(L"Resources/warningBar.png", { WindowSize::window_width,(float)WindowSize::window_height / 3 * 1.0f }));
	//warningBarTexture_1->SetSlideTextureFlag(true, true);
	warningBarTexture_2.reset(Sprite::Create(L"Resources/warningBar.png", { -WindowSize::window_width,(float)WindowSize::window_height / 3 * 2.0f }));
	//warningBarTexture_2->SetSlideTextureFlag(true, false);


	//�L�����N�^�[1�̐���
	boss = Boss::Create(device, arg_cmdList, { scrollPos + 5.0f + 25.0f,0.0f,0.0f });
	//���f��1�̃Z�b�g
	boss->SetOBJModel(enemyPlaneModel.get(), redBulletModel.get());
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

	//���f��1�̃Z�b�g
	playerObject->SetOBJModel(planeModel.get(), yellowBulletModel.get(), unionModel.get(), yellowBulletModel.get(), laserModel.get(), missileModel.get());

	//�J�����^�[�Q�b�g�Z�b�g
	camera->SetTarget({ 0,0,0 });
	camera->SetEye({ 5,5,-20.2f });
	//�}�l�[�W���[�N���X�ɃI�u�W�F�N�g�̃A�h���X��S�Ēǉ�
	gameObjectManager->AddGameObject(playerObject);

	//�}�l�[�W���[�N���X�ɓ����Ă���I�u�W�F�N�g�S�Ă��A�b�v�f�[�g
	laserGauge.reset(OBJHighCharacter::Create(device, arg_cmdList));
	laserGauge->SetOBJModel(laserModel.get());
	laserGauge->SetColor({ 0.0f,0.0f,1.0f,1.0f });
	laserGauge->SetPosition({ -8.0f,-8.0f,-4.0f });

	lifeTexture.reset(Sprite::Create(L"Resources/player_texture.png", { 100.0f, 190.0f }));
	lifeTexture->SetAnchorPoint({ 0.5f,0.5f });
	lifeTexture->SetScale({ 104.0f,68.0f });

	gaugeFrame.reset(Sprite::Create(L"Resources/gauge_frame.png", { (float)WindowSize::window_width / 2 + 8.0f,(float)WindowSize::window_height - 50.0f }));
	gaugeFrame->SetScale({ 675.0f,70.0f });
	gaugeFrame->SetAnchorPoint({ 0.5f,0.5f });

	gaugeParticle.reset(ParticleManager::Create(device));

	bossPosition = boss->GetPosition();

	camera->SetTarget(bossPosition);
}

void BossScene1::CreatePostEffect()
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

void BossScene1::LoadAllModel()
{
	//���f���f�[�^1�̃��[�h(���@)
	planeModel.reset(OBJHighModel::CreateFromOBJ("Plane_2"));
	enemyPlaneModel.reset(OBJHighModel::CreateFromOBJ("Enemy_Plane"));
	missileModel.reset(OBJHighModel::CreateFromOBJ("Missile"));
	unionModel.reset(OBJHighModel::CreateFromOBJ("UnionCharacter"));

	//���f���f�[�^1�̃��[�h(���F�e�e)
	yellowBulletModel.reset(OBJLoader::GetInstance()->LoadModel("bullet"));

	//���f���f�[�^1�̃��[�h(�ԏe�e)
	redBulletModel.reset(OBJHighModel::CreateFromOBJ("fragment_bullet"));

	//���f���f�[�^1�̃��[�h(�u���b�N)
	laserModel.reset(OBJHighModel::CreateFromOBJ("red_lay"));
}

void BossScene1::Update(DebugText* debugText)
{
	//���[�V�����u���[�̃A�b�v�f�[�g
	motionBlur->Update();

	camera->MoveEyeTarget({ incrementValue,0.0f,0.0f });

	//���[�U�[�Q�[�W����
	LaserGaugeProcessing();


	boss->Update({ incrementValue,0.0f,0.0f }, playerObject->GetPosition());

	//�X�N���[���̈ړ���
	scrollPos += incrementValue;


	gameObjectManager->AllObjectCollision();


	gameObjectManager->UpdateAllObject({ incrementValue,0.0f,0.0f });

	if (playerObject->GetIsDeadFlag() || boss->GetIsDeadFlag())
	{
		if (boss->GetIsDeadFlag())
		{
			score += 100;
		}
		endFlag = true;
	}

	//�E�F�[�u�؂�ւ�����
	SceneProcessing();

	std::ostringstream scoreText;

	scoreText << "Score:" << std::setfill('0') << std::setw(8)
		<< std::fixed << gameObjectManager->GetScore() << "00";

	debugText->Print(scoreText.str(), WindowSize::window_width - 300.0f, WindowSize::window_height - 60.0f, 0.5f);

	std::ostringstream hpText;

	hpText << ":" << std::fixed << playerObject->GetHP();

	debugText->Print(hpText.str(), 150.0f, 150.0f, 1.25f);

	debugText->Print("BEAM:", 200.0f, WindowSize::window_height - 60.0f, 0.5f);
}

void BossScene1::DrawRenderTexture(ID3D12GraphicsCommandList* cmdList, DebugText* debugText)
{
	const int buttleScene = 3;
	if (sceneWave >= buttleScene)
	{//�o�g���V�[���ł���΃��[�V�����u���[��K�p
		//motionBlur->DrawRenderTexture(cmdList, playerObject);

		bloom->PreDrawScene(cmdList);

		ParticleManager::PreDraw(cmdList);
		gaugeParticle->Draw();
		ParticleManager::PostDraw();
		gameObjectManager->DrawAllObject();

		laserGauge->Draw();

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

		ParticleManager::PreDraw(cmdList);
		gaugeParticle->Draw();
		ParticleManager::PostDraw();
		gameObjectManager->DrawAllObject();

		fadeOut->PostDrawScene(cmdList);
	}
}

void BossScene1::PostEffectDraw(ID3D12GraphicsCommandList* cmdList)
{
	const int buttleScene = 3;
	if (sceneWave >= buttleScene)
	{//�o�g���V�[���ł���΃��[�V�����u���[��K�p
		motionBlur->Draw(cmdList);
	}
}

void BossScene1::Draw(ID3D12GraphicsCommandList* cmdList, DebugText* debugText)
{
	const int buttleScene = 3;
	if (sceneWave >= buttleScene)
	{//�o�g���V�[���ł����UI��\��
		bloom->Draw(cmdList);
	}
	else
	{//�o�g���V�[���O�̉��o
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

void BossScene1::SceneProcessing()
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

void BossScene1::Scene1()
{
	boss->SetShotFlag(false);
	if (easingTime < 1.0f)
	{
		const Vector2 startPosition = { 0.0f,0.0f };
		const Vector2 endPosition = { 630.0f,0.0f };
		easingTime += 0.001f;
		float rotationEasing = easeOut(startPosition, endPosition, easingTime).x;
		camera->SetEye({ cos(rotationEasing / 180 * XM_PI) * (bossPosition.z),0,sin(rotationEasing / 180 * XM_PI) * (bossPosition.z) + (bossPosition.z) });

		const float changeSceneTime = 0.6f;
		if (easingTime > changeSceneTime)
			fadeOut->SetFadeOutFlag(true);
	}
	else
	{
		sceneWave++;
		fadeOut->SetFadeOutFlag(false);
		easingTime = 0;
	}
}

void BossScene1::Scene2()
{
	boss->SetShotFlag(false);
	Vector3 playerPosition = playerObject->GetPosition();

	const float incrementPlayerPosition = 0.05f;
	const float playerEndPosition = 0.0f;
	if (playerPosition.z < playerEndPosition)
		playerPosition.z += incrementPlayerPosition;

	playerObject->SetPosition(playerPosition);
	//�J�����^�[�Q�b�g�Z�b�g
	camera->SetTarget({ 0,0,playerPosition.z });
	//camera->SetEye({ 5,5,-20.2f });

	const float easingMaxTime = 1.0f;
	if (playerPosition.z < playerEndPosition || easingTime < easingMaxTime)
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
		boss->SetShotLugTime(240);
		playerObject->SetMoveFlag(true);
		playerObject->SetShotFlag(true);
	}
}

void BossScene1::Scene3()
{
	//boss->SetShotFlag(false);
	//if (easingTime < 1.0f)
	//{
	//	easingTime += 0.005f;
	//	float rotationEasing = easeOut({ 5.0f,0.0f }, { 0.0f,0.0f }, easingTime).x;
	//	camera->SetEye({ rotationEasing,rotationEasing,-20.2f });
	//}
	//else
	//{
	//	sceneWave++;
	//	easingTime = 0;
	//	camera->SetTarget({ 0,0,0 });
	//	camera->SetEye({ 0,0,-20.2f });
	//	boss->SetShotFlag(true);
	//	boss->SetShotLugTime(240);
	//	playerObject->SetMoveFlag(true);
	//	playerObject->SetShotFlag(true);
	//}
}

void BossScene1::Scene4()
{
}

void BossScene1::Scene5()
{

}

Scene BossScene1::Next()
{
	if (playerObject->GetIsDeadFlag() == true)
	{
		return Scene::GAMEOVER;
	}

	return Scene::RESULT;
}

void BossScene1::LaserGaugeProcessing()
{
	int laserEnergy = playerObject->GetLaserEnergy();
	const float maxEnergy = 200.0f;
	if (laserEnergy < maxEnergy)
		laserGauge->SetColor({ 1 - (laserEnergy / maxEnergy),0.0f,laserEnergy / maxEnergy,1.0f });
	//���[�U�[�̃G�l���M�[�Q�[�W�̍X�V����
	laserGauge->SetScale({ 4.0f ,(laserEnergy / maxEnergy),4.0f });
	laserGauge->Update({ incrementValue,0.0f,0.0f });
	gaugeParticle->Update();

	if (particleLugtime <= 0 && playerObject->GetLaserIsDead() == false)
		if (gaugeParticle->GetParticleLength() < 20)
		{//���[�U�[�Q�[�W����̃p�[�e�B�N�����o
			const float rnd_pos = 1.5f;
			//�ύX�����܂�
			Vector3 pos{};
			const float rnd_vel = 0.2f;
			Vector3 vel{};
			pos = { laserGauge->GetPosition().x + ((float)rand() / RAND_MAX * rnd_pos * 4.0f + 10.0f), laserGauge->GetPosition().y + ((float)rand() / RAND_MAX * rnd_pos * 8.0f - 5.0f),laserGauge->GetPosition().z };

			Vector3 dis = { 0.0f,0.0f,0.0f };
			dis = { laserGauge->GetPosition().x - pos.x,laserGauge->GetPosition().y - pos.y ,0.0f };

			dis = dis.normalize();
			//X,Y,Z�S��[-0.05f,+0.05f]�Ń����_���ɕ��z
			vel = -dis * 0.2f;
			//�d�͂Ɍ����Ă�Y�̂�[-0.001f,0]�Ń����_���ɕ��z
			const float rnd_acc = 0.0001f;
			Vector3 acc{};
			int life = 0;

			acc.x = -rnd_acc;
			life = (int)(20.0f * ((dis.x * 2.0f) * (-1.0f)));

			pos = laserGauge->GetPosition();
			pos.x = pos.x - scrollPos + (laserEnergy / maxEnergy) * 16.0f;

			gaugeParticle->Add(life, pos, vel, acc, 1.0f, 0.0f, { 1 - (laserEnergy / maxEnergy),0.0f,laserEnergy / maxEnergy,1.0f }, { 1.0f,1.0f,1.0f,1.0f });

			particleLugtime = 2;
		}

	if (particleLugtime > 0)
	{
		particleLugtime--;
	}
}
