#pragma once
#include"GameObject.h"
#include<vector>
class GameObjectManager
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	GameObjectManager();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameObjectManager();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="score">スコア</param>
	void Initialize(unsigned int arg_score);

	/// <summary>
	/// オブジェクト追加処理
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	void AddGameObject(GameObject* gameObject);

	/// <summary>
	/// すべてのオブジェクトのアップデート処理
	/// </summary>
	void UpdateAllObject(const Vector3& incrementValue);

	/// <summary>
	/// すべてのオブジェクトの描画処理
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	void DrawAllObject();

	/// <summary>
	/// 全てのオブジェクトの当たり判定
	/// </summary>
	void AllObjectCollision();

	int GetScore() { return score; }

	/// <summary>
	/// ゲームオブジェクト配列の取得
	/// </summary>
	/// <returns>ゲームオブジェクト配列</returns>
	std::vector<GameObject*> GetGameObject() { return gameObjects; }
private:
	//ゲームオブジェクト配列
	std::vector<GameObject*>gameObjects;
	int score = 0;
};

