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
#include"HomingBullet.h"
#include"NormalEnemy.h"
#include"ParticleManager.h"
#include"Block.h"
#include"SquidEye.h"
#define BULLETMAXNUM 10
#define MAXENEMYLUGTIME 60
#define LANCHMAXTIME 800
#define TENTACLESNUM 40
//�{�X�N���X
class Squid :public GameObject
{
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	using XMMATRIX = DirectX::XMMATRIX;

public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="arg_cmdList">�R�}���h���X�g</param>
	Squid(ID3D12GraphicsCommandList* arg_cmdList);

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	~Squid();

	/// <summary>
	/// �I�u�W�F�N�g�̐�������
	/// </summary>
	/// <param name="arg_device">�f�o�C�X</param>
	/// <param name="arg_cmdList">�R�}���h���X�g</param>
	/// <param name="arg_position">�|�W�V����</param>
	/// <returns>�L�����N�^�[�I�u�W�F�N�g</returns>
	static Squid* Create(ID3D12Device* arg_device, ID3D12GraphicsCommandList* arg_cmdList,const Vector3& arg_position = { 0.0f,0.0f,0.0f });


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
	/// <param name="arg_playerPosition">�v���C���[�I�u�W�F�N�g</param>
	void Update(const Vector3& arg_incrementValue, const Vector3& arg_playerPosition);

	/// <summary>
	/// �`�揈��
	/// </summary>
	void Draw();

	/// <summary>
	/// ���f���̃Z�b�g
	/// </summary>
	/// <param name="arg_eyeModel">�ڂ̃��f��</param>
	/// <param name="arg_bodyModel">�̂̃��f��</param>
	void SetOBJModel(OBJModel* arg_eyeModel, OBJModel* arg_bodyModel);

	/// <summary>
	/// �̂̃��f���̃Z�b�g
	/// </summary>
	/// <param name="arg_bulletModel">���f��</param>
	void SetBodyModel(OBJModel* arg_bodyModel);

	/// <summary>
	/// �ڂ̃��f���̃Z�b�g
	/// </summary>
	/// <param name="arg_eyeModel">���f��</param>
	void SetEyeModel(OBJModel* arg_eyeModel);

	/// <summary>
	/// �G�胂�f���̃Z�b�g
	/// </summary>
	/// <param name="arg_tentacleModel">���f��</param>
	void SetTentaclesModel(OBJModel* arg_tentacleModel);

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
	static void SetCamera(Camera* arg_camera) { Squid::camera = arg_camera; }

	/// <summary>
	/// �̂����T�C�Y����������
	/// </summary>
	/// <param name="arg_device">�f�o�C�X</param>
	void AttachBody(ID3D12Device* arg_device);

	/// <summary>
	/// �G������T�C�Y����������
	/// </summary>
	/// <param name="arg_device">�f�o�C�X</param>
	void AttachTentacles(ID3D12Device* arg_device);

	/// <summary>
	/// �ڂ����T�C�Y����������
	/// </summary>
	/// <param name="arg_device">�f�o�C�X</param>
	void AttachEye(ID3D12Device* arg_device);
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

	//�e�N���X
	Squid* parent = nullptr;

	//�����o�����߂̎���(�����̓G�ő����g�݂����Ƃ��Ɏg�p����)
	int moveLugTime = 0;

	//���݂̓����̒i�K���i�[����ϐ�
	MOVEPHASE currentPhase = MOVEPHASE::PHASE1;

	//���݂̓����̎�ނ��i�[����ϐ�
	MOVETYPE  currentType = MOVETYPE::TRIANGLE;

	OBJModel* bodyModel = nullptr;
	std::vector<std::vector<Block*>>bodyBlocks;
	std::vector<Block*>tentaclesBlocks;
	SquidEye* leftEye = nullptr;
	SquidEye* rightEye = nullptr;

	bool moveEndFlag = false;
	float rotateX = 0.0f;
	bool shotFlag = true;
	float centerPosition = 0.0f;
	int hp = 400;

	std::unique_ptr<ParticleManager> deathParticle;
	bool deathParticleFlag = false;

	std::vector<std::vector<short>> map;

	MOVEPHASE attackPhase = MOVEPHASE::PHASE1;

	float attackFlag = false;
	int rnd = 0;
	float tentacleAngle = 0.0f;
	float bodyAngle = 0.0f;
	float homePositionX = 20.0f;
	bool upTentacleAttackFlag = false;
	float eyeRotationCount = 0.0f;
	float eyeScaleCount = 0;
public:
	/// <summary>
	/// �萔�o�b�t�@�̓]��
	/// </summary>
	void TransferConstBuff();

	/// <summary>
	/// �ړ�����
	/// </summary>
	/// <param name="arg_playerPosition">�v���C���[�|�W�V����</param>
	void Move(const Vector3& arg_playerPosition);

	/// <summary>
	/// ���[�u�t�F�C�Y�̃Z�b�g
	/// </summary>
	/// <param name="arg_movePhase">���[�u�t�F�C�Y</param>
	void SetMovePhase(const MOVEPHASE& arg_currentPhase) { currentPhase = arg_currentPhase; }

	/// <summary>
	/// ���[�u�t�F�C�Y�̎擾
	/// </summary>
	/// <returns>���[�u�t�F�C�Y</returns>
	MOVEPHASE GetMovePhase() { return currentPhase; }

	/// <summary>
	/// ���[�u�^�C�v�̎擾
	/// </summary>
	/// <returns>���[�u�^�C�v</returns>
	MOVETYPE GetMoveType() { return currentType; }

	/// <summary>
	/// ������x�i��Œ�~���鏈������
	/// </summary>
	/// <param name="arg_playerPosition">�v���C���[�|�W�V����</param>
	void StayMove(const Vector3& arg_playerPosition);

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
	/// <param name="moveEndFlag">���[�u�G���h�t���O</param>
	void SetMoveEndFlag(bool arg_moveEndFlag) { moveEndFlag = arg_moveEndFlag; }

	/// <summary>
	/// ���[�u�G���h�t���O�̏���
	/// </summary>
	/// <returns>���[�u�G���h�t���O</returns>
	bool GetMoveEndFlag() { return moveEndFlag; }

	/// <summary>
	/// isDeadFlag��true�ɂ���
	/// </summary>
	void IsDead();

	/// <summary>
	/// �ˌ��ł��邩�̃t���O���Z�b�g����
	/// </summary>
	/// <param name="shotFlag">�ˌ��ł��邩�ǂ����̃t���O</param>
	void SetShotFlag(bool arg_shotFlag) { shotFlag = arg_shotFlag; }

	/// <summary>
	/// �|�W�V�����̈ړ�
	/// </summary>
	/// <param name="arg_incrementValue">�|�W�V�����̑�����</param>
	void SetScrollIncrement(const Vector3& arg_incrementValue);

	/// <summary>
	/// �Z���^�[�|�W�V�����̃Z�b�g
	/// </summary>
	/// <param name="arg_centerPosition">��ʂ̒��S���W</param>
	void SetCenterPos(float arg_centerPosition) { centerPosition = arg_centerPosition; }

	/// <summary>
	/// HP�̃Z�b�g
	/// </summary>
	/// <param name="arg_hp">HP</param>
	void SetHP(int arg_hp) { hp = arg_hp; }

	/// <summary>
	/// HP�̎擾
	/// </summary>
	/// <returns>HP</returns>
	int GetHP() { return hp; }

	/// <summary>
	/// �_���[�W����
	/// </summary>
	void Damage()override;

	/// <summary>
	/// ��_���[�W����
	/// </summary>
	void CriticalDamage()override;

	/// <summary>
	/// �_���[�W���o
	/// </summary>
	void DamageEffect();

	/// <summary>
	/// ���S���o�p�̃p�[�e�B�N���̃Z�b�g
	/// </summary>
	/// <param name="arg_deathParticle">�p�[�e�B�N��</param>
	void SetDeathParticleManager(ParticleManager* arg_deathParticle) { deathParticle.reset(arg_deathParticle); }

	/// <summary>
	/// ���S���o�̔����t���O
	/// </summary>
	/// <param name="deathParticle">���S�t���O�̔����t���O</param>
	void SetDeathParticleFlag(bool arg_deathParticleFlag) { deathParticleFlag = arg_deathParticleFlag; }

	/// <summary>
	/// ���S�p�[�e�B�N������
	/// </summary>
	void DeathParticleProcessing();

	/// <summary>
	/// �����蔻��
	/// </summary>
	/// <param name="otherObject">����̃I�u�W�F�N�g</param>
	/// <returns>�����������ǂ���</returns>
	bool IsCollision(GameObject* arg_otherObject);

	SquidEye* GetLeftSquidEye() { return leftEye; }

	SquidEye* GetRightSquidEye() { return rightEye; }

	std::vector<Block*> GetTentacle() { return tentaclesBlocks; }

	/// <summary>
	/// �ォ��̐G��U��
	/// </summary>
	void UpTentacleAttack();

	/// <summary>
	/// ������̐G��U��
	/// </summary>
	void DownTentacleAttack();

	/// <summary>
	/// �A�^�b�N�t���O�̃Z�b�g
	/// </summary>
	/// <param name="arg_attackFlag">�A�^�b�N�t���O</param>
	void SetAttackFlag(bool arg_attackFlag) { attackFlag = arg_attackFlag; }

	/// <summary>
	/// �G��̂��˂菈��
	/// </summary>
	void TentacleMove();

	/// <summary>
	/// ���ɕ����Ă���悤�ȉ��o�̏���
	/// </summary>
	void FloatMove();

	/// <summary>
	/// �ċz�̉��o����
	/// </summary>
	void BreathMove();
};

