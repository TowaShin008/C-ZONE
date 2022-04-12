#include "LightObject.h"


LightObject::Material LightObject::material;
ID3D12Device* LightObject::device = nullptr;
ID3D12GraphicsCommandList* LightObject::cmdList = nullptr;
XMMATRIX LightObject::matView{};
XMMATRIX LightObject::matProjection{};
Vector3 LightObject::eye = { 0,0,-20.0f };
Vector3 LightObject::target = { 0,0,0 };
Vector3 LightObject::up = { 0,1,0 };
ComPtr<ID3D12DescriptorHeap>LightObject::descHeap;
ComPtr<ID3D12Resource>LightObject::vertBuff;
ComPtr<ID3D12Resource>LightObject::indexBuff;
ComPtr<ID3D12Resource>LightObject::texBuff;
D3D12_VERTEX_BUFFER_VIEW LightObject::vbView;
D3D12_INDEX_BUFFER_VIEW LightObject::ibView;
vector<LightObject::VertexPosNormalUv>LightObject::vertices;
vector<unsigned short>LightObject::indices;
Light* LightObject::light = nullptr;
Light* LightObject::light_2 = nullptr;

LightObject::LightObject()
{
	matProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), (float)WindowSize::window_width / (float)WindowSize::window_height, 0.1f, 1000.0f);
}


LightObject::~LightObject()
{
}



void LightObject::CreateDescriptorHeap(ID3D12Device* device)
{
	LightObject::device = device;
	HRESULT result;
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = 1;
	//デスクリプターヒープの生成
	result = device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap));
}

void LightObject::LoadMaterial(const std::string& directoryPath, const std::string& filename)
{
	std::ifstream file;
	file.open(directoryPath + filename);//ファイルの名称がここに入る

	string line;

	while (getline(file, line))
	{
		std::istringstream line_stream(line);

		string key;
		getline(line_stream, key, ' ');


		if (key == "newmtl") {
			line_stream >> material.name;
		}
		if (key == "Ka")
		{
			line_stream >> material.ambient.x;
			line_stream >> material.ambient.y;
			line_stream >> material.ambient.z;
		}
		if (key == "Kd")
		{
			line_stream >> material.diffuse.x;
			line_stream >> material.diffuse.y;
			line_stream >> material.diffuse.z;
		}
		if (key == "Ks")
		{
			line_stream >> material.specular.x;
			line_stream >> material.specular.y;
			line_stream >> material.specular.z;
		}
		if (key == "map_Kd") {
			line_stream >> material.textureFilename;
			HRESULT result = S_FALSE;

			// WICテクスチャのロード
			TexMetadata metadata{};
			ScratchImage scratchImg{};

			//ファイルパスを結合
			string filepath = directoryPath + material.textureFilename;

			//ユニコード文字列に変換する
			wchar_t wfilepath[128];
			int iBufferSize = MultiByteToWideChar(CP_ACP, 0,
				filepath.c_str(), -1, wfilepath, _countof(wfilepath));

			result = LoadFromWICFile(
				wfilepath, WIC_FLAGS_NONE,
				&metadata, scratchImg);

			if (FAILED(result)) {
				assert(0);
			}

			const Image* img = scratchImg.GetImage(0, 0, 0); // 生データ抽出

			// リソース設定
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
				IID_PPV_ARGS(&texBuff));
			if (FAILED(result)) {
				assert(0);
			}

			// テクスチャバッファにデータ転送
			result = texBuff->WriteToSubresource(
				0,
				nullptr, // 全領域へコピー
				img->pixels,    // 元データアドレス
				(UINT)img->rowPitch,  // 1ラインサイズ
				(UINT)img->slicePitch // 1枚サイズ
			);
			if (FAILED(result)) {
				assert(0);
			}

			// シェーダリソースビュー作成
			cpuDescHandleSRV = CD3DX12_CPU_DESCRIPTOR_HANDLE(descHeap->GetCPUDescriptorHandleForHeapStart(), 0, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
			gpuDescHandleSRV = CD3DX12_GPU_DESCRIPTOR_HANDLE(descHeap->GetGPUDescriptorHandleForHeapStart(), 0, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{}; // 設定構造体
			D3D12_RESOURCE_DESC resDesc = texBuff->GetDesc();

			srvDesc.Format = resDesc.Format;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
			srvDesc.Texture2D.MipLevels = 1;

			device->CreateShaderResourceView(texBuff.Get(), //ビューと関連付けるバッファ
				&srvDesc, //テクスチャ設定情報
				cpuDescHandleSRV
			);
		}
	}
	file.close();
}

void LightObject::CreateCharacter(const string& modelName)
{
	std::ifstream file;
	const string modelname = modelName;
	const string filename = modelname + ".obj";//"triangle_mat.obj"
	const string directoryPath = "Resources/" + modelname + "/";//"Resources/triangle_mat/"
	file.open(directoryPath + filename);//"Resources/triangle_mat/triangle_mat.obj"

	if (file.fail()) {
		assert(0);
		return;
	}

	vector<Vector3>positions;  //頂点座標
	vector<Vector3>normals;    //法線ベクトル
	vector<Vector2>texcoords;  //テクスチャUV
	string line;//読み込んだ一行分の文字列がここに入る
	while (getline(file, line)) {
		//1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		string key;
		getline(line_stream, key, ' ');

		if (key == "v")
		{
			Vector3 position{};
			line_stream >> position.x;
			line_stream >> position.y;
			line_stream >> position.z;
			positions.emplace_back(position);
		}
		if (key == "vt") {
			Vector2 texcoord{};
			line_stream >> texcoord.x;
			line_stream >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoord.x = -texcoord.x;
			texcoords.emplace_back(texcoord);
		}
		if (key == "vn") {
			Vector3 normal{};
			line_stream >> normal.x;
			line_stream >> normal.y;
			line_stream >> normal.z;
			normals.emplace_back(normal);
		}
		if (key == "f")
		{
			int indexCount = 0;
			VertexPosNormalUv vertex_1, vertex_3;
			string index_string;
			while (getline(line_stream, index_string, ' '))
			{
				indexCount++;
				if (indexCount < 4)
				{
					std::istringstream index_stream(index_string);
					unsigned short indexPosition, indexNormal, indexTexcoord;
					index_stream >> indexPosition;

					index_stream.seekg(1, ios_base::cur);//スラッシュを飛ばす
					index_stream >> indexTexcoord;
					index_stream.seekg(1, ios_base::cur);//スラッシュを飛ばす
					index_stream >> indexNormal;


					VertexPosNormalUv vertex{};
					vertex.pos = positions[indexPosition - 1];
					vertex.normal = normals[indexNormal - 1];
					vertex.uv = texcoords[indexTexcoord - 1];

					if (indexCount == 1)
					{
						vertex_1 = vertex;
					}
					else if (indexCount == 3)
					{
						vertex_3 = vertex;
					}
					vertices.emplace_back(vertex);
					indices.emplace_back(indices.size());
				}
				else if (indexCount == 4)
				{
					std::istringstream index_stream(index_string);
					unsigned short indexPosition, indexNormal, indexTexcoord;
					index_stream >> indexPosition;

					index_stream.seekg(1, ios_base::cur);//スラッシュを飛ばす
					index_stream >> indexTexcoord;
					index_stream.seekg(1, ios_base::cur);//スラッシュを飛ばす
					index_stream >> indexNormal;


					VertexPosNormalUv vertex{};
					vertex.pos = positions[indexPosition - 1];
					vertex.normal = normals[indexNormal - 1];
					vertex.uv = texcoords[indexTexcoord - 1];

					vertices.emplace_back(vertex);
					indices.emplace_back(indices.size());


					vertices.emplace_back(vertex_1);
					indices.emplace_back(indices.size());


					vertices.emplace_back(vertex_3);
					indices.emplace_back(indices.size());
				}
			}
		}
		if (key == "mtllib")
		{
			string filename;
			line_stream >> filename;
			LoadMaterial(directoryPath, filename);
		}
	}
	file.close();

	HRESULT result = S_FALSE;

	UINT sizeVB = static_cast<UINT>(sizeof(VertexPosNormalUv)*vertices.size());
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short)*indices.size());	// 頂点バッファ生成



	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	if (FAILED(result)) {
		assert(0);
		return;
	}

	// インデックスバッファ生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeIB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));
	if (FAILED(result)) {
		assert(0);
		return;
	}

	// 頂点バッファへのデータ転送
	VertexPosNormalUv* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		std::copy(vertices.begin(), vertices.end(), vertMap);
		vertBuff->Unmap(0, nullptr);
	}

	// インデックスバッファへのデータ転送
	unsigned short* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	if (SUCCEEDED(result)) {

		std::copy(indices.begin(), indices.end(), indexMap);

		indexBuff->Unmap(0, nullptr);
	}

	// 頂点バッファビューの作成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(vertices[0]);

	// インデックスバッファビューの作成
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;

	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
}

LightObject* LightObject::Create(ID3D12Device* device)
{
	LightObject* lightObject = new LightObject();
	float scale_2 = 1.0f;
	lightObject->scale = { scale_2,scale_2,scale_2 };

	lightObject->CreateConstBuffer(device);

	return lightObject;
}

void LightObject::CreateConstBuffer(ID3D12Device* device)
{
	HRESULT result;

	this->device = device;

	result = this->device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff)&~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB0));

	if (FAILED(result))
	{
		assert(0);
	}

	result = this->device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB1) + 0xff)&~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB1));

	if (FAILED(result))
	{
		assert(0);
	}
}

void LightObject::Update()
{
	HRESULT result;
	XMMATRIX matScale, matRot, matTrans;

	// スケール、回転、平行移動行列の計算
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	// ワールド行列の合成
	matWorld = XMMatrixIdentity(); // 変形をリセット
	matWorld *= matScale; // ワールド行列にスケーリングを反映
	matWorld *= matRot; // ワールド行列に回転を反映
	matWorld *= matTrans; // ワールド行列に平行移動を反映
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

	const XMMATRIX& matViewProjection = matView * matProjection;
	const Vector3& cameraPos = GetEye();

	// 定数バッファへデータ転送
	ConstBufferDataB0* constMap = nullptr;
	result = constBuffB0->Map(0, nullptr, (void**)&constMap);

	constMap->viewproj = matViewProjection;
	constMap->world = matWorld;
	constMap->cameraPos = cameraPos;
	constBuffB0->Unmap(0, nullptr);

	ConstBufferDataB1* constMap1 = nullptr;
	result = constBuffB1->Map(0, nullptr, (void**)&constMap1);
	constMap1->ambient = material.ambient;
	constMap1->diffuse = material.diffuse;
	constMap1->specular = material.specular;
	constMap1->alpha = material.alpha;
	constBuffB1->Unmap(0, nullptr);
}

void LightObject::Draw(ID3D12GraphicsCommandList* cmdList)
{

	this->cmdList = cmdList;
	this->cmdList->SetPipelineState(PipelineState::lightObjectPipelineState.Get());
	this->cmdList->SetGraphicsRootSignature(RootSignature::lightObjectRootsignature.Get());

	this->cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	assert(device);
	assert(this->cmdList);
	// 頂点バッファの設定
	this->cmdList->IASetVertexBuffers(0, 1, &vbView);
	// インデックスバッファの設定
	this->cmdList->IASetIndexBuffer(&ibView);

	// デスクリプタヒープの配列
	ID3D12DescriptorHeap* ppHeaps[] = { descHeap.Get() };
	this->cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// 定数バッファビューをセット
	this->cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());
	this->cmdList->SetGraphicsRootConstantBufferView(1, constBuffB1->GetGPUVirtualAddress());

	//ライトの描画
	light->Draw(cmdList, 3);
	light_2->Draw(cmdList, 4);
	// シェーダリソースビューをセット
	this->cmdList->SetGraphicsRootDescriptorTable(2, gpuDescHandleSRV);
	// 描画コマンド
	this->cmdList->DrawIndexedInstanced((UINT)indices.size(), 1, 0, 0, 0);

	this->cmdList = nullptr;
}
