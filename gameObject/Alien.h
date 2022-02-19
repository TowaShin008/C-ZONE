#pragma once
#include<wrl.h>
#include<d3d12.h>
#include<DirectXMath.h>
#include<string>
#include<d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")
#include"GameObject.h"
#include"Camera.h"
#include"Input.h"
#include"ParticleManager.h"
#include"OBJCharacter.h"
#define BULLETMAXNUM 10
#define MAXENEMYLUGTIME 60

//�G�G�C���A���N���X
class Alien :public GameObject
{
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	using XMMATRIX = DirectX::XMMATRIX;

public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="arg_cmdList">�R�}���h���X�g</param>
	Alien(ID3D12GraphicsCommandList* arg_cmdList);

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	~Alien();
	// �萔�o�b�t�@�p�f�[�^�\����
	struct  DissolveConstBufferData
	{
		Vector4 color;
		XMMATRIX mat;
		float dissolveCount;
	};

	/// <summary>
	/// �I�u�W�F�N�g�̐�������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	/// <param name="position">�|�W�V����</param>
	/// <returns>�L�����N�^�[�I�u�W�F�N�g</returns>
	static Alien* Create(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList, Vector3 position = { 0.0f,0.0f,0.0f });


	/// <summary>
	/// �萔�o�b�t�@�̐���
	/// </summary>
	void CreateConstBuffer(ID3D12Device* device);

	/// <summary>
	/// ����������
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// �X�V����
	/// </summary>
	void Update(const Vector3& incrementValue)override;

	/// <summary>
	/// �`�揈��
	/// </summary>
	/// <param name="cmdList">�R�}���h���X�g</param>
	void Draw();

	/// <summary>
	/// ���f���̃Z�b�g
	/// </summary>
	/// <param name="objModel">���f��</param>
	void SetOBJModel(OBJModel* arg_objModel,OBJModel* scoreModel);

	/// <summary>
	/// ���_���W�̃Z�b�g
	/// </summary>
	/// <param name="eye">���_���W</param>
	static void SetEye(const Vector3& eye);

	/// <summary>
	/// �����_���W�̃Z�b�g
	/// </summary>
	/// <param name="target">�����_���W</param>
	static void SetTarget(const Vector3& target);

	/// <summary>
	/// �r���[�s��̍X�V����
	/// </summary>
	static void UpdateViewMatrix();

	/// <summary>
	/// �J�����̃Z�b�g
	/// </summary>
	/// <param name="camera">�J����</param>
	static void SetCamera(Camera* camera) { Alien::camera = camera; }

private:
	UINT descpriptorSize;
	static ID3D12GraphicsCommandList* cmdList;
	static ID3D12Device* device;
	ComPtr<ID3D12Resource>constBuffB0;
	ComPtr<ID3D12Resource>constBuffB1;
	ComPtr<ID3D12Resource>dissolveTexBuff;
	ComPtr<ID3D12DescriptorHeap>descHeapSRV;

	//�J�����֌W
	// 
	//�r���[�s��
	static XMMATRIX matView;
	//�v���W�F�N�V�����s��
	static XMMATRIX matProjection;
	//�J�����N���X
	static Camera* camera;

	//���f���f�[�^�i�[�p�ϐ�
	OBJModel* objModel = nullptr;
	//�e�N���X
	Alien* parent = nullptr;

	//�����o���܂ł̎���(�����̓G�ő����g�݂����Ƃ��Ɏg�p����)
	int moveLugTime = 0;

	//���݂̓����̒i�K���i�[����ϐ�
	MOVEPHASE currentPhase = MOVEPHASE::PHASE1;

	//���݂̓����̎�ނ��i�[����ϐ�
	MOVETYPE  currentType = MOVETYPE::TRIANGLE;

	bool moveEndFlag = false;
	float curveTime = 0.0f;
	float rotateX = 0.0f;
	float centerPosition = 0.0f;
	int life = 0;
	float dissolveCount = 1.0f;

	std::unique_ptr<OBJCharacter> scoreCharacter;
public:

	/// <summary>
	/// �萔�o�b�t�@�̓]��
	/// </summary>
	void TransferConstBuff();

	/// <summary>
	/// �ړ�����
	/// </summary>
	void Move();

	/// <summary>
	/// ���[�u�t�F�C�Y�̃Z�b�g
	/// </summary>
	/// <param name="movePhase">���[�u�t�F�C�Y</param>
	void SetMovePhase(const MOVEPHASE& arg_currentPhase) { currentPhase = arg_currentPhase; }

	/// <summary>
	/// ���[�u�t�F�C�Y�̎擾
	/// </summary>
	/// <returns>���[�u�t�F�C�Y</returns>
	MOVEPHASE GetMovePhase() { return currentPhase; }

	/// <summary>
	/// ���[�u�^�C�v�̃Z�b�g
	/// </summary>
	/// <param name="moveType">���[�u�^�C�v</param>
	void SetMoveType(const MOVETYPE& moveType);

	/// <summary>
	/// ���[�u�^�C�v�̎擾
	/// </summary>
	/// <returns>���[�u�^�C�v</returns>
	MOVETYPE GetMoveType() { return currentType; }

	/// <summary>
	/// �l�p�`�̌`�Ɉړ�����X�V����
	/// </summary>
	void RectAngleMove();

	/// <summary>
	/// �O�p�`�̌`�Ɉړ�����X�V����
	/// </summary>
	void TriAngleMove();

	/// <summary>
	/// �΂߂Ɉړ����鏈��
	/// </summary>
	void LeftDiagonallyMove();

	/// <summary>
	/// ������ɋȐ���`���Ĉړ����鏈��
	/// </summary>
	void UpCurveMove();

	/// <summary>
	/// �������ɋȐ���`���Ĉړ����鏈��
	/// </summary>
	void DownCurveMove();

	/// <summary>
	/// ���[�u���O�^�C���̃Z�b�g
	/// </summary>
	/// <param name="moveLugTime">���[�u���O�^�C��</param>
	void SetMoveLugTime(int arg_moveLugTime) { moveLugTime = arg_moveLugTime; }

	/// <summary>
	/// ���[�u���O�^�C���̎擾
	/// </summary>
	/// <returns>���[�u���O�^�C��</returns>
	int GetMoveLugTime() { return moveLugTime; }

	/// <summary>
	/// ���[�u�G���h�t���O�̃Z�b�g
	/// </summary>
	/// <param name="moveEndFlag"></param>
	void SetMoveEndFlag(bool arg_moveEndFlag) { moveEndFlag = arg_moveEndFlag; }

	/// <summary>
	/// ���[�u�G���h�t���O�̏���
	/// </summary>
	/// <returns></returns>
	bool GetMoveEndFlag() { return moveEndFlag; }

	/// <summary>
	/// isDeadFlag��true�ɂ���
	/// </summary>
	void IsDead();

	/// <summary>
	/// �|�W�V�����̈ړ�
	/// </summary>
	/// <param name="incrementValue">�|�W�V�����̑�����</param>
	void SetScrollIncrement(const Vector3& incrementValue);

	/// <summary>
	/// �Z���^�[�|�W�V�����̃Z�b�g
	/// </summary>
	/// <param name="centerPosition">�Z���^�[�|�W�V����</param>
	void SetCenterPos(float arg_centerPosition) { centerPosition = arg_centerPosition; }

	/// <summary>
	/// �_���[�W����
	/// </summary>
	void Damage()override;

	/// <summary>
	/// ��_���[�W����
	/// </summary>
	void CriticalDamage()override;

	/// <summary>
	/// �����蔻��
	/// </summary>
	/// <param name="otherObject">����̃I�u�W�F�N�g</param>
	/// <returns>�����������ǂ���</returns>
	bool IsCollision(GameObject* arg_otherObject)override;

	/// <summary>
	/// �X�R�A���o����
	/// </summary>
	/// <param name="incrementValue">�X�N���[����</param>
	void ScoreProcessing(Vector3 incrementValue);
};

