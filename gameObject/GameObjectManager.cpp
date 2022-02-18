#include "GameObjectManager.h"

GameObjectManager::GameObjectManager()
{

}

GameObjectManager::~GameObjectManager()
{
	for (int i = 0; i < gameObjects.size(); i++)
	{
		delete gameObjects[i];
	}
}

void GameObjectManager::Initialize(unsigned int arg_score)
{
	score = arg_score;
}

void GameObjectManager::AddGameObject(GameObject* gameObject)
{
	gameObjects.emplace_back(gameObject);
}

void GameObjectManager::UpdateAllObject(const Vector3& incrementValue)
{
	for (int i = 0; i < gameObjects.size(); i++)
	{
		gameObjects[i]->Update(incrementValue);
	}
}

void GameObjectManager::DrawAllObject()
{
	for (int i = 0; i < gameObjects.size(); i++)
	{
		gameObjects[i]->Draw();
	}
}

void GameObjectManager::AllObjectCollision()
{
	for (int i = 0; i < gameObjects.size(); i++)
	{
		for (int j = 0; j < gameObjects.size(); j++)
		{//同じオブジェクト同士はとばす
			if (i == j) { continue; }
			bool currentDeadFlag = gameObjects[j]->GetIsDeadFlag();
			gameObjects[i]->IsCollision(gameObjects[j]);

			if (currentDeadFlag == false && gameObjects[j]->GetIsDeadFlag())
			{
				score++;
			}
		}
	}
}
