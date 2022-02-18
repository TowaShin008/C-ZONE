#include "MineMath.h"
#include<cmath>

#pragma region Vector

Vector4::Vector4()
	:Vector4(0, 0, 0, 0)
{
}

Vector4::Vector4(float x, float y, float z, float w)
	: XMFLOAT4(x,y,z,w)
{
}
//�P�����Z�q�̃I�[�o�[���[�h
Vector4 Vector4::operator+() const
{
	return *this;
}

Vector4 Vector4::operator-() const
{
	return Vector4(-x, -y, -z, -w);
}
//������Z�q�I�[�o�[���[�h
Vector4& Vector4::operator+=(const Vector4& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;
	return *this;
}

Vector4& Vector4::operator-=(const Vector4& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;
	return *this;
}

Vector4& Vector4::operator*=(float s)
{
	x *= s;
	y *= s;
	z *= s;
	w *= s;
	return *this;
}

Vector4& Vector4::operator/=(float s)
{
	x /= s;
	y /= s;
	z /= s;
	w /= s;
	return *this;
}
//2�����Z�q�I�[�o�[���[�h
//�������Ȉ����̃p�^�[���ɑΉ�(�����̏���)���邽�߁A�ȉ��̂悤�ɏ������Ă���
const Vector4 operator+(const Vector4& v1, const Vector4& v2)
{
	Vector4 temp(v1);
	return temp += v2;
}

const Vector4 operator-(const Vector4& v1, const Vector4& v2)
{
	Vector4 temp(v1);
	return temp -= v2;
}

const Vector4 operator*(const Vector4& v, float s)
{
	Vector4 temp(v);
	return temp *= s;
}

const Vector4 operator*(float s, const Vector4& v)
{
	return v * s;
}

const Vector4 operator/(const Vector4& v, float s)
{
	Vector4 temp(v);
	return temp /= s;
}


Vector3::Vector3()
	:Vector3(0, 0, 0)
{
}

Vector3::Vector3(float x, float y, float z)
	: XMFLOAT3(x,y,z)
{
}

//�����o�֐�
float Vector3::length() const                    //�m����(����)�����߂�
{
	return sqrt(x * x + y * y + z * z);
}

Vector3& Vector3::normalize()                    //���K������
{
	float len = length();
	if (len != 0)
	{
		return *this /= len;
	}
	return *this;
}

float Vector3::dot(const Vector3& v) const       //���ς����߂�
{
	return (x * v.x + y * v.y + z * v.z);
}

Vector3 Vector3::cross(const Vector3& v)const      //�O�ς����߂�
{
	return { y * v.z - z * v.y,z * v.x - x * v.z,x * v.y - y * v.x };
}
//�P�����Z�q�̃I�[�o�[���[�h
Vector3 Vector3::operator+() const
{
	return *this;
}

Vector3 Vector3::operator-() const
{
	return Vector3(-x, -y, -z);
}
//������Z�q�I�[�o�[���[�h
Vector3& Vector3::operator+=(const Vector3& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

Vector3& Vector3::operator-=(const Vector3& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

Vector3& Vector3::operator*=(const Vector3& v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;
	return *this;
}

Vector3& Vector3::operator*=(float s)
{
	x *= s;
	y *= s;
	z *= s;
	return *this;
}

Vector3& Vector3::operator/=(float s)
{
	x /= s;
	y /= s;
	z /= s;
	return *this;
}
//2�����Z�q�I�[�o�[���[�h
//�������Ȉ����̃p�^�[���ɑΉ�(�����̏���)���邽�߁A�ȉ��̂悤�ɏ������Ă���
const Vector3 operator+(const Vector3& v1, const Vector3& v2)
{
	Vector3 temp(v1);
	return temp += v2;
}

const Vector3 operator-(const Vector3& v1, const Vector3& v2)
{
	Vector3 temp(v1);
	return temp -= v2;
}

const Vector3 operator*(const Vector3& v1, const Vector3& v2)
{
	Vector3 temp(v1);
	return temp *= v2;
}

const Vector3 operator*(const Vector3& v, float s)
{
	Vector3 temp(v);
	return temp *= s;
}

const Vector3 operator*(float s, const Vector3& v)
{
	return v * s;
}

const Vector3 operator/(const Vector3& v, float s)
{
	Vector3 temp(v);
	return temp /= s;
}

const Vector3 lerp(const Vector3& start, const Vector3& end, const float t)
{
	return start * (1.0f - t) + end * t;
}
//ease-in���
const Vector3 easeIn(const Vector3& start, const Vector3& end, const float t)
{
	float y = t * t;
	return start * (1.0f - y) + end * y;
}
//ease-out
const Vector3 easeOut(const Vector3& start, const Vector3& end, const float t)
{
	float y = t * (2 - t);
	return start * (1.0f - y) + end * y;
}
//ease-inout
const Vector3 easeInOut(const Vector3& start, const Vector3& end, const float t)
{
	float y = t * t * (3 - 2 * t);
	return start * (1.0f - y) + end * y;
}


Vector2::Vector2()
	:Vector2(0, 0)
{
}

Vector2::Vector2(float x, float y)
	: XMFLOAT2(x,y)
{
}
//�����o�֐�
float Vector2::length() const                    //�m����(����)�����߂�
{
	return sqrt(x * x + y * y);
}

Vector2& Vector2::normalize()                    //���K������
{
	float len = length();
	if (len != 0)
	{
		return *this /= len;
	}
	return *this;
}

float Vector2::dot(const Vector2& v) const       //���ς����߂�
{
	return (x * v.x + y * v.y);
}

float Vector2::cross(const Vector2& v)const      //�O�ς����߂�
{
	return { x * v.y - y * v.x };
}
//�P�����Z�q�̃I�[�o�[���[�h
Vector2 Vector2::operator+() const
{
	return *this;
}

Vector2 Vector2::operator-() const
{
	return Vector2(-x, -y);
}
//������Z�q�I�[�o�[���[�h
Vector2& Vector2::operator+=(const Vector2& v)
{
	x += v.x;
	y += v.y;
	return *this;
}

Vector2& Vector2::operator-=(const Vector2& v)
{
	x -= v.x;
	y -= v.y;
	return *this;
}

Vector2& Vector2::operator*=(float s)
{
	x *= s;
	y *= s;
	return *this;
}

Vector2& Vector2::operator/=(float s)
{
	x /= s;
	y /= s;
	return *this;
}
//2�����Z�q�I�[�o�[���[�h
//�������Ȉ����̃p�^�[���ɑΉ�(�����̏���)���邽�߁A�ȉ��̂悤�ɏ������Ă���
const Vector2 operator+(const Vector2& v1, const Vector2& v2)
{
	Vector2 temp(v1);
	return temp += v2;
}

const Vector2 operator-(const Vector2& v1, const Vector2& v2)
{
	Vector2 temp(v1);
	return temp -= v2;
}

const Vector2 operator*(const Vector2& v, float s)
{
	Vector2 temp(v);
	return temp *= s;
}

const Vector2 operator*(float s, const Vector2& v)
{
	return v * s;
}

const Vector2 operator/(const Vector2& v, float s)
{
	Vector2 temp(v);
	return temp /= s;
}

const Vector2 lerp(const Vector2& start, const Vector2& end, const float t)
{
	return start * (1.0f - t) + end * t;
}
//ease-in���
const Vector2 easeIn(const Vector2& start, const Vector2& end, const float t)
{
	float y = t * t;
	return start * (1.0f - y) + end * y;
}
//ease-out
const Vector2 easeOut(const Vector2& start, const Vector2& end, const float t)
{
	float y = t * (2 - t);
	return start * (1.0f - y) + end * y;
}
//ease-inout
const Vector2 easeInOut(const Vector2& start, const Vector2& end, const float t)
{
	float y = t * t * (3 - 2 * t);
	return start * (1.0f - y) + end * y;
}

#pragma endregion

#pragma region Matrix

//�P�ʍs������߂�
Matrix4 identity()
{
	static const Matrix4 result
	{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};
	return result;
}
//�g��k���s������߂�
Matrix4 scale(const Vector3& s)
{
	Matrix4 result
	{
		s.x,0.0f,0.0f,0.0f,
		0.0f,s.y,0.0f,0.0f,
		0.0f,0.0f,s.z,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};
	return result;
}
//x���܂��̉�]�s������߂�
Matrix4 rotateX(float angle)
{
	float sin = std::sin(angle);
	float cos = std::cos(angle);
	Matrix4 result
	{
		1.0f,0.0f,0.0f,0.0f,
		0.0f, cos, sin,0.0f,
		0.0f,-sin, cos,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};
	return result;
}
//y���܂��̉�]�s������߂�
Matrix4 rotateY(float angle)
{
	float sin = std::sin(angle);
	float cos = std::cos(angle);
	Matrix4 result
	{
		 cos,0.0f,-sin,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		 sin,0.0f, cos,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};
	return result;
}
//z���܂��̉�]�s������߂�
Matrix4 rotateZ(float angle)
{
	float sin = std::sin(angle);
	float cos = std::cos(angle);
	Matrix4 result
	{
		 cos, sin,0.0f,0.0f,
		-sin, cos,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};
	return result;
}
//���s�ړ��s������߂�
Matrix4 translate(const Vector3& t)
{
	Matrix4 result
	{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		 t.x, t.y, t.z,1.0f
	};
	return result;
}
//���W�ϊ�(�x�N�g���ƍs��̊|���Z)���s��transform�֐����쐬����B(�����ϊ����Ή����Ă���)
Vector3 transform(const Vector3& v, const Matrix4& m)
{
	float w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + m.m[3][3];

	Vector3 result
	{
		(v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + m.m[3][0]) / w,
		(v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + m.m[3][1]) / w,
		(v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + m.m[3][2]) / w
	};
	return result;
}
//������Z�q*=�̃I�[�o�[���[�h�֐�(�s��ƍs��̐�)
Matrix4& operator *= (Matrix4& m1, const Matrix4& m2)
{
	Matrix4 result{ 0 };

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				result.m[i][j] += m1.m[i][k] * m2.m[k][j];
			}
		}
	}
	m1 = result;

	return m1;
}
//2�����Z�q * �̃I�[�o�[���[�h�֐�(�s��ƍs��̐�)
Matrix4 operator * (const Matrix4& m1, const Matrix4& m2)
{
	Matrix4 result = m1;
	return result *= m2;
}
//2�����Z�q * �̃I�[�o�[���[�h�֐�(�x�N�g���ƍs��̐�)
Vector3 operator * (const Vector3& v, const Matrix4& m)
{
	return transform(v, m);
}

#pragma endregion

#pragma region Quaternion

//�������w�肵�ā@�N�H�[�^�j�I�����쐬
Quaternion quaternion(float x, float y, float z, float w)
{
	Quaternion result{ x,y,z,w };
	return result;
}
//�C�ӎ��܂��̉�]�̎w�肩��N�H�[�^�j�I���𐶐�
Quaternion quaternion(const Vector3& v, float angle)
{
	float _sin = sin(angle / 2.0f);
	return quaternion(_sin * v.x, _sin * v.y, _sin * v.z, cos(angle / 2.0f));
}
//���x�N�g��v���N�H�[�^�j�I��q�ŉ�]���A���ʂ��N�H�[�^�j�I���ŕԂ�
Quaternion quaternion(const Vector3& v, const Quaternion& q)
{
	Quaternion qq = conjugate(q);                    //q�̋����l�������쐬����
	Quaternion qv = quaternion(v.x, v.y, v.z, 0);    //�x�N�g��v���N�H�[�^�j�I��������

	return q * qv * qq;//�x�N�g����]�����N�H�[�^�j�I����Ԃ�
}
//���ς����߂�
float dot(const Quaternion& q1, const Quaternion& q2)
{
	return (q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z);
}
//�m���������߂�
float length(const Quaternion& q)
{
	return sqrtf(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z);
}
//���K������
Quaternion normalize(const Quaternion& q)
{
	Quaternion result = q;
	float len = length(result);
	if (len != 0) {
		result /= len;
	}
	return result;
}
//����(���傤�₭)���f�������߂�conjugate[�R���W���Q�[�g:����]
Quaternion conjugate(const Quaternion& q)
{
	Quaternion result = { -q.x,-q.y,-q.z,q.w };
	return result;
}
//�P�����Z�q�I�[�o�[���[�h
//�P�����Z�q+�̃I�[�o�[���[�h
//�P�����Z�q+�͓��ɈӖ��������Ȃ�
//(+�N�H�[�^�j�I��)
Quaternion operator + (const Quaternion& q)
{
	return q;
}

Quaternion operator - (const Quaternion& q)
{
	Quaternion result;
	result.x = -q.x;
	result.y = -q.y;
	result.z = -q.z;
	result.w = -q.w;
	return result;
}
//������Z�q�I�[�o�[���[�h
Quaternion& operator +=(Quaternion& q1, const Quaternion& q2)
{
	q1.x += q2.x;
	q1.y += q2.y;
	q1.z += q2.z;
	q1.w += q2.w;
	return q1;
}

Quaternion& operator -=(Quaternion& q1, const Quaternion& q2)
{
	q1.x -= q2.x;
	q1.y -= q2.y;
	q1.z -= q2.z;
	q1.w -= q2.w;
	return q1;
}

Quaternion& operator *=(Quaternion& q, float s)
{
	q.x = q.x * s;
	q.y = q.y * s;
	q.z = q.z * s;
	q.w = q.w * s;
	return q;
}

Quaternion& operator /=(Quaternion& q, float s)
{
	return q *= 1.0f / s;
}

Quaternion& operator *=(Quaternion& q1, const Quaternion& q2)
{
	Quaternion result{
		q1.x * q2.w + q1.y * q2.z - q1.z * q2.y + q1.w * q2.x,
	   -q1.x * q2.z + q1.y * q2.w + q1.z * q2.x + q1.w * q2.y,
		q1.x * q2.y - q1.y * q2.x + q1.z * q2.w + q1.w * q2.z,
	   -q1.x * q2.x - q1.y * q2.y - q1.z * q2.z + q1.w * q2.w,
	};
	q1 = result;
	return q1;
}
//2�����Z�q�I�[�o�[���[�h
Quaternion operator + (const Quaternion& q1, const Quaternion& q2)
{
	Quaternion result = q1;
	return result += q2;
}

Quaternion operator - (const Quaternion& q1, const Quaternion& q2)
{
	Quaternion result = q1;
	return result -= q2;
}

Quaternion operator * (const Quaternion& q1, const Quaternion& q2)
{
	Quaternion result = q1;
	return result *= q2;
}

Quaternion operator * (const Quaternion& q, float s)
{
	Quaternion result = q;
	return result *= s;
}

Quaternion operator * (float s, Quaternion& q)
{
	Quaternion result = s * q;
	return result;
}

Quaternion operator / (const Quaternion& q, float s)
{
	Quaternion result = q;
	return result /= s;
}

Quaternion slerp(const Quaternion& q1, const Quaternion& q2, float t)
{
	float cos = dot(q1, q2);
	Quaternion t2 = q2;
	if (cos < 0.0f) {
		cos = -cos;
		t2 = -q2;
	}
	float k0 = 1.0f - t;
	float k1 = t;
	if ((1.0f - cos) > 0.001f) {
		float theta = (float)acos(cos);
		k0 = (float)(sin(theta * k0) / sin(theta));
		k1 = (float)(sin(theta * k1) / sin(theta));
	}
	return q1 * k0 + t2 * k1;
}

Quaternion lerp(const Quaternion& q1, const Quaternion& q2, float t)
{
	float cos = dot(q1, q2);
	Quaternion t2 = q2;
	if (cos < 0.0f) {
		cos = -cos;
		t2 = -q2;
	}
	float k0 = 1.0f - t;
	float k1 = t;
	return q1 * k0 + t2 * k1;
}
//���N�H�[�^�j�I�������]�s��ɕϊ�����֐�
Matrix4 rotate(const Quaternion& q)
{
	float xx = q.x * q.x * 2.0f;
	float yy = q.y * q.y * 2.0f;
	float zz = q.z * q.z * 2.0f;
	float xy = q.x * q.y * 2.0f;
	float xz = q.x * q.z * 2.0f;
	float yz = q.y * q.z * 2.0f;
	float wx = q.w * q.x * 2.0f;
	float wy = q.w * q.y * 2.0f;
	float wz = q.w * q.z * 2.0f;
	Matrix4 result = {
	1.0f - yy - zz, xy + wz, xz - wy, 0.0f,
	xy - wz, 1.0f - xx - zz, yz + wx, 0.0f,
	xz + wy, yz - wx, 1.0f - xx - yy, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
	};
	return result;
}

Quaternion quaternion(const Matrix4& m)
{
	Quaternion result;
	float tr = m.m[0][0] + m.m[1][1] + m.m[2][2] + m.m[3][3];
	if (tr >= 1.0f) {
		float fourD = 2.0f * sqrt(tr);
		result.x = (m.m[1][2] - m.m[2][1]) / fourD;
		result.y = (m.m[2][0] - m.m[0][2]) / fourD;
		result.z = (m.m[0][1] - m.m[1][0]) / fourD;
		result.w = fourD / 4.0f;
		return result;
	}
	int i = 0;
	if (m.m[0][0] <= m.m[1][1])
	{
		i = 1;
	}
	if (m.m[2][2] > m.m[i][i]) {
		i = 2;
	}
	int j = (i + 1) % 3;
	int k = (j + 1) % 3;
	tr = m.m[i][i] - m.m[j][j] - m.m[k][k] + 1.0f;
	float fourD = 2.0f * sqrt(tr);
	float qa[4];
	qa[i] = fourD / 4.0f;
	qa[j] = (m.m[j][i] + m.m[i][j]) / fourD;
	qa[k] = (m.m[k][i] + m.m[i][k]) / fourD;
	qa[3] = (m.m[j][k] - m.m[k][j]) / fourD;
	result.x = qa[0];
	result.y = qa[1];
	result.z = qa[2];
	result.w = qa[3];
	return result;
}

Vector3 getAxis(const Quaternion& q)
{
	Vector3 result;

	float x = q.x;
	float y = q.y;
	float z = q.z;
	float _len = length(q);

	result.x = x / _len;
	result.y = y / _len;
	result.z = z / _len;

	return result;
}

#pragma endregion
