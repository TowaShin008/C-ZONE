#pragma once
#include<d3d12.h>
#include<string>
#include"OBJModel.h"
#include<fstream>
#include<sstream>
#include<d3d12.h>
#include<memory>

class OBJLoader
{//�V���O���g���p�^�[���Ŏ���
public:
	/// <summary>
	/// ��������̎��̂�Ԃ��֐�
	/// </summary>
	/// <returns>���g�̎���</returns>
	static OBJLoader* GetInstance()
	{
		return objLoader;
	}

	/// <summary>
	/// ��������
	/// </summary>
	static void Create();

	/// <summary>
	/// �폜����
	/// </summary>
	static void Destroy();

	/// <summary>
	/// ���f���̃��[�h
	/// </summary>
	/// <param name="modelName">���f����</param>
	/// <returns>���f��</returns>
	OBJModel* LoadModel(const std::string& modelName);

	/// <summary>
	/// �}�e���A���ǂݍ��ݏ���
	/// </summary>
	/// <param name="directoryPath">�f�B���N�g���p�X</param>
	/// <param name="filename">�t�@�C���l�[��</param>
	/// <param name="objModel">�R�Â��������f���N���X</param>
	void LoadMaterial(const std::string& directoryPath, const std::string& filename, OBJModel* objModel);
	
	/// <summary>
	/// OBJ�t�@�C������̃f�[�^�ǂݍ���
	/// </summary>
	/// <param name="modelName">���f���l�[��</param>
	/// <param name="objModel">�R�Â��������f���N���X</param>
	void CreateCharacter(const std::string& modelName, OBJModel* objModel);
	
	/// <summary>
	/// �f�o�C�X�̃Z�b�g���� �K��Create()�֐��̌�ɋL�q����
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	void SetDevice(ID3D12Device* device) { this->device = device; };
private:
	ID3D12Device* device = nullptr;
protected:
	/// <summary>
	/// ���g�̕ϐ�
	/// </summary>
	static OBJLoader* objLoader;
	/// <summary>
	/// �����I�ȃf�t�H���g��`
	/// </summary>
	OBJLoader() = default;
	~OBJLoader() = default;
};

