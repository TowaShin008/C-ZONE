#include "Player.h"
using namespace Microsoft::WRL;

ID3D12GraphicsCommandList* Player::cmdList;
XMMATRIX Player::matView;
XMMATRIX Player::matProjection;
Camera* Player::camera = nullptr;
ID3D12Device* Player::device = nullptr;

Player::Player(ID3D12GraphicsCommandList* arg_cmdList, ID3D12Device* arg_device)
{
	cmdList = arg_cmdList;
	device = arg_device;
	speed = { 0.2f,0.2f,0.0f };
	rotation = { 270.0f,0.0f,270.0f };
	scale = { 0.4f,0.4f,0.4f };
	velocity = { 0,0,0 };
	collisionRadius = 1.0f;
	moveFlag = true;
	shotFlag = true;
	laserEnergy = CHARGEMAXTIME;
	hp = 3;
	characterType = CHARACTERTYPE::PLAYER;
	matProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), (float)WindowSize::window_width / (float)WindowSize::window_height, 0.1f, 1000.0f);
}

Player::~Player()
{
	for (int i = 0; i < bullets.size(); i++)
	{
		delete bullets[i];
	}

	for (int i = 0; i < missiles.size(); i++)
	{
		delete missiles[i];
	}
}

void Player::CreateConstBuffer()
{
	HRESULT result;

	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// �A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB0));

	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// �A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB1) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB1));

	if (FAILED(result))
	{
		assert(0);
	}

	UpdateViewMatrix();
}

Player* Player::Create(ID3D12Device* arg_device, ID3D12GraphicsCommandList* arg_cmdList,const Vector3& arg_position)
{
	Player* player = new Player(arg_cmdList,arg_device);

	UnionCharacter* unionCharacter = UnionCharacter::Create(arg_device, arg_cmdList);

	//�}�l�[�W���[�N���X�ɓ����Ă���I�u�W�F�N�g�S�Ă��A�b�v�f�[�g
	ObjFileCharacter* laserGauge = ObjFileCharacter::Create(arg_device, arg_cmdList);
	laserGauge->SetColor({ 0.0f,0.0f,1.0f,1.0f });
	laserGauge->SetPosition({ -8.0f,-8.0f,-4.0f });


	player->SetUnionCharacter(unionCharacter);
	player->SetLaserGauge(laserGauge);

	player->Initialize();
	player->SetPosition(arg_position);
	player->CreateConstBuffer();
	player->AttachBullet();

	ParticleManager* deathParticle = ParticleManager::Create(arg_device);

	deathParticle->SetSelectColor(0);

	player->SetDeathParticleManager(deathParticle);

	ParticleManager* chargeParticleMan;
	chargeParticleMan = ParticleManager::Create(arg_device);
	player->SetChargePerticleManager(chargeParticleMan);


	ParticleManager* chargeMaxParticleMan;
	chargeMaxParticleMan = ParticleManager::Create(arg_device);
	player->SetChargeMaxPerticleManager(chargeMaxParticleMan);


	ParticleManager* chargeBulletParticleMan;
	chargeBulletParticleMan = ParticleManager::Create(arg_device);
	player->SetChargeBulletPerticleManager(chargeBulletParticleMan);

	ParticleManager* jetParticleMan;
	jetParticleMan = ParticleManager::Create(arg_device);
	player->SetJetPerticleManager(jetParticleMan);

	ParticleManager* laserGaugeParticletManager;
	laserGaugeParticletManager = ParticleManager::Create(arg_device);
	player->SetLaserGaugePerticleManager(laserGaugeParticletManager);

	return player;
}

void Player::SetEye(const Vector3& arg_eye)
{
	Player::camera->SetEye(arg_eye);

	UpdateViewMatrix();
}

void Player::SetTarget(const Vector3& arg_target)
{
	Player::camera->SetTarget(arg_target);

	UpdateViewMatrix();
}

void Player::UpdateViewMatrix()
{
	matView = XMMatrixLookAtLH(XMLoadFloat3(&camera->GetEye()), XMLoadFloat3(&camera->GetTarget()), XMLoadFloat3(&camera->GetUp()));
}

void Player::SetOBJModel(ObjFileModel* arg_objModel, ObjFileModel* arg_bulletModel, ObjFileModel* arg_unionModel, ObjFileModel* arg_uBulletmodel, ObjFileModel* arg_laserModel, ObjFileModel* arg_laserGauseModel, ObjFileModel* arg_missileModel)
{
	objModel = arg_objModel;
	SetBulletModel(arg_bulletModel);
	SetLaserModel(arg_laserModel);
	SetMissileModel(arg_missileModel);
	unionCharacter->SetOBJModel(arg_unionModel, arg_uBulletmodel);
	laserGauge->SetOBJModel(arg_laserGauseModel);
}

void Player::SetBulletModel(ObjFileModel* arg_bulletModel)
{
	for (int i = 0; i < bullets.size();i++)
	{
		bullets[i]->SetOBJModel(arg_bulletModel);
	}

	chargeBullet->SetOBJModel(arg_bulletModel);
}

void Player::SetLaserModel(ObjFileModel* arg_laserModel)
{
	laser->SetOBJModel(arg_laserModel);
}

void Player::SetLaserGaugeModel(ObjFileModel* arg_laserGaugeModel)
{
	laserGauge->SetOBJModel(arg_laserGaugeModel);
}


void Player::SetMissileModel(ObjFileModel* arg_missileModel)
{
	for (int i = 0; i < missiles.size(); i++)
	{
		missiles[i]->SetOBJModel(arg_missileModel);
	}
}

void Player::AttachBullet()
{
	bullets.resize(BULLETMAXNUM);

	for (int i = 0; i < bullets.size(); i++)
	{
		bullets[i] = Bullet::Create(device,cmdList);
		bullets[i]->SetIsDeadFlag(true);
		bullets[i]->SetCharacterType(CHARACTERTYPE::FRIEND);
		bullets[i]->SetVelocity({1.0f,0.0f,0.0f});
	}

	chargeBullet.reset(Bullet::Create(device,cmdList));
	chargeBullet->SetIsDeadFlag(true);
	chargeBullet->SetSpeed({ 1.0f,1.0f,1.0f });
	chargeBullet->SetCharacterType(CHARACTERTYPE::FRIEND);
	chargeBullet->SetScale({2.5f,2.5f,2.5f});
	chargeBullet->SetVelocity({ 0.5f,0.0f,0.0f });

	missiles.resize(MISSILEMAXNUM);

	for (int i = 0; i < missiles.size()/2; i++)
	{
		missiles[i] = Missile::Create(device,cmdList);
		missiles[i]->SetIsDeadFlag(true);
		missiles[i]->SetUpShotFlag(true);
		missiles[i]->SetCharacterType(CHARACTERTYPE::FRIEND);
		missiles[i]->SetVelocity({ 0.0f,0.0f,0.0f });
	}

	for (int i = missiles.size() / 2; i < missiles.size(); i++)
	{
		missiles[i] = Missile::Create(device,cmdList);
		missiles[i]->SetIsDeadFlag(true);
		missiles[i]->SetUpShotFlag(false);
		missiles[i]->SetCharacterType(CHARACTERTYPE::FRIEND);
		missiles[i]->SetVelocity({ 0.0f,0.0f,0.0f });
	}

	laser.reset(Laser::Create(device,cmdList));
	laser->SetIsDeadFlag(true);
	laser->SetCharacterType(CHARACTERTYPE::FRIEND);
	laser->SetScale({ 0.0f,5.0f,0.0f });
	laser->SetVelocity({ 0.0f,0.0f,0.0f });
}

void Player::SetShotFlag(bool arg_shotFlag)
{
	shotFlag = arg_shotFlag;
	unionCharacter->SetShotFlag(arg_shotFlag);
}

void Player::Initialize()
{
	rotation = { 270.0f,0.0f,270.0f };
	speed = { 0.2f,0.2f,0.0f };
	velocity = { 0,0,0 };
	isDeadFlag = false;
	moveFlag = true;
	shotFlag = true;
	shotLugTime = MAXPLAYERLUGTIME;
	shotMissileLugTime = MAXPLAYERLUGTIME;
	laserEnergy = CHARGEMAXTIME;
	centerPosition = 0.1f;
	deathParticleFlag = false;
	hp = 3;
	unionCharacter->Initialize();
	unionCharacter->SetLanchFlag(false);
	invisibleTime = 0;
}

//�v���C���[�̍X�V����
void Player::Update(const Vector3& arg_incrementValue)
{
	if (isDeadFlag)
	{
		return;
	}

	//�ړ�����
	if (moveFlag)
	{
		Move(arg_incrementValue);
	}

	if (shotFlag)
	{
		//�e�̔��ˏ���
		ShotBullet(arg_incrementValue);
		//�`���[�W�e�̔��ˏ���
		ShotChargeBullet(arg_incrementValue);
		//���[�U�[�̔��ˏ���
		ShotLaser(arg_incrementValue);
		//�~�T�C���̔��ˏ���
		ShotMissile(arg_incrementValue);
	}

	DeathParticleProcessing();

	//�v���C���[�Ǝq�@�̓����蔻��
	unionCharacter->IsPlayerCollision(position, 0.5f);
	//�q�@�̍X�V����
	unionCharacter->Update(this, arg_incrementValue);

	SetScrollIncrement(arg_incrementValue);
	//�萔�o�b�t�@�̓]��
	TransferConstBuff();

	UpdateAttack(arg_incrementValue);

	//���[�U�[�Q�[�W����
	LaserGaugeProcessing(arg_incrementValue);
	//�e�p�[�e�B�N���̃A�b�v�f�[�g����
	chargeParticleManager->Update();
	chargeMaxParticleManager->Update();
	chargeBulletParticleManager->Update();
	jetParticleManager->Update();
}

void Player::Draw()
{
	if (isDeadFlag == false)
	{
		Player::cmdList->SetPipelineState(PipelineState::simplePipelineState.Get());

		Player::cmdList->SetGraphicsRootSignature(RootSignature::simpleRootsignature.Get());

		Player::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		Player::cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());
		Player::cmdList->SetGraphicsRootConstantBufferView(1, constBuffB1->GetGPUVirtualAddress());

		objModel->Draw(Player::cmdList);

		if (motionBlurFlag == false)
		{
			unionCharacter->Draw();

			for (int i = 0; i < bullets.size(); i++)
			{
				bullets[i]->Draw();
			}

			chargeBullet->Draw();

			laser->Draw();

			for (int i = 0; i < missiles.size(); i++)
			{
				missiles[i]->Draw();
			}
			//�e�p�[�e�B�N���`��
			ParticleManager::PreDraw(cmdList);
			chargeParticleManager->Draw();
			chargeMaxParticleManager->Draw();
			chargeBulletParticleManager->Draw();
			jetParticleManager->Draw();
			ParticleManager::PostDraw();
		}
	}

	if (spawnFlag)
	{
		ParticleManager::PreDraw(cmdList);
		deathParticle->Draw();
		ParticleManager::PostDraw();
	}

	if (disPlayLaserGaugeFlag)
	{
		laserGauge->Draw();
		ParticleManager::PreDraw(cmdList);
		laserGaugeParticleManager->Draw();
		ParticleManager::PostDraw();
	}
}

void Player::SetScrollIncrement(const Vector3& arg_incrementValue)
{
	position += arg_incrementValue;
}

void Player::Damage()
{
	if (invisibleTime < 1)
	{
		const int maxRespawnTime = 360;
		hp--;
		deathPosition = position;
		Vector3 restartPosition = { centerPosition ,0.0f,-15.0f };

		if (bossSceneFlag == false)
		{
			const float normalFixPosX = -10.0f;
			restartPosition.x += normalFixPosX;
		}
		position = restartPosition;
		spawnFlag = true;
		deathParticleFlag = true;
		invisibleTime = maxRespawnTime;
	}
	if (hp < 1)
	{
		isDeadFlag = true;
	}

}

void Player::CriticalDamage()
{
	if (invisibleTime < 1)
	{
		hp--;
		deathPosition = position;
		Vector3 restartPosition = { centerPosition ,0.0f,-15.0f };

		if (bossSceneFlag == false)
		{
			const float normalFixPosX = -10.0f;
			restartPosition.x += normalFixPosX;
		}
		position = restartPosition;
		spawnFlag = true;
		deathParticleFlag = true;
		const int maxRespawnTime = 360;
		invisibleTime = maxRespawnTime;
	}
	if (hp < 1)
	{
		isDeadFlag = true;
	}
}

void Player::SetBossSceneFlag(bool arg_bossSceneFlag)
{
	bossSceneFlag = arg_bossSceneFlag;
	if (bossSceneFlag)
	{
		for (int i = 0; i < bullets.size(); i++)
		{
			bullets[i]->SetIsDeadFlag(true);
			bullets[i]->SetRotation({ 0.0f,270.0f,0.0f });
			bullets[i]->SetVelocity({ 0.0f,0.0f,1.0f });
		}

		chargeBullet->SetIsDeadFlag(true);
		chargeBullet->SetCollisionRadius(1.0f);
		chargeBullet->SetRotation({ 0.0f,270.0f,0.0f });
		chargeBullet->SetScale({ 2.5f,2.5f,2.5f });
		chargeBullet->SetVelocity({ 0.0f,0.0f,0.5f });

		for (int i = 0; i < missiles.size(); i++)
		{
			missiles[i]->SetIsDeadFlag(true);
			missiles[i]->SetBossSceneFlag(true);
		}

		laser->SetBossSceneFlag(true);

		rotation = { 270.0f,0.0f,180.0f };
	}
	else
	{
		for (int i = 0; i < bullets.size(); i++)
		{
			bullets[i]->SetIsDeadFlag(true);
			bullets[i]->SetRotation({ 0.0f,0.0f,0.0f });
			bullets[i]->SetVelocity({ 1.0f,0.0f,0.0f });
		}

		chargeBullet->SetIsDeadFlag(true);
		chargeBullet->SetCollisionRadius(1.0f);
		chargeBullet->SetRotation({ 0.0f,0.0f,0.0f });
		chargeBullet->SetScale({ 2.5f,2.5f,2.5f });
		chargeBullet->SetVelocity({ 0.5f,0.0f,0.0f });
		for (int i = 0; i < missiles.size(); i++)
		{
			missiles[i]->SetBossSceneFlag(false);
		}

		rotation = { 270.0f,0.0f,270.0f };
	}

	unionCharacter->SetBossSceneFlag(bossSceneFlag);
}

void Player::ShotBullet(const Vector3& arg_incrementValue)
{
	//�e���ˏ���
	if ((Input::GetInstance()->KeyState(DIK_SPACE) || Input::GetInstance()->PadButtonState(XINPUT_GAMEPAD_A))&& shotLugTime < 1)
	{
		for (int i = 0; i < bullets.size(); i++)
		{
			if (bullets[i]->GetIsDeadFlag() == false)
				continue;
			
			bullets[i]->Initialize();
			if (bossSceneFlag == false)
			bullets[i]->SetPosition({ position.x + 2.0f, position.y, position.z });

			if (bossSceneFlag)
				bullets[i]->SetPosition({ position.x -1.0f, position.y, position.z + 2.0f });

			shotLugTime = MAXPLAYERLUGTIME;
			break;
		}
	}
	//�Ĕ��ˎ���
	if (shotLugTime >= 1)
	{
		shotLugTime--;
	}
}

void Player::ShotChargeBullet(const Vector3& arg_incrementValue)
{
	//�`���[�W�e���ˏ���
	if (Input::GetInstance()->KeyState(DIK_LCONTROL))
	{
		if (chargeCount <= CHARGEMAXTIME&& particleLugtime < 1)
		if (chargeParticleManager->GetParticleLength() < ParticleManager::vertexCount)
		{//�`���[�W���̃p�[�e�B�N������
			for (int i = 0; i < 10; i++)
			{
				const float rnd_pos = 1.5f;

				Vector3 pos{};
				const float rnd_vel = 0.2f;
				Vector3 vel{};
				Vector3 dis = { 0.0f,0.0f,0.0f };
				const float rnd_acc = 0.0001f;
				Vector3 acc{};

				if (bossSceneFlag)
				{
					//�����_���ɐݒ肵���ʒu�ƃv���C���[�̈ʒu����������v�Z
					pos = { position.x + ((float)rand() / RAND_MAX * rnd_pos * 4.0f),position.y + ((float)rand() / RAND_MAX * rnd_pos * 8.0f - 5.0f),position.z + ((float)rand() / RAND_MAX * rnd_pos * 4.0f) + 10.0f };
					dis = { position.x - pos.x,position.y - pos.y ,position.y - pos.z };
					dis = dis.normalize();
					vel = dis * 0.2f;
					acc.z = -rnd_acc;
					life = (int)(20.0f * ((dis.z * 2.0f) * (-1.0f)));
				}
				else
				{
					//�����_���ɐݒ肵���ʒu�ƃv���C���[�̈ʒu����������v�Z
					pos = { position.x + ((float)rand() / RAND_MAX * rnd_pos * 4.0f + 10.0f), position.y + ((float)rand() / RAND_MAX * rnd_pos * 8.0f - 5.0f),position.z };
					dis = { position.x - pos.x,position.y - pos.y ,0.0f };
					dis = dis.normalize();
					vel = dis * 0.2f;
					acc.x = -rnd_acc;
					life = (int)(20.0f * ((dis.x * 2.0f) * (-1.0f)));
				}

				pos.x = pos.x - centerPosition;

				const float startScale = 2.0f;
				const float endScale = 0.0f;
				const Vector4 blue = { 0.0f,0.0f,1.0f,1.0f };
				const Vector4 white = { 1.0f,1.0f,1.0f,1.0f };
				const Vector4 startColor = blue;
				const Vector4 endColor = white;
				chargeParticleManager->Add(life, pos, vel, acc, startScale, endScale, startColor, endColor);
			}
			particleLugtime = MAXPLAYERLUGTIME;
		}
		chargeCount++;
	}
	else
	{//�`���[�W�e���ˏ���
		if (chargeCount > CHARGEMAXTIME)
		{
			chargeBullet->SetIsDeadFlag(false);
			chargeBullet->Initialize();
			if(bossSceneFlag)
			chargeBullet->SetPosition({ position.x, position.y, position.z + 2.0f });

			if(bossSceneFlag==false)
			chargeBullet->SetPosition({ position.x + 2.0f, position.y, position.z });
		}
		chargeCount = 0;
	}
	//�`���[�W�������̃p�[�e�B�N������
	if (chargeCount > CHARGEMAXTIME&& particleLugtime < 1&& chargeMaxParticleManager->GetParticleLength() < 1)
	{
		const float rnd_pos = 1.5f;

		Vector3 pos{};

		Vector3 vel = { 0.0f,0.0f,0.0f };

		Vector3 acc = { 0.0f,0.0f,0.0f };

		const float playerFrontPosition = 2.0f;

		if (bossSceneFlag == false)
		{
			pos = { position.x + playerFrontPosition, position.y,  position.z };
		}

		if (bossSceneFlag)
		{
			pos = { position.x, position.y,  position.z + playerFrontPosition };
		}

		life = 40;
		pos.x = pos.x - centerPosition;

		const float startScale = 4.0f;
		const float endScale = 0.0f;
		const Vector4 blue = { 0.0f,0.0f,1.0f,1.0f };
		const Vector4 white = { 1.0f,1.0f,1.0f,1.0f };
		const Vector4 startColor = blue;
		const Vector4 endColor = white;
		chargeMaxParticleManager->Add(life, pos, vel, acc, startScale, endScale, startColor, endColor);

		particleLugtime = MAXPLAYERLUGTIME;
	}

	if (particleLugtime >= 1)
	{
		particleLugtime--;
	}
}

void Player::ShotLaser(const Vector3& arg_incrementValue)
{
	//�e���ˏ���
	if ((Input::GetInstance()->KeyState(DIK_B) || Input::GetInstance()->PadButtonState(XINPUT_GAMEPAD_B))&&isAbleToChargeLaser == false)
	{
		if (laserEnergy <= 0) 
		{
			isAbleToChargeLaser = true;
			return;
		}

		Vector3 scale = laser->GetScale();
		if (scale.x < 5.0f)
		{
			scale.x += 0.5f;
			scale.z += 0.5f;
			laser->SetScale(scale);
			laser->Initialize();
		}
		else
		{
			laserEnergy--;
		}
		laser->SetPosition(position);


		if (bossSceneFlag == false)
			laser->SetPosition({ position.x + 1.5f, position.y, position.z });

		if (bossSceneFlag)
			laser->SetPosition({ position.x, position.y, position.z + 2.0f });
	}
	else
	{
		if (chargeLugTime <= 0)
		{
			if (isAbleToChargeLaser)
				laserEnergy++;

			chargeLugTime = CHARGELUTIME;
		}
		else
		{
			chargeLugTime--;
		}

		if (laserEnergy >= CHARGEMAXTIME)
		{
			isAbleToChargeLaser = false;
		}

		Vector3 scale = laser->GetScale();
		if (scale.x > 0.0f)
		{
			scale.x -= 0.5f;
			scale.z -= 0.5f;
			laser->SetScale(scale);
		}
		else
		{
			laser->SetIsDeadFlag(true);
		}
	}
}

void Player::ShotMissile(const Vector3& arg_incrementValue)
{
	//�e���ˏ���
	if ((Input::GetInstance()->KeyState(DIK_SPACE) || Input::GetInstance()->PadButtonState(XINPUT_GAMEPAD_A)) && shotMissileLugTime < 1)
	{
		for (int i = 0; i < missiles.size()/2; i++)
		{
			if (missiles[i]->GetIsDeadFlag() == false || missiles[i + 2]->GetIsDeadFlag() == false)
				continue;

			//�㕔����o��~�T�C��
			missiles[i]->Initialize();

			//��������o��~�T�C��
			missiles[i + 2]->Initialize();

			if (bossSceneFlag)
			{
				missiles[i]->SetPosition({ position.x, position.y + 1.0f, position.z });
				missiles[i + 2]->SetPosition({ position.x, position.y - 2.0f, position.z });
			}
			else
			{
				missiles[i]->SetPosition({ position.x, position.y + 1.0f, position.z });
				missiles[i + 2]->SetPosition({ position.x, position.y - 2.0f, position.z });
			}
			shotMissileLugTime = MAXPLAYERLUGTIME;
			break;
		}
	}

	if (shotMissileLugTime >= 1)
	{
		shotMissileLugTime--;
	}
}

void Player::TransferConstBuff()
{
	velocity = velocity.normalize();
	position = position + velocity * speed;

	HRESULT result;
	XMMATRIX matScale, matRot, matTrans;
	UpdateViewMatrix();
	// �X�P�[���A��]�A���s�ړ��s��̌v�Z
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));

	matTrans = XMMatrixTranslation(position.x, position.y - 0.5f, position.z);

	// ���[���h�s��̍���
	matWorld = XMMatrixIdentity(); // �ό`�����Z�b�g
	matWorld *= matScale; // ���[���h�s��ɃX�P�[�����O�𔽉f
	matWorld *= matRot; // ���[���h�s��ɉ�]�𔽉f
	matWorld *= matTrans; // ���[���h�s��ɕ��s�ړ��𔽉f

	// �e�I�u�W�F�N�g�������
	if (parent != nullptr) {
		// �e�I�u�W�F�N�g�̃��[���h�s����|����
		matWorld *= parent->matWorld;
	}
	//���G���o
	if (invisibleTime > 0)
	{
		invisibleTime--;
		if (invisibleTime % 20 <= 10)
		{
			const Vector4 black = { 0.0f,0.0f,0.0f,1.0f };
			color = black;
		}
		else
		{
			const Vector4 white = { 1.0f,1.0f,1.0f,1.0f };
			color = white;
		}
	}
	else
	{
		const Vector4 white = { 1.0f,1.0f,1.0f,1.0f };
		color = white;
	}
	//���[���h�s��̓]��
	ConstBufferDataB0* constMap0 = nullptr;
	result = constBuffB0->Map(0, nullptr, (void**)&constMap0);
	constMap0->color = color;
	constMap0->mat = matWorld * matView * matProjection;	// �s��̍���
	constBuffB0->Unmap(0, nullptr);
}

void Player::Move(const Vector3& arg_incrementValue)
{
	if (bossSceneFlag == false)
		rotation.x = 270.0f;

	if (bossSceneFlag)
		rotation.y = 0.0f;

	velocity = { 0,0,0 };
	centerPosition += arg_incrementValue.x;

	if (spawnFlag == false)
	{
		//��
		if (Input::GetInstance()->KeyState(DIK_UP) || Input::GetInstance()->PadLStickState() == DIRTYPE::UP)
		{
			velocity.y = 0.2f;
			if (bossSceneFlag == false)
				rotation.x = 270.0f + 45.0f;
		}
		//��
		if (Input::GetInstance()->KeyState(DIK_DOWN) || Input::GetInstance()->PadLStickState() == DIRTYPE::DOWN)
		{
			velocity.y = -0.2f;
			if (bossSceneFlag == false)
				rotation.x = 270.0f - 45.0f;
		}
		//��
		if (Input::GetInstance()->KeyState(DIK_LEFT) || Input::GetInstance()->PadLStickState() == DIRTYPE::LEFT)
		{
			velocity.x = -0.2f;
			if (bossSceneFlag)
				rotation.y = 270.0f + 45.0f;
		}
		//�E
		if (Input::GetInstance()->KeyState(DIK_RIGHT) || Input::GetInstance()->PadLStickState() == DIRTYPE::RIGHT)
		{
			velocity.x = 0.2f;
			if (bossSceneFlag)
				rotation.y = 45.0f;
		}
		//�q�@�̎��O������
		if (Input::GetInstance()->KeyTriggerState(DIK_Z))
		{
			if (unionCharacter->GetLanchFlag() == false)
			{
				unionCharacter->SetLanchFlag(true);
			}
			else
			{
				unionCharacter->SetLanchFlag(false);
			}
		}
	}


	if (position.y > SCREENTOP)
	{
		position.y = SCREENTOP;
	}
	if (position.y < SCREENBUTTOM)
	{
		position.y = SCREENBUTTOM;
	}
	if (position.x < SCREENLEFT + centerPosition)
	{
		position.x = SCREENLEFT + centerPosition;
	}
	if (position.x > SCREENRIGHT + centerPosition)
	{
		position.x = SCREENRIGHT + centerPosition;
	}

	if (spawnFlag)
	{
		SpawnProcessing();
	}

	//�W�F�b�g�̃p�[�e�B�N������
	if (jetParticleManager->GetParticleLength() < 2)
	{
		const float rnd_pos = 1.5f;
		//�ύX�����܂�
		Vector3 pos{};
		Vector3 dis{};
		const float rnd_vel = 0.2f;
		Vector3 vel{};
		Vector3 acc{};

		if (bossSceneFlag)
		{
			//�����_���ɐݒ肵���ʒu�ƃv���C���[�̈ʒu����������v�Z
			pos = { position.x + ((float)rand() / RAND_MAX * rnd_pos * 4.0f + 10.0f),position.y + ((float)rand() / RAND_MAX * rnd_pos * 8.0f - 5.0f),position.z };
			dis = { position.x - pos.x,position.y - pos.y ,position.z - pos.z };
			dis = dis.normalize();
			vel = dis * 0.05f;
			acc.z = -0.01f;
			pos = { position.x - centerPosition + 0.5f,position.y,position.z - 2.0f };
		}
		else
		{
			//�����_���ɐݒ肵���ʒu�ƃv���C���[�̈ʒu����������v�Z
			pos = { position.x + ((float)rand() / RAND_MAX * rnd_pos * 4.0f + 10.0f), position.y + ((float)rand() / RAND_MAX * rnd_pos * 8.0f - 5.0f),position.z };
			dis = { position.x - pos.x,position.y - pos.y ,0.0f };
			dis = dis.normalize();
			vel = dis * 0.05f;
			acc.x = -0.01f;
			pos = { position.x - centerPosition - 2.0f,position.y,position.z };
		}

		const float startScale = 0.0f;
		const float endScale = 0.5f;
		const Vector4 blue = { 0.0f,0.0f,1.0f,1.0f };
		const Vector4 white = { 1.0f,1.0f,1.0f,1.0f };
		const Vector4 startColor = blue;
		const Vector4 endColor = white;
		jetParticleManager->Add(10, pos, vel, acc, startScale, endScale, startColor, endColor);
	}
}

void Player::DeathParticleProcessing()
{
	if (deathParticleFlag)
	{
		if (deathParticle->GetParticleLength() < 10)
		{
			for (int i = 0; i < 10; i++)
			{

				const float rnd_pos = 10.0f / 30;
				Vector3 pos = deathPosition;
				const float rnd_vel = 0.1f;
				Vector3 vel{};

				//X[-0.05f,+0.15f]�Ń����_���ɕ��z
				vel.x = ((float)rand() / RAND_MAX * rnd_vel - rnd_vel / 4.0f) * 2;
				//Y[-0.0f,+0.2f]�Ń����_���ɕ��z
				vel.y = (float)rand() / RAND_MAX * rnd_vel * 2;
				vel.z = 0.0f;

				const float rnd_acc = 0.01f;
				Vector3 acc{};

				//�d�͂Ɍ����Ă�Y�̂�[-0.01f,0]�Ń����_���ɕ��z
				acc.y = -(float)rand() / RAND_MAX * rnd_acc;
				life = 40;
				pos.x = pos.x - centerPosition;

				const float startScale = 0.5f;
				const float endScale = 0.0f;
				const Vector4 blue = { 0.0f,0.0f,1.0f,1.0f };
				const Vector4 white = { 1.0f,1.0f,1.0f,1.0f };
				const Vector4 startColor = blue;
				const Vector4 endColor = white;
				deathParticle->Add(life, pos, vel, acc, startScale, endScale, startColor, endColor);
			}
			deathParticleFlag = false;
		}
	}

	deathParticle->Update();
}

bool Player::IsCollision(GameObject* arg_otherObject)
{
	GameObject* otherObject = arg_otherObject;

	if (isDeadFlag || otherObject->GetIsDeadFlag() || otherObject->GetCharacterType() == CHARACTERTYPE::FRIEND) { return false; }

	for (int i = 0; i < bullets.size(); i++)
	{//�ʏ�e�̓����蔻��
		bullets[i]->IsCollision(otherObject);
	}

	for (int i = 0; i < missiles.size(); i++)
	{//�ʏ�e�̓����蔻��
		missiles[i]->IsCollision(otherObject);
	}
	//�`���[�W�e�̓����蔻��
	chargeBullet->IsCollision(otherObject);
	//���[�U�[�e�̓����蔻��
	laser->IsCollision(otherObject);
	//�q�@�̓����蔻��
	unionCharacter->IsCollision(otherObject);

	if (Collision::CheckSphereToSphere({ otherObject->GetPosition(),otherObject->GetCollisionRadius() },
		{ position,collisionRadius }))
	{
		Damage();
		return true;
	}

	return false;
}

bool Player::SpawnProcessing()
{
	if (position.z < 0.0f)
	{
		shotFlag = false;
		position.z += 0.1f;
	}
	else
	{
		shotFlag = true;
		spawnFlag = false;
		return true;
	}

	return false;
}

void Player::UpdateAttack(const Vector3& arg_incrementValue)
{
	//�ʏ�e�X�V����
	for (int i = 0; i < bullets.size(); i++)
	{
		bullets[i]->Update(arg_incrementValue);

		if (bossSceneFlag == false)
		{
			if (bullets[i]->GetPosition().x > centerPosition + SCREENRIGHT + 10.0f)
			{
				bullets[i]->SetIsDeadFlag(true);
			}
		}
		else
		{
			if (bullets[i]->GetPosition().z > position.z + SCREENBACK)
			{
				bullets[i]->SetIsDeadFlag(true);
			}
		}
	}
	//�`���[�W�e�X�V����
	if (chargeBullet->GetIsDeadFlag() == false)
	{
		chargeBullet->Update(arg_incrementValue);

		if (chargeBulletParticleManager->GetParticleLength() < 1)
		{
			const float rnd_pos = 1.5f;
			Vector3 pos{};
			const float rnd_vel = 0.2f;
			Vector3 vel = { 0.0f,0.0f,0.0f };
			Vector3 acc = { 0.0f,0.0f,0.0f };
			if (bossSceneFlag == false)
			{
				pos = { chargeBullet->GetPosition().x, chargeBullet->GetPosition().y, 0.0f };
			}

			if (bossSceneFlag)
			{
				pos = { chargeBullet->GetPosition().x, chargeBullet->GetPosition().y, chargeBullet->GetPosition().z };
			}

			life = 10;
			pos.x = pos.x - centerPosition;

			const float startScale = 2.0f;
			const float endScale = 0.0f;
			const Vector4 blue = { 0.0f,0.0f,1.0f,1.0f };
			const Vector4 white = { 1.0f,1.0f,1.0f,1.0f };
			const Vector4 startColor = blue;
			const Vector4 endColor = white;
			chargeBulletParticleManager->Add(life, pos, vel, acc, startScale, endScale, blue, white);

			particleLugtime = MAXPLAYERLUGTIME;
		}


		if (bossSceneFlag == false)
		{
			if (chargeBullet->GetPosition().x > centerPosition + SCREENRIGHT + 10.0f)
			{
				chargeBullet->SetIsDeadFlag(true);
			}
		}
		else
		{
			if (chargeBullet->GetPosition().z > position.z + SCREENBACK)
			{
				chargeBullet->SetIsDeadFlag(true);
			}
		}
	}
	//���[�U�[�X�V����
	if (laser->GetIsDeadFlag() == false)
	{
		laser->Update(arg_incrementValue);
	}

	//�~�T�C���X�V����
	for (int i = 0; i < missiles.size(); i++)
	{
		missiles[i]->Update(arg_incrementValue, centerPosition);

		if (bossSceneFlag == false)
		{
			if (missiles[i]->GetPosition().x > centerPosition + SCREENRIGHT + 10.0f)
			{
				missiles[i]->SetIsDeadFlag(true);
			}
		}
		else
		{
			if (missiles[i]->GetPosition().z > position.z + SCREENBACK)
			{
				missiles[i]->SetIsDeadFlag(true);
			}
		}
	}
}

void Player::LaserGaugeProcessing(const Vector3& arg_incrementValue)
{
	const float maxEnergy = 200.0f;
	if (laserEnergy < maxEnergy)
	{
		laserGauge->SetColor({ 1 - (laserEnergy / maxEnergy),0.0f,laserEnergy / maxEnergy,1.0f });
	}
	//���[�U�[�̃G�l���M�[�Q�[�W�̍X�V����
	laserGauge->SetScale({ 4.0f ,(laserEnergy / maxEnergy),4.0f });
	laserGauge->Update(arg_incrementValue);
	laserGaugeParticleManager->Update();

	if (laserParticleLugtime <= 0 && laser->GetIsDeadFlag() == false)
	{
		if (laserGaugeParticleManager->GetParticleLength() < 20)
		{//���[�U�[�Q�[�W����̃p�[�e�B�N�����o
			const float rnd_pos = 1.5f;

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
			pos.x = pos.x - centerPosition + (laserEnergy / maxEnergy) * 16.0f;

			const float startScale = 1.0f;
			const float endScale = 0.0f;
			const Vector4 white = { 1.0f,1.0f,1.0f,1.0f };
			const Vector4 startColor = { 1 - (laserEnergy / maxEnergy),0.0f,laserEnergy / maxEnergy,1.0f };
			const Vector4 endColor = white;
			laserGaugeParticleManager->Add(life, pos, vel, acc, startScale, endScale, startColor, endColor);

			laserParticleLugtime = 2;
		}
	}

	if (laserParticleLugtime > 0)
	{
		laserParticleLugtime--;
	}
}
