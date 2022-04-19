#include "Alien.h"
using namespace Microsoft::WRL;

ID3D12GraphicsCommandList* Alien::cmdList;
ID3D12Device* Alien::device;
XMMATRIX Alien::matView;
XMMATRIX Alien::matProjection;
Camera* Alien::camera = nullptr;

Alien::Alien(ID3D12GraphicsCommandList* arg_cmdList, ID3D12Device* arg_device)
{
	cmdList = arg_cmdList;
	device = arg_device;
	scale = { 0.5f,0.5f,0.5f };
	speed = { 0.1f,0.1f,0.1f };
	collisionRadius = 1.0f;
	currentPhase = MOVEPHASE::PHASE1;
	currentType = MOVETYPE::TRIANGLE;
	characterType = CHARACTERTYPE::ENEMY;
	matProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), (float)WindowSize::window_width / (float)WindowSize::window_height, 0.1f, 1000.0f);
}

Alien::~Alien()
{
}

void Alien::CreateConstBuffer()
{
	HRESULT result;

	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// �A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(DissolveConstBufferData) + 0xff) & ~0xff),
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

	{
		// WIC�e�N�X�`���̃��[�h
		TexMetadata metadata{};
		ScratchImage scratchImg{};

		result = LoadFromWICFile(
			L"Resources/DissolveMap.png", WIC_FLAGS_NONE,
			&metadata, scratchImg);
		if (FAILED(result)) {
			assert(0);
			return;
		}

		const Image* img = scratchImg.GetImage(0, 0, 0); // ���f�[�^���o

		// �摜�̐ݒ�
		CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			metadata.format,
			metadata.width,
			(UINT)metadata.height,
			(UINT16)metadata.arraySize,
			(UINT16)metadata.mipLevels
		);

		// �e�N�X�`���p�o�b�t�@�̐���
		result = device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
			D3D12_HEAP_FLAG_NONE,
			&texresDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ, // �e�N�X�`���p�w��
			nullptr,
			IID_PPV_ARGS(&dissolveTexBuff));

		// �e�N�X�`���o�b�t�@�Ƀf�[�^�]��
		result = dissolveTexBuff->WriteToSubresource(
			0,
			nullptr, // �S�̈�փR�s�[
			img->pixels,    // ���f�[�^�A�h���X
			(UINT)img->rowPitch,  // 1���C���T�C�Y
			(UINT)img->slicePitch // 1���T�C�Y
		);

		D3D12_DESCRIPTOR_HEAP_DESC srvDescHeapDesc = {};
		srvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		srvDescHeapDesc.NumDescriptors = 1;

		result = device->CreateDescriptorHeap(&srvDescHeapDesc, IID_PPV_ARGS(&descHeapSRV));
		// �V�F�[�_���\�[�X�r���[�쐬
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{}; // �ݒ�\����
		D3D12_RESOURCE_DESC resDesc = dissolveTexBuff->GetDesc();
		//�V�F�[�_�[���\�[�X�r���[�̐ݒ�
		srvDesc.Format = resDesc.Format;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
		srvDesc.Texture2D.MipLevels = 1;

		//�V�F�[�_�[���\�[�X�r���[�̐���
		device->CreateShaderResourceView(dissolveTexBuff.Get(),
			&srvDesc,
			CD3DX12_CPU_DESCRIPTOR_HANDLE(
				descHeapSRV->GetCPUDescriptorHandleForHeapStart(), 0,
				device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
			)
		);
	}

	scoreCharacter.reset(ObjFileCharacter::Create(device,cmdList));
	scoreCharacter->SetScale({ 2.0f,2.0f,2.0f });
	scoreCharacter->SetRotation({ 0,0,180.0f });

	if (FAILED(result))
	{
		assert(0);
	}

	UpdateViewMatrix();
}

Alien* Alien::Create(ID3D12Device* arg_device, ID3D12GraphicsCommandList* arg_cmdList,const Vector3& arg_position)
{
	Alien* alian = new Alien(arg_cmdList, arg_device);

	alian->Initialize();

	alian->SetPosition(arg_position);

	alian->CreateConstBuffer();

	return alian;
}

void Alien::SetEye(const Vector3& arg_eye)
{
	Alien::camera->SetEye(arg_eye);

	UpdateViewMatrix();
}

void Alien::SetTarget(const Vector3& arg_target)
{
	Alien::camera->SetTarget(arg_target);

	UpdateViewMatrix();
}

void Alien::UpdateViewMatrix()
{
	matView = XMMatrixLookAtLH(XMLoadFloat3(&camera->GetEye()), XMLoadFloat3(&camera->GetTarget()), XMLoadFloat3(&camera->GetUp()));
}

void Alien::SetOBJModel(ObjFileModel* arg_objModel, ObjFileModel* arg_scoreModel)
{
	objModel = arg_objModel;
	scoreCharacter->SetOBJModel(arg_scoreModel);
}

void Alien::Initialize()
{
	isDeadFlag = false;
	moveLugTime = 0;
	currentPhase = MOVEPHASE::PHASE1;
	currentType = MOVETYPE::TRIANGLE;
	moveEndFlag = false;
	velocity = { 0,0,0 };
	rotation = { 270.0f,0.0f,90.0f };
	const Vector4 red = { 1.0f,0.0f,0.0f,1.0f };
	color = red;
	dissolveCount = 1.0f;
	hp = 1;
	invisibleTime = 0;
}
//�v���C���[�̍X�V����
void Alien::Update(const Vector3& arg_incrementValue)
{
	centerPosition += arg_incrementValue.x;
	SetScrollIncrement(arg_incrementValue);
	if (moveLugTime <= 0)
	{
		if (isDeadFlag == false)
		{
			if (moveEndFlag == false)
			{//�ړ�����
				Move();
			}
		}
		else
		{
			//�X�R�A���o
			ScoreProcessing(arg_incrementValue);
		}
	}
	else
	{
		moveLugTime--;

		if (moveLugTime > 0)
		{
			moveEndFlag = true;
		}
		else
		{
			moveEndFlag = false;
		}
	}
}

void Alien::Draw()
{
	if ((isDeadFlag == false || dissolveCount > 0.0f)&&moveLugTime<=0)
	{
		//�萔�o�b�t�@�̓]��
		TransferConstBuff();

		Alien::cmdList->SetPipelineState(PipelineState::dissolvePostEffectPipelineState.Get());

		Alien::cmdList->SetGraphicsRootSignature(RootSignature::dissolvePostEffectSignature.Get());

		ID3D12DescriptorHeap* ppHeaps[] = { descHeapSRV.Get() };
		// �f�X�N���v�^�q�[�v���Z�b�g
		cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

		cmdList->SetGraphicsRootDescriptorTable(3,
			CD3DX12_GPU_DESCRIPTOR_HANDLE(
				descHeapSRV->GetGPUDescriptorHandleForHeapStart(), 0,
				device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
			)
		);

		Alien::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		Alien::cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());
		Alien::cmdList->SetGraphicsRootConstantBufferView(1, constBuffB1->GetGPUVirtualAddress());

		objModel->Draw(Alien::cmdList);
	}

	if (isDeadFlag)
	{
		if (scoreCharacter->GetColor().w >= 0.1f)
		{
			scoreCharacter->Draw();
		}
	}
}

void Alien::TransferConstBuff()
{
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
	DissolveConstBufferData* constMap0 = nullptr;
	result = constBuffB0->Map(0, nullptr, (void**)&constMap0);
	constMap0->color = color;
	if (isDeadFlag)
	{//���S���o����
		const float dessolveCountIncrementSize = 0.01f;
		dissolveCount -= dessolveCountIncrementSize;
	}
	constMap0->dissolveCount = dissolveCount;
	constMap0->mat = matWorld * matView * matProjection;	// �s��̍���
	constBuffB0->Unmap(0, nullptr);
}

void Alien::Move()
{
	//�����̎��
	switch (currentType)
	{
	case MOVETYPE::RECTANGLE:

		RectAngleMove();

		break;

	case MOVETYPE::TRIANGLE:

		TriAngleMove();

		break;

	case MOVETYPE::LEFTDIAGONALLY:

		LeftDiagonallyMove();

		break;

	case MOVETYPE::UPCURVE:

		UpCurveMove();

		break;

	case MOVETYPE::DOWNCURVE:

		DownCurveMove();

		break;

	default:
		break;
	}

	velocity = velocity.normalize();

	position.x = position.x + velocity.x * speed.x;
	position.y = position.y + velocity.y * speed.y;
}

void Alien::RectAngleMove()
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

void Alien::TriAngleMove()
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

void Alien::LeftDiagonallyMove()
{
	if (currentPhase == MOVEPHASE::PHASE1)//�΂߂Ɉړ����鏈��
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

void Alien::UpCurveMove()
{
	if (currentPhase == MOVEPHASE::PHASE1)//������ɃJ�[�u���鏈��
	{
		if (curveTime < 2.0f)
		{
			const float curveTimeIncrementSize = 0.01f;
			curveTime += curveTimeIncrementSize;
		}

		if (rotation.y > 0.0f)
		{
			const float rotationIncrementSize = 1.0f;
			rotation.y -= rotationIncrementSize;
		}

		velocity.x = cosf((XM_PI / 2) * curveTime);
		velocity.y = sinf((XM_PI / 2) * curveTime);


		if (position.x < SCREENLEFT - 5.0f)
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

void Alien::DownCurveMove()
{
	if (currentPhase == MOVEPHASE::PHASE1)//�������ɃJ�[�u���鏈��
	{
		if (curveTime > 2.0f)
		{
			const float curveTimeIncrementSize = 0.01f;
			curveTime -= curveTimeIncrementSize;
		}

		const float rotationMaxAngle = 360.0f;
		if (rotation.y < rotationMaxAngle)
		{
			const float rotationIncrementSize = 1.0f;
			rotation.y += rotationIncrementSize;
		}

		velocity.x = cosf((XM_PI / 2) * curveTime);
		velocity.y = sinf((XM_PI / 2) * curveTime);


		if (position.x < SCREENLEFT - 5.0f)
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

void Alien::IsDead()
{
	isDeadFlag = true;
}

void Alien::SetMoveType(const MOVETYPE& arg_currentType)
{
	currentType = arg_currentType;

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

void Alien::SetScrollIncrement(const Vector3& arg_incrementValue)
{
	position += arg_incrementValue;
}

void Alien::Damage()
{
	if (invisibleTime < 1)
	{
		hp--;
		invisibleTime = INVISIBLEMAXTIME;
	}
	if (hp == 0)
	{
		isDeadFlag = true;
		const Vector4 white = { 1.0f,1.0f,1.0f,1.0f };
		scoreCharacter->SetColor(white);
	}
}

void Alien::CriticalDamage()
{
	if (invisibleTime < 1)
	{
		hp--;
		invisibleTime = INVISIBLEMAXTIME;
	}
	if (hp == 0)
	{
		isDeadFlag = true;
		const Vector4 white = { 1.0f,1.0f,1.0f,1.0f };
		scoreCharacter->SetColor(white);
	}
}

bool Alien::IsCollision(GameObject* arg_otherObject)
{
	GameObject* otherObject = arg_otherObject;

	if (isDeadFlag || otherObject->GetIsDeadFlag() || otherObject->GetCharacterType() == characterType
		|| otherObject->GetCharacterType() == CHARACTERTYPE::BLOCK)
	{
		return false;
	}

	if (otherObject->GetCharacterType() == CHARACTERTYPE::PLAYER)
	{
		return false;
	}

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

	return false;
}

void Alien::ScoreProcessing(const Vector3& arg_incrementValue)
{
	//���݂̃X�R�A�̐F�ƃX�P�[�����擾
	Vector4 scoreColor = scoreCharacter->GetColor();
	Vector3 scoreScale = scoreCharacter->GetScale();

	if (scoreScale.x <= 2.5f)
	{//�������g�債�Ă���
		scoreScale.x += 0.05f;
		scoreScale.y += 0.05f;
		scoreScale.z += 0.05f;
	}

	if (scoreColor.w > 0.0f)
	{//���������߂��Ă���
		scoreColor.w -= 0.01f;
	}

	scoreCharacter->SetScale(scoreScale);
	scoreCharacter->SetColor(scoreColor);
	scoreCharacter->SetPosition({ position.x,position.y,position.z + 5.0f });
	scoreCharacter->Update(arg_incrementValue);
}
