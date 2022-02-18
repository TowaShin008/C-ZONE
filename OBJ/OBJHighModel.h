#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "Mesh.h"
#include <memory>

/// <summary>
/// ���f���f�[�^
/// </summary>
class OBJHighModel
{
private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMMATRIX = DirectX::XMMATRIX;

private:
	static const std::string baseDirectory;

private: // �ÓI�����o�ϐ�
	// �f�o�C�X
	static ID3D12Device* device;
	// �f�X�N���v�^�T�C�Y
	static UINT descriptorHandleIncrementSize;

public: // �ÓI�����o�֐�

	/// <summary>
	/// �ÓI������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void SetDevice(ID3D12Device* device);

	/// <summary>
	/// OBJ�t�@�C�����烁�b�V������
	/// </summary>
	/// <param name="modelname">���f����</param>
	/// <param name="modelname">�G�b�W�������t���O</param>
	/// <returns>�������ꂽ���f��</returns>
	static OBJHighModel* CreateFromOBJ(const std::string& modelname, bool smoothing = false);

public: // �����o�֐�
	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	~OBJHighModel();

	/// <summary>
	/// ������
	/// </summary>
	/// <param name="modelname">���f����</param>
	/// <param name="modelname">�G�b�W�������t���O</param>
	void Initialize(const std::string& modelname, bool smoothing);

	/// <summary>
	/// �`��
	/// </summary>
	/// <param name="cmdList">���ߔ��s��R�}���h���X�g</param>
	void Draw(ID3D12GraphicsCommandList* cmdList);

private: // �����o�ϐ�
	// ���O
	std::string name;
	// ���b�V���R���e�i
	std::vector<Mesh*> meshes;
	// �}�e���A���R���e�i
	std::unordered_map<std::string, Material*> materials;
	// �f�t�H���g�}�e���A��
	Material* defaultMaterial = nullptr;
	// �f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeap;

private: // �����o�֐�
	/// <summary>
	/// ���f���ǂݍ���
	/// </summary>
	/// <param name="modelname">���f����</param>
	/// <param name="modelname">�G�b�W�������t���O</param>
	void LoadModel(const std::string& modelname, bool smoothing);

	/// <summary>
	/// �}�e���A���ǂݍ���
	/// </summary>
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// �}�e���A���o�^
	/// </summary>
	void AddMaterial(Material* material);

	/// <summary>
	/// �f�X�N���v�^�q�[�v�̐���
	/// </summary>
	void CreateDescriptorHeap();

	/// <summary>
	/// �e�N�X�`���ǂݍ���
	/// </summary>
	void LoadTextures();

};

