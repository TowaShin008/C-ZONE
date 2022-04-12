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

void GameObjectManager::AddGameObject(GameObject* arg_gameObject)
{
	gameObjects.emplace_back(arg_gameObject);
}

void GameObjectManager::UpdateAllObject(const Vector3& arg_incrementValue)
{
	for (int i = 0; i < gameObjects.size(); i++)
	{
		gameObjects[i]->Update(arg_incrementValue);
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

void GameObjectManager::DisPlayScore(DebugText* debugText)
{
	std::ostringstream scoreText;

	scoreText << "Score:" << std::setfill('0') << std::setw(8)
		<< std::fixed << score << "00";

	debugText->Print(scoreText.str(), WindowSize::window_width - 300.0f, WindowSize::window_height - 60.0f, 0.5f);
}
