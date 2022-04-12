#pragma once
#include<DirectXMath.h>

using XMFLOAT2 = DirectX::XMFLOAT2;
using XMFLOAT3 = DirectX::XMFLOAT3;
using XMFLOAT4 = DirectX::XMFLOAT4;

#pragma region Vector

//�x�N�g��
struct Vector4 : public XMFLOAT4
{
	//�R���X�g���N�^
	Vector4();                                //��x�N�g���Ƃ���
	Vector4(float x, float y, float z, float w);       //x����,y����,z���� ���w�肵�Ă̐���

	//�P�����Z�q�̃I�[�o�[���[�h
	Vector4 operator+() const;
	Vector4 operator-() const;

	//������Z�q�I�[�o�[���[�h
	Vector4& operator+=(const Vector4& v);
	Vector4& operator-=(const Vector4& v);
	Vector4& operator*=(float s);
	Vector4& operator/=(float s);
};

//2�����Z�q�I�[�o�[���[�h
//�������Ȉ����̃p�^�[���ɑΉ�(�����̏���)���邽�߁A�ȉ��̂悤�ɏ������Ă���
const Vector4 operator+(const Vector4& v1, const Vector4& v2);
const Vector4 operator-(const Vector4& v1, const Vector4& v2);
const Vector4 operator*(const Vector4& v, float s);
const Vector4 operator*(float s, const Vector4& v);
const Vector4 operator/(const Vector4& v, float s);


struct Vector3 : public XMFLOAT3
{

	//�R���X�g���N�^
	Vector3();                                //��x�N�g���Ƃ���
	Vector3(float x, float y, float z);       //x����,y����,z���� ���w�肵�Ă̐���

	//�����o�֐�

	/// <summary>
	/// �m���������߂�
	/// </summary>
	/// <returns>�m����</returns>
	float length() const;

	/// <summary>
	/// ���K��
	/// </summary>
	/// <returns>�v�Z����</returns>
	Vector3& normalize();

	/// <summary>
	/// ����
	/// </summary>
	/// <param name="v">�x�N�g��</param>
	/// <returns>�v�Z����</returns>
	float dot(const Vector3& v) const;

	/// <summary>
	/// �O��
	/// </summary>
	/// <param name="v">�x�N�g��</param>
	/// <returns>�v�Z����</returns>
	Vector3 cross(const Vector3& v)const;      //�O�ς����߂�

	//�P�����Z�q�̃I�[�o�[���[�h
	Vector3 operator+() const;
	Vector3 operator-() const;

	//������Z�q�I�[�o�[���[�h
	Vector3& operator+=(const Vector3& v);
	Vector3& operator-=(const Vector3& v);
	Vector3& operator*=(const Vector3& v);
	Vector3& operator*=(float s);
	Vector3& operator/=(float s);
};

//2�����Z�q�I�[�o�[���[�h
//�������Ȉ����̃p�^�[���ɑΉ�(�����̏���)���邽�߁A�ȉ��̂悤�ɏ������Ă���
const Vector3 operator+(const Vector3& v1, const Vector3& v2);
const Vector3 operator-(const Vector3& v1, const Vector3& v2);
const Vector3 operator*(const Vector3& v1, const Vector3& v2);
const Vector3 operator*(const Vector3& v, float s);
const Vector3 operator*(float s, const Vector3& v);
const Vector3 operator/(const Vector3& v, float s);

//��Ԋ֐�

/// <summary>
/// ���`���(�ꎟ�֐����)
/// </summary>
/// <param name="start">�X�^�[�g�ʒu</param>
/// <param name="end">�S�[���ʒu</param>
/// <param name="t">��Ԏ���</param>
/// <returns>���Ԃɉ������ʒu</returns>
const Vector3 lerp(const Vector3& start, const Vector3& end, const float t);

/// <summary>
/// ease-In���
/// </summary>
/// <param name="start">�X�^�[�g�ʒu</param>
/// <param name="end">�S�[���ʒu</param>
/// <param name="t">��Ԏ���</param>
/// <returns>���Ԃɉ������ʒu</returns>
const Vector3 easeIn(const Vector3& start, const Vector3& end, const float t);

/// <summary>
/// ease-Out���
/// </summary>
/// <param name="start">�X�^�[�g�ʒu</param>
/// <param name="end">�S�[���ʒu</param>
/// <param name="t">��Ԏ���</param>
/// <returns>���Ԃɉ������ʒu</returns>
const Vector3 easeOut(const Vector3& start, const Vector3& end, const float t);

/// <summary>
/// easeInOut���
/// </summary>
/// <param name="start">�X�^�[�g�ʒu</param>
/// <param name="end">�S�[���ʒu</param>
/// <param name="t">��Ԏ���</param>
/// <returns>���Ԃɉ������ʒu</returns>
const Vector3 easeInOut(const Vector3& start, const Vector3& end, const float t);


struct Vector2 : public XMFLOAT2
{
	//�R���X�g���N�^
	Vector2();                                //��x�N�g���Ƃ���
	Vector2(float x, float y);       //x����,y����,z���� ���w�肵�Ă̐���

	//�����o�֐�
	float length() const;                    //�m����(����)�����߂�
	Vector2& normalize();                    //���K������
	float dot(const Vector2& v) const;       //���ς����߂�
	float cross(const Vector2& v)const;      //�O�ς����߂�

	//�P�����Z�q�̃I�[�o�[���[�h
	Vector2 operator+() const;
	Vector2 operator-() const;

	//������Z�q�I�[�o�[���[�h
	Vector2& operator+=(const Vector2& v);
	Vector2& operator-=(const Vector2& v);
	Vector2& operator*=(float s);
	Vector2& operator/=(float s);
};

//2�����Z�q�I�[�o�[���[�h
//�������Ȉ����̃p�^�[���ɑΉ�(�����̏���)���邽�߁A�ȉ��̂悤�ɏ������Ă���
const Vector2 operator+(const Vector2& v1, const Vector2& v2);
const Vector2 operator-(const Vector2& v1, const Vector2& v2);
const Vector2 operator*(const Vector2& v, float s);
const Vector2 operator*(float s, const Vector2& v);
const Vector2 operator/(const Vector2& v, float s);
//��Ԋ֐�

/// <summary>
/// ���`���(�ꎟ�֐����)
/// </summary>
/// <param name="start">�X�^�[�g�ʒu</param>
/// <param name="end">�S�[���ʒu</param>
/// <param name="t">��Ԏ���</param>
/// <returns>���Ԃɉ������ʒu</returns>
const Vector2 lerp(const Vector2& start, const Vector2& end, const float t);

/// <summary>
/// ease-In���
/// </summary>
/// <param name="start">�X�^�[�g�ʒu</param>
/// <param name="end">�S�[���ʒu</param>
/// <param name="t">��Ԏ���</param>
/// <returns>���Ԃɉ������ʒu</returns>
const Vector2 easeIn(const Vector2& start, const Vector2& end, const float t);

/// <summary>
/// ease-Out���
/// </summary>
/// <param name="start">�X�^�[�g�ʒu</param>
/// <param name="end">�S�[���ʒu</param>
/// <param name="t">��Ԏ���</param>
/// <returns>���Ԃɉ������ʒu</returns>
const Vector2 easeOut(const Vector2& start, const Vector2& end, const float t);

/// <summary>
/// ease-InOut���
/// </summary>
/// <param name="start">�X�^�[�g�ʒu</param>
/// <param name="end">�S�[���ʒu</param>
/// <param name="t">��Ԏ���</param>
/// <returns>���Ԃɉ������ʒu</returns>
const Vector2 easeInOut(const Vector2& start, const Vector2& end, const float t);

#pragma endregion

#pragma region Matrix

//�s��
struct Matrix4
{
	float m[4][4];
};

/// <summary>
/// �P�ʍs��̎擾
/// </summary>
/// <returns>�P�ʍs��</returns>
Matrix4 identity();

/// <summary>
/// �g��k���s��̌v�Z
/// </summary>
/// <param name="s">�g�傷�銄��</param>
/// <returns>�g��k���s��</returns>
Matrix4 scale(const Vector3& s);

/// <summary>
/// ��]�s��̌v�Z
/// </summary>
/// <param name="angle">��]�p�x</param>
/// <returns>��]�s��</returns>
Matrix4 rotateX(float angle);
Matrix4 rotateY(float angle);
Matrix4 rotateZ(float angle);

/// <summary>
/// ���s�ړ��s��̌v�Z
/// </summary>
/// <param name="t">�ړ�����</param>
/// <returns>���s�ړ��s��</returns>
Matrix4 translate(const Vector3& t);

/// <summary>
/// ���W�ϊ�(�x�N�g���ƍs��̊|���Z������)
/// </summary>
/// <param name="v">�ړ��s��</param>
/// <param name="m">�s��</param>
/// <returns>�x�N�g��</returns>
Vector3 transform(const Vector3& v, const Matrix4& m);

//������Z�q�I�[�o�[���[�h
Matrix4& operator *= (Matrix4& m1, const Matrix4& m2);

//2�����Z�q�I�[�o�[���[�h
Matrix4 operator * (const Matrix4& m1, const Matrix4& m2);
Vector3 operator * (const Vector3& v, const Matrix4& m);

#pragma endregion

#pragma region Quaternion

//�N�H�[�^�j�I��
struct Quaternion
{
	float x;//x����
	float y;//y����
	float z;//z����
	float w;//w����
};

//�N�H�[�^�j�I�����쐬
Quaternion quaternion(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f);
Quaternion quaternion(const Vector3& v, float angle);

/// <summary>
/// ���ς̌v�Z
/// </summary>
/// <param name="q1">�N�H�[�^�j�I��</param>
/// <param name="q2">�N�H�[�^�j�I��</param>
/// <returns>�v�Z����</returns>
float dot(const Quaternion& q1, const Quaternion& q2);

/// <summary>
/// �m���������߂�
/// </summary>
/// <param name="q">�N�H�[�^�j�I��</param>
/// <returns>�v�Z����</returns>
float length(const Quaternion& q);

/// <summary>
/// ���K������
/// </summary>
/// <param name="q">�N�H�[�^�j�I��</param>
/// <returns>�v�Z����</returns>
Quaternion normalize(const Quaternion& q);

/// <summary>
/// ����(���傤�₭)�l���������߂�
/// </summary>
/// <param name="q">�N�H�[�^�j�I��</param>
/// <returns>�v�Z����</returns>
Quaternion conjugate(const Quaternion& q);

//�P�����Z�q�I�[�o�[���[�h
Quaternion operator + (Quaternion& q);
Quaternion operator - (Quaternion& q);
//������Z�q�I�[�o�[���[�h
Quaternion& operator +=(Quaternion& q1, const Quaternion& q2);
Quaternion& operator -=(Quaternion& q1, const Quaternion& q2);
Quaternion& operator *=(Quaternion& q, float s);
Quaternion& operator /=(Quaternion& q, float s);
Quaternion& operator *=(Quaternion& q1, const Quaternion& q2);
//2�����Z�q�I�[�o�[���[�h
Quaternion operator + (const Quaternion& q1, const Quaternion& q2);
Quaternion operator - (const Quaternion& q1, const Quaternion& q2);
Quaternion operator * (const Quaternion& q1, const Quaternion& q2);
Quaternion operator * (const Quaternion& q, float s);
Quaternion operator * (float s, Quaternion& q);
Quaternion operator / (const Quaternion& q, float s);

/// <summary>
/// ���ʐ��`���
/// </summary>
/// <param name="q1">�N�H�[�^�j�I��</param>
/// <param name="q2">�N�H�[�^�j�I��</param>
/// <param name="t">��Ԏ���</param>
/// <returns>�N�H�[�^�j�I��</returns>
Quaternion slerp(const Quaternion& q1, const Quaternion& q2, float t);

/// <summary>
/// ���`���
/// </summary>
/// <param name="q1">�N�H�[�^�j�I��</param>
/// <param name="q2">�N�H�[�^�j�I��</param>
/// <param name="t">��Ԏ���</param>
/// <returns>�N�H�[�^�j�I��</returns>
Quaternion lerp(const Quaternion& q1, const Quaternion& q2, float t);

/// <summary>
/// ��]�s��̌v�Z
/// </summary>
/// <param name="q">�N�H�[�^�j�I��</param>
/// <returns>��]�s��</returns>
Matrix4 rotate(const Quaternion& q);

/// <summary>
/// �N�H�[�^�j�I���̌v�Z
/// </summary>
/// <param name="m">�s��</param>
/// <returns>�v�Z����</returns>
Quaternion quaternion(const Matrix4& m);

/// <summary>
/// 3���̌v�Z
/// </summary>
/// <param name="q">�N�H�[�^�j�I��</param>
/// <returns>�x�N�g��</returns>
Vector3 getAxis(const Quaternion& q);

#pragma endregion
