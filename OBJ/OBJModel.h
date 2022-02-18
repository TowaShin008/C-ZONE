#pragma once
#include<string>
#include<directXMath.h>
#include <vector>
#include<DirectXTex.h>
#include<Windows.h>
#include<wrl.h>
#include<d3d12.h>
#include<d3dx12.h>
#include<string>
#include"MineMath.h"
using namespace DirectX;

// ���_�f�[�^�\����
struct VertexPosNormalUv
{
	Vector3 pos; // xyz���W
	Vector3 normal; // �@���x�N�g��
	Vector2 uv;  // uv���W
};
class OBJModel
{
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	OBJModel();
	~OBJModel();

	friend class OBJLoader;

	struct Material
	{
		std::string name;
		Vector3 ambient;
		Vector3 diffuse;
		Vector3 specular;
		float alpha;
		std::string textureFilename;
		Material() {
			ambient = { 0.3f,0.3f,0.3f };
			diffuse = { 0.0f,0.0f,0.0f };
			specular = { 0.0f,0.0f,0.0f };
			alpha = 1.0f;
		}
	};

	/// <summary>
	/// �R�̃o�b�t�@�[�̐�������(���_�o�b�t�@�A�C���f�b�N�X�o�b�t�@�A�e�N�X�`���o�b�t�@)
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	void CreateThreeBuffer(ID3D12Device* device);

	/// <summary>
	/// �`�揈��
	/// </summary>
	/// <param name="cmdList">�R�}���h���X�g</param>
	void Draw(ID3D12GraphicsCommandList* cmdList);

	//�}�e���A��
	Material material;
private:
	std::string modelname;
	std::string filename;
	std::string directoryPath;
	TexMetadata metadata = {};
	ScratchImage scratchImg = {};
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	ComPtr<ID3D12DescriptorHeap>descHeap;
	ComPtr<ID3D12Resource>texBuff;
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;
	std::vector<VertexPosNormalUv>vertices;
	std::vector<unsigned short>indices;
	
	ComPtr<ID3D12Resource>vertBuff;
	ComPtr<ID3D12Resource>indexBuff;

	D3D12_VERTEX_BUFFER_VIEW vbView;
	D3D12_INDEX_BUFFER_VIEW ibView;
};

