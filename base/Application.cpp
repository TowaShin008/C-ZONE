#include "Application.h"
#include <cassert>
#include <sstream>
#include <iomanip>
#include"FbxLoader.h"
#include <iostream>

//�R���X�g���N�^
Application::Application()
{
}

//�f�X�g���N�^
Application::~Application()
{
	//�I������
	directXBase->EndWindow();
	//�폜����
	Input::Destroy();
	delete directXBase;
	Audio::Destroy();

	delete sceneManager;

	FbxLoader::GetInstance()->Finalize();

	delete camera;
}

//����������
void Application::Initialize()
{
	//�C���v�b�g�N���X��������
	Input::Create();
	//�I�[�f�B�I�N���X��������
	Audio::Create();
	//DirectXBase�N���X��������
	directXBase = new DirectXBase;

	directXBase->CreateAllObject();

	//�C���v�b�g�N���X����������
	Input::GetInstance()->Initialize(directXBase->GetWndClass(), directXBase->Gethwnd());
	//�I�[�f�B�I�N���X����������
	Audio::GetInstance()->Initialize();

	ID3D12Device* device = directXBase->GetDevice().Get();

	//�e�p�C�v���C���̐���
	CreatePipeline();

	//FBXLoader����������
	FbxLoader::GetInstance()->Create(device);
	//�J�����N���X��������
	camera = new Camera(WindowSize::window_width, WindowSize::window_height);
	//�X�v���C�g�N���X�̃f�X�N���v�^�[�q�[�v�𐶐�
	Sprite::CreateDescriptorHeap(device);
	ObjFileModel::SetDevice(device);

	if (!Sprite::LoadTexture(L"Resources/debugfont_digital_L.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(L"Resources/title.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(L"Resources/stageclear.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(L"Resources/gameclear.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(L"Resources/gameover.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(L"Resources/warning.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(L"Resources/warningBar.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(L"Resources/white_hexagon_s.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(L"Resources/push_space.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(L"Resources/stage1.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(L"Resources/stage2.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(L"Resources/stage3.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(L"Resources/window.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(L"Resources/player_texture.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(L"Resources/score.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(L"Resources/titleicon.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(L"Resources/gauge_frame.png")) {
		assert(0);
		return;
	}

	//�f�o�b�O�e�L�X�g�̏���������
	debugText.Initialize(debugTextTexNumber);

	//�J�����^�[�Q�b�g�Z�b�g
	camera->SetTarget({ 0,0,0 });
	camera->SetEye({ 0,0,-20.2f });

	//�Q�[���I�u�W�F�N�g�̐ÓI����������
	CameraSetAllObject();

	//�V�[���̐���(�e�V�[���̓V�[���}�l�[�W���N���X�Ńf���[�g����邽�߃f���[�g�����͋L�q���Ȃ��Ă悢)
	title.reset(new Title());
	stage.reset(new Stage1());
	stage2.reset(new Stage2());
	stage3.reset(new Stage3());
	bossScene1.reset(new BossScene1());
	bossScene2.reset(new BossScene2());
	bossScene3.reset(new BossScene3());
	gameClear.reset(new GameClear());
	gameOver.reset(new GameOver());
	resultScene.reset(new ResultScene());

	//�V�[���}�l�[�W���N���X�̐���
	sceneManager = new SceneManager(device,directXBase->GetCmdList().Get());
	//�V�[���}�l�[�W���[�ւ̒ǉ�(�ǉ�����ɂ�GameScene.h����enum Scene�ɒǉ����邱��)
	AddScene();
}
//�S�ẴI�u�W�F�N�g�ɋ��ʂ̃J�������Z�b�g(�V�����I�u�W�F�N�g�N���X�����������ꍇ�͂����ŃJ�������Z�b�g����̂�Y�ꂸ��)
void Application::CameraSetAllObject()
{
	ObjFileCharacter::SetCamera(camera);

	Player::SetCamera(camera);
	UnionCharacter::SetCamera(camera);
	Bullet::SetCamera(camera);
	Laser::SetCamera(camera);
	Missile::SetCamera(camera);

	NormalEnemy::SetCamera(camera);
	TankEnemy::SetCamera(camera);
	Alien::SetCamera(camera);
	AircraftCarrier::SetCamera(camera);
	Boss::SetCamera(camera);
	Squid::SetCamera(camera);
	SquidEye::SetCamera(camera);
	FinalBoss::SetCamera(camera);
	FinalBossEye::SetCamera(camera);
	HomingBullet::SetCamera(camera);

	Block::SetCamera(camera);
}

void Application::CreatePipeline()
{
	ID3D12Device* device = directXBase->GetDevice().Get();
	//�e�Ȃ��̃I�u�W�F�N�g�p�̃p�C�v���C������
	RootSignature::InitializeSimpleRootSignature(device);
	PipelineState::InitializeSimplePipelineState(device);
	//�ʏ�I�u�W�F�N�g�p�̃p�C�v���C������
	RootSignature::InitializeBasicRootSignature(device);
	PipelineState::InitializeBasicPipelineState(device);
	//�X�v���C�g�p�̃p�C�v���C������
	RootSignature::InitializeSpriteRootSignature(device);
	PipelineState::InitializeSpritePipelineState(device);
	//�X���C�h����X�v���C�g�p�̃p�C�v���C������
	RootSignature::InitializeSlideSpriteRootSignature(device);
	PipelineState::InitializeSlideSpritePipelineState(device);
	//�u���[���X�v���C�g�p�̃p�C�v���C������
	RootSignature::InitializeBloomSpriteRootSignature(device);
	PipelineState::InitializeBloomSpritePipelineState(device);
	//FBX�p�̃p�C�v���C������
	RootSignature::InitializeFBXRootSignature(device);
	PipelineState::InitializeFBXPipelineState(device);
	//���C�g����I�u�W�F�N�g�p�̃p�C�v���C������
	RootSignature::InitializeLightObjectRootSignature(device);
	PipelineState::InitializeLightObjectPipelineState(device);
	//�e�ۗp�̃p�C�v���C������
	RootSignature::InitializeBulletRootSignature(device);
	PipelineState::InitializeBulletPipelineState(device);
	//�p�[�e�B�N���p�̃p�C�v���C������
	RootSignature::InitializeParticleRootSignature(device);
	PipelineState::InitializeParticlePipelineState(device);
	//�k���o�b�t�@�p�̃p�C�v���C������
	RootSignature::InitializeShrinkTextureRootSignature(device);
	PipelineState::InitializeShrinkTexturePipelineState(device);
	//�u���[���p�̃p�C�v���C������
	RootSignature::InitializeBloomPostEffectRootSignature(device);
	PipelineState::InitializeBloomPostEffectPipelineState(device);
	//�K�E�V�A���u���[�p�̃p�C�v���C������
	PipelineState::InitializeGaussianPostEffectPipelineState(device);
	//�t�F�[�h�A�E�g�p�̃p�C�v���C������
	RootSignature::InitializeFadeOutPostEffectRootSignature(device);
	PipelineState::InitializeFadeOutPostEffectPipelineState(device);
	//���H�p�̃p�C�v���C������
	RootSignature::InitializeDropNormalMapPostEffectRootSignature(device);
	PipelineState::InitializeDropNormalMapPostEffectPipelineState(device);
	//�Ђъ���p�̃p�C�v���C������
	RootSignature::InitializeGlassNormalMapPostEffectRootSignature(device);
	PipelineState::InitializeGlassNormalMapPostEffectPipelineState(device);
	//�X�L�����m�C�Y�p�̃p�C�v���C������
	RootSignature::InitializeNoisePostEffectRootSignature(device);
	PipelineState::InitializeNoisePostEffectPipelineState(device);
	//�|�X�g�G�t�F�N�g�p�̃p�C�v���C������
	RootSignature::InitializePostEffectRootSignature(device);
	PipelineState::InitializePostEffectPipelineState(device);
	//�f�B�]���u�p�̃p�C�v���C������
	RootSignature::InitializeDissolvePostEffectRootSignature(device);
	PipelineState::InitializeDissolvePostEffectPipelineState(device);
}

void Application::AddScene()
{
	sceneManager->Add(Scene::TITLE    , title.get()      , camera);
	sceneManager->Add(Scene::STAGE1   , stage.get()      , camera);
	sceneManager->Add(Scene::BOSS1    , bossScene1.get() , camera);
	sceneManager->Add(Scene::STAGE2   , stage2.get()     , camera);
	sceneManager->Add(Scene::BOSS2    , bossScene2.get() , camera);
	sceneManager->Add(Scene::STAGE3   , stage3.get()     , camera);
	sceneManager->Add(Scene::BOSS3    , bossScene3.get() , camera);
	sceneManager->Add(Scene::GAMECLEAR, gameClear.get()  , camera);
	sceneManager->Add(Scene::GAMEOVER , gameOver.get()   , camera);
	sceneManager->Add(Scene::RESULT   , resultScene.get(), camera);
}

//�X�V����
void Application::Update()
{
	//�C���v�b�g�X�V����
	Input::GetInstance()->Update();

	//�v���O�����I������
	if (directXBase->CreateMessage()|| Input::GetInstance()->KeyTriggerState(DIK_ESCAPE))
	{
		endFlag = true;
	}

	//�V�[���}�l�[�W���[�̍X�V����
	sceneManager->Update(&debugText);
}

//�`�揈��
void Application::Draw()
{
	sceneManager->DrawRenderTexture(&debugText);


	//�`��̊J�n
	directXBase->BeginDraw();
	//�[�x�o�b�t�@�̃N���A
	directXBase->DepthClear();

	sceneManager->PostEffectDraw();

	directXBase->DepthClear();

	sceneManager->Draw(&debugText);


	//�`��̏I��
	directXBase->EndDraw();

}

void Application::DrawUI()
{
	//ImGui::Begin("Metarial");
	//ImGui::SetWindowPos(ImVec2(0, 0));
	//ImGui::SetWindowSize(ImVec2(300, 130));
	//ImGui::ColorEdit3("baseColor", baseColor, ImGuiColorEditFlags_Float);
	//ImGui::SliderFloat("metalness", &metalness, 0, 1);
	//ImGui::SliderFloat("specular", &specular, 0, 1);
	//ImGui::SliderFloat("roughness", &roughness, 0, 1);
	//ImGui::End();
}
