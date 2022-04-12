#pragma once
#include<wrl.h>
#include<d3d12.h>
#include<DirectXMath.h>
#include<string>
#include"ObjFileModel.h"
#include<d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")
#include"GameObject.h"
#include"Camera.h"
#include"Input.h"
#include"PipelineState.h"

//���[�U�[�e�N���X
class Laser :public GameObject
{
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	using XMMATRIX = DirectX::XMMATRIX;

public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="arg_cmdList">�R�}���h���X�g</param>
	/// <param name="arg_device">�f�o�C�X</param>
	Laser(ID3D12GraphicsCommandList* arg_cmdList, ID3D12Device* arg_device);

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	~Laser();

	/// <summary>
	/// �I�u�W�F�N�g�̐�������
	/// </summary>
	/// <param name="arg_device">�f�o�C�X</param>
	/// <param name="arg_cmdList">�R�}���h���X�g</param>
	/// <param name="arg_position">�|�W�V����</param>
	/// <returns>�L�����N�^�[�I�u�W�F�N�g</returns>
	static Laser* Create(ID3D12Device* arg_device, ID3D12GraphicsCommandList* arg_cmdList,const Vector3& arg_position = { 0.0f,0.0f,0.0f });


	/// <summary>
	/// �萔�o�b�t�@�̐���
	/// </summary>
	void CreateConstBuffer();

	/// <summary>
	/// ����������
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// �X�V����
	/// </summary>
	/// <param name="arg_incrementValue">�X�N���[����</param>
	void Update(const Vector3& arg_incrementValue);

	/// <summary>
	/// �`�揈��
	/// </summary>
	void Draw()override;

	/// <summary>
	/// ���f���̃Z�b�g
	/// </summary>
	/// <param name="arg_objModel">���f��</param>
	void SetOBJModel(ObjFileModel* arg_objModel) { objModel = arg_objModel; }

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
	static void SetCamera(Camera* arg_camera) { Laser::camera = arg_camera; }

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
	bool IsCollision(GameObject* arg_otherObject);

	/// <summary>
	/// �{�X�V�[�����ǂ����̃t���O
	/// </summary>
	/// <param name="arg_bossSceneFlag">�{�X�V�[���t���O</param>
	void SetBossSceneFlag(bool arg_bossSceneFlag) { bossSceneFlag = arg_bossSceneFlag; }
private:
	UINT descpriptorSize;
	static ID3D12GraphicsCommandList* cmdList;
	ComPtr<ID3D12Resource>constBuffB0;
	ComPtr<ID3D12Resource>constBuffB1;

	//�J�����֌W
	static XMMATRIX matView;
	static XMMATRIX matProjection;
	static Camera* camera;
	static ID3D12Device* device;

	ObjFileModel* objModel = nullptr;
	Laser* parent = nullptr;

	bool bossSceneFlag = false;
public:
	/// <summary>
	/// �萔�o�b�t�@�̓]��
	/// </summary>
	void TransferConstBuff();
};

