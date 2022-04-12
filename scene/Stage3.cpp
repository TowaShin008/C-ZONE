#include "Stage3.h"
#include <iomanip>

Stage3::Stage3()
{
}

Stage3::~Stage3()
{
}

void Stage3::Initialize(unsigned int arg_score)
{
	endFlag = false;
	rotateY = 0.0f;
	delayTime = 0;
	score = 0;
	blurFlag = false;
	beginFlag = true;
	wave = 0;
	allEnemyDeadFlag = false;
	scrollPos = 0.1f;
}

void Stage3::CreateAllObject(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList)
{
	gameObjectManager.reset(new GameObjectManager());
	gameObjectManager->Initialize(score);

	//ポストエフェクトの生成
	CreatePostEffect();
	
	//モデルのロード
	LoadAllModel();
	
	//マップデータのセット
	SetMapData(device,arg_cmdList);
	
	//ゲームオブジェクトの生成
	CreateGameObject(device, arg_cmdList);
}

void Stage3::CreatePostEffect()
{
	bloom.reset(new Bloom());
	bloom->Initialize();
	bloom->SetFadeOutFlag(false);

	fadeOut.reset(new FadeOut());
	fadeOut->Initialize();
}

void Stage3::LoadAllModel()
{
	//モデルデータ1のロード(自機)
	planeModel.reset(ObjFileModel::CreateFromOBJ("Plane_2"));
	aircraftCarrierPlaneModel.reset(ObjFileModel::CreateFromOBJ("Enemy_Plane"));
	enemyPlaneModel.reset(ObjFileModel::CreateFromOBJ("SpaceShip"));
	sphereModel.reset(ObjFileModel::CreateFromOBJ("NeedleBall"));
	missileModel.reset(ObjFileModel::CreateFromOBJ("Missile"));
	unionModel.reset(ObjFileModel::CreateFromOBJ("UnionCharacter"));
	alienModel.reset(ObjFileModel::CreateFromOBJ("Alien"));

	scoreModel.reset(ObjFileModel::CreateFromOBJ("Score"));

	//モデルデータ1のロード(黄色銃弾)
	yellowBulletModel.reset(ObjFileModel::CreateFromOBJ("bullet"));

	//モデルデータ1のロード(赤銃弾)
	redBulletModel.reset(ObjFileModel::CreateFromOBJ("eBullet"));
	tankBulletModel.reset(ObjFileModel::CreateFromOBJ("eBullet_2"));

	//モデルデータ1のロード(ブロック)
	blockModel.reset(ObjFileModel::CreateFromOBJ("Block"));

	//モデルデータ1のロード(ブロック)
	laserModel.reset(ObjFileModel::CreateFromOBJ("red_lay"));
}

void Stage3::Update(DebugText* debugText)
{
	incrementValue = 0.02f;

	camera->MoveEyeTarget({ incrementValue,0.0f,0.0f });

	//スクロールの移動量
	scrollPos += incrementValue;

	gameObjectManager->AllObjectCollision();


	gameObjectManager->UpdateAllObject({ incrementValue,0.0f,0.0f });

	if (playerObject->GetIsDeadFlag())
	{
		endFlag = true;
	}

	//ウェーブ切り替え処理
	WaveProcessing();

	//スコアの表示
	gameObjectManager->DisPlayScore(debugText);

	std::ostringstream hpText;

	hpText << ":" << std::fixed << playerObject->GetHP();

	debugText->Print(hpText.str(), 150.0f, 150.0f, 1.25f);

	debugText->Print("BEAM:", 200.0f, WindowSize::window_height - 60.0f, 0.5f);
}

void Stage3::DrawRenderTexture(ID3D12GraphicsCommandList* cmdList, DebugText* debugText)
{
	bloom->PreDrawScene(cmdList);

	gameObjectManager->DrawAllObject();

	debugText->DrawAll(cmdList);

	const int nextWaveMaxCount = 350;
	if (wave >= 0 && wave <= 1 && nextWaveCount < nextWaveMaxCount)
	{
		Sprite::BeginDraw(cmdList, false);

		const int alphaMaxCount = 180;
		if (alphaCount < alphaMaxCount)
		{
			alphaCount++;
		}
		else
		{
			const int initializeTime = 0;
			alphaCount = initializeTime;
		}
		Vector4 buttomTextureColor = stage3Texture->GetColor();
		buttomTextureColor.w = sinf((alphaCount / 180.0f) * XM_PI);

		stage3Texture->SetColor(buttomTextureColor);
		stage3Texture->Draw();

		Sprite::EndDraw();
	}

	Sprite::BeginDraw(cmdList, false);

	lifeTexture->Draw();
	gaugeFrame->Draw();

	Sprite::EndDraw();

	bloom->PostDrawScene(cmdList);
}

void Stage3::PostEffectDraw(ID3D12GraphicsCommandList* cmdList)
{
	bloom->Draw(cmdList);
}

void Stage3::Draw(ID3D12GraphicsCommandList* cmdList, DebugText* debugText)
{
}

void Stage3::WaveProcessing()
{
	if (nextWaveCount > MAXNEXTWAVECOUNT)
	{
		wave++;

		switch (wave)
		{
		case 1:

			Wave1();

			break;

		case 2:

			Wave2();

			break;

		case 3:

			Wave3();

			break;

		case 4:

			Wave4();

			break;

		case 5:

			Wave5();

			break;

		default:
			break;
		}
		nextWaveCount = 0;
	}

	if (wave == 0)
	{
		StageStartProcessing();
	}

	const int subBossScene = 4;
	if (wave < subBossScene)
	{//中ボスシーン以外であれば次のウェーブへ進むためのカウンターを進める
		nextWaveCount++;
	}

	if (wave == subBossScene && aircraftCarrier->GetIsDeadFlag())
	{
		bloom->SetFadeOutFlag(true);
		wave++;
	}

	if (bloom->GetFadeOutCount() <= 0.0f && wave > subBossScene)
	{//ステージが終わったら現在のスコアを保存
		score = gameObjectManager->GetScore();
		endFlag = true;
	}

}

void Stage3::StageStartProcessing()
{
	Vector3 position = playerObject->GetPosition();
	const float playerStopPosition = 0.0f;
	if (position.z < playerStopPosition)
	{
		position.z += 0.1f;
		playerObject->SetPosition(position);
	}
	else
	{
		wave++;
	}
}

void Stage3::Wave1()
{
	for (int i = 0; i < ENEMYNUM; i++)
	{
		normalEnemyObjects[i]->Initialize();
		normalEnemyObjects[i]->SetMoveLugTime(i * 30);
		normalEnemyObjects[i]->SetPosition({ scrollPos + 5.0f, 12.0f,0.0f });
		normalEnemyObjects[i]->SetMoveType(MOVETYPE::DOWNCURVE);
		normalEnemyObjects[i]->SetShotFlag(false);
	}
	tankEnemys[0]->Initialize();
}

void Stage3::Wave2()
{
	for (int i = 0; i < ENEMYNUM / 2; i++)
	{
		normalEnemyObjects[i]->Initialize();
		normalEnemyObjects[i]->SetMoveLugTime(i * 30);
		normalEnemyObjects[i]->SetPosition({ scrollPos + 5.0f + 25.0f,10.0f,0.0f });
		normalEnemyObjects[i]->SetMoveType(MOVETYPE::RECTANGLE);
	}

	for (int i = ENEMYNUM / 2; i < ENEMYNUM; i++)
	{
		normalEnemyObjects[i]->Initialize();
		normalEnemyObjects[i]->SetMoveLugTime((i - ENEMYNUM / 2) * 30 + 240);
		normalEnemyObjects[i]->SetPosition({ scrollPos + 5.0f + 25.0f,10.0f,0.0f });
		normalEnemyObjects[i]->SetMoveType(MOVETYPE::LEFTDIAGONALLY);
	}
}

void Stage3::Wave3()
{
	for (int i = 0; i < ENEMYNUM / 2; i++)
	{
		normalEnemyObjects[i]->Initialize();
		normalEnemyObjects[i]->SetMoveLugTime(i * 30);
		normalEnemyObjects[i]->SetPosition({ scrollPos + 5.0f + 15.0f,15.0f,0.0f });
		normalEnemyObjects[i]->SetMoveType(MOVETYPE::LEFTDIAGONALLY);
	}
}

void Stage3::Wave4()
{
	aircraftCarrier->Initialize();
	aircraftCarrier->SetMoveLugTime(0);
	aircraftCarrier->SetPosition({ scrollPos + 5.0f + 15.0f,0.0f,0.0f });
	aircraftCarrier->SetMoveType(MOVETYPE::STAY);
}

void Stage3::Wave5()
{
}

Scene Stage3::Next()
{
	if (playerObject->GetIsDeadFlag() == true)
	{
		return Scene::GAMEOVER;
	}

	return Scene::BOSS3;
}

void Stage3::SetMapData(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList)
{
	map = {
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	};

	map_2 = {
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0},
	{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	};

	blocks.resize(map.size());
	tankEnemys.resize(1);

	for (int i = 0; i < map.size(); ++i)
	{
		blocks[i].resize(map[i].size());
		for (int j = 0; j < map[i].size(); ++j)
		{
			if (map[i][j] == 1 || map[i][j] == 2)//iが縦でjが横
			{//通常ブロックの生成
				blocks[i][j] = Block::Create(device, arg_cmdList, { j * 1.31f - 19.25f, -i * 1.4f + 9.8f ,0.0f });
				//モデル1のセット
				blocks[i][j]->SetOBJModel(blockModel.get());
				blocks[i][j]->Update({ incrementValue,0,0 });
				blocks[i][j]->SetColor({ 1.0f,0.0f,0.0f,1.0f });
				gameObjectManager->AddGameObject(blocks[i][j]);
			}
		}
	}

	blocks_2.resize(map_2.size());


	for (int i = 0; i < map_2.size(); ++i)
	{
		blocks_2[i].resize(map_2[i].size());
		for (int j = 0; j < map_2[i].size(); ++j)
		{
			if (map_2[i][j] == 1 || map_2[i][j] == 2)//iが縦でjが横
			{//通常ブロックの生成
				blocks_2[i][j] = Block::Create(device, arg_cmdList, { (j + 25) * 1.31f - 19.25f, -i * 1.4f + 9.8f ,0.0f });
				//モデル1のセット
				blocks_2[i][j]->SetOBJModel(blockModel.get());
				blocks_2[i][j]->Update({ incrementValue,0,0 });
				blocks_2[i][j]->SetColor({ 1.0f,0.0f,0.0f,1.0f });
				gameObjectManager->AddGameObject(blocks_2[i][j]);
			}
			if (map_2[i][j] == 3)
			{
				//キャラクター1の生成
				tankEnemys[0] = TankEnemy::Create(device, arg_cmdList, { (j + 25) * 1.31f, -i * 1.4f ,0.0f });
				//モデル1のセット
				tankEnemys[0]->SetOBJModel(sphereModel.get(), tankBulletModel.get(), scoreModel.get());
				gameObjectManager->AddGameObject(tankEnemys[0]);
			}
		}
	}
}

void Stage3::CreateGameObject(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList)
{
	stage3Texture.reset(Sprite::Create(L"Resources/stage3.png", { (float)WindowSize::window_width / 2,(float)WindowSize::window_height / 2 }));
	stage3Texture->SetAnchorPoint({ 0.5f,0.5f });

	lifeTexture.reset(Sprite::Create(L"Resources/player_texture.png", { 100.0f, 190.0f }));
	lifeTexture->SetAnchorPoint({ 0.5f,0.5f });
	lifeTexture->SetScale({ 104.0f,68.0f });

	gaugeFrame.reset(Sprite::Create(L"Resources/gauge_frame.png", { (float)WindowSize::window_width / 2 + 8.0f,(float)WindowSize::window_height - 50.0f }));
	gaugeFrame->SetScale({ 675.0f,70.0f });
	gaugeFrame->SetAnchorPoint({ 0.5f,0.5f });
	//キャラクター1の生成
	playerObject = Player::Create(device, arg_cmdList, { -10.0f,0.0f,-15.0f });

	//モデル1のセット
	playerObject->SetOBJModel(planeModel.get(), yellowBulletModel.get(), unionModel.get(), yellowBulletModel.get(), laserModel.get(), laserModel.get(), missileModel.get());

	normalEnemyObjects.resize(ENEMYNUM);

	//カメラターゲットセット
	camera->SetTarget({ 0,0,0 });
	camera->SetEye({ 0,0,-20.2f });
	//マネージャークラスにオブジェクトのアドレスを全て追加
	gameObjectManager->AddGameObject(playerObject);

	//先に敵機の生成とモデルセットとオブジェクトマネージャーへの追加をしておく
	for (int i = 0; i < normalEnemyObjects.size(); i++)
	{
		//キャラクター1の生成
		normalEnemyObjects[i] = NormalEnemy::Create(device, arg_cmdList, { 5.0f + 25.0f,12.0f,0.0f });
		//モデル1のセット
		normalEnemyObjects[i]->SetOBJModel(enemyPlaneModel.get(), redBulletModel.get(), scoreModel.get());
		normalEnemyObjects[i]->SetIsDeadFlag(true);
		normalEnemyObjects[i]->Update({ incrementValue,0,0 });
		gameObjectManager->AddGameObject(normalEnemyObjects[i]);
	}

	for (int i = 0; i < normalEnemyObjects.size(); i++)
	{
		//キャラクター1の生成
		normalEnemyObjects[i]->SetCenterPos(scrollPos);
	}



	for (int i = 0; i < tankEnemys.size(); i++)
	{
		//キャラクター1の生成
		tankEnemys[i]->SetCenterPos(scrollPos);
	}

	//キャラクター1の生成
	aircraftCarrier = AircraftCarrier::Create(device, arg_cmdList, { scrollPos + 5.0f + 25.0f,0.0f,0.0f });
	//モデル1のセット
	aircraftCarrier->SetOBJModel(aircraftCarrierPlaneModel.get(), alienModel.get(), scoreModel.get());
	aircraftCarrier->SetIsDeadFlag(true);
	aircraftCarrier->SetCenterPos(scrollPos);
	aircraftCarrier->Update({ incrementValue,0.0f,0.0f });
	gameObjectManager->AddGameObject(aircraftCarrier);

	for (int i = 0; i < aircraftCarrier->GetAliens().size(); i++)
	{
		gameObjectManager->AddGameObject(aircraftCarrier->GetAliens()[i]);
	}
}
