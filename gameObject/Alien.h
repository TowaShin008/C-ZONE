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
#include"ObjFileCharacter.h"
#define BULLETMAXNUM 10
#define MAXENEMYLUGTIME 60

//敵エイリアンクラス
class Alien :public GameObject
{
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	using XMMATRIX = DirectX::XMMATRIX;

public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="arg_cmdList">コマンドリスト</param>
	/// <param name="arg_device">デバイス</param>
	Alien(ID3D12GraphicsCommandList* arg_cmdList, ID3D12Device* arg_device);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Alien();

	// 定数バッファ用データ構造体
	struct  DissolveConstBufferData
	{
		Vector4 color;
		XMMATRIX mat;
		float dissolveCount;
	};

	/// <summary>
	/// オブジェクトの生成処理
	/// </summary>
	/// <param name="arg_device">デバイス</param>
	/// <param name="arg_position">ポジション</param>
	/// <returns>キャラクターオブジェクト</returns>
	static Alien* Create(ID3D12Device* arg_device, ID3D12GraphicsCommandList* arg_cmdList,const Vector3& arg_position = { 0.0f,0.0f,0.0f });

	/// <summary>
	/// 定数バッファの生成
	/// </summary>
	void CreateConstBuffer();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="arg_incrementValue">スクロール量</param>
	void Update(const Vector3& arg_incrementValue)override;

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	void Draw();

	/// <summary>
	/// モデルのセット
	/// </summary>
	/// <param name="objModel">モデル</param>
	void SetOBJModel(ObjFileModel* arg_objModel, ObjFileModel* arg_scoreModel);

	/// <summary>
	/// 視点座標のセット
	/// </summary>
	/// <param name="eye">視点座標</param>
	static void SetEye(const Vector3& eye);

	/// <summary>
	/// 注視点座標のセット
	/// </summary>
	/// <param name="target">注視点座標</param>
	static void SetTarget(const Vector3& target);

	/// <summary>
	/// ビュー行列の更新処理
	/// </summary>
	static void UpdateViewMatrix();

	/// <summary>
	/// カメラのセット
	/// </summary>
	/// <param name="arg_camera">カメラ</param>
	static void SetCamera(Camera* arg_camera) { Alien::camera = arg_camera; }

private:
	UINT descpriptorSize;
	static ID3D12GraphicsCommandList* cmdList;
	static ID3D12Device* device;
	ComPtr<ID3D12Resource>constBuffB0;
	ComPtr<ID3D12Resource>constBuffB1;
	ComPtr<ID3D12Resource>dissolveTexBuff;
	ComPtr<ID3D12DescriptorHeap>descHeapSRV;

	//カメラ関係
	// 
	//ビュー行列
	static XMMATRIX matView;
	//プロジェクション行列
	static XMMATRIX matProjection;
	//カメラクラス
	static Camera* camera;

	//モデルデータ格納用変数
	ObjFileModel* objModel = nullptr;
	//親クラス
	Alien* parent = nullptr;

	//動き出すまでの時間(複数の敵で隊列を組みたいときに使用する)
	int moveLugTime = 0;

	//現在の動きの段階を格納する変数
	MOVEPHASE currentPhase = MOVEPHASE::PHASE1;

	//現在の動きの種類を格納する変数
	MOVETYPE  currentType = MOVETYPE::TRIANGLE;

	bool moveEndFlag = false;
	float curveTime = 0.0f;
	float rotateX = 0.0f;
	float centerPosition = 0.0f;
	int life = 0;
	float dissolveCount = 1.0f;

	std::unique_ptr<ObjFileCharacter> scoreCharacter;
public:

	/// <summary>
	/// 定数バッファの転送
	/// </summary>
	void TransferConstBuff();

	/// <summary>
	/// 移動処理
	/// </summary>
	void Move();

	/// <summary>
	/// ムーブフェイズのセット
	/// </summary>
	/// <param name="arg_currentPhase">ムーブフェイズ</param>
	void SetMovePhase(const MOVEPHASE& arg_currentPhase) { currentPhase = arg_currentPhase; }

	/// <summary>
	/// ムーブフェイズの取得
	/// </summary>
	/// <returns>ムーブフェイズ</returns>
	MOVEPHASE GetMovePhase() { return currentPhase; }

	/// <summary>
	/// ムーブタイプのセット
	/// </summary>
	/// <param name="arg_moveType">ムーブタイプ</param>
	void SetMoveType(const MOVETYPE& arg_moveType);

	/// <summary>
	/// ムーブタイプの取得
	/// </summary>
	/// <returns>ムーブタイプ</returns>
	MOVETYPE GetMoveType() { return currentType; }

	/// <summary>
	/// 四角形の形に移動する更新処理
	/// </summary>
	void RectAngleMove();

	/// <summary>
	/// 三角形の形に移動する更新処理
	/// </summary>
	void TriAngleMove();

	/// <summary>
	/// 斜めに移動する処理
	/// </summary>
	void LeftDiagonallyMove();

	/// <summary>
	/// 上方向に曲線を描いて移動する処理
	/// </summary>
	void UpCurveMove();

	/// <summary>
	/// 下方向に曲線を描いて移動する処理
	/// </summary>
	void DownCurveMove();

	/// <summary>
	/// ムーブラグタイムのセット
	/// </summary>
	/// <param name="arg_moveLugTime">ムーブラグタイム</param>
	void SetMoveLugTime(int arg_moveLugTime) { moveLugTime = arg_moveLugTime; }

	/// <summary>
	/// ムーブラグタイムの取得
	/// </summary>
	/// <returns>ムーブラグタイム</returns>
	int GetMoveLugTime() { return moveLugTime; }

	/// <summary>
	/// ムーブエンドフラグのセット
	/// </summary>
	/// <param name="arg_moveEndFlag"></param>
	void SetMoveEndFlag(bool arg_moveEndFlag) { moveEndFlag = arg_moveEndFlag; }

	/// <summary>
	/// ムーブエンドフラグの所得
	/// </summary>
	/// <returns>移動が終わっているかどうか</returns>
	bool GetMoveEndFlag() { return moveEndFlag; }

	/// <summary>
	/// isDeadFlagをtrueにする
	/// </summary>
	void IsDead();

	/// <summary>
	/// ポジションの移動
	/// </summary>
	/// <param name="arg_incrementValue">ポジションの増加量</param>
	void SetScrollIncrement(const Vector3& arg_incrementValue);

	/// <summary>
	/// センターポジションのセット
	/// </summary>
	/// <param name="arg_centerPosition">センターポジション</param>
	void SetCenterPos(float arg_centerPosition) { centerPosition = arg_centerPosition; }

	/// <summary>
	/// ダメージ処理
	/// </summary>
	void Damage()override;

	/// <summary>
	/// 大ダメージ処理
	/// </summary>
	void CriticalDamage()override;

	/// <summary>
	/// 当たり判定
	/// </summary>
	/// <param name="arg_otherObject">相手のオブジェクト</param>
	/// <returns>当たったかどうか</returns>
	bool IsCollision(GameObject* arg_otherObject)override;

	/// <summary>
	/// スコア演出処理
	/// </summary>
	/// <param name="arg_incrementValue">スクロール量</param>
	void ScoreProcessing(const Vector3& arg_incrementValue);
};

