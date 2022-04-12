#include "UnionCharacter.h"
using namespace Microsoft::WRL;

ID3D12GraphicsCommandList* UnionCharacter::cmdList;
XMMATRIX UnionCharacter::matView;
XMMATRIX UnionCharacter::matProjection;
Camera* UnionCharacter::camera = nullptr;
ID3D12Device* UnionCharacter::device = nullptr;

UnionCharacter::UnionCharacter(ID3D12GraphicsCommandList* arg_cmdList, ID3D12Device* arg_device)
{
	cmdList = arg_cmdList;
	device = arg_device;
	scale = { 0.2f,0.2f,0.2f };
	rotation = { 0.0f,0.0f,90.0f };
	speed = { 0.2f,0.2f,0.2f };
	characterType = CHARACTERTYPE::FRIEND;
	collisionRadius = 0.25f;
	shotFlag = true;
	matProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), (float)WindowSize::window_width / (float)WindowSize::window_height, 0.1f, 1000.0f);
}

UnionCharacter::~UnionCharacter()
{
	for (int i = 0; i < bullets.size(); i++)
	{
		delete bullets[i];
	}
}

void UnionCharacter::CreateConstBuffer()
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

UnionCharacter* UnionCharacter::Create(ID3D12Device* arg_device, ID3D12GraphicsCommandList* arg_cmdList,const Vector3& arg_position)
{
	UnionCharacter* unionCharacter = new UnionCharacter(arg_cmdList, arg_device);

	unionCharacter->Initialize();

	unionCharacter->SetPosition(arg_position);

	unionCharacter->CreateConstBuffer();

	unionCharacter->AttachBullet(arg_device);

	unionCharacter->SetLanchFlag(true);

	return unionCharacter;
}

void UnionCharacter::SetEye(const Vector3& arg_eye)
{
	UnionCharacter::camera->SetEye(arg_eye);

	UpdateViewMatrix();
}

void UnionCharacter::SetTarget(const Vector3& arg_target)
{
	UnionCharacter::camera->SetTarget(arg_target);

	UpdateViewMatrix();
}

void UnionCharacter::UpdateViewMatrix()
{
	matView = XMMatrixLookAtLH(XMLoadFloat3(&camera->GetEye()), XMLoadFloat3(&camera->GetTarget()), XMLoadFloat3(&camera->GetUp()));
}

void UnionCharacter::SetOBJModel(ObjFileModel* arg_objModel, ObjFileModel* arg_bulletModel)
{
	objModel = arg_objModel;
	SetBulletModel(arg_bulletModel);
}

void UnionCharacter::SetBulletModel(ObjFileModel* arg_bulletModel)
{
	for (int i = 0; i < bullets.size();i++)
	{
		bullets[i]->SetOBJModel(arg_bulletModel);
	}
}

void UnionCharacter::AttachBullet(ID3D12Device* arg_device)
{
	bullets.resize(BULLETMAXNUM);

	for (int i = 0; i < bullets.size(); i++)
	{
		bullets[i] = Bullet::Create(arg_device, cmdList);
		bullets[i]->SetIsDeadFlag(true);
		bullets[i]->SetCharacterType(CHARACTERTYPE::FRIEND);
		bullets[i]->SetVelocity({1.0f,0.0f,0.0f});
	}
}

void UnionCharacter::Initialize()
{
	isDeadFlag = false;
	playerFrontFlag = true;
	shotFlag = true;
	speed = { 0.2f,0.2f,0.2f };
	shotLugTime = MAXPLAYERLUGTIME;
}

//ユニオンの更新処理
void UnionCharacter::Update(GameObject* arg_player, const Vector3& arg_incrementValue)
{
	if (isDeadFlag)
	{ 
		return;
	}

	//移動処理
	Move(arg_player);
	//弾の発射処理
	if (shotFlag && arg_player->GetPosition().z >= 0.0f)
	{
		ShotBullet(arg_incrementValue);
	}

	UpdateAttack(arg_incrementValue);
	//定数バッファの転送
	TransferConstBuff(arg_incrementValue);
}

void UnionCharacter::Draw()
{
	if (isDeadFlag)
	{
		return;
	}
	
	UnionCharacter::cmdList->SetPipelineState(PipelineState::basicPipelineState.Get());

	UnionCharacter::cmdList->SetGraphicsRootSignature(RootSignature::basicRootsignature.Get());

	UnionCharacter::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UnionCharacter::cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());
	UnionCharacter::cmdList->SetGraphicsRootConstantBufferView(1, constBuffB1->GetGPUVirtualAddress());

	objModel->Draw(UnionCharacter::cmdList);


	for (int i = 0; i < bullets.size(); i++)
	{
		bullets[i]->Draw();
	}
	
}

void UnionCharacter::SetScrollIncrement(const Vector3& arg_incrementValue)
{
	position += arg_incrementValue;
}

void UnionCharacter::SetBossSceneFlag(bool arg_bossSceneFlag)
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

		rotation = { -90.0f,0.0f,90.0f };
	}
	else
	{
		for (int i = 0; i < bullets.size(); i++)
		{
			bullets[i]->SetIsDeadFlag(true);
			bullets[i]->SetRotation({ 0.0f,0.0f,0.0f });
			bullets[i]->SetVelocity({ 1.0f,0.0f,0.0f });
		}

		rotation = { 0.0f,0.0f,90.0f };
	}
}

bool UnionCharacter::IsPlayerCollision(const Vector3& arg_otherPosition, float arg_otherRadius)
{
	if (lanchFlag)
	{
		return false;
	}

	if (Collision::CheckSphereToSphere({ {arg_otherPosition.x + 1.0f,arg_otherPosition.y,arg_otherPosition.z},{arg_otherRadius} },
		{ {position.x     ,position.y     ,position.z     },{0.5f       } }))
	{
		playerFrontFlag = true;
		playerBackFlag = false;
		return true;
	}
	else if (Collision::CheckSphereToSphere({ {arg_otherPosition.x - 1.0f,arg_otherPosition.y,arg_otherPosition.z},{arg_otherRadius} },
		{ {position.x     ,position.y     ,position.z     },{0.5f       } }))
	{
		playerFrontFlag = false;
		playerBackFlag = true;
		return true;
	}

	return false;
}

void UnionCharacter::SetLanchFlag(bool arg_lanchFlag)
{ 
	lanchFlag = arg_lanchFlag;
	if (lanchFlag)
	{
		playerBackFlag = false;
		playerFrontFlag = false;
	}
}

void UnionCharacter::ShotBullet(const Vector3& arg_incrementValue)
{
	if (shotLugTime < 1)
	{
		if (Input::GetInstance()->KeyState(DIK_SPACE) || Input::GetInstance()->PadButtonState(XINPUT_GAMEPAD_A))
		{
			for (int i = 0; i < bullets.size(); i++)
			{
				if (bullets[i]->GetIsDeadFlag() == false)
					continue;
				
				bullets[i]->SetIsDeadFlag(false);
				if (bossSceneFlag == false)
				bullets[i]->SetPosition({ position.x + 0.5f , position.y, position.z });
				
				if (bossSceneFlag)
					bullets[i]->SetPosition({ position.x, position.y, position.z + 0.5f });

				shotLugTime = MAXPLAYERLUGTIME;
				break;
			}
		}
	}
	else
	{
		shotLugTime--;
	}
}

void UnionCharacter::TransferConstBuff(const Vector3& arg_incrementValue)
{
	HRESULT result;
	XMMATRIX matScale, matRot, matTrans;
	UpdateViewMatrix();
	SetScrollIncrement(arg_incrementValue);

	if (bossSceneFlag)
	{
		if (rotation.z > 360.0f)
		{
			rotation.z = 0.0f;
		}
		else
		{
			const float rotationIncrementSize = 1.0f;
			rotation.z += rotationIncrementSize;
		}
	}
	else
	{
		if (rotation.y > 360.0f)
		{
			rotation.y = 0.0f;
		}
		else
		{
			const float rotationIncremntSize = 1.0f;
			rotation.y += rotationIncremntSize;
		}
	}

	// スケール、回転、平行移動行列の計算
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

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


	//ワールド行列の転送
	ConstBufferDataB0* constMap0 = nullptr;
	result = constBuffB0->Map(0, nullptr, (void**)&constMap0);
	constMap0->color = color;
	constMap0->mat = matWorld * matView * matProjection;	// 行列の合成
	constBuffB0->Unmap(0, nullptr);
}

void UnionCharacter::Move(GameObject* arg_player)
{
	const Vector3 playerPosition = arg_player->GetPosition();
	if (arg_player != nullptr)
	{//2点のベクトル計算
		Vector3 basePosition = { 0,0,0 };
		if (lanchFlag)
		{
			const float playerFrontPosition = 20.0f;
			if (bossSceneFlag == false)
			{
				basePosition = { playerPosition.x + playerFrontPosition - position.x,playerPosition.y - position.y,playerPosition.z };
			}

			if (bossSceneFlag)
			{
				basePosition = { playerPosition.x - position.x,playerPosition.y - position.y,playerPosition.z - position.z + playerFrontPosition };
			}

			Vector3 dir = basePosition;
			//ノルム計算
			float length = dir.length();
			//限界の長さよりも長ければ計算を適用
			if (length > maxWireLength)
			{//伸びた分だけ計算を適用
				dir = dir * ((float)length - (float)maxWireLength) / (float)maxWireLength;
				accel = dir * 0.001f;//ばね定数
			}
		}
		else if (playerFrontFlag == false && playerBackFlag == false)
		{
			//子機を装着する際プレイヤーの手前で止まってしまうため実際のポジションより後ろ側にずらす為の変数
			const float playerFixPos = 2.0f;
			if (bossSceneFlag == false)
			{
				basePosition = { (playerPosition.x - playerFixPos) - position.x,playerPosition.y - position.y,playerPosition.z };
			}

			if (bossSceneFlag)
			{
				basePosition = { (playerPosition.x - playerFixPos) - position.x,playerPosition.y - position.y,playerPosition.z - position.z };
			}

			Vector3 dir = basePosition;
			//ノルム計算
			float length = dir.length();
			//限界の長さよりも長ければ計算を適用
			if (length > maxWireLength)
			{//伸びた分だけ計算を適用
				const float k = 0.0002f;
				dir = dir * ((float)length - (float)maxWireLength) / (float)maxWireLength;
				accel = dir * k;//ばね定数
			}
		}
	}

	speed += accel;
	speed -= speed * 0.04f;

	if (playerFrontFlag)
	{
		if (bossSceneFlag)
		{
			const float playerFrontPosition = 2.0f;
			position = { playerPosition.x, playerPosition.y,playerPosition.z + playerFrontPosition };
		}
		else
		{
			const float playerFrontPosition = 3.0f;
			position = { playerPosition.x + playerFrontPosition, playerPosition.y,playerPosition.z };
		}
	}
	else if (playerBackFlag)
	{
		if (bossSceneFlag)
		{
			const float playerBackPosition = -2.0f;
			position = { playerPosition.x, playerPosition.y,playerPosition.z + playerBackPosition };
		}
		else
		{
			const float playerBackPosition = -2.0f;
			position = { playerPosition.x + playerBackPosition, playerPosition.y,playerPosition.z };
		}
	}
	else
	{
		position.x = position.x + speed.x;
		position.y = position.y + speed.y;

		if (bossSceneFlag)
		{
			position.z = position.z + speed.z;
		}
		else
		{
			position.z = playerPosition.z;
		}
	}
}

bool UnionCharacter::IsCollision(GameObject* arg_otherObject)
{
	GameObject* otherObject = arg_otherObject;

	if (isDeadFlag || otherObject->GetIsDeadFlag() || otherObject->GetCharacterType() == CHARACTERTYPE::FRIEND 
		|| otherObject->GetCharacterType() == CHARACTERTYPE::PLAYER || otherObject->GetCharacterType() == CHARACTERTYPE::BLOCK) { return false; }

	for (int i = 0; i < bullets.size(); i++)
	{
		bullets[i]->IsCollision(otherObject);
	}

	if (Collision::CheckSphereToSphere({ otherObject->GetPosition(),otherObject->GetCollisionRadius() },
		{ position,collisionRadius }))
	{
		otherObject->Damage();
		return true;
	}

	return false;
}

void UnionCharacter::UpdateAttack(const Vector3& arg_incrementValue)
{
	for (int i = 0; i < bullets.size(); i++)
	{
		if (bullets[i]->GetIsDeadFlag() == true)
			continue;

		bullets[i]->Update(arg_incrementValue);

		if (bossSceneFlag == false)
		{
			const float bulletRange = 10.0f;
			if (bullets[i]->GetPosition().x <= position.x + SCREENRIGHT + bulletRange)
			{
				continue;
			}
		}

		if (bossSceneFlag)
		{
			if (bullets[i]->GetPosition().z <= position.z + SCREENBACK)
			{
				continue;
			}
		}

		bullets[i]->SetIsDeadFlag(true);
	}
}
