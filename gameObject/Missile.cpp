#include "Missile.h"
using namespace Microsoft::WRL;

ID3D12GraphicsCommandList* Missile::cmdList;
XMMATRIX Missile::matView;
XMMATRIX Missile::matProjection;
Camera* Missile::camera = nullptr;
ID3D12Device* Missile::device = nullptr;

Missile::Missile(ID3D12GraphicsCommandList* arg_cmdList, ID3D12Device* arg_device)
{
	cmdList = arg_cmdList;
	device = arg_device;
	scale = { 0.2f,0.2f,0.2f };
	rotation = { 0.0f,90.0f,0.0f };
	speed = { 1.0f,1.0f,1.0f };
	velocity = { 0.0f,0.0f,0.0f };
	matProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), (float)WindowSize::window_width / (float)WindowSize::window_height, 0.1f, 1000.0f);
	collisionRadius = 0.25f;
	upShotFlag = true;
	bossSceneFlag = false;
}

Missile::~Missile()
{
}

void Missile::CreateConstBuffer()
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

Missile* Missile::Create(ID3D12Device* arg_device, ID3D12GraphicsCommandList* arg_cmdList,const Vector3& arg_position)
{
	Missile* missile = new Missile(arg_cmdList, arg_device);

	missile->SetPosition(arg_position);

	missile->CreateConstBuffer();

	ParticleManager* missileParticleMan;
	missileParticleMan = ParticleManager::Create(arg_device);
	missile->SetMissilePerticleMan(missileParticleMan);

	return missile;
}

void Missile::SetEye(const Vector3& arg_eye)
{
	Missile::camera->SetEye(arg_eye);

	UpdateViewMatrix();
}

void Missile::SetTarget(const Vector3& arg_target)
{
	Missile::camera->SetTarget(arg_target);

	UpdateViewMatrix();
}

void Missile::UpdateViewMatrix()
{
	matView = XMMatrixLookAtLH(XMLoadFloat3(&camera->GetEye()), XMLoadFloat3(&camera->GetTarget()), XMLoadFloat3(&camera->GetUp()));
}

void Missile::Initialize()
{
	hp = 1;
	particleLugtime = MAXMISSILELUGTIME;
	invisibleTime = 0;
	isDeadFlag = false;
	curveTime = 1.0f;
	velocity = { 0.0f,0.0f,0.0f };
	speed = { 0.0f,0.0f,0.0f };


	//�{�X�V�[�����ǂ����ɂ���ă��f���̊p�x��������
	if (bossSceneFlag)
	{
		rotation = { 0.0f,0.0f,0.0f };

		if (upShotFlag)
			rotation.x = 270.0f;
		else
			rotation.x = 90.0f;
	}
	else
	{
		rotation = { 0.0f,90.0f,0.0f };

		if (upShotFlag)
			rotation.x = 270.0f;
		else
			rotation.x = 90.0f;
	}
}

//�v���C���[�̍X�V����
void Missile::Update(const Vector3& arg_incrementValue, float arg_centerPosition)
{
	if (isDeadFlag)
	{
		return;
	}
	//�ړ�����
	Move();
	//���p�[�e�B�N��
	MissileParticleProcessing(arg_centerPosition);
	
	SetScrollIncrement(arg_incrementValue);
	//�萔�o�b�t�@�̓]��
	TransferConstBuff();
}

void Missile::Draw()
{
	missileParticleMan->Update();
	ParticleManager::PreDraw(cmdList);
	missileParticleMan->Draw();
	ParticleManager::PostDraw();

	if (isDeadFlag)
	{
		return;
	}

	Missile::cmdList->SetPipelineState(PipelineState::bulletPipelineState.Get());

	Missile::cmdList->SetGraphicsRootSignature(RootSignature::bulletRootsignature.Get());

	Missile::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Missile::cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());
	Missile::cmdList->SetGraphicsRootConstantBufferView(1, constBuffB1->GetGPUVirtualAddress());

	objModel->Draw(Missile::cmdList);
}

void Missile::TransferConstBuff()
{
	HRESULT result;

	XMMATRIX matScale, matRot, matTrans;
	UpdateViewMatrix();
	// �X�P�[���A��]�A���s�ړ��s��̌v�Z
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

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
}

void Missile::Move()
{
	position += velocity * speed;

	if (curveTime > 0.0f)
	{
		const float curveTimeIncrementSize = 0.02f;
		curveTime -= curveTimeIncrementSize;
	}

	const Vector3 startSpeed = { 0.0f,0.0f,0.0f };
	const Vector3 endSpeed = { 0.6f,0.6f,0.6f };
	Vector3 missileEasing = easeIn(startSpeed, endSpeed, (1 - curveTime));
	speed = missileEasing;
	const float rotationIncrementSize = 4.0f;

	if (bossSceneFlag)
	{//�ˌ����̋O���Ƒ��x�̌v�Z
		if (upShotFlag)
		{//�ォ��o��~�T�C���̋O��
			if (rotation.x < 360.0f)
			{
				
				rotation.x += rotationIncrementSize;
			}
			else
			{
				velocity.z = cosf((XM_PI / 2) * curveTime);
				velocity.y = sinf((XM_PI / 2) * curveTime);
			}
		}
		else
		{//������o��~�T�C���̋O��
			if (rotation.x > 0.0f)
			{
				rotation.x -= rotationIncrementSize;
			}
			else
			{
				velocity.z = cosf((XM_PI / 2) * curveTime);
				velocity.y = -sinf((XM_PI / 2) * curveTime);
			}
		}
	}
	else
	{	//�ˌ����̋O���Ƒ��x�̌v�Z
		if (upShotFlag)
		{//�ォ��o��~�T�C���̋O��
			if (rotation.x < 360.0f)
			{
				rotation.x += rotationIncrementSize;
			}
			else
			{
				velocity.x = cosf((XM_PI / 2) * curveTime);
				velocity.y = sinf((XM_PI / 2) * curveTime);
			}
		}
		else
		{//������o��~�T�C���̋O��
			if (rotation.x > 0.0f)
			{
				rotation.x -= rotationIncrementSize;
			}
			else
			{
				velocity.x = cosf((XM_PI / 2) * curveTime);
				velocity.y = -sinf((XM_PI / 2) * curveTime);
			}
		}
	}
}

void Missile::SetScrollIncrement(const Vector3& arg_incrementValue)
{
	position += arg_incrementValue;
}

void Missile::MissileParticleProcessing(float arg_centerPosition)
{
	//�~�T�C���̌�납��o��p�[�e�B�N������
	if (rotation.x >= 360.0f || rotation.x <= 0.0f)
		if (missileParticleMan->GetParticleLength() < 20 && particleLugtime <= 0)
		{
			const float rnd_pos = 1.5f;

			Vector3 pos{};
			Vector3 dis{};
			Vector3 vel{};
			Vector3 acc{};
			const float k = 0.05f;
			if (bossSceneFlag)
			{//�����_���ɐݒ肵���ʒu�ƃ~�T�C���̈ʒu����������v�Z
				pos = { position.x + ((float)rand() / RAND_MAX * rnd_pos * 4.0f + 10.0f),position.y + ((float)rand() / RAND_MAX * rnd_pos * 8.0f - 5.0f),position.z };
				dis = { position.x - pos.x,position.y - pos.y ,position.z - pos.z };
				dis = dis.normalize();
				vel = dis * k;
				acc.z = -0.001f;
			}
			else
			{//�����_���ɐݒ肵���ʒu�ƃ~�T�C���̈ʒu����������v�Z
				pos = { position.x + ((float)rand() / RAND_MAX * rnd_pos * 4.0f + 10.0f), position.y + ((float)rand() / RAND_MAX * rnd_pos * 8.0f - 5.0f),position.z };
				dis = { position.x - pos.x,position.y - pos.y ,0.0f };
				dis = dis.normalize();
				vel = dis * k;
				acc.x = -0.001f;
			}


			pos = position;
			pos.x = pos.x - arg_centerPosition;
			pos.x += 0.5f;

			const float startScale = 0.0f;
			const float endScale = 0.5f;
			const Vector4 black = { 0.0f,0.0f,0.0f,0.0f };
			const Vector4 white = { 1.0f,1.0f,1.0f,1.0f };
			const Vector4 startColor = black;
			const Vector4 endColor = white;
			missileParticleMan->Add(40, pos, vel, acc, startScale, endScale, startColor, endColor);

			particleLugtime = MAXMISSILELUGTIME;
		}

	if (particleLugtime >= 1)
	{
		particleLugtime--;
	}
}

bool Missile::IsCollision(GameObject* arg_otherObject)
{
	GameObject* otherObject = arg_otherObject;

	if (isDeadFlag || otherObject->GetIsDeadFlag() || otherObject->GetCharacterType() == characterType
		|| otherObject->GetCharacterType() == CHARACTERTYPE::BLOCK || (otherObject->GetCharacterType() == CHARACTERTYPE::PLAYER && characterType == CHARACTERTYPE::FRIEND)) {
		return false;
	}

	if (Collision::CheckSphereToSphere({ otherObject->GetPosition(),otherObject->GetCollisionRadius() },
		{ position,collisionRadius }))
	{
		if (scale.x >= 2.0f)
		{
			otherObject->CriticalDamage();
			if (otherObject->GetScale().x >= 2.0f)
			{
				Damage();
			}
		}
		else
		{
			Damage();
			otherObject->Damage();
		}
		return true;
	}

	return false;
}
