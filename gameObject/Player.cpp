#include "Player.h"
using namespace Microsoft::WRL;

//ID3D12Device* Player::device;
ID3D12GraphicsCommandList* Player::cmdList;
XMMATRIX Player::matView;
XMMATRIX Player::matProjection;
Camera* Player::camera = nullptr;

Player::Player(ID3D12GraphicsCommandList* arg_cmdList)
{
	cmdList = arg_cmdList;
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

void Player::CreateConstBuffer(ID3D12Device* device)
{
	HRESULT result;

	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB0));

	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
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

Player* Player::Create(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList,Vector3 position)
{
	Player* player = new Player(arg_cmdList);

	UnionCharacter* unionCharacter = UnionCharacter::Create(device, arg_cmdList);
	player->SetUnionCharacter(unionCharacter);

	player->Initialize();
	player->SetPosition(position);
	player->CreateConstBuffer(device);
	player->AttachBullet(device);

	ParticleManager* deathParticle = ParticleManager::Create(device);

	deathParticle->SetSelectColor(0);

	player->SetDeathParticleManager(deathParticle);

	ParticleManager* chargeParticleMan;
	chargeParticleMan = ParticleManager::Create(device);
	player->SetChargePerticleManager(chargeParticleMan);


	ParticleManager* chargeMaxParticleMan;
	chargeMaxParticleMan = ParticleManager::Create(device);
	player->SetChargeMaxPerticleManager(chargeMaxParticleMan);


	ParticleManager* chargeBulletParticleMan;
	chargeBulletParticleMan = ParticleManager::Create(device);
	player->SetChargeBulletPerticleManager(chargeBulletParticleMan);

	ParticleManager* jetParticleMan;
	jetParticleMan = ParticleManager::Create(device);
	player->SetJetPerticleManager(jetParticleMan);

	return player;
}

void Player::SetEye(const Vector3& eye)
{
	Player::camera->SetEye(eye);

	UpdateViewMatrix();
}

void Player::SetTarget(const Vector3& target)
{
	Player::camera->SetTarget(target);

	UpdateViewMatrix();
}

void Player::UpdateViewMatrix()
{
	matView = XMMatrixLookAtLH(XMLoadFloat3(&camera->GetEye()), XMLoadFloat3(&camera->GetTarget()), XMLoadFloat3(&camera->GetUp()));
}

void Player::SetOBJModel(OBJHighModel* arg_objModel, OBJModel* bulletModel, OBJHighModel* unionModel, OBJModel* uBulletmodel,OBJHighModel* laserModel, OBJHighModel* missileModel)
{
	objModel = arg_objModel;
	SetBulletModel(bulletModel);
	SetLaserModel(laserModel);
	SetMissileModel(missileModel);
	unionCharacter->SetOBJModel(unionModel, uBulletmodel);
}

void Player::SetBulletModel(OBJModel* bulletModel)
{
	for (int i = 0; i < bullets.size();i++)
	{
		bullets[i]->SetOBJModel(bulletModel);
	}

	chargeBullet->SetOBJModel(bulletModel);
}

void Player::SetLaserModel(OBJHighModel* laserModel)
{
	laser->SetOBJModel(laserModel);
}

void Player::SetMissileModel(OBJHighModel* missileModel)
{
	for (int i = 0; i < missiles.size(); i++)
	{
		missiles[i]->SetOBJModel(missileModel);
	}
}

void Player::AttachBullet(ID3D12Device* device)
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

//プレイヤーの更新処理
void Player::Update(const Vector3& incrementValue)
{
	if (isDeadFlag)
	{
		return;
	}

	//移動処理
	if (moveFlag)
	{
		Move(incrementValue);
	}

	if (shotFlag)
	{
		//弾の発射処理
		ShotBullet(incrementValue);
		//チャージ弾の発射処理
		ShotChargeBullet(incrementValue);
		//レーザーの発射処理
		ShotLaser(incrementValue);
		//ミサイルの発射処理
		ShotMissile(incrementValue);
	}

	DeathParticleProcessing();

	//プレイヤーと子機の当たり判定
	unionCharacter->IsPlayerCollision(position, 0.5f);
	//子機の更新処理
	unionCharacter->Update(this, incrementValue);

	SetScrollIncrement(incrementValue);
	//定数バッファの転送
	TransferConstBuff();

	UpdateAttack(incrementValue);

	//各パーティクルのアップデート処理
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
}

void Player::SetScrollIncrement(const Vector3& incrementValue)
{
	position += incrementValue;
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

void Player::ShotBullet(const Vector3& incrementValue)
{
	//弾発射処理
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

	if (shotLugTime >= 1)
	{
		shotLugTime--;
	}
}

void Player::ShotChargeBullet(const Vector3& incrementValue)
{
	//チャージ弾発射処理
	if (Input::GetInstance()->KeyState(DIK_LCONTROL))
	{
		if (chargeCount <= CHARGEMAXTIME&& particleLugtime < 1)
		if (chargeParticleManager->GetParticleLength() < ParticleManager::vertexCount)
		{//チャージ時のパーティクル処理
			for (int i = 0; i < 10; i++)
			{
				const float rnd_pos = 1.5f;
				//変更ここまで
				Vector3 pos{};
				const float rnd_vel = 0.2f;
				Vector3 vel{};
				Vector3 dis = { 0.0f,0.0f,0.0f };
				//重力に見立ててYのみ[-0.001f,0]でランダムに分布
				const float rnd_acc = 0.0001f;
				Vector3 acc{};

				if (bossSceneFlag)
				{
					pos = { position.x + ((float)rand() / RAND_MAX * rnd_pos * 4.0f),position.y + ((float)rand() / RAND_MAX * rnd_pos * 8.0f - 5.0f),position.z + ((float)rand() / RAND_MAX * rnd_pos * 4.0f) + 10.0f };
					dis = { position.x - pos.x,position.y - pos.y ,position.y - pos.z };
					dis = dis.normalize();
					//X,Y,Z全て[-0.05f,+0.05f]でランダムに分布
					vel = dis * 0.2f;
					acc.z = -rnd_acc;
					life = (int)(20.0f * ((dis.z * 2.0f) * (-1.0f)));
				}
				else
				{
					pos = { position.x + ((float)rand() / RAND_MAX * rnd_pos * 4.0f + 10.0f), position.y + ((float)rand() / RAND_MAX * rnd_pos * 8.0f - 5.0f),position.z };
					dis = { position.x - pos.x,position.y - pos.y ,0.0f };
					dis = dis.normalize();
					//X,Y,Z全て[-0.05f,+0.05f]でランダムに分布
					vel = dis * 0.2f;
					acc.x = -rnd_acc;
					life = (int)(20.0f * ((dis.x * 2.0f) * (-1.0f)));
				}

				pos.x = pos.x - centerPosition;
				chargeParticleManager->Add(life, pos, vel, acc, 2.0f, 0.0f, { 0.0f,0.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f,1.0f });
			}
			particleLugtime = MAXPLAYERLUGTIME;
		}
		chargeCount++;
	}
	else
	{//チャージ弾発射処理
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
	//チャージ完了時のパーティクル処理
	if (chargeCount > CHARGEMAXTIME&& particleLugtime < 1&& chargeMaxParticleManager->GetParticleLength() < 1)
	{
		const float rnd_pos = 1.5f;
		//変更ここまで
		Vector3 pos{};
		const float rnd_vel = 0.2f;
		Vector3 vel{};
		if(bossSceneFlag==false)
		pos = { position.x + 2.0f, position.y,  position.z };

		if (bossSceneFlag)
		pos = { position.x, position.y,  position.z + 2.0f };
		//X,Y,Z全て[-0.05f,+0.05f]でランダムに分布
		vel.x = 0.0f;
		vel.y = 0.0f;
		vel.z = 0.0f;

		Vector3 acc{};
		acc.x = 0.0f;

		life = 40;
		pos.x = pos.x - centerPosition;
		chargeMaxParticleManager->Add(life, pos, vel, acc, 4.0f, 0.0f, { 0.0f,0.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f,1.0f });

		particleLugtime = MAXPLAYERLUGTIME;
	}

	if (particleLugtime >= 1)
	{
		particleLugtime--;
	}
}

void Player::ShotLaser(const Vector3& incrementValue)
{
	//弾発射処理
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

void Player::ShotMissile(const Vector3& incrementValue)
{
	//弾発射処理
	if ((Input::GetInstance()->KeyState(DIK_SPACE) || Input::GetInstance()->PadButtonState(XINPUT_GAMEPAD_A)) && shotMissileLugTime < 1)
	{
		for (int i = 0; i < missiles.size()/2; i++)
		{
			if (missiles[i]->GetIsDeadFlag() == false || missiles[i + 2]->GetIsDeadFlag() == false)
				continue;

			//上部から出るミサイル
			missiles[i]->Initialize();

			//下部から出るミサイル
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
	// スケール、回転、平行移動行列の計算
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));

	matTrans = XMMatrixTranslation(position.x, position.y - 0.5f, position.z);

	// ワールド行列の合成
	matWorld = XMMatrixIdentity(); // 変形をリセット
	matWorld *= matScale; // ワールド行列にスケーリングを反映
	matWorld *= matRot; // ワールド行列に回転を反映
	matWorld *= matTrans; // ワールド行列に平行移動を反映

		// 親オブジェクトがあれば
	if (parent != nullptr) {
		// 親オブジェクトのワールド行列を掛ける
		matWorld *= parent->matWorld;
	}
	//無敵演出
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
	//ワールド行列の転送
	ConstBufferDataB0* constMap0 = nullptr;
	result = constBuffB0->Map(0, nullptr, (void**)&constMap0);
	constMap0->color = color;
	constMap0->mat = matWorld * matView * matProjection;	// 行列の合成
	constBuffB0->Unmap(0, nullptr);

	////マテリアルの転送
	//ConstBufferDataB1* constMap1 = nullptr;
	//result = constBuffB1->Map(0, nullptr, (void**)&constMap1);
	//constMap1->ambient = objModel->material.ambient;
	//constMap1->diffuse = objModel->material.diffuse;
	//constMap1->specular = objModel->material.specular;
	//constMap1->alpha = objModel->material.alpha;
	//constBuffB1->Unmap(0, nullptr);
}

void Player::Move(const Vector3& incrementValue)
{
	if (bossSceneFlag == false)
		rotation.x = 270.0f;

	if (bossSceneFlag)
		rotation.y = 0.0f;

	velocity = { 0,0,0 };
	centerPosition += incrementValue.x;

	if (spawnFlag == false)
	{
		if (Input::GetInstance()->KeyState(DIK_UP) || Input::GetInstance()->PadLStickState() == DIRTYPE::UP)
		{
			velocity.y = 0.2f;
			if (bossSceneFlag == false)
				rotation.x = 270.0f + 45.0f;
		}
		if (Input::GetInstance()->KeyState(DIK_DOWN) || Input::GetInstance()->PadLStickState() == DIRTYPE::DOWN)
		{
			velocity.y = -0.2f;
			if (bossSceneFlag == false)
				rotation.x = 270.0f - 45.0f;
		}
		if (Input::GetInstance()->KeyState(DIK_LEFT) || Input::GetInstance()->PadLStickState() == DIRTYPE::LEFT)
		{
			velocity.x = -0.2f;
			if (bossSceneFlag)
				rotation.y = 270.0f + 45.0f;
		}
		if (Input::GetInstance()->KeyState(DIK_RIGHT) || Input::GetInstance()->PadLStickState() == DIRTYPE::RIGHT)
		{
			velocity.x = 0.2f;
			if (bossSceneFlag)
				rotation.y = 45.0f;
		}
		//子機の取り外し処理
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

	//ジェットのパーティクル処理
	if (jetParticleManager->GetParticleLength() < 2)
	{
		const float rnd_pos = 1.5f;
		//変更ここまで
		Vector3 pos{};
		Vector3 dis{};
		const float rnd_vel = 0.2f;
		Vector3 vel{};
		Vector3 acc{};

		if (bossSceneFlag)
		{
			pos = { position.x + ((float)rand() / RAND_MAX * rnd_pos * 4.0f + 10.0f),position.y + ((float)rand() / RAND_MAX * rnd_pos * 8.0f - 5.0f),position.z };
			dis = { position.x - pos.x,position.y - pos.y ,position.z - pos.z };
			//X,Y,Z全て[-0.05f,+0.05f]でランダムに分布
			dis = dis.normalize();
			//X,Y,Z全て[-0.05f,+0.05f]でランダムに分布
			vel = dis * 0.05f;
			acc.z = -0.01f;
			pos = position;
			pos.x = pos.x - centerPosition;
			pos.x += 0.5f;
			pos.z -= 2.0f;
		}
		else
		{
			pos = { position.x + ((float)rand() / RAND_MAX * rnd_pos * 4.0f + 10.0f), position.y + ((float)rand() / RAND_MAX * rnd_pos * 8.0f - 5.0f),position.z };
			dis = { position.x - pos.x,position.y - pos.y ,0.0f };
			//X,Y,Z全て[-0.05f,+0.05f]でランダムに分布
			dis = dis.normalize();
			//X,Y,Z全て[-0.05f,+0.05f]でランダムに分布
			vel = dis * 0.05f;
			acc.x = -0.01f;
			pos = position;
			pos.x = pos.x - centerPosition;
			pos.x -= 2.0f;
		}

		jetParticleManager->Add(10, pos, vel, acc, 0.0f, 0.5f, { 0.0f,0.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f,1.0f });
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
				//X,Y,Z全て[-5.0f,+5.0f]でランダムに分布
				const float rnd_pos = 10.0f / 30;
				Vector3 pos = deathPosition;
				const float rnd_vel = 0.1f;
				Vector3 vel{};

				//X,Y,Z全て[-0.05f,+0.05f]でランダムに分布
				vel.x = ((float)rand() / RAND_MAX * rnd_vel - rnd_vel / 4.0f) * 2;
				vel.y = (float)rand() / RAND_MAX * rnd_vel * 2;
				vel.z = 0.0f;
				//重力に見立ててYのみ[-0.001f,0]でランダムに分布
				const float rnd_acc = 0.01f;
				Vector3 acc{};
				acc.y = -(float)rand() / RAND_MAX * rnd_acc;
				life = 40;
				pos.x = pos.x - centerPosition;
				deathParticle->Add(life, pos, vel, acc, 0.5f, 0.0f, { 0.0f,0.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f,1.0f });
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
	{//通常弾の当たり判定
		bullets[i]->IsCollision(otherObject);
	}

	for (int i = 0; i < missiles.size(); i++)
	{//通常弾の当たり判定
		missiles[i]->IsCollision(otherObject);
	}
	//チャージ弾の当たり判定
	chargeBullet->IsCollision(otherObject);
	//レーザー弾の当たり判定
	laser->IsCollision(otherObject);
	//子機の当たり判定
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

void Player::UpdateAttack(const Vector3& incrementValue)
{
	//通常弾更新処理
	for (int i = 0; i < bullets.size(); i++)
	{
		bullets[i]->Update(incrementValue);

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
	//チャージ弾更新処理
	if (chargeBullet->GetIsDeadFlag() == false)
	{
		chargeBullet->Update(incrementValue);

		if (chargeBulletParticleManager->GetParticleLength() < 1)
		{
			const float rnd_pos = 1.5f;
			//変更ここまで
			Vector3 pos{};
			const float rnd_vel = 0.2f;
			Vector3 vel{};
			if (bossSceneFlag == false)
				pos = { chargeBullet->GetPosition().x, chargeBullet->GetPosition().y, 0.0f };

			if (bossSceneFlag)
				pos = { chargeBullet->GetPosition().x, chargeBullet->GetPosition().y, chargeBullet->GetPosition().z };

			//X,Y,Z全て[-0.05f,+0.05f]でランダムに分布
			vel.x = 0.0f;
			vel.y = 0.0f;
			vel.z = 0.0f;

			Vector3 acc{};
			acc.x = 0.0f;

			life = 10;
			pos.x = pos.x - centerPosition;
			chargeBulletParticleManager->Add(life, pos, vel, acc, 2.0f, 0.0f, { 0.0f,0.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f,1.0f });

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
	//レーザー更新処理
	if (laser->GetIsDeadFlag() == false)
	{
		laser->Update(incrementValue);
	}

	//ミサイル更新処理
	for (int i = 0; i < missiles.size(); i++)
	{
		missiles[i]->Update(incrementValue, centerPosition);

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
