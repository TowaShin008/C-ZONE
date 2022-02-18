#pragma once
#include<DirectXMath.h>//XMFLOAT
#include<d3d12.h>
#include<dxgi1_6.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

#include<math.h>

#include<d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")

#pragma comment(lib,"dxguid.lib")

#include<DirectXTex.h>

//#include"Input.h"
#include<wrl.h>
#include<d3dx12.h>
#include<string>
#include<fstream>
#include<sstream>
#include"MineMath.h"
#include"Collision.h"
#include"PipelineState.h"
#include"WindowSize.h"
#include"OBJModel.h"
#include"OBJHighModel.h"
#include<memory>
using namespace DirectX;
#define SCREENTOP 10.0f
#define SCREENBUTTOM -10.0f
#define SCREENLEFT -17.0f
#define SCREENRIGHT 17.0f
#define SCREENBACK 50.0f
#define INVISIBLEMAXTIME 120

//�����̒i�K
enum class MOVEPHASE
{
	PHASE1,
	PHASE2,
	PHASE3,
	PHASE4,
	PHASE5,
	PHASE6,
};

//�����̃^�C�v
enum class MOVETYPE
{
	RECTANGLE,
	TRIANGLE,
	LEFTDIAGONALLY,
	UPCURVE,
	DOWNCURVE,
	STAY,
};
class GameObject
{
protected:
	using XMMATRIX = DirectX::XMMATRIX;

	enum class CHARACTERTYPE
	{
		PLAYER,
		FRIEND,
		ENEMY,
		BLOCK
	};
public:
	// �萔�o�b�t�@�p�f�[�^�\����
	struct  ConstBufferDataB0
	{
		Vector4 color;
		XMMATRIX mat;
	};

	struct ConstBufferDataB1
	{
		Vector3 ambient;
		float pad1;
		Vector3 diffuse;
		float pad2;
		Vector3 specular;
		float alpha;
	};

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	GameObject();

	/// <summary>
	/// ���z�f�X�g���N�^(���N���X�ɂ���ꍇ�͕K���L������)
	/// </summary>
	virtual ~GameObject();

	/// <summary>
	/// ����������
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// �X�V����
	/// </summary>
	virtual void Update(const Vector3& incrementValue);

	/// <summary>
	/// �`�揈��
	/// </summary>
	/// <param name="cmdList"></param>
	virtual void Draw();

	/// <summary>
	/// �|�W�V�����̎擾
	/// </summary>
	/// <returns>�|�W�V����</returns>
	const Vector3& GetPosition() { return position; }


	/// <summary>
	/// �|�W�V�����̃Z�b�g
	/// </summary>
	/// <param name="position">�|�W�V����</param>
	void SetPosition(const Vector3& arg_position) { position = arg_position; }

	/// <summary>
	/// ���[���h���W�̎擾
	/// </summary>
	/// <returns></returns>
	XMMATRIX GetWorldMatrix() { return matWorld; }

	/// <summary>
	/// ���S�t���O�擾
	/// </summary>
	/// <returns>���S�t���O</returns>
	bool GetIsDeadFlag() { return isDeadFlag; }

	/// <summary>
	/// ���S�t���O�̃Z�b�g
	/// </summary>
	/// <param name="isDeadFlag">���S�t���O</param>
	void SetIsDeadFlag(bool arg_isDeadFlag) { isDeadFlag = arg_isDeadFlag; }

	/// <summary>
	/// �X�P�[���̎擾
	/// </summary>
	/// <returns>�X�P�[��</returns>
	const Vector3& GetScale() { return scale; }

	/// <summary>
	/// �X�P�[���̃Z�b�g
	/// </summary>
	/// <param name="scale">�X�P�[��</param>
	void SetScale(const Vector3& arg_scale) { scale = arg_scale; }

	/// <summary>
	/// ��]�p�x�̎擾
	/// </summary>
	/// <returns>��]�p�x</returns>
	const Vector3& GetRotasion() { return rotation; }

	/// <summary>
	/// ��]�p�x�̃Z�b�g
	/// </summary>
	/// <param name="rotation"></param>
	void SetRotation(const Vector3& arg_rotation) { rotation = arg_rotation; }

	/// <summary>
	/// �J���[�̎擾
	/// </summary>
	/// <returns>�J���[</returns>
	const Vector4& GetColor() { return color; }

	/// <summary>
	/// �J���[�̃Z�b�g
	/// </summary>
	/// <param name="color">�J���[</param>
	void SetColor(const Vector4& arg_color) { color = arg_color; }
protected:
	Vector3 position = { 0.0f,0.0f,0.0f };
	XMMATRIX matWorld;
	Vector3 scale = { 1.0f,1.0f,1.0f };
	Vector3 rotation = { 0.0f,0.0f,0.0f };
	Vector4 color = { 1.0f,1.0f,1.0f,1.0f };
	bool isDeadFlag = false;
	Vector3 velocity = { 0.0f,0.0f,0.0f };
	Vector3 speed = { 1.0f,1.0f,1.0f };
	bool motionBlurFlag = false;
	float collisionRadius = 0.5f;
	CHARACTERTYPE characterType;
	int hp = 1;
	int invisibleTime = 0;
public:
	/// <summary>
	/// �^������
	/// </summary>
	/// <param name="velocity"></param>
	void SetVelocity(const Vector3& arg_velocity) { velocity = arg_velocity; }
	const Vector3& GetVelocity() { return velocity; }
	void SetSpeed(const Vector3& arg_speed) { speed = arg_speed; }
	const Vector3& GetSpeed() { return speed; }

	int GetHp() { return hp; };

	/// <summary>
	/// ���[�V�����u���[�̓K�p
	/// </summary>
	/// <param name="motionBlurFlag">���[�V�����u���[�t���O</param>
	void SetMotionBlurFlag(bool arg_motionBlurFlag) { motionBlurFlag = arg_motionBlurFlag; }

	/// <summary>
	/// �����蔻��̔��a���Z�b�g
	/// </summary>
	/// <param name="arg_collisionRadius"></param>
	void SetCollisionRadius(float arg_collisionRadius) { collisionRadius = arg_collisionRadius; }

	/// <summary>
	/// �����蔻��̔��a���Z�b�g
	/// </summary>
	/// <param name="arg_collisionRadius"></param>
	float GetCollisionRadius() { return collisionRadius; }

	void SetCharacterType(CHARACTERTYPE arg_characterType) { characterType = arg_characterType; }

	CHARACTERTYPE GetCharacterType() { return characterType; }

	/// <summary>
	/// �����蔻��
	/// </summary>
	/// <param name="otherObject">����̃I�u�W�F�N�g</param>
	/// <returns>�����������ǂ���</returns>
	virtual bool IsCollision(GameObject* arg_otherObject);

	/// <summary>
	/// �_���[�W����
	/// </summary>
	virtual void Damage();

	/// <summary>
	/// ��_���[�W����
	/// </summary>
	virtual void CriticalDamage();
};

