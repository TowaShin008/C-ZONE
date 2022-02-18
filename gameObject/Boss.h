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
#define BULLETMAXNUM 10
#define MAXENEMYLUGTIME 60
#define LANCHMAXTIME 800
//�{�X�N���X
class Boss :public GameObject
{
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	using XMMATRIX = DirectX::XMMATRIX;

public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="arg_cmdList">�R�}���h���X�g</param>
	Boss(ID3D12GraphicsCommandList* arg_cmdList);

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	~Boss();

	/// <summary>
	/// �I�u�W�F�N�g�̐�������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	/// <param name="position">�|�W�V����</param>
	/// <returns>�L�����N�^�[�I�u�W�F�N�g</returns>
	static Boss* Create(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList,Vector3 position = { 0.0f,0.0f,0.0f });


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
	void Update(const Vector3& incrementValue, const Vector3& playerPosition);

	/// <summary>
	/// �`�揈��
	/// </summary>
	/// <param name="cmdList">�R�}���h���X�g</param>
	void Draw();

	/// <summary>
	/// ���f���̃Z�b�g
	/// </summary>
	/// <param name="objModel">���f��</param>
	void SetOBJModel(OBJHighModel* arg_objModel, OBJHighModel* bulletModel);

	/// <summary>
	/// �e�ۃ��f���̃Z�b�g
	/// </summary>
	/// <param name="bulletModel">���f��</param>
	void SetBulletModel(OBJHighModel* bulletModel);

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
	static void SetCamera(Camera* camera) { Boss::camera = camera; }

	/// <summary>
	/// �G�̒e�����T�C�Y����������
	/// </summary>
	void AttachBullet(ID3D12Device* device);

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
	Boss* parent = nullptr;

	//�Ĕ��˂���܂ł̎���
	int shotLugTime = MAXENEMYLUGTIME;

	//�����o�����߂̎���(�����̓G�ő����g�݂����Ƃ��Ɏg�p����)
	int moveLugTime = 0;

	//���݂̓����̒i�K���i�[����ϐ�
	MOVEPHASE currentPhase = MOVEPHASE::PHASE1;

	//���݂̓����̎�ނ��i�[����ϐ�
	MOVETYPE  currentType = MOVETYPE::TRIANGLE;

	bool moveEndFlag = false;
	float curveTime = 0.0f;
	float rotateX = 0.0f;
	bool shotFlag = true;
	float centerPosition = 0.0f;
	bool lanchFlag = false;
	int lanchLugTime = 0;
	int hp = 400;

	std::unique_ptr<ParticleManager> deathParticle;
	HomingBullet* bigBullet = nullptr;
	bool deathParticleFlag = false;
public:

	/// <summary>
	/// �萔�o�b�t�@�̓]��
	/// </summary>
	void TransferConstBuff();
	/// <summary>
	/// �`���[�W�e�̎擾
	/// </summary>
	/// <returns>�`���[�W�e</returns>
	HomingBullet* GetBigBullet() { return bigBullet; }

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
	/// ������x�i��Œ�~���鏈������
	/// </summary>
	void StayMove();

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
	/// ���˂܂ł̃��O�^�C��
	/// </summary>
	/// <param name="arg_shotLugTime">���O�^�C��</param>
	void SetShotLugTime(int arg_shotLugTime) { shotLugTime = arg_shotLugTime; }

	/// <summary>
	/// �|�W�V�����̈ړ�
	/// </summary>
	/// <param name="incrementValue">�|�W�V�����̑�����</param>
	void SetScrollIncrement(const Vector3& incrementValue);

	/// <summary>
	/// �Z���^�[�|�W�V�����̃Z�b�g
	/// </summary>
	/// <param name="centerPosition"></param>
	void SetCenterPos(float arg_centerPosition) { centerPosition = arg_centerPosition; }

	/// <summary>
	/// �G�̔��˃t���O�̎擾
	/// </summary>
	/// <returns>���˃t���O</returns>
	bool GetLanchFlag() { return lanchFlag; }

	/// <summary>
	/// �G�̔��˃t���O�̃Z�b�g
	/// </summary>
	/// <param name="lanchFlag">�G�̔��˃t���O�̃Z�b�g</param>
	void SetLanchFlag(bool arg_lanchFlag) { lanchFlag = arg_lanchFlag; }

	/// <summary>
	/// HP�̃Z�b�g
	/// </summary>
	/// <param name="hp">HP</param>
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
	/// <param name="deathParticle">�p�[�e�B�N��</param>
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

	/// <summary>
	/// �e�̔��ˏ���
	/// </summary>
	/// <param name="incrementValue">�|�W�V�����̑�����</param>
	/// <param name="playerPosition">�v���C���[�̃|�W�V����</param>
	void ShotBullet(const Vector3& incrementValue, const Vector3& playerPosition);
};

