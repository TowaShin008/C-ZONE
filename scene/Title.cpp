#include "Title.h"

Title::Title()
{
}

Title::~Title()
{
}

void Title::Initialize(unsigned int arg_score)
{
	endFlag = false;
	delayTime = 0;
	score = 0;
	blurFlag = false;
	beginFlag = true;
	pushButtonFlag = false;
}

void Title::CreateAllObject(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList)
{
	gameObjectManager.reset(new GameObjectManager());
	gameObjectManager->Initialize(score);

	CreatePostEffect();

	//�J�����^�[�Q�b�g�Z�b�g
	camera->SetTarget({ 0,0,0 });
	camera->SetEye({ 0,0,-20.2f });

	//�}�l�[�W���[�N���X�ɓ����Ă���I�u�W�F�N�g�S�Ă��A�b�v�f�[�g
	gameObjectManager->UpdateAllObject({ 0.0f,0.0f,0.0f });

	LoadAllModel();
	//�e�N�X�`���̐�������
	titleTexture.reset(Sprite::Create(L"Resources/title.png", { WindowSize::window_width / 2.0f,WindowSize::window_height / 2.0f }));
	titleTexture->SetAnchorPoint({ 0.5f,0.5f });

	titleIcon.reset(Sprite::Create(L"Resources/titleicon.png", { WindowSize::window_width / 2.0f - 100.0f,WindowSize::window_height / 2.0f - 50.0f}));
	titleIcon->SetAnchorPoint({ 0.5f,0.5f });

	titleButtomTexture.reset(Sprite::Create(L"Resources/push_space.png", { WindowSize::window_width / 2.0f,WindowSize::window_height / 5.0f * 4.0f }));
	titleButtomTexture->SetAnchorPoint({ 0.5f,0.5f });
	titleButtomTexture->SetScale({ 480.0f,75.0f });


	//�w�i�̃O���f�[�V���������邽�߂̔z��
	gradationMap = {
	{10,11,12,13,14,15,16,17},
	{ 9,10,11,12,13,14,15,16},
	{ 8, 9,10,11,12,13,14,15},
	{ 7, 8, 9,10,11,12,13,14},
	{ 6, 7, 8, 9,10,11,12,13},
	{ 5, 6, 7, 8, 9,10,11,12},
	{ 4, 5, 6, 7, 8, 9,10,11},
	{ 3, 4, 5, 6, 7, 8, 9,10},
	{ 2, 3, 4, 5, 6, 7, 8, 9},
	{ 1, 2, 3, 4, 5, 6, 7, 8},
	{ 0, 1, 2, 3, 4, 5, 6, 7},
	};

	bgHexagon.resize(gradationMap.size());

	for (int i = 0; i < gradationMap.size(); i++)
	{
		bgHexagon[i].resize(gradationMap[i].size());

		for (int j = 0; j < gradationMap[i].size(); j++)
		{
			//�����ʒu����s���݂��Ⴂ�ɂ��炷
			if (j % 2 == 1)
			{
				bgHexagon[i][j].reset(Sprite::Create(L"Resources/white_hexagon_s.png", { 130.0f * i - 35.0f, 115.0f * j - 200.0f }));
				bgHexagon[i][j]->SetLugTime(gradationMap[i][j] * 30);
				bgHexagon[i][j]->SetScale({ 170.0f,170.0f });
			}
			else
			{
				bgHexagon[i][j].reset(Sprite::Create(L"Resources/white_hexagon_s.png", { 130.0f * i - 100.0f, 115.0f * j - 200.0f }));
				bgHexagon[i][j]->SetLugTime(gradationMap[i][j] * 30);
				bgHexagon[i][j]->SetScale({ 170.0f,170.0f });
			}
			bgHexagon[i][j]->SetLugTime(gradationMap[i][j] * 30);
			bgHexagon[i][j]->SetAlphaFlag(false);
			bgHexagon[i][j]->SetColor(blue);
		}
	}
}

void Title::CreatePostEffect()
{
	//�|�X�g�G�t�F�N�g��������
	bloom.reset(new Bloom());
	bloom->Initialize();
	bloom->SetFadeOutFlag(false);
}

void Title::LoadAllModel()
{

}

void Title::Update(DebugText* debugText)
{
	gameObjectManager->UpdateAllObject({ 0.0f,0.0f,0.0f });

	for (int i = 0; i < gradationMap.size(); i++)
	{
		for (int j = 0; j < gradationMap[i].size(); j++)
		{
			bgHexagon[i][j]->GradatioUpdate();
		}
	}

	if (pushButtonFlag == false && Input::GetInstance()->KeyTriggerState(DIK_SPACE))
	{
		bloom->SetFadeOutFlag(true);
		pushButtonFlag = true;
	}

	if (pushButtonFlag && bloom->GetFadeOutCount() <= 0.0f)
	{
		endFlag = true;
	}
}

void Title::DrawRenderTexture(ID3D12GraphicsCommandList* cmdList, DebugText* debugText)
{
	bloom->PreDrawScene(cmdList);

	gameObjectManager->DrawAllObject();

	Sprite::BeginDraw(cmdList, false);
	//�w�i�`��
	for (int i = 0; i < gradationMap.size(); i++)
	{
		for (int j = 0; j < gradationMap[i].size(); j++)
		{
			bgHexagon[i][j]->Draw();
		}
	}

	titleIcon->Draw();

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
	Vector4 buttomTextureColor = titleButtomTexture->GetColor();
	buttomTextureColor.w = sinf((alphaCount / 180.0f) * XM_PI);

	titleButtomTexture->SetColor(buttomTextureColor);
	titleButtomTexture->Draw();
	
	
	Sprite::EndDraw();



	Sprite::BeginDraw(cmdList,true);

	titleTexture->Draw();

	Sprite::EndDraw();

	bloom->PostDrawScene(cmdList);
}

void Title::PostEffectDraw(ID3D12GraphicsCommandList* cmdList)
{
	//�|�X�g�G�t�F�N�g�`�揈��
	bloom->Draw(cmdList);
}

void Title::Draw(ID3D12GraphicsCommandList* cmdList, DebugText* debugText)
{

}

Scene Title::Next()
{
	return Scene::STAGE1;
}