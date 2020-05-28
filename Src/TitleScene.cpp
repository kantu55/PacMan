/*
@file TitleScene.cpp
*/
#include "TitleScene.h"
#include "MainGameScene.h"
#include "GameClearScene.h"
#include "GLFWEW.h"
#include <Windows.h>

bool quit;

/*
�V�[��������������

@retval true  ����������
@retval false ���������s �Q�[���i�s���ׂɂ��A�v���O�������I�����邱��
*/
bool TitleScene::Initialize()
{
	spriteRenderer.Init(1000, "Res/Sprite.vert", "Res/Sprite.frag");
	sprites.reserve(100);
	// �o�b�N�O���E���h�̍�����
	Sprite spr(Texture::Image2D::Create("Res/ActionTitleBg.dds"));
	spr.SetScale(glm::vec2(1.5f));
	sprites.push_back(spr);
	// �^�C�g�����S�̉摜
	sprTitle.SetTexture(Texture::Image2D::Create("Res/Title.tga"));
	sprTitle.SetScale(glm::vec2(3.0f));
	sprTitle.SetPosition(glm::vec3(0, 200, 0));
	sprites.push_back(sprTitle);
	// �X�^�[�g�{�^���̉摜
	sprGameStart.SetTexture(Texture::Image2D::Create("Res/GameStart.tga"));
	sprGameStart.SetPosition(glm::vec3(0,-200,0));
	// �`���[�g���A���̃{�^��
	sprTutorialBtn.SetTexture(Texture::Image2D::Create("Res/TutorialBtn.tga"));
	sprTutorialBtn.SetPosition(glm::vec3(0, -300, 0));
	// �Q�[���I���̃{�^��
	sprExit.SetTexture(Texture::Image2D::Create("Res/Exit.tga"));
	sprExit.SetPosition(glm::vec3(0, -400, -200));
	// �`���[�g���A���̉摜
	sprTutorial.SetTexture(Texture::Image2D::Create("Res/Tutorial.tga"));
	sprTutorial.SetScale(glm::vec2(scaleTimer));
	// �t�F�[�h�p�̉摜
	sprFade.SetTexture(Texture::Image2D::Create("Res/black.tga"));
	sprFade.SetScale(glm::vec2(10.0f));
	sprFade.SetColor(glm::vec4(1, 1, 1, fadeTimer));
	sprites.push_back(sprFade);
	// ���[�f�B���O�p�̉摜
	sprLoad.SetTexture(Texture::Image2D::Create("Res/Load.tga"));
	sprLoad.SetPosition(glm::vec3(700, -450, 0));
	// �`���[�g���A���̃e�L�X�g�摜(���[�h��)
	sprTutorialText.SetTexture(Texture::Image2D::Create("Res/Tutorial_font.tga"));
	sprTutorialText.SetPosition(glm::vec3(0, 100, 0));
	// �`���[�g���A�����ɕ\������}�b�v�̉摜(���[�h��)
	sprMap.SetTexture(Texture::Image2D::Create("Res/Tutorial_Map.tga"));
	sprMap.SetPosition(glm::vec3(-500, 0, 0));
	sprMap.SetScale(glm::vec2(0.5f));

	// BGM���Đ�����
	bgm = Audio::Engine::Instance().Prepare("Res/Audio/Title.mp3");
	bgm->Play(Audio::Flag_Loop);

	// ����{�^����
	seEnter = Audio::Engine::Instance().Prepare("Res/Audio/Start.wav");
	seEnter->SetVolume(0.1f);
	
	fontRenderer.Init(1000);
	fontRenderer.LoadFromFile("Res/font.fnt");

	return true;
}

/*
�v���C���[�̓��͂���������
*/
void TitleScene::ProcessInput()
{
	GLFWEW::Window& window = GLFWEW::Window::Instance();
	const GamePad gamepad = GLFWEW::Window::Instance().GetGamePad();



	// �`���[�g���A����ʂ��\������Ă���{�^���I�����󂯕t���Ȃ�
	if (buttonTimer <= 0 && isTutorial)
	{
		// �L�[�{�[�h�������ƃ`���[�g���A����ʂ��I���
		if (gamepad.buttons & GamePad::A)
		{
			isTutorial = false;
			buttonTimer = 0.5f;
		}
	}
	// �{�^���I���ƌ���̏���
	if (buttonTimer <= 0 && !isTutorial)
	{
		// �I���{�^����
		seCursor = Audio::Engine::Instance().Prepare("Res/Audio/cursor.mp3");
		if (timer > 0)
			return;

		// ���j���[�̑I��
		switch (select)
		{
		case TitleScene::gamestart:
			if (timer <= 0 && gamepad.buttons & GamePad::A)
			{
				seEnter->Play();
				timer = 2.0f;
			}
			if (gamepad.buttons & GamePad::DPAD_DOWN)
			{
				seCursor->Play();
				SelectCoolDown(Select::tutorial);
			}
			break;
		case TitleScene::tutorial:
			if (gamepad.buttons & GamePad::DPAD_UP)
			{
				seCursor->Play();
				SelectCoolDown(Select::gamestart);
			}
			if (gamepad.buttons & GamePad::DPAD_DOWN)
			{
				seCursor->Play();
				SelectCoolDown(Select::exit);
			}
			if (gamepad.buttons & GamePad::A)
			{
				seEnter->Play();
				isTutorial = true;
				buttonTimer = 0.5f;
			}
			break;
		case TitleScene::exit:
			if (gamepad.buttons & GamePad::DPAD_UP)
			{
				seCursor->Play();
				SelectCoolDown(Select::tutorial);
			}
			if (gamepad.buttons & GamePad::A)
			{
				seEnter->Play();
				quit = true;
			}
			break;
		}
	}
}

/*
�V�[�����X�V����

@param deltaTime �O��̍X�V����̌o�ߎ��ԁi�b�j
*/
void TitleScene::Update(float deltaTime)
{
	const GLFWEW::Window& window = GLFWEW::Window::Instance();
	const float w = window.Width();
	const float h = window.Height();
	const float lineHenght = fontRenderer.LineHeight();

	// �I���{�^��������������̃C���^�[�o��
	if (buttonTimer > 0)
	{
		buttonTimer -= deltaTime;
	}
	else
	{
		buttonTimer = 0;
	}

	spriteRenderer.BeginUpdate();
	for (const Sprite& e : sprites)
	{
		spriteRenderer.AddVertices(e);
	}

	if (isTutorial)
	{
		scaleTimer += deltaTime;
		if (scaleTimer >= 1)
		{
			scaleTimer = 1.0f;
		}
	}
	else if (!isTutorial)
	{
		scaleTimer -= deltaTime;
		if (scaleTimer <= 0)
		{
			scaleTimer = 0;
		}
	}

	selectTimer += deltaTime;
	flashingTimer = selectTimer;

	// �I������Ă鍀�ڂ͕������g��k��������
	switch (select)
	{
	case TitleScene::gamestart:
		
		if (selectTimer - flashingTimer <= 0.5f)
		{
			sprGameStart.SetScale(glm::vec2(1.0f));
		}
		else
		{
			sprGameStart.SetScale(glm::vec2(1.1f));
		}
		sprTutorialBtn.SetScale(glm::vec2(1.0f));
		sprExit.SetScale(glm::vec2(1.0f));
		break;
	case TitleScene::tutorial:
		if (selectTimer - flashingTimer <= 0.5f)
		{
			sprTutorialBtn.SetScale(glm::vec2(1.0f));
		}
		else
		{
			sprTutorialBtn.SetScale(glm::vec2(1.1f));
		}
		sprGameStart.SetScale(glm::vec2(1.0f));
		sprExit.SetScale(glm::vec2(1.0f));
		break;
	case TitleScene::exit:
		if (selectTimer - flashingTimer <= 0.5f)
		{
			sprExit.SetScale(glm::vec2(1.0f));
		}
		else
		{
			sprExit.SetScale(glm::vec2(1.1f));
		}
		sprGameStart.SetScale(glm::vec2(1.0f));
		sprTutorialBtn.SetScale(glm::vec2(1.0f));
		break;
	}

	sprTutorial.SetScale(glm::vec2(scaleTimer));
	spriteRenderer.AddVertices(sprGameStart);
	spriteRenderer.AddVertices(sprTutorialBtn);
	spriteRenderer.AddVertices(sprExit);
	spriteRenderer.AddVertices(sprTutorial);
	// �V�[���؂�ւ��҂�
	if (timer > 0)
	{
		timer -= deltaTime;
		if (timer <= 0)
		{
			bgm->Stop();
			isNext = true;
		}
	}

	// ���C���Q�[���ɓ���Ƃ��Ƀt�F�[�h����
	if (isNext)
	{
		fadeTimer += deltaTime;
		sprFade.SetColor(glm::vec4(1, 1, 1, fadeTimer));
		spriteRenderer.AddVertices(sprFade);
		if (fadeTimer >= 0.9)
		{
			spriteRenderer.AddVertices(sprLoad);
		}
		spriteRenderer.EndUpdate();
		if (fadeTimer >= 1)
		{
			SceneStack::Instance().Replace(std::make_shared<MainGameScene>());
			return;
		}
	}
	spriteRenderer.EndUpdate();
}

/*
�V�[����`�悷��
*/
void TitleScene::Render()
{
	const GLFWEW::Window& window = GLFWEW::Window::Instance();
	const glm::vec2 screenSize(window.Width(), window.Height());
	fontRenderer.Draw(screenSize);
	spriteRenderer.Draw(screenSize);
}

void TitleScene::SelectCoolDown(Select s)
{
	buttonTimer = 0.2f;
	selectTimer = 0;
	flashingTimer = 0;
	select = s;
}