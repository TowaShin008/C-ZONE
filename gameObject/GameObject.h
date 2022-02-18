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

//動きの段階
enum class MOVEPHASE
{
	PHASE1,
	PHASE2,
	PHASE3,
	PHASE4,
	PHASE5,
	PHASE6,
};

//動きのタイプ
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
	// 定数バッファ用データ構造体
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
	/// コンストラクタ
	/// </summary>
	GameObject();

	/// <summary>
	/// 仮想デストラクタ(基底クラスにする場合は必ず記入する)
	/// </summary>
	virtual ~GameObject();

	/// <summary>
	/// 初期化処理
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update(const Vector3& incrementValue);

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="cmdList"></param>
	virtual void Draw();

	/// <summary>
	/// ポジションの取得
	/// </summary>
	/// <returns>ポジション</returns>
	const Vector3& GetPosition() { return position; }


	/// <summary>
	/// ポジションのセット
	/// </summary>
	/// <param name="position">ポジション</param>
	void SetPosition(const Vector3& arg_position) { position = arg_position; }

	/// <summary>
	/// ワールド座標の取得
	/// </summary>
	/// <returns></returns>
	XMMATRIX GetWorldMatrix() { return matWorld; }

	/// <summary>
	/// 死亡フラグ取得
	/// </summary>
	/// <returns>死亡フラグ</returns>
	bool GetIsDeadFlag() { return isDeadFlag; }

	/// <summary>
	/// 死亡フラグのセット
	/// </summary>
	/// <param name="isDeadFlag">死亡フラグ</param>
	void SetIsDeadFlag(bool arg_isDeadFlag) { isDeadFlag = arg_isDeadFlag; }

	/// <summary>
	/// スケールの取得
	/// </summary>
	/// <returns>スケール</returns>
	const Vector3& GetScale() { return scale; }

	/// <summary>
	/// スケールのセット
	/// </summary>
	/// <param name="scale">スケール</param>
	void SetScale(const Vector3& arg_scale) { scale = arg_scale; }

	/// <summary>
	/// 回転角度の取得
	/// </summary>
	/// <returns>回転角度</returns>
	const Vector3& GetRotasion() { return rotation; }

	/// <summary>
	/// 回転角度のセット
	/// </summary>
	/// <param name="rotation"></param>
	void SetRotation(const Vector3& arg_rotation) { rotation = arg_rotation; }

	/// <summary>
	/// カラーの取得
	/// </summary>
	/// <returns>カラー</returns>
	const Vector4& GetColor() { return color; }

	/// <summary>
	/// カラーのセット
	/// </summary>
	/// <param name="color">カラー</param>
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
	/// 運動方向
	/// </summary>
	/// <param name="velocity"></param>
	void SetVelocity(const Vector3& arg_velocity) { velocity = arg_velocity; }
	const Vector3& GetVelocity() { return velocity; }
	void SetSpeed(const Vector3& arg_speed) { speed = arg_speed; }
	const Vector3& GetSpeed() { return speed; }

	int GetHp() { return hp; };

	/// <summary>
	/// モーションブラーの適用
	/// </summary>
	/// <param name="motionBlurFlag">モーションブラーフラグ</param>
	void SetMotionBlurFlag(bool arg_motionBlurFlag) { motionBlurFlag = arg_motionBlurFlag; }

	/// <summary>
	/// 当たり判定の半径をセット
	/// </summary>
	/// <param name="arg_collisionRadius"></param>
	void SetCollisionRadius(float arg_collisionRadius) { collisionRadius = arg_collisionRadius; }

	/// <summary>
	/// 当たり判定の半径をセット
	/// </summary>
	/// <param name="arg_collisionRadius"></param>
	float GetCollisionRadius() { return collisionRadius; }

	void SetCharacterType(CHARACTERTYPE arg_characterType) { characterType = arg_characterType; }

	CHARACTERTYPE GetCharacterType() { return characterType; }

	/// <summary>
	/// 当たり判定
	/// </summary>
	/// <param name="otherObject">相手のオブジェクト</param>
	/// <returns>当たったかどうか</returns>
	virtual bool IsCollision(GameObject* arg_otherObject);

	/// <summary>
	/// ダメージ処理
	/// </summary>
	virtual void Damage();

	/// <summary>
	/// 大ダメージ処理
	/// </summary>
	virtual void CriticalDamage();
};

