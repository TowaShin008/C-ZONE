#include "TankEnemy.h"
using namespace Microsoft::WRL;

ID3D12GraphicsCommandList* TankEnemy::cmdList;
XMMATRIX TankEnemy::matView;
XMMATRIX TankEnemy::matProjection;
Camera* TankEnemy::camera = nullptr;

TankEnemy::TankEnemy(ID3D12GraphicsCommandList* arg_cmdList)
{
	cmdList = arg_cmdList;
	scale = { 0.5f,0.5f,0.5f };
	speed = { 0.1f,0.1f,0.1f };
	brakeCount = 0;
	collisionRadius = 1.0f;
	currentPhase = MOVEPHASE::PHASE1;
	currentType = MOVETYPE::TRIANGLE;
	characterType = CHARACTERTYPE::ENEMY;
	matProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), (float)WindowSize::window_width / (float)WindowSize::window_height, 0.1f, 1000.0f);
}

TankEnemy::~TankEnemy()
{
	for (int i = 0; i < bullets.size(); i++)
	{
		delete bullets[i];
	}
}

void TankEnemy::CreateConstBuffer(ID3D12Device* device)
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

	scoreCharacter.reset(OBJCharacter::Create(device));
	scoreCharacter->SetScale({ 2.0f,2.0f,2.0f });
	scoreCharacter->SetRotation({ 0,0,180.0f });

	UpdateViewMatrix();
}

TankEnemy* TankEnemy::Create(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList, Vector3 position)
{
	TankEnemy* tankEnemy = new TankEnemy(arg_cmdList);

	tankEnemy->Initialize();

	tankEnemy->SetPosition({ position.x - 19.25f,position.y + 9.8f,0.0f });

	tankEnemy->CreateConstBuffer(device);

	tankEnemy->AttachBullet(device);

	ParticleManager* deathParticle = ParticleManager::Create(device);

	deathParticle->SetSelectColor(2);

	tankEnemy->SetDeathParticleManager(deathParticle);

	return tankEnemy;
}

void TankEnemy::SetEye(const Vector3& eye)
{
	TankEnemy::camera->SetEye(eye);

	UpdateViewMatrix();
}

void TankEnemy::SetTarget(const Vector3& target)
{
	TankEnemy::camera->SetTarget(target);

	UpdateViewMatrix();
}

void TankEnemy::UpdateViewMatrix()
{
	matView = XMMatrixLookAtLH(XMLoadFloat3(&camera->GetEye()), XMLoadFloat3(&camera->GetTarget()), XMLoadFloat3(&camera->GetUp()));
}

void TankEnemy::SetOBJModel(OBJHighModel* arg_objModel, OBJModel* bulletModel, OBJModel* scoreModel)
{
	objModel = arg_objModel;
	SetBulletModel(bulletModel);
	scoreCharacter->SetOBJModel(scoreModel);
}

void TankEnemy::SetBulletModel(OBJModel* bulletModel)
{
	for (int i = 0; i < bullets.size(); i++)
	{
		bullets[i]->SetOBJModel(bulletModel);
	}
}

void TankEnemy::AttachBullet(ID3D12Device* device)
{
	bullets.resize(BULLETMAXNUM);

	for (int i = 0; i < bullets.size(); i++)
	{
		bullets[i] = Bullet::Create(device, cmdList);
		bullets[i]->SetIsDeadFlag(true);
		bullets[i]->SetSpeed({ 0.2f,0.2f,0.2f });
		bullets[i]->SetCharacterType(CHARACTERTYPE::ENEMY);
		bullets[i]->SetVelocity({ -0.2f,0.0f,0.0f });
	}
}

void TankEnemy::Initialize()
{
	isDeadFlag = false;
	const int maxShotLugTime = 240;
	shotLugTime = maxShotLugTime;
	moveLugTime = 0;
	currentPhase = MOVEPHASE::PHASE1;
	currentType = MOVETYPE::TRIANGLE;
	moveEndFlag = false;
	velocity = { -1,-1,0 };
	rotation = { 270.0f,0.0f,90.0f };
	color = { 1.0f,0.0f,0.0f,1.0f };
	deathParticleFlag = false;
	hp = 1;
	invisibleTime = 0;
	brakeCount = 0.0f;
}
//�v���C���[�̍X�V����
void TankEnemy::Update(const Vector3& incrementValue)
{
	centerPosition += incrementValue.x;
	//SetScrollIncrement(incrementValue);
	if (moveLugTime <= 0)
	{
		if (isDeadFlag == false)
		{
			if (moveEndFlag == false)
			{
				//�v���C���[�̈��͈͂ɓ������瓮���n�߂�
				if (position.x < playerPosition.x + 30.0f)
				{
					//�ړ�����
					Move();
				}
			}
			else
			{//�ړ����I�������U�����n�߂�
				//�e�ۂ̔��ˏ���
				ShotBullet();
				//�u���[�L����
				Brake();
			}
			//�萔�o�b�t�@�̓]��
			TransferConstBuff();
		}
		else
		{
			//�X�R�A���o
			ScoreProcessing(incrementValue);
		}
	}
	else
	{
		moveLugTime--;

		if (moveLugTime > 0)
		{
			isDeadFlag = true;
		}
		else
		{
			isDeadFlag = false;
		}
	}
	//���S�p�[�e�B�N�����o
	DeathParticleProcessing();


	for (int i = 0; i < bullets.size(); i++)
	{
		bullets[i]->Update(incrementValue);
		if (bullets[i]->GetPosition().x < centerPosition + SCREENLEFT - 4.0f)
		{
			bullets[i]->SetIsDeadFlag(true);
		}
	}
}

void TankEnemy::ShotBullet()
{
	//�ˌ�����
	if (shotLugTime < 1)
	{
		for (int i = 0; i < bullets.size(); i++)
		{
			if (bullets[i]->GetIsDeadFlag() == true)
			{
				Vector3 dirPosition = { playerPosition.x + 1.0f - position.x ,playerPosition.y - position.y ,playerPosition.z - position.z };
				dirPosition.normalize();
				bullets[i]->SetVelocity(dirPosition);
				bullets[i]->SetIsDeadFlag(false);
				bullets[i]->SetPosition({ position.x, position.y, position.z });
				const int maxShotLugTime = 240;
				shotLugTime = maxShotLugTime;
				break;
			}
		}
	}
	else
	{//�Ĕ��˂���܂ł̃��O�^�C��
		shotLugTime--;
	}
}

void TankEnemy::TransferConstBuff()
{
	if (rotation.y > 0.0f)
	{
		rotation.y -= 10.0f * speed.x;
	}
	else
	{
		rotation.y = 360.0f;
	}
	HRESULT result;
	XMMATRIX matScale, matRot, matTrans;
	UpdateViewMatrix();
	// �X�P�[���A��]�A���s�ړ��s��̌v�Z
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matTrans = XMMatrixTranslation(position.x + 0.2f, position.y - 0.1f, position.z);

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


	//���[���h�s��̓]��
	ConstBufferDataB0* constMap0 = nullptr;
	result = constBuffB0->Map(0, nullptr, (void**)&constMap0);
	constMap0->color = color;
	constMap0->mat = matWorld * matView * matProjection;	// �s��̍���
	constBuffB0->Unmap(0, nullptr);

	//�}�e���A���̓]��
	//ConstBufferDataB1* constMap1 = nullptr;
	//result = constBuffB1->Map(0, nullptr, (void**)&constMap1);
	//constMap1->ambient = objModel->material.ambient;
	//constMap1->diffuse = objModel->material.diffuse;
	//constMap1->specular = objModel->material.specular;
	//constMap1->alpha = objModel->material.alpha;
	//constBuffB1->Unmap(0, nullptr);
}

void TankEnemy::Move()
{
	//position.x = 0.15f;

	velocity = velocity.normalize();

	position.x = position.x + velocity.x * speed.x;
	position.y = position.y + velocity.y * speed.y;

	if (playerPosition.x + 5.0f > position.x)
	{
		moveEndFlag = true;
	}
}

void TankEnemy::Brake()
{
	if (brakeCount < 1.0f)
	{
		brakeCount += 0.01f;
		//�u���[�L���̃C�[�W���O����
		speed = easeOut({ 0.1f,0.1f,0.1f }, { 0.0f,0.0f,0.0f }, brakeCount);
		position.x = position.x + velocity.x * speed.x;
		position.y = position.y + velocity.y * speed.y;
	}
}

void TankEnemy::Draw()
{
	if (isDeadFlag == false)
	{
		TankEnemy::cmdList->SetPipelineState(PipelineState::basicPipelineState.Get());

		TankEnemy::cmdList->SetGraphicsRootSignature(RootSignature::basicRootsignature.Get());

		TankEnemy::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		TankEnemy::cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());
		TankEnemy::cmdList->SetGraphicsRootConstantBufferView(1, constBuffB1->GetGPUVirtualAddress());

		objModel->Draw(TankEnemy::cmdList);

	}
	else
	{
		ParticleManager::PreDraw(cmdList);
		deathParticle->Draw();
		ParticleManager::PostDraw();

		if (scoreCharacter->GetColor().w >= 0.1f)
		{
			scoreCharacter->Draw(cmdList);
		}
	}



	for (int i = 0; i < bullets.size(); i++)
	{
		if (bullets[i]->GetIsDeadFlag() == false)
			bullets[i]->Draw();
	}

}

void TankEnemy::RectAngleMove()
{
	switch (currentPhase)
	{
	case MOVEPHASE::PHASE1://���ɐi�ޏ���

		velocity.x = -0.1f;
		if (position.x <= centerPosition + (SCREENRIGHT / 3.0f) * 2.0f)
		{
			velocity = { 0,0,0 };
			rotation = { 270.0f,0.0f,90.0f };
			currentPhase = MOVEPHASE::PHASE2;
		}

		break;

	case MOVEPHASE::PHASE2://���ɐi�ޏ���

		velocity.y = -0.1f;
		rotation.x = 270.0f - 45.0f;

		if (position.y <= SCREENBUTTOM)
		{
			velocity = { 0,0,0 };
			rotation = { 270.0f,0.0f,90.0f };
			currentPhase = MOVEPHASE::PHASE3;
		}

		break;

	case MOVEPHASE::PHASE3://�E�ɐi�ޏ���

		velocity.x = 0.1f;

		if (position.x >= centerPosition + SCREENRIGHT + 8.0f)
		{
			velocity = { 0,0,0 };
			rotation = { 270.0f,0.0f,90.0f };
			currentPhase = MOVEPHASE::PHASE4;
		}

		break;

	case MOVEPHASE::PHASE4://��ɐi�ޏ���

		velocity.y = 0.1f;
		rotation.x = 270.0f + 45.0f;

		if (position.y >= SCREENTOP)
		{
			velocity = { 0,0,0 };
			rotation = { 270.0f,0.0f,90.0f };
			currentPhase = MOVEPHASE::PHASE1;
			IsDead();
			moveEndFlag = true;
		}

		break;


	default:
		break;
	}
}

void TankEnemy::TriAngleMove()
{
	switch (currentPhase)
	{
	case MOVEPHASE::PHASE1://���ɐi�ޏ���

		velocity.x = -0.2f;
		velocity.y = -0.1f;
		rotation.x = 270.0f - 45.0f;
		if (position.y <= 0.0f)
		{
			velocity = { 0,0,0 };
			rotation = { 270.0f,0.0f,90.0f };
			currentPhase = MOVEPHASE::PHASE2;
		}

		break;

	case MOVEPHASE::PHASE2://�E�ɐi�ޏ���

		velocity.x = 0.2f;
		velocity.y = -0.1f;
		rotation.x = 270.0f - 45.0f;

		if (position.y <= SCREENBUTTOM)
		{
			velocity = { 0,0,0 };
			rotation = { 270.0f,0.0f,90.0f };
			currentPhase = MOVEPHASE::PHASE3;
		}

		break;

	case MOVEPHASE::PHASE3://��ɐi�ޏ���

		velocity.y = 0.1f;
		rotation.x = 270.0f + 45.0f;

		if (position.y >= SCREENTOP)
		{
			velocity = { 0,0,0 };
			rotation = { 270.0f,0.0f,90.0f };
			currentPhase = MOVEPHASE::PHASE1;
			IsDead();
			moveEndFlag = true;
		}

		break;

	default:
		break;
	}
}

void TankEnemy::LeftDiagonallyMove()
{
	if (currentPhase == MOVEPHASE::PHASE1)
	{
		velocity.x = -0.1f;
		velocity.y = -0.1f;
		rotation.x = 270.0f - 45.0f;

		if (position.y < SCREENBUTTOM - 5.0f)
		{
			velocity = { 0,0,0 };
			rotation = { 270.0f,0.0f,90.0f };
			currentPhase = MOVEPHASE::PHASE1;
			IsDead();
			moveEndFlag = true;
		}
	}
}

void TankEnemy::UpCurveMove()
{
	if (currentPhase == MOVEPHASE::PHASE1)
	{
		if (curveTime < 2.0f)
		{
			curveTime += 0.01f;
		}

		if (rotation.y > 0.0f)
		{
			rotation.y -= 1.0f;
		}

		velocity.x = cosf((XM_PI / 2) * curveTime);
		velocity.y = sinf((XM_PI / 2) * curveTime);


		if (position.y < SCREENLEFT - 5.0f)
		{
			velocity = { 0,0,0 };
			rotation = { 270.0f,0.0f,90.0f };
			curveTime = 0.0f;
			currentPhase = MOVEPHASE::PHASE1;
			IsDead();
			moveEndFlag = true;
		}
	}
}

void TankEnemy::DownCurveMove()
{
	if (currentPhase == MOVEPHASE::PHASE1)
	{
		if (curveTime > 2.0f)
		{
			curveTime -= 0.01f;
		}

		if (rotation.y < 360.0f)
		{
			rotation.y += 1.0f;
		}

		velocity.x = cosf((XM_PI / 2) * curveTime);
		velocity.y = sinf((XM_PI / 2) * curveTime);


		//rotation.z = 45.0f;

		if (position.y < SCREENLEFT - 5.0f)
		{
			velocity = { 0,0,0 };
			rotation = { 270.0f,0.0f,90.0f };
			curveTime = 0.0f;
			currentPhase = MOVEPHASE::PHASE1;
			IsDead();
			moveEndFlag = true;
		}
	}
}

void TankEnemy::IsDead()
{
	isDeadFlag = true;
}

void TankEnemy::SetMoveType(const MOVETYPE& moveType)
{
	currentType = moveType;

	if (currentType == MOVETYPE::UPCURVE)
	{
		curveTime = 1.0f;
		rotation.y = 90.0f;
	}
	if (currentType == MOVETYPE::DOWNCURVE)
	{
		curveTime = 3.0;
		rotation.y = 270.0f;
	}
}

void TankEnemy::SetScrollIncrement(const Vector3& incrementValue)
{
	position += incrementValue;
}

void TankEnemy::DeathParticleProcessing()
{
	if (deathParticleFlag)
	{
		if (deathParticle->GetParticleLength() < 10)
		{
			for (int i = 0; i < 10; i++)
			{
				//X,Y,Z�S��[-5.0f,+5.0f]�Ń����_���ɕ��z
				const float rnd_pos = 10.0f / 30;
				Vector3 pos = position;
				const float rnd_vel = 0.1f;
				Vector3 vel{};

				//X,Y,Z�S��[-0.05f,+0.05f]�Ń����_���ɕ��z
				vel.x = ((float)rand() / RAND_MAX * rnd_vel - rnd_vel / 4.0f) * 2;
				vel.y = (float)rand() / RAND_MAX * rnd_vel * 2;
				vel.z = 0.0f;
				//�d�͂Ɍ����Ă�Y�̂�[-0.001f,0]�Ń����_���ɕ��z
				const float rnd_acc = 0.02f;
				Vector3 acc{};
				acc.y = -(float)rand() / RAND_MAX * rnd_acc;
				life = 40;
				pos.x = pos.x - centerPosition;
				deathParticle->Add(life, pos, vel, acc, 0.5f, 0.0f, { 1.0f,0.0f,0.0f,1.0f }, { 1.0f,1.0f,1.0f,1.0f });
			}
			deathParticleFlag = false;
		}
	}

	deathParticle->Update();
}


bool TankEnemy::IsCollision(GameObject* arg_otherObject)
{
	GameObject* otherObject = arg_otherObject;

	if (isDeadFlag || otherObject->GetIsDeadFlag() || otherObject->GetCharacterType() == characterType)
	{
		return false;
	}

	for (int i = 0; i < bullets.size(); i++)
	{
		bullets[i]->IsCollision(otherObject);
	}

	Vector3 otherPosition = otherObject->GetPosition();
	if (otherObject->GetCharacterType() == CHARACTERTYPE::PLAYER)
	{
		playerPosition = otherPosition;
		return false;
	}

	if (otherObject->GetCharacterType() == CHARACTERTYPE::BLOCK)
	{//�u���b�N�������ꍇ�̃u���b�N�̏�ɂ̂�ׂ̓����蔻��
		if (Collision::CheckSphereToSphere({ otherPosition,otherObject->GetCollisionRadius() },
			{ position,collisionRadius }))
		{
			position.y = otherPosition.y + 1.25f;
			return true;
		}
	}
	else
	{
		if (Collision::CheckSphereToSphere({ otherObject->GetPosition(),otherObject->GetCollisionRadius() },
			{ position,collisionRadius }))
		{
			if (otherObject->GetScale().x >= 2.0f)
			{
				Damage();
			}
			else
			{
				Damage();
				otherObject->Damage();
			}
			return true;
		}
	}

	return false;
}

void TankEnemy::Damage()
{
	if (invisibleTime < 1)
	{
		hp--;
		if (invisibleTime <= 0)
		{
			invisibleTime = INVISIBLEMAXTIME;
		}
	}
	if (hp == 0)
	{
		isDeadFlag = true;
		scoreCharacter->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		deathParticleFlag = true;
	}
}

void TankEnemy::CriticalDamage()
{
	if (invisibleTime < 1)
	{
		hp--;
		if (invisibleTime <= 0)
		{
			invisibleTime = INVISIBLEMAXTIME;
		}
	}
	if (hp == 0)
	{
		isDeadFlag = true;
		scoreCharacter->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		deathParticleFlag = true;
	}
}

void TankEnemy::ScoreProcessing(Vector3 incrementValue)
{
	Vector4 scoreColor = scoreCharacter->GetColor();
	Vector3 scoreScale = scoreCharacter->GetScale();

	if (scoreScale.x <= 2.5f)
	{
		scoreScale.x += 0.05f;
		scoreScale.y += 0.05f;
		scoreScale.z += 0.05f;
	}

	if (scoreColor.w > 0.0f)
		scoreColor.w -= 0.01f;

	scoreCharacter->SetScale(scoreScale);
	scoreCharacter->SetColor(scoreColor);
	scoreCharacter->SetPosition({ position.x,position.y,position.z + 5.0f });
	scoreCharacter->Update(incrementValue);
}
