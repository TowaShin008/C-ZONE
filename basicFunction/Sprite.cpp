#include "Sprite.h"
#include <cassert>
#include <d3dx12.h>
#include <d3dcompiler.h>
#include <DirectXTex.h>

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace Microsoft::WRL;


ID3D12Device* Sprite::device = nullptr;
UINT Sprite::descriptorSize;
ID3D12GraphicsCommandList* Sprite::cmdList = nullptr;
XMMATRIX Sprite::matProjection;
ComPtr<ID3D12DescriptorHeap> Sprite::descHeap;
std::map<const wchar_t*,int> Sprite::texKey;
ComPtr<ID3D12Resource> Sprite::texBuff[srvCount];
int Sprite::texNum = 0;

Sprite::Sprite(const UINT& arg_texNumber, const Vector2& arg_position, const Vector2& arg_scale, const Vector4& arg_color, const Vector2& arg_anchorpoint)
{
	texNumber = arg_texNumber;
	position = arg_position;
	scale = arg_scale;
	anchorpoint = arg_anchorpoint;
	matWorld = XMMatrixIdentity();
	color = arg_color;
	texScale = arg_scale;
	slideUVCount = 0.0f;
	//射影行列
	matProjection = XMMatrixOrthographicOffCenterLH(
		0.0f, (float)WindowSize::window_width,
		(float)WindowSize::window_height, 0.0f,
		0.0f, 1.0f);
}

bool Sprite::CreateDescriptorHeap(ID3D12Device* arg_device)
{
	Sprite::device = arg_device;
	HRESULT result;
	//デスクリプターのサイズを記憶
	descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	const int srvCount = 512;
	//デスクリプタヒープを生成	
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダから見えるように
	descHeapDesc.NumDescriptors = srvCount;
	result = device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap));//生成
	if (FAILED(result)) {
		assert(0);
		return false;
	}

	return true;
}

bool Sprite::LoadTexture(const wchar_t * filename)
{
	if (texNum >= srvCount)
	{
		return false;
	}
	texKey[filename] = texNum;

	texNum++;

	HRESULT result;
	// WICテクスチャのロード
	TexMetadata metadata{};
	ScratchImage scratchImg{};

	result = LoadFromWICFile(
		filename, WIC_FLAGS_NONE,
		&metadata, scratchImg);
	if (FAILED(result)) {
		assert(0);
		return false;
	}

	const Image* img = scratchImg.GetImage(0, 0, 0); // 生データ抽出

	// 画像の設定
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels
	);

	// テクスチャ用バッファの生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, // テクスチャ用指定
		nullptr,
		IID_PPV_ARGS(&texBuff[texKey[filename]]));

	// テクスチャバッファにデータ転送
	result = texBuff[texKey[filename]]->WriteToSubresource(
		0,
		nullptr, // 全領域へコピー
		img->pixels,    // 元データアドレス
		(UINT)img->rowPitch,  // 1ラインサイズ
		(UINT)img->slicePitch // 1枚サイズ
	);
	// シェーダリソースビュー作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{}; // 設定構造体
	D3D12_RESOURCE_DESC resDesc = texBuff[texKey[filename]]->GetDesc();
	//シェーダーリソースビューの設定
	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;

	//シェーダーリソースビューの生成
	device->CreateShaderResourceView(texBuff[texKey[filename]].Get(), //ビューと関連付けるバッファ
		&srvDesc,
		CD3DX12_CPU_DESCRIPTOR_HANDLE(descHeap->GetCPUDescriptorHandleForHeapStart(), texKey[filename], descriptorSize)
	);

	return true;
}

void Sprite::BeginDraw(ID3D12GraphicsCommandList * arg_cmdList, bool bloomFlag)
{
	Sprite::cmdList = arg_cmdList;

	if (bloomFlag == false)
	{
		// パイプラインステートの設定
		cmdList->SetPipelineState(PipelineState::spritePipelineState.Get());
		// ルートシグネチャの設定
		cmdList->SetGraphicsRootSignature(RootSignature::spriteRootsignature.Get());
	}
	else
	{
		// パイプラインステートの設定
		cmdList->SetPipelineState(PipelineState::bloomSpritePipelineState.Get());
		// ルートシグネチャの設定
		cmdList->SetGraphicsRootSignature(RootSignature::bloomSpriteRootsignature.Get());
	}
	// プリミティブ形状を設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void Sprite::EndDraw()
{
	Sprite::cmdList = nullptr;
}

Sprite * Sprite::Create(const wchar_t* arg_filename, const Vector2& arg_position, const Vector4& arg_color, const Vector2& arg_anchorpoint, bool arg_isFlipX, bool arg_isFlipY)
{
	// 仮サイズ
	Vector2 size = { 50.0f, 50.0f };

	if (texBuff[texKey[arg_filename]])
	{
		// テクスチャ情報取得
		D3D12_RESOURCE_DESC resDesc = texBuff[texKey[arg_filename]]->GetDesc();
		// スプライトのサイズをテクスチャのサイズに設定
		size = { (float)resDesc.Width, (float)resDesc.Height };
	}

	// Spriteのインスタンスを生成
	Sprite* sprite = new Sprite(texKey[arg_filename], arg_position, size, arg_color);
	if (sprite == nullptr) {
		return nullptr;
	}

	// 初期化
	if (!sprite->Initialize()) {
		delete sprite;
		assert(0);
		return nullptr;
	}

	return sprite;
}

bool Sprite::Initialize()
{
	// nullptrチェック
	assert(device);

	HRESULT result = S_FALSE;

	// 頂点バッファ生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(VertexPosUv) * vertNum),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	if (FAILED(result)) {
		assert(0);
		return false;
	}

	// 頂点バッファへのデータ転送
	UpdateVertices();

	// 頂点バッファビューの作成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(VertexPosUv) * 4;
	vbView.StrideInBytes = sizeof(VertexPosUv);

	// 定数バッファの生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff)&~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));
	if (FAILED(result)) {
		assert(0);
		return false;
	}

	// 定数バッファにデータ転送
	ConstBufferData* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result)) {
		constMap->color = color;
		constMap->mat = matProjection;
		constBuff->Unmap(0, nullptr);
	}

	return true;
}

void Sprite::SetRotation(float arg_rotation)
{
	rotation = arg_rotation;

	// 頂点バッファへのデータ転送
	UpdateVertices();
}

void Sprite::SetPosition(const Vector2& arg_position)
{
	position = arg_position;

	// 頂点バッファへのデータ転送
	UpdateVertices();
}

void Sprite::SetAnchorPoint(const Vector2& arg_anchorpoint)
{
	anchorpoint = arg_anchorpoint;

	UpdateVertices();
}

void Sprite::SetScale(const Vector2& arg_scale)
{
	scale = arg_scale;

	// 頂点バッファへのデータ転送
	UpdateVertices();
}

void Sprite::Draw()
{
	// ワールド行列の更新
	matWorld = XMMatrixIdentity();
	matWorld *= XMMatrixRotationZ(XMConvertToRadians(rotation));
	matWorld *= XMMatrixTranslation(position.x, position.y, 0.0f);

	SlideProcessing();

	// 定数バッファにデータ転送
	ConstBufferData* constMap = nullptr;
	HRESULT result = constBuff->Map(0, nullptr, (void**)&constMap);
		constMap->color = color;
		constMap->slideUVCount = slideUVCount;
		constMap->mat = matWorld * matProjection;	// 行列の合成	
		constBuff->Unmap(0, nullptr);

	// 頂点バッファの設定
	cmdList->IASetVertexBuffers(0, 1, &vbView);

	ID3D12DescriptorHeap* ppHeaps[] = { descHeap.Get() };
	// デスクリプタヒープをセット
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	// 定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());
	// シェーダリソースビューをセット
	cmdList->SetGraphicsRootDescriptorTable(1, CD3DX12_GPU_DESCRIPTOR_HANDLE(descHeap->GetGPUDescriptorHandleForHeapStart(), texNumber, descriptorSize));
	// 描画コマンド
	cmdList->DrawInstanced(4, 1, 0, 0);
}

void Sprite::UpdateVertices()
{
	HRESULT result = S_FALSE;

	// 左下、左上、右下、右上
	enum { LB, LT, RB, RT };

	float left = (0.0f - anchorpoint.x) * scale.x;
	float right = (1.0f - anchorpoint.x) * scale.x;
	float top = (0.0f - anchorpoint.y) * scale.y;
	float bottom = (1.0f - anchorpoint.y) * scale.y;
	// 頂点データ
	VertexPosUv vertices[vertNum];

	vertices[LB].pos = { left,	bottom,	0.0f }; // 左下
	vertices[LT].pos = { left,	top,	0.0f }; // 左上
	vertices[RB].pos = { right,	bottom,	0.0f }; // 右下
	vertices[RT].pos = { right,	top,	0.0f }; // 右上

	// テクスチャ情報取得
	if (texBuff[texNumber])
	{
		D3D12_RESOURCE_DESC resDesc = texBuff[texNumber]->GetDesc();

		float tex_left = texBase.x / resDesc.Width;
		float tex_right = (texBase.x + texScale.x) / resDesc.Width;
		float tex_top = texBase.y / resDesc.Height;
		float tex_bottom = (texBase.y + texScale.y) / resDesc.Height;

		vertices[LB].uv = { tex_left,	tex_bottom }; // 左下
		vertices[LT].uv = { tex_left,	tex_top }; // 左上
		vertices[RB].uv = { tex_right,	tex_bottom }; // 右下
		vertices[RT].uv = { tex_right,	tex_top }; // 右上
	}

	// 頂点バッファへのデータ転送
	VertexPosUv* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		memcpy(vertMap, vertices, sizeof(vertices));
		vertBuff->Unmap(0, nullptr);
	}
}

void Sprite::MoveUpdate()
{
	if (lugTime > 0)
	{
		lugTime--;
	}
	else
	{
		//単振動計算
		accel.x = -0.001f*(position.x - window_width / 2);
		accel.x = -0.001f*(position.y - window_height / 2);

		speed += accel;

		position.x = position.x + speed.x;
		position.y = position.y + speed.y;
	}
}

void Sprite::GradatioUpdate()
{
	if (lugTime > 0)
	{
		lugTime--;
	}
	else
	{
		if (color.w <= 0)
		{//アルファ値が0になったら一定時間待機
			stayInvisibleTime--;
			if (stayInvisibleTime <= 0)
			{
				stayInvisibleTime = 60;
				alphaFlag = true;
			}
		}
		else if (color.w >= 0.6f)
		{//アルファ値がある程度あれば徐々に薄く
			alphaFlag = false;
		}

		if (alphaFlag == true && color.w <= 0.6f)
		{
			color.w += 0.002f;
		}
		if (alphaFlag == false && color.w >= 0.0f)
		{
			color.w -= 0.002f;
		}
	}
}

//ばねの挙動を使いたい場合はこちらのアップデート処理を使用
void Sprite::WireUpdate()
{
	if (!(parentTop == nullptr && parentButtom))
	{
		accel.x = 0.0f;
		accel.y = g;
		//上部に連結しているオブジェクトがあれば計算を適用する。
		if (parentTop != nullptr)
		{
			Vector2 dir = { parentTop->GetPosition().x - position.x,parentTop->GetPosition().y - position.y };
			float length = dir.length();
			if (length > LENGTH_LINK)
			{//ここが適用されたときに挙動がおかしくなる
				dir = dir * (length - LENGTH_LINK) / LENGTH_LINK;
				accel += dir * 0.01f / m;
			}
		}
		//下部に連結しているオブジェクトがあれば計算を適用する
		if (parentButtom != nullptr)
		{
			Vector2 dir = { parentButtom->GetPosition().x - position.x,parentButtom->GetPosition().y - position.y };
			float length = dir.length();
			if (length > LENGTH_LINK)
			{
				dir = dir * (length - LENGTH_LINK) / LENGTH_LINK;
				accel += dir * 0.01f / m;
			}
		}

		speed += accel;

		speed -= speed * 0.04f;

		position.x = position.x + speed.x;
		position.y = position.y + speed.y;
	}
}

void Sprite::EndingMoveUpdate()
{
	if (position.x <= 50.0f || position.x > 1250.0f)
	{
		speed.x *= -1.0f;
		collisionFlag = true;
	}
	//単振動計算
	accel.x = -0.001f * (position.x - window_width / 2);
	accel.y = -0.001f * (position.y - window_height / 2);

	speed.x += accel.x;
	speed.y += accel.y;

	speed.x -= speed.x * 0.005f;
	speed.y -= speed.y * 0.005f;

	position.x = position.x + speed.x;
	position.y = position.y + speed.y;
}

void Sprite::SetSlideTextureFlag(bool arg_slideFlag,bool arg_rightSlideFlag)
{//スライドするか
	slideFlag = arg_slideFlag;
	rightSlideFlag = arg_rightSlideFlag;
}

void Sprite::SetVelocity(const Vector2& arg_velocity)
{
	velocity = arg_velocity;
}

void Sprite::SetTextureRect(const Vector2& arg_texBase, const Vector2& arg_texScale)
{
	texBase = arg_texBase;
	texScale = arg_texScale;

	// 頂点バッファへのデータ転送
	UpdateVertices();
}

bool Sprite::EndingCollision(Sprite* arg_otherBall, float radius)
{
	Sprite* otherBall = arg_otherBall;
	//現在の数値を変数に格納
	float V = speed.x;
	float v = otherBall->GetSpeedX();
	float m_2 = otherBall->GetM();

	float dy = otherBall->GetPosition().y - position.y;

	float dx = otherBall->GetPosition().x - position.x;


	float c = dy * dy + dx * dx;

	//衝突からある程度たったら再度当たり判定を適用
	if (collisionLugtime > 0)
	{
		collisionLugtime--;
	}
	else
	{
		collisionFlag = true;
	}

	if ((radius * 2) * (radius * 2) > c)
	{
		if (collisionFlag == true)
		{
			//自身の運動量の計算
			speed.x = (e * m_2 * (v - V) + m * V + m_2 * v) / (m + m_2);

			//相手の運動量の計算
			otherBall->SetSpeedX((e * m * (V - v) + m * V + m_2 * v) / (m + m_2));


			collisionLugtime = 30;
			collisionFlag = false;
		}
		return true;
	}

	return false;
}

void Sprite::SlideProcessing()
{
	if (slideFlag)
	{
		if (rightSlideFlag)
		{
			if (slideUVCount < 1.0f)
			{
				slideUVCount += 0.002f;
			}
			else
			{
				slideUVCount = 0.0f;
			}
		}
		else
		{
			if (slideUVCount > 0.0f)
			{
				slideUVCount -= 0.002f;
			}
			else
			{
				slideUVCount = 1.0f;
			}
		}
	}
}

