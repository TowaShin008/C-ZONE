#include "FinalBoss.h"
using namespace Microsoft::WRL;

ID3D12GraphicsCommandList* FinalBoss::cmdList;
XMMATRIX FinalBoss::matView;
XMMATRIX FinalBoss::matProjection;
Camera* FinalBoss::camera = nullptr;

FinalBoss::FinalBoss(ID3D12GraphicsCommandList* arg_cmdList)
{
	cmdList = arg_cmdList;
	scale = { 1.0f,1.0f,1.0f };
	collisionRadius = 2.0f;
	rotation = { 0.0f,0.0f,0.0f };
	speed = { 0.1f,0.1f,0.0f };
	characterType = CHARACTERTYPE::ENEMY;
	matProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), (float)WindowSize::window_width / (float)WindowSize::window_height, 0.1f, 1000.0f);
}

FinalBoss::~FinalBoss()
{
	delete bodyBlock;

	for (int i = 0; i < bigBullets.size(); ++i)
	{
		delete bigBullets[i];
		bigBullets[i] = nullptr;
	}

	delete parentLeftWing;
	for (int i = 0; i < leftWing.size(); ++i)
	{
		delete leftWing[i];
		leftWing[i] = nullptr;
	}

	delete parentRightWing;
	for (int i = 0; i < rightWing.size(); ++i)
	{
		delete rightWing[i];
		rightWing[i] = nullptr;
	}

	//�����̓Q�[���I�u�W�F�N�g�}�l�[�W���[�ō폜���Ă��邽�ߋL�q����K�v���Ȃ�
	//delete leftEye;
	//delete rightEye;
}

void FinalBoss::CreateConstBuffer(ID3D12Device* device)
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

FinalBoss* FinalBoss::Create(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList, Vector3 position)
{
	FinalBoss* finalBoss = new FinalBoss(arg_cmdList);
	finalBoss->SetPosition(position);
	finalBoss->CreateConstBuffer(device);
	finalBoss->AttachBody(device);
	finalBoss->AttachEye(device);
	finalBoss->AttachLeftWing(device);
	finalBoss->AttachRightWing(device);
	finalBoss->AttachBullet(device);
	finalBoss->Initialize();

	ParticleManager* deathParticle = ParticleManager::Create(device);
	const int red = 2;
	deathParticle->SetSelectColor(red);
	finalBoss->SetDeathParticleManager(deathParticle);

	return finalBoss;
}

void FinalBoss::SetEye(const Vector3& eye)
{
	FinalBoss::camera->SetEye(eye);

	UpdateViewMatrix();
}

void FinalBoss::SetTarget(const Vector3& target)
{
	FinalBoss::camera->SetTarget(target);

	UpdateViewMatrix();
}

void FinalBoss::UpdateViewMatrix()
{
	matView = XMMatrixLookAtLH(XMLoadFloat3(&camera->GetEye()), XMLoadFloat3(&camera->GetTarget()), XMLoadFloat3(&camera->GetUp()));
}

void FinalBoss::SetOBJModel(OBJModel* eyeModel, OBJModel* bodyModel, OBJModel* wingModel, OBJHighModel* bulletModel)
{
	SetEyeModel(eyeModel);
	SetBodyModel(bodyModel);
	SetWingModel(wingModel);
	SetBulletModel(bulletModel);
}

void FinalBoss::SetBodyModel(OBJModel* bodyModel)
{
	bodyBlock->SetOBJModel(bodyModel);
}


void FinalBoss::SetEyeModel(OBJModel* eyeModel)
{
	finalBossEye->SetOBJModel(eyeModel);
}

void FinalBoss::SetWingModel(OBJModel* wingModel)
{
	for (int i = 0; i < leftWing.size(); ++i)
	{
		leftWing[i]->SetOBJModel(wingModel);
	}

	for (int i = 0; i < rightWing.size(); ++i)
	{
		rightWing[i]->SetOBJModel(wingModel);
	}
}

void FinalBoss::SetBulletModel(OBJHighModel* bulletModel)
{
	for (int i = 0; i < bigBullets.size(); ++i)
	{
		bigBullets[i]->SetOBJModel(bulletModel);
	}
}

void FinalBoss::AttachBody(ID3D12Device* device)
{
	bodyBlock = Block::Create(device, cmdList, { 0,0,position.z });
	bodyBlock->SetStageBlockFlag(false);
	bodyBlock->SetScale({ 1.0f,1.0f,1.0f });
	bodyBlock->Update({ 0,0,0 });
	const Vector4 white = { 1.0f,1.0f,1.0f,1.0f };
	bodyBlock->SetColor(white);
}

void FinalBoss::AttachEye(ID3D12Device* device)
{
	finalBossEye = FinalBossEye::Create(device, cmdList, { 0.0f,2.0f + position.y , position.z - 13.5f });
	finalBossEye->SetScale({ 3.0f,3.0f,1.0f });
	finalBossEye->Update({ 0.0f,0.0f,0.0f });
	const Vector4 white = { 1.0f,1.0f,1.0f,1.0f };
	finalBossEye->SetColor(white);
}

void FinalBoss::AttachLeftWing(ID3D12Device* device)
{
	parentLeftWing = Block::Create(device, cmdList, { -3.0f, position.y,position.z });
	parentLeftWing->SetScale({ 2.0f,2.0f,2.0f });
	parentLeftWing->Update({ 0,0,0 });
	const int leftWingNum = 6;
	leftWing.resize(leftWingNum);
	for (int i = 0; i < leftWing.size(); ++i)
	{
		if (i < leftWingNum / 2)
		{
			leftWing[i] = Block::Create(device, cmdList, { - 9.0f, i * 2.0f + position.y,position.z });
			leftWing[i]->SetSpeed({ 0.01f,0.01f,0.01f });
		}
		else
		{
			leftWing[i] = Block::Create(device, cmdList, { - 12.0f, (i - 3) * 4.0f + position.y - 2.0f,position.z });
			leftWing[i]->SetSpeed({ 0.05f,0.05f,0.05f });
		}
		leftWing[i]->SetStageBlockFlag(false);
		leftWing[i]->SetScale({ 1.0f,1.0f,1.0f });
		leftWing[i]->Update({ 0.0f,0,0 });
		leftWing[i]->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		leftWing[i]->SetParent(parentLeftWing);

		const int windNum = i % 3;
		if (windNum == 0)
		{
			leftWing[i]->SetRotation({ 0.0f,0.0f, 45.0f });
		}
		else if (windNum == 2)
		{
			leftWing[i]->SetRotation({ 0.0f,0.0f,-45.0f });
		}
	}
}

void FinalBoss::AttachRightWing(ID3D12Device* device)
{
	parentRightWing = Block::Create(device, cmdList, { 3.0f, position.y,position.z });
	parentRightWing->SetScale({ 2.0f,2.0f,2.0f });
	parentRightWing->Update({ 0,0,0 });
	const int RightWingNum = 6;
	rightWing.resize(RightWingNum);
	for (int i = 0; i < rightWing.size(); ++i)
	{
		if (i < RightWingNum / 2)
		{
			rightWing[i] = Block::Create(device, cmdList, { 9.0f, i * 2.0f + position.y,position.z });
			rightWing[i]->SetSpeed({ 0.01f,0.01f,0.01f });
		}
		else
		{
			rightWing[i] = Block::Create(device, cmdList, { 12.0f, (i - 3) * 4.0f + position.y - 2.0f,position.z });
			rightWing[i]->SetSpeed({ 0.05f,0.05f,0.05f });
		}
		//rightWing[i] = Block::Create(device, cmdList, { i * 3.0f + 3.0f, 7.1f + position.y,position.z });
		rightWing[i]->SetStageBlockFlag(false);
		rightWing[i]->SetScale({ 1.0f,1.0f,1.0f });

		rightWing[i]->Update({ 0.0f,0,0 });
		rightWing[i]->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		rightWing[i]->SetParent(parentRightWing);

		const int windNum = i % 3;
		if (windNum == 0)
		{
			rightWing[i]->SetRotation({ 0.0f,0.0f,-45.0f });
		}
		else if (windNum == 2)
		{
			rightWing[i]->SetRotation({ 0.0f,0.0f, 45.0f });
		}
	}
}

void FinalBoss::AttachBullet(ID3D12Device* device)
{
	bigBullets.resize(BIGBULLETMAXNUM);
	for (int i = 0; i < bigBullets.size(); ++i)
	{
		bigBullets[i] = HomingBullet::Create(device, cmdList);
		bigBullets[i]->SetIsDeadFlag(true);
		bigBullets[i]->SetCollisionRadius(2.0f);
		bigBullets[i]->SetCharacterType(CHARACTERTYPE::ENEMY);
		bigBullets[i]->SetScale({ 2.5f,2.5f,2.5f });
	}

}

void FinalBoss::Initialize()
{
	isDeadFlag = false;
	shotLugTime = 60;
	currentPhase = MOVEPHASE::PHASE1;
	currentType = MOVETYPE::STAY;
	moveEndFlag = false;
	shotFlag = true;
	velocity = { 0,0,0 };
	color = { 1.0f,1.0f,1.0f,1.0f };
	speed = { 0.1f,0.1f,0.0f };
	hp = 400;
	deathParticleFlag = false;
	invisibleTime = 0;
	entryFlag = true;
	InitialPosture();
}
//�v���C���[�̍X�V����
void FinalBoss::Update(const Vector3& incrementValue, const Vector3& playerPosition)
{
	centerPosition += incrementValue.x;

	if (isDeadFlag == false)
	{
		SetScrollIncrement(incrementValue);

		if (moveEndFlag == false)
		{
			Move();
		}

		DamageEffect();


		if (finalBossEye->GetHp() <= 0)
		{
			hp = 0;
		}

		if (entryFlag)
		{
			EntryPerformance();
		}
		else
		{
			if (bodyAttackFlag&&currentPhase!=MOVEPHASE::PHASE3)
			{
				BodyAttack();
			}
			else
			{
				const int randBodyAttack = rand() % 400;
				BreathMove();
				if (randBodyAttack == 0)
				{//�̓�����ȑO�̗��̉�]���ۑ����Ă���
					previousParentLeftWingRotation = parentLeftWing->GetRotasion();
					previousParentRightWingRotation = parentRightWing->GetRotasion();
					bodyAttackFlag = true;
				}
			}
		}


		//�ˌ�����
		ShotBullet(incrementValue, playerPosition);

		if (blinkFlag)
		{//�u������
			Blink();
		}

		//�}�e���A���̓]��
		//ConstBufferDataB1* constMap1 = nullptr;
		//result = constBuffB1->Map(0, nullptr, (void**)&constMap1);
		//constMap1->ambient = objModel->material.ambient;
		//constMap1->diffuse = objModel->material.diffuse;
		//constMap1->specular = objModel->material.specular;
		//constMap1->alpha = objModel->material.alpha;
		//constBuffB1->Unmap(0, nullptr);
	}

	DeathParticleProcessing();
}

void FinalBoss::Draw()
{
	if (isDeadFlag)
	{
		return;
	}

	TransferConstBuff();

	FinalBoss::cmdList->SetPipelineState(PipelineState::simplePipelineState.Get());

	FinalBoss::cmdList->SetGraphicsRootSignature(RootSignature::simpleRootsignature.Get());

	FinalBoss::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	FinalBoss::cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());
	FinalBoss::cmdList->SetGraphicsRootConstantBufferView(1, constBuffB1->GetGPUVirtualAddress());

	finalBossEye->Draw();

	for (int i = 0; i < leftWing.size(); ++i)
	{
		leftWing[i]->Draw();
	}

	for (int i = 0; i < rightWing.size(); ++i)
	{
		rightWing[i]->Draw();
	}

	if (deathParticleFlag)
	{
		ParticleManager::PreDraw(cmdList);
		deathParticle->Draw();
		ParticleManager::PostDraw();
	}

	for (int i = 0; i < bigBullets.size(); i++)
	{
		if (bigBullets[i]->GetIsDeadFlag() == false)
		{
			bigBullets[i]->Draw();
		}
	}
}

void FinalBoss::TransferConstBuff()
{
	HRESULT result;

	XMMATRIX matScale, matRot, matTrans;
	UpdateViewMatrix();
	// �X�P�[���A��]�A���s�ړ��s��̌v�Z
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matTrans = XMMatrixTranslation(position.x, position.y - 4.0f, position.z);

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

	bodyBlock->Draw();

	//���[���h�s��̓]��
	ConstBufferDataB0* constMap0 = nullptr;
	result = constBuffB0->Map(0, nullptr, (void**)&constMap0);
	constMap0->color = color;
	constMap0->mat = matWorld * matView * matProjection;	// �s��̍���
	constBuffB0->Unmap(0, nullptr);
}

void FinalBoss::Move()
{
	//�����̎��
	switch (currentType)
	{
	case MOVETYPE::STAY:

		StayMove();

		break;

	default:
		break;
	}

	velocity = velocity.normalize();

	position.x = position.x + velocity.x * speed.x;
	position.y = position.y + velocity.y * speed.y;
}

void FinalBoss::StayMove()
{
	switch (currentPhase)
	{
		float moveEndPositionY;
	case MOVEPHASE::PHASE1://���ɐi�ޏ���

		if (position.y >= 0)
		{
			velocity = { 0,0,0 };
			currentPhase = MOVEPHASE::PHASE2;
		}

		break;

	case MOVEPHASE::PHASE2://���ɐi�ޏ���


		if (hp < 1)
		{
			velocity = { 0,0,0 };
			rotation = { 270.0f,0.0f,0.0f };
			deathParticleFlag = true;
			currentPhase = MOVEPHASE::PHASE3;
		}

		break;

	case MOVEPHASE::PHASE3://���ɐi�ޏ���

		velocity = { 0,-1,0 };
		shotFlag = false;
		EndPerformance();

		moveEndPositionY = -100.0f;
		if (finalBossEye->GetPosition().y < moveEndPositionY)
		{
			velocity = { 0,0,0 };
			rotation = { 0,270,0 };
			currentPhase = MOVEPHASE::PHASE1;
			IsDead();
			moveEndFlag = true;
		}

		break;


	default:
		break;
	}
}

void FinalBoss::IsDead()
{
	isDeadFlag = true;
}

void FinalBoss::SetScrollIncrement(const Vector3& incrementValue)
{
	position += incrementValue;
}

void FinalBoss::Damage()
{
	if (invisibleTime <= 0)
	{
		invisibleTime = INVISIBLEMAXTIME;
	}
	hp--;
}

void FinalBoss::CriticalDamage()
{
	if (invisibleTime <= 0)
	{
		invisibleTime = INVISIBLEMAXTIME;
	}
	hp -= 50;
}

void FinalBoss::DamageEffect()
{
	//�_���[�W���o
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
}

void FinalBoss::DeathParticleProcessing()
{
	//���S���p�[�e�B�N��
	if (deathParticleFlag)
	{
		if (deathParticle->GetParticleLength() < ParticleManager::vertexCount)
		{
			const float rnd_pos = 10.0f;
			Vector3 pos = bodyBlock->GetPosition();
			pos.x = pos.x - centerPosition;

			pos.x += (float)rand() / RAND_MAX * rnd_pos;
			pos.y += (float)rand() / RAND_MAX * rnd_pos;
			for (int i = 0; i < 10; i++)
			{
				//X,Y,Z�S��[-5.0f,+5.0f]�Ń����_���ɕ��z
				const float rnd_vel = 0.1f;
				Vector3 vel{};

				//X,Y,Z�S��[-0.05f,+0.05f]�Ń����_���ɕ��z
				vel.x = ((float)rand() / RAND_MAX * rnd_vel - rnd_vel / 4.0f) * 2;
				vel.y = (float)rand() / RAND_MAX * rnd_vel * 2;
				vel.z = 0.0f;
				//�d�͂Ɍ����Ă�Y�̂�[-0.001f,0]�Ń����_���ɕ��z
				const float rnd_acc = 0.01f;
				Vector3 acc{};
				acc.y = -(float)rand() / RAND_MAX * rnd_acc;

				const float startScale = 3.0f;
				const float endScale = 0.0f;
				const Vector4 startColor = { 1.0f,0.0f,0.0f,1.0f };
				const Vector4 endColor = { 1.0f,1.0f,1.0f,1.0f };
				deathParticle->Add(60, pos, vel, acc, startScale, endScale, startColor, endColor);
			}
		}
	}

	deathParticle->Update();
}

bool FinalBoss::IsCollision(GameObject* arg_otherObject)
{
	GameObject* otherObject = arg_otherObject;

	if (isDeadFlag || otherObject->GetIsDeadFlag() || otherObject->GetCharacterType() == characterType
		|| otherObject->GetCharacterType() == CHARACTERTYPE::BLOCK) {
		return false;
	}

	for (int i = 0; i < bigBullets.size(); i++)
	{
		bigBullets[i]->IsCollision(otherObject);
	}
	

	if (Collision::CheckSphereToSphere({ otherObject->GetPosition(),otherObject->GetCollisionRadius() },
		{ finalBossEye->GetPosition(),collisionRadius }))
	{
		if (otherObject->GetCharacterType() == CHARACTERTYPE::PLAYER)
		{
			otherObject->Damage();
			return true;
		}
		if (otherObject->GetScale().x >= 2.0f)
		{
			CriticalDamage();
		}
		else
		{
			Damage();
		}
		return true;
	}

	return false;
}

void FinalBoss::BreathMove()
{
	bodyAngle += 0.5f;
	if (bodyAngle > 360)
	{
		bodyAngle = 0.0f;
	}
	bodyBlock->SetPosition({  position.x,  position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f,position.z });
	bodyBlock->Update({ 0,0,0 });

	//parentFinalBossEye->SetRotation({0,0,0});

	const Vector3 eyeFixPosition = { 0.0f,2.0f,-13.5f };
	finalBossEye->SetPosition({ position.x,eyeFixPosition.y + position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f , position.z + eyeFixPosition.z });

	const float leftWingPositionX = position.x - 3.0f;
	parentLeftWing->SetPosition({ leftWingPositionX,position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f,position.z });

	const Vector3 basicLeftWingRotation = { 0.0f,-10.0f,0.0f };
	parentLeftWing->SetRotation({ basicLeftWingRotation.x,cos((bodyAngle / 180.0f) * XM_PI) * 20.0f + basicLeftWingRotation.y,basicLeftWingRotation.z});

	parentLeftWing->Update({0,0,0});
	for (int i = 0; i < leftWing.size(); ++i)
	{
		leftWing[i]->Update({0,0,0});
	}

	const float rightWingPositionX = position.x + 3.0f;
	parentRightWing->SetPosition({ rightWingPositionX,position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f,position.z });

	const Vector3 basicRightWingRotation = { 0.0f,10.0f,0.0f };
	parentRightWing->SetRotation({ basicRightWingRotation.x, - cos((bodyAngle / 180.0f) * XM_PI) * 20.0f + basicRightWingRotation.y,basicRightWingRotation.z });

	parentRightWing->Update({ 0,0,0 });
	for (int i = 0; i < rightWing.size(); ++i)
	{
		rightWing[i]->Update({0,0,0});
	}
}

void FinalBoss::ShotBullet(const Vector3& incrementValue, const Vector3& playerPosition)
{
	if (shotFlag)
	{
		for (int i = 0; i < bigBullets.size(); i++)
		{
			//�ˌ�����
			if (bigBullets[i]->GetIsDeadFlag())
			{
				Vector3 shotPosition = finalBossEye->GetPosition();
				shotPosition.y -= 2.0f;
				bigBullets[i]->Initialize();
				bigBullets[i]->SetPosition(shotPosition);
				shotLugTime = 120;
				blinkFlag = true;
				break;
			}
		}
	}

	if (shotLugTime > 0)
	{//�Ĕ��˂���܂ł̃��O�^�C��
		shotFlag = false;
		shotLugTime--;
	}
	else
	{
		shotFlag = true;
	}

	for (int i = 0; i < bigBullets.size(); i++)
	{
		if (bigBullets[i]->GetIsDeadFlag() == false)
		{
			Vector3 targetPosition;
			targetPosition = playerPosition;

			Vector3 dir = { playerPosition.x - position.x,playerPosition.y - position.y, playerPosition.z - position.z };
			dir.normalize();
			bigBullets[i]->SetVelocity({ dir.x,dir.y,dir.z });
			Vector3 rotation = bigBullets[i]->GetRotasion();
			rotation.z += 2.0f;
			bigBullets[i]->SetRotation(rotation);
			bigBullets[i]->Update(incrementValue, targetPosition);

			if (bigBullets[i]->GetPosition().z < position.z - SCREENBACK)
			{
				bigBullets[i]->SetIsDeadFlag(true);
			}
		}
	}
}

void FinalBoss::Blink()
{
	Vector3 eyeScale = finalBossEye->GetScale();
	if (blinkAngle < 180.0f)
	{
		blinkAngle += 8.0f;
		eyeScale.y = 3.0f * (1 - sin((blinkAngle / 180.0f) * XM_PI));
	}
	else
	{
		blinkFlag = false;
		blinkAngle = 0.0f;
		eyeScale.y = 3.0f;
	}

	finalBossEye->SetScale(eyeScale);
}

void FinalBoss::EntryPerformance()
{
	Vector3 leftWingRotation = parentLeftWing->GetRotasion();
	Vector3 rightWingRotation = parentRightWing->GetRotasion();
	Vector3 eyeScale = finalBossEye->GetScale();

	if (leftWingRotation.y > 180.0f || rightWingRotation.y < -180.0f || eyeScale.y < 3.0f)
	{
		bodyAngle += 0.5f;
		if (bodyAngle > 360)
		{
			bodyAngle = 0.0f;
		}
		bodyBlock->SetPosition({ position.x,  position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f,position.z });
		bodyBlock->Update({ 0,0,0 });


		finalBossEye->SetPosition({ position.x,2.0f + position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f , position.z - 13.5f });

		const float maxEyeScale = 3.0f;
		if (eyeScale.y < maxEyeScale)
		{
			eyeScale.y += 0.005f;
		}
		finalBossEye->SetScale(eyeScale);


		parentLeftWing->SetPosition({ -3.0f,position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f,position.z });
		const float maxLeftWingRotation = 180.0f;
		if (leftWingRotation.y > maxLeftWingRotation)
		{
			leftWingRotation.y -= 0.1f;
		}

		parentLeftWing->SetRotation(leftWingRotation);
		parentLeftWing->Update({ 0,0,0 });
		for (int i = 0; i < leftWing.size(); ++i)
		{
			leftWing[i]->Update({ 0,0,0 });
		}


		parentRightWing->SetPosition({ 3.0f,position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f,position.z });
		const float maxRightWingRotation = -180.0f;
		if (rightWingRotation.y < maxRightWingRotation)
		{
			rightWingRotation.y += 0.1f;
		}

		parentRightWing->SetRotation(rightWingRotation);
		parentRightWing->Update({ 0,0,0 });
		for (int i = 0; i < rightWing.size(); ++i)
		{
			rightWing[i]->Update({ 0,0,0 });
		}
	}
	else
	{
		entryFlag = false;
	}
}

void FinalBoss::InitialPosture()
{
	const Vector3 initialLeftWingRotation = { 0.0f,270.0f,0.0f };
	parentLeftWing->SetRotation(initialLeftWingRotation);

	const Vector3 initialRightWingRotation = { 0.0f,-270.0f,0.0f };
	parentRightWing->SetRotation(initialRightWingRotation);

	const Vector3 initialEyeScale = { 3.0f,0.0f,1.0f };
	finalBossEye->SetScale(initialEyeScale);
}

void FinalBoss::NormalPosture()
{
	parentLeftWing->SetRotation(previousParentLeftWingRotation);

	parentRightWing->SetRotation(previousParentRightWingRotation);

	finalBossEye->SetScale({ 3.0f,3.0f,1.0f });
}

void FinalBoss::EndPerformance()
{
	bodyBlock->SetPosition({ position.x,  position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f,position.z });
	bodyBlock->Update({ 0,0,0 });

	parentLeftWing->IsDead();
	for (int i = 0; i < leftWing.size(); ++i)
	{
		const int windNum = i % 3;
		if (windNum == 0)
		{
			Vector3 velocity = { -1.0f,-1.0f,0.0f };
			velocity = velocity.normalize();
			leftWing[i]->SetVelocity(velocity);
		}
		else if (windNum == 1)
		{
			Vector3 velocity = { -1.0f,0.0f,0.0f };
			velocity = velocity.normalize();
			leftWing[i]->SetVelocity(velocity);
		}
		else if (windNum == 2)
		{
			Vector3 velocity = { -1.0f,1.0f,0.0f };
			velocity = velocity.normalize();
			leftWing[i]->SetVelocity(velocity);
		}
		leftWing[i]->Update({ 0,0,0 });
	}

	parentRightWing->IsDead();
	for (int i = 0; i < rightWing.size(); ++i)
	{
		//rightWing[i]->SetParent(nullptr);
		const int windNum = i % 3;
		if (windNum == 0)
		{
			Vector3 velocity = { 1.0f,-1.0f,0.0f };
			velocity = velocity.normalize();
			rightWing[i]->SetVelocity(velocity);
		}
		else if (windNum == 1)
		{
			Vector3 velocity = { 1.0f,0.0f,0.0f };
			velocity = velocity.normalize();
			rightWing[i]->SetVelocity(velocity);
		}
		else if (windNum == 2)
		{
			Vector3 velocity = { 1.0f,1.0f,0.0f };
			velocity = velocity.normalize();
			rightWing[i]->SetVelocity(velocity);
		}
		rightWing[i]->Update({ 0,0,0 });
	}


	const Vector3 basicEyePosition = { 0.0f,2.0f ,-13.5f };
	finalBossEye->SetPosition({ basicEyePosition.x + position.x,basicEyePosition.y + position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f ,  basicEyePosition.z + position.z});

	Vector3 eyeScale = finalBossEye->GetScale();
	if (eyeScale.y > 0.0f)
	{
		eyeScale.y -= 0.005f;
		finalBossEye->SetScale(eyeScale);
	}
	else
	{
		finalBossEye->IsDead();
	}
}

void FinalBoss::BodyAttack()
{
	Vector3 bodyScale = bodyBlock->GetScale();
	Vector3 parentLeftWingRotation = parentLeftWing->GetRotasion();
	Vector3 parentRightWingRotation = parentRightWing->GetRotasion();
	switch (bodyAttackPhase)
	{
	case MOVEPHASE::PHASE1://��ɏオ�鏈��

		position.y += 1.0f;


		finalBossEye->SetPosition({ position.x,position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f ,position.z -13.5f });

		if (bodyScale.x > 0.0f)
		{
			bodyScale.x -= 0.005f;
		}

		bodyBlock->SetScale(bodyScale);
		bodyBlock->SetPosition({ position.x,  position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f,position.z });
		bodyBlock->Update({ 0,0,0 });


		if (parentLeftWingRotation.y < 270.0f)
		{
			parentLeftWingRotation.y -= 5.0f;
		}
		parentLeftWing->SetRotation(parentLeftWingRotation);


		if (parentRightWingRotation.y > -270.0f)
		{
			parentRightWingRotation.y += 5.0f;
		}
		parentRightWing->SetRotation(parentRightWingRotation);


		parentLeftWing->SetPosition({ -3.0f,position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f,position.z });

		parentLeftWing->Update({ 0,0,0 });
		for (int i = 0; i < leftWing.size(); ++i)
		{
			leftWing[i]->Update({ 0,0,0 });
		}

		parentRightWing->SetPosition({ 3.0f,position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f,position.z });

		parentRightWing->Update({ 0,0,0 });
		for (int i = 0; i < rightWing.size(); ++i)
		{
			rightWing[i]->Update({ 0,0,0 });
		}

		if (position.y > 100.0f)
		{
			position.z = 0.0f;
			bodyAttackPhase = MOVEPHASE::PHASE2;
		}

		break;

	case MOVEPHASE::PHASE2://�̓����菈��

		position.y -= 1.0f;


		finalBossEye->SetPosition({ position.x, position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f , position.z -13.5f });


		bodyBlock->SetPosition({ position.x,  position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f,position.z });
		bodyBlock->Update({ 0,0,0 });


		parentLeftWing->SetPosition({ -3.0f,position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f,position.z });

		parentLeftWing->Update({ 0,0,0 });
		for (int i = 0; i < leftWing.size(); ++i)
		{
			leftWing[i]->Update({ 0,0,0 });
		}


		parentRightWing->SetPosition({ 3.0f,position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f,position.z });

		parentRightWing->Update({ 0,0,0 });
		for (int i = 0; i < rightWing.size(); ++i)
		{
			rightWing[i]->Update({ 0,0,0 });
		}

		if (position.y < -100.0f)
		{
			position.z = 50.0f;
			NormalPosture();
			bodyAttackPhase = MOVEPHASE::PHASE3;
		}

		break;

	case MOVEPHASE::PHASE3://�߂鏈��


		position.y += 1.0f;


		finalBossEye->SetPosition({ position.x,position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f ,position.z -13.5f });


		bodyBlock->SetScale({ 1.0f,1.0f,1.0f });
		bodyBlock->SetPosition({ position.x,  position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f,position.z });
		bodyBlock->Update({ 0,0,0 });



		parentLeftWing->SetPosition({ -3.0f,position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f,position.z });

		parentLeftWing->Update({ 0,0,0 });
		for (int i = 0; i < leftWing.size(); ++i)
		{
			leftWing[i]->Update({ 0,0,0 });
		}


		parentRightWing->SetPosition({ 3.0f,position.y + cos((bodyAngle / 180.0f) * XM_PI) * 5.0f,position.z });

		parentRightWing->Update({ 0,0,0 });
		for (int i = 0; i < rightWing.size(); ++i)
		{
			rightWing[i]->Update({ 0,0,0 });
		}

		if (position.y > 0.0f)
		{
			bodyAttackFlag = false;
			bodyAttackPhase = MOVEPHASE::PHASE1;
		}

		break;

	default:
		break;
	}
}
