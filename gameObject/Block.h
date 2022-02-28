#pragma once
#include<wrl.h>
#include<d3d12.h>
#include<DirectXMath.h>
#include<string>
#include"OBJModel.h"
#include<d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")
#include"GameObject.h"
#include"Camera.h"
#include"Input.h"
#include"Bullet.h"

//�u���b�N(��Q��)�N���X
class Block:public GameObject
{
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	using XMMATRIX = DirectX::XMMATRIX;

public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="arg_cmdList">�R�}���h���X�g</param>
	Block(ID3D12GraphicsCommandList* arg_cmdList);

	//�f�X�g���N�^
	~Block();

	/// <summary>
	/// �I�u�W�F�N�g�̐�������
	/// </summary>
	/// <param name="arg_device">�f�o�C�X</param>
	/// <param name="arg_cmdList">�R�}���h���X�g</param>
	/// <param name="arg_position">�|�W�V����</param>
	/// <returns>�L�����N�^�[�I�u�W�F�N�g</returns>
	static Block* Create(ID3D12Device* arg_device, ID3D12GraphicsCommandList* arg_cmdList,const Vector3& arg_position = { 0.0f,0.0f,0.0f });

	/// <summary>
	/// �萔�o�b�t�@�̐���
	/// </summary>
	/// <param name="arg_device">�f�o�C�X</param>
	void CreateConstBuffer(ID3D12Device* arg_device);

	/// <summary>
	/// ����������
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// �X�V����
	/// </summary>
	/// <param name="arg_incrementValue">�X�N���[����</param>
	void Update(const Vector3& arg_incrementValue)override;

	/// <summary>
	/// �`�揈��
	/// </summary>
	void Draw();

	/// <summary>
	/// �萔�o�b�t�@�̓]��
	/// </summary>
	void TransferConstBuff();

	/// <summary>
	/// ���f���̃Z�b�g
	/// </summary>
	/// <param name="arg_objModel">���f��</param>
	void SetOBJModel(OBJModel* arg_objModel);

	/// <summary>
	/// ���_���W�̃Z�b�g
	/// </summary>
	/// <param name="arg_eye">���_���W</param>
	static void SetEye(const Vector3& arg_eye);

	/// <summary>
	/// �����_���W�̃Z�b�g
	/// </summary>
	/// <param name="arg_target">�����_���W</param>
	static void SetTarget(const Vector3& arg_target);

	/// <summary>
	/// �r���[�s��̍X�V����
	/// </summary>
	static void UpdateViewMatrix();

	/// <summary>
	/// �J�����̃Z�b�g
	/// </summary>
	/// <param name="arg_camera">�J����</param>
	static void SetCamera(Camera* arg_camera) { Block::camera = arg_camera; }
	
	/// <summary>
	/// �X�e�[�W�u���b�N���̃t���O�̃Z�b�g
	/// </summary>
	/// <param name="arg_stageblockFlag">�X�e�[�W�u���b�N�t���O</param>
	void SetStageBlockFlag(bool arg_stageblockFlag) { stageblockFlag = arg_stageblockFlag; }

	/// <summary>
	/// �e�I�u�W�F�N�g�̃Z�b�g
	/// </summary>
	/// <param name="arg_parent">�e�I�u�W�F�N�g</param>
	void SetParent(Block* arg_parent) { parent = arg_parent; }

private:
	UINT descpriptorSize;
	static ID3D12GraphicsCommandList* cmdList;
	ComPtr<ID3D12Resource>constBuffB0;
	ComPtr<ID3D12Resource>constBuffB1;

	//�r���[�s��
	static XMMATRIX matView;
	//�v���W�F�N�V�����s��
	static XMMATRIX matProjection;
	//�J�����N���X
	static Camera* camera;

	//���f���f�[�^�i�[�p�ϐ�
	OBJModel* objModel = nullptr;
	//�e�N���X
	Block* parent = nullptr;

	float centerPos = 0.0f;
	bool stageblockFlag = true;

public:

	/// <summary>
	/// isDeadFlag��true�ɂ���
	/// </summary>
	void IsDead();

	/// <summary>
	/// �|�W�V�����̈ړ�
	/// </summary>
	/// <param name="arg_incrementValue">�|�W�V�����̑�����</param>
	void SetScrollIncrement(const Vector3& arg_incrementValue);

	/// <summary>
	/// �����蔻��
	/// </summary>
	/// <param name="arg_otherObject">����̃I�u�W�F�N�g</param>
	/// <returns>�����������ǂ���</returns>
	bool IsCollision(GameObject* arg_otherObject)override;
};

