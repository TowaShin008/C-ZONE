#pragma once
#include<string>
#include"MineMath.h"
#include <vector>
#include<DirectXTex.h>
#include<Windows.h>
#include<wrl.h>
#include<d3d12.h>
#include<d3dx12.h>
#include<fbxsdk.h>

struct Node
{
	std::string name;

	DirectX::XMVECTOR scaleing = { 1,1,1,0 };

	DirectX::XMVECTOR rotation = { 0,0,0,0 };

	DirectX::XMVECTOR translation = { 0,0,0,1 };

	DirectX::XMMATRIX transform = DirectX::XMMatrixIdentity();

	DirectX::XMMATRIX globalTransform = DirectX::XMMatrixIdentity();

	Node* parent = nullptr;
};

class Model
{
private:
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	using XMMATRIX = DirectX::XMMATRIX;
	using TexMetadata = DirectX::TexMetadata;
	using ScratchImage = DirectX::ScratchImage;

	using string = std::string;
	template<class T>using vector = std::vector<T>;
public:
	//�f�X�g���N�^
	~Model();
	//�{�[���C���f�b�N�X�̍ő吔
	static const int MAX_BONE_INDICES = 4;
	//�t�����h�N���X
	friend class FbxLoader;
	struct VertexPosNormalUvSkin
	{
		Vector3 pos;
		Vector3 normal;
		Vector2 uv;
		UINT boneIndex[MAX_BONE_INDICES];//�{�[�� �ԍ�
		float boneWeight[MAX_BONE_INDICES];//�{�[�� �d��
	};
	//�{�[���\����
	struct Bone
	{
		//���O
		std::string name;
		//�����p���̋t�s��
		DirectX::XMMATRIX invInitialPose;
		//�N���X�^�[(FBX���̃{�[���v�f)
		FbxCluster* fbxCluster;
		//�R���X�g���N�^
		Bone(const std::string& name)
		{
			this->name = name;
		}
	};
private:
	//���f����
	std::string name;
	std::vector<Node> nodes;
	Node* meshNode = nullptr;
	std::vector<VertexPosNormalUvSkin>vertices;
	std::vector<unsigned short>indices;
	//�A���r�G���g�W��
	Vector3 ambient = { 1,1,1 };
	//�f�B�t���[�Y�W��
	Vector3 diffuse = { 1,1,1 };
	//�e�N�X�`�����^�f�[�^
	DirectX::TexMetadata metadata= {};
	//�X�N���b�`�C���[�W
	DirectX::ScratchImage scratchImg = {};
	ComPtr<ID3D12Resource>vertBuff;
	ComPtr<ID3D12Resource>indexBuff;
	ComPtr<ID3D12Resource>texBuff;
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	D3D12_INDEX_BUFFER_VIEW ibView = {};
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;
	//�{�[���z��
	std::vector<Bone>bones;
	//FBX�V�[��
	FbxScene* fbxScene = nullptr;
public:
	void CreateBuffers(ID3D12Device* device);
	void Draw(ID3D12GraphicsCommandList* cmdlist);
	const XMMATRIX& GetModelTransform() { return meshNode->globalTransform; }
	std::vector<Bone>& GetBones() { return bones; }
	FbxScene* GetFbxScene() { return fbxScene; }
};

