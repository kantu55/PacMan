/*
@file GameOverScene.cpp
*/
#include "GameOverScene.h"
#include "TitleScene.h"
#include "MainGameScene.h"
#include "GLFWEW.h"

bool GameOverScene::Initialize()
{
	spriteRenderer.Init(1000, "Res/Sprite.vert", "Res/Sprite.frag");
	sprites.reserve(100);
	// �o�b�N�O���E���h�p�̉摜
	sprBackGround.SetTexture(Texture::Image2D::Create("Res/black.tga"));
	sprBackGround.SetScale(glm::vec2(10.0f));
	sprites.push_back(sprBackGround);
	// �t�F�[�h�p�̉摜
	sprFade.SetTexture(Texture::Image2D::Create("Res/black.tga"));
	sprFade.SetScale(glm::vec2(10.0f));
	sprFade.SetColor(glm::vec4(1, 1, 1, fadeTimer));
	// �Q�[���I�[�o�[�̉摜
	sprGameOver.SetTexture(Texture::Image2D::Create("Res/GameOver.tga"));
	sprGameOver.SetPosition(glm::vec3(0, 200, 0));
	sprGameOver.SetScale(glm::vec2(3.0f));
	sprites.push_back(sprGameOver);
	// Continue�{�^���̉摜
	sprContinue.SetTexture(Texture::Image2D::Create("Res/Continue.tga"));
	sprContinue.SetPosition(glm::vec3(0, -150, 0));
	// Quit�{�^���̉摜
	sprQuit.SetTexture(Texture::Image2D::Create("Res/Quit.tga"));
	sprQuit.SetPosition(glm::vec3(0, -300, 0));
	// ���[�f�B���O�p�̉摜
	sprLoad.SetTexture(Texture::Image2D::Create("Res/Load.tga"));
	sprLoad.SetPosition(glm::vec3(700, -450, 0));

	seEnter = Audio::Engine::Instance().Prepare("Res/Audio/Start.wav");
	seEnter->SetVolume(0.1f);
	return true;
}

/*
�v���C���[�̓��͂���������
*/
void GameOverScene::ProcessInput()
{
	GLFWEW::Window& window = GLFWEW::Window::Instance();
	const GamePad gamepad = GLFWEW::Window::Instance().GetGamePad();
	if (nextScene)
		return;

	if (gamepad.buttons & GamePad::A)
	{
		seEnter->Play();
		nextScene = true;
	}

	// �{�^���̓��͎�t���Ԃ�����΁A���̏����͎��s���Ȃ�
	if (buttonTimer > 0)
		return;

	seCursor = Audio::Engine::Instance().Prepare("Res/Audio/cursor.mp3");
	// �{�^���̑I����ς��鏈��
	switch (select)
	{
	case GameOverScene::continues:
		if (gamepad.buttons & GamePad::DPAD_DOWN)
		{
			seCursor->Play();
			SelectCoolDown(Select::quit);
		}
	case GameOverScene::quit:
		if (gamepad.buttons & GamePad::DPAD_UP)
		{
			seCursor->Play();
			SelectCoolDown(Select::continues);
		}
	}
}

/*
�V�[�����X�V����

@param deltaTime �O��̍X�V����̌o�ߎ��ԁi�b�j
*/
void GameOverScene::Update(float deltaTime)
{
	spriteRenderer.BeginUpdate();
	for (const Sprite& e : sprites)
	{
		spriteRenderer.AddVertices(e);
	}
	if (buttonTimer > 0)
	{
		buttonTimer -= deltaTime;
	}
	/*
	 �I�𒆂̃{�^�����킩��₷���\�����邽�߂�
	 �g��k��������
	*/
	selectTimer += deltaTime;
	flashingTimer = selectTimer;
	switch (select)
	{
	case GameOverScene::continues:
		if (selectTimer - flashingTimer <= 0.5f)
		{
			sprContinue.SetScale(glm::vec2(1.0f));
		}
		else
		{
			sprContinue.SetScale(glm::vec2(1.1f));
		}
		sprQuit.SetScale(glm::vec2(1.0f));
		break;
	case GameOverScene::quit:
		if (selectTimer - flashingTimer <= 0.5f)
		{
			sprQuit.SetScale(glm::vec2(1.0f));
		}
		else
		{
			sprQuit.SetScale(glm::vec2(1.1f));
		}
		sprContinue.SetScale(glm::vec2(1.0f));
		break;
	}
	spriteRenderer.AddVertices(sprContinue);
	spriteRenderer.AddVertices(sprQuit);

	/*
	 �{�^�����������Ƃ��Ɏ��̃V�[���ɑJ�ڂ��鏈��
	*/
	if (nextScene)
	{
		// �t�F�[�h�̏���
		fadeTimer += deltaTime;
		sprFade.SetColor(glm::vec4(1, 1, 1, fadeTimer));
		spriteRenderer.AddVertices(sprFade);
		// ���C���Q�[���Ɉڍs����Ƃ��̓��[�h���̉摜��\��
		if (fadeTimer >= 0.9f && select == Select::continues)
		{
			spriteRenderer.AddVertices(sprLoad);
		}
		// �t�F�[�h���I�������V�[���ɑJ��
		if (fadeTimer >= 1)
		{
			switch (select)
			{
			case GameOverScene::continues:
				SceneStack::Instance().Replace(std::make_shared<MainGameScene>());
				return;
			case GameOverScene::quit:
				SceneStack::Instance().Replace(std::make_shared<TitleScene>());
				return;
			}
		}
	}
	spriteRenderer.EndUpdate();
}

/*
�V�[����`�悷��
*/
void GameOverScene::Render()
{
	const GLFWEW::Window& window = GLFWEW::Window::Instance();
	const glm::vec2 screenSize(window.Width(), window.Height());
	spriteRenderer.Draw(screenSize);
}

// �I���{�^����ς������ɓ��͎�t���Ԃ�݂���
void GameOverScene::SelectCoolDown(Select s)
{
	buttonTimer = 0.2f;
	selectTimer = 0;
	flashingTimer = 0;
	select = s;
}