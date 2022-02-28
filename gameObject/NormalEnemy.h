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
#include"Bullet.h"
#include"ParticleManager.h"
#include"OBJCharacter.h"
#define BULLETMAXNUM 10

//�ʏ�G�N���X
class NormalEnemy :public GameObject
{
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	using XMMATRIX = DirectX::XMMATRIX;

public:
	//�R���X�g���N�^
	NormalEnemy(ID3D12GraphicsCommandList* arg_cmdList);

	//�f�X�g���N�^
	~NormalEnemy();

	/// <summary>
	/// �I�u�W�F�N�g�̐�������
	/// </summary>
	/// <param name="arg_device">�f�o�C�X</param>
	/// <param name="arg_cmdList">�R�}���h���X�g</param>
	/// <param name="arg_position">�|�W�V����</param>
	/// <returns>�L�����N�^�[�I�u�W�F�N�g</returns>
	static NormalEnemy* Create(ID3D12Device* arg_device, ID3D12GraphicsCommandList* arg_cmdList,const Vector3& arg_position = { 0.0f,0.0f,0.0f });


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
	/// ���f���̃Z�b�g
	/// </summary>
	/// <param name="arg_objModel">���f��</param>
	/// <param name="bulletModel">�e�ۃ��f��</param>
	/// <param name="arg_scoreModel">�X�R�A���f��</param>
	void SetOBJModel(OBJHighModel* arg_objModel, OBJModel* bulletModel,OBJModel* arg_scoreModel);

	/// <summary>
	/// �e�ۃ��f���̃Z�b�g
	/// </summary>
	/// <param name="arg_bulletModel">���f��</param>
	void SetBulletModel(OBJModel* arg_bulletModel);

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
	static void SetCamera(Camera* arg_camera) { NormalEnemy::camera = arg_camera; }

	/// <summary>
	/// �e�����T�C�Y����������
	/// </summary>
	/// <param name="arg_device"></param>
	void AttachBullet(ID3D12Device* arg_device);

private:
	UINT descpriptorSize;
	static ID3D12GraphicsCommandList* cmdList;
	ComPtr<ID3D12Resource>constBuffB0;
	ComPtr<ID3D12Resource>constBuffB1;

	//�J�����֌W
	// 
	//�r���[�s��
	static XMMATRIX matView;
	//�v���W�F�N�V�����s��
	static XMMATRIX matProjection;
	//�J�����N���X
	static Camera* camera;

	//���f���f�[�^�i�[�p�ϐ�
	OBJHighModel* objModel = nullptr;
	//�e�N���X
	NormalEnemy* parent = nullptr;
	//�e
	std::vector<Bullet*>bullets;
	//�Ĕ��˂���܂ł̎���
	int shotLugTime = 0;

	//�����o���܂ł̎���(�����̓G�ő����g�݂����Ƃ��Ɏg�p����)
	int moveLugTime = 0;

	//���݂̓����̒i�K���i�[����ϐ�
	MOVEPHASE currentPhase = MOVEPHASE::PHASE1;

	//���݂̓����̎�ނ��i�[����ϐ�
	MOVETYPE  currentType  = MOVETYPE::TRIANGLE;

	bool moveEndFlag = false;
	float curveTime = 0.0f;
	float rotateX = 0.0f;
	bool shotFlag = true;
	float centerPosition = 0.0f;
	int life = 0;
	bool deathParticleFlag = false;

	std::unique_ptr<ParticleManager> deathParticle;
	std::unique_ptr<OBJCharacter> scoreCharacter;
public:
	/// <summary>
	/// �萔�o�b�t�@�̓]��
	/// </summary>
	void TransferConstBuff();

	/// <summary>
	/// �e�ۂ̔z��̎擾
	/// </summary>
	/// <returns>�e�ۂ̔z��</returns>
	std::vector<Bullet*> GetBullets() { return bullets; }

	/// <summary>
	/// �ړ�����
	/// </summary>
	void Move();

	/// <summary>
	/// ���[�u�t�F�C�Y�̃Z�b�g
	/// </summary>
	/// <param name="arg_movePhase">���[�u�t�F�C�Y</param>
	void SetMovePhase(const MOVEPHASE& arg_movePhase) { currentPhase = arg_movePhase; }

	/// <summary>
	/// ���[�u�t�F�C�Y�̎擾
	/// </summary>
	/// <returns>���[�u�t�F�C�Y</returns>
	MOVEPHASE GetMovePhase() { return currentPhase; }

	/// <summary>
	/// ���[�u�^�C�v�̃Z�b�g
	/// </summary>
	/// <param name="arg_moveType">���[�u�^�C�v</param>
	void SetMoveType(const MOVETYPE& arg_moveType);

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
	/// <param name="arg_moveLugTime">���[�u���O�^�C��</param>
	void SetMoveLugTime(int arg_moveLugTime) { moveLugTime = arg_moveLugTime; }

	/// <summary>
	/// ���[�u���O�^�C���̎擾
	/// </summary>
	/// <returns>���[�u���O�^�C��</returns>
	int GetMoveLugTime() { return moveLugTime; }

	/// <summary>
	/// ���[�u�G���h�t���O�̃Z�b�g
	/// </summary>
	/// <param name="arg_moveEndFlag">���[�u�G���h�t���O</param>
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
	/// �ˌ��ł��邩�̃t���O���Z�b�g����
	/// </summary>
	/// <param name="arg_shotFlag">�ˌ��ł��邩�ǂ����̃t���O</param>
	void SetShotFlag(bool arg_shotFlag) { shotFlag = arg_shotFlag; }

	/// <summary>
	/// �|�W�V�����̈ړ�
	/// </summary>
	/// <param name="arg_incrementValue">�|�W�V�����̑�����</param>
	void SetScrollIncrement(const Vector3& arg_incrementValue);

	/// <summary>
	/// �Z���^�[�|�W�V�����̃Z�b�g
	/// </summary>
	/// <param name="arg_centerPosition">�Z���^�[�|�W�V����</param>
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
	/// ���S���o�p�̃p�[�e�B�N���̃Z�b�g
	/// </summary>
	/// <param name="arg_deathParticle">�p�[�e�B�N��</param>
	void SetDeathParticleManager(ParticleManager* arg_deathParticle) { deathParticle.reset(arg_deathParticle); }

	/// <summary>
	/// ���S���o�̔����t���O
	/// </summary>
	/// <param name="arg_deathParticle">���S�t���O�̔����t���O</param>
	void SetDeathParticleFlag(bool arg_deathParticleFlag) { deathParticleFlag = arg_deathParticleFlag; }

	/// <summary>
	/// ���S�p�[�e�B�N������
	/// </summary>
	void DeathParticleProcessing();

	/// <summary>
	/// �����蔻��
	/// </summary>
	/// <param name="arg_otherObject">����̃I�u�W�F�N�g</param>
	/// <returns>�����������ǂ���</returns>
	bool IsCollision(GameObject* arg_otherObject)override;

	/// <summary>
	/// �e�̔��ˏ���
	/// </summary>
	void ShotBullet();

	/// <summary>
	/// �X�R�A���o����
	/// </summary>
	/// <param name="arg_incrementValue">�X�N���[����</param>
	void ScoreProcessing(const Vector3& arg_incrementValue);
};

