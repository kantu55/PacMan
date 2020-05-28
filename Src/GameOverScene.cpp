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
	// バックグラウンド用の画像
	sprBackGround.SetTexture(Texture::Image2D::Create("Res/black.tga"));
	sprBackGround.SetScale(glm::vec2(10.0f));
	sprites.push_back(sprBackGround);
	// フェード用の画像
	sprFade.SetTexture(Texture::Image2D::Create("Res/black.tga"));
	sprFade.SetScale(glm::vec2(10.0f));
	sprFade.SetColor(glm::vec4(1, 1, 1, fadeTimer));
	// ゲームオーバーの画像
	sprGameOver.SetTexture(Texture::Image2D::Create("Res/GameOver.tga"));
	sprGameOver.SetPosition(glm::vec3(0, 200, 0));
	sprGameOver.SetScale(glm::vec2(3.0f));
	sprites.push_back(sprGameOver);
	// Continueボタンの画像
	sprContinue.SetTexture(Texture::Image2D::Create("Res/Continue.tga"));
	sprContinue.SetPosition(glm::vec3(0, -150, 0));
	// Quitボタンの画像
	sprQuit.SetTexture(Texture::Image2D::Create("Res/Quit.tga"));
	sprQuit.SetPosition(glm::vec3(0, -300, 0));
	// ローディング用の画像
	sprLoad.SetTexture(Texture::Image2D::Create("Res/Load.tga"));
	sprLoad.SetPosition(glm::vec3(700, -450, 0));

	seEnter = Audio::Engine::Instance().Prepare("Res/Audio/Start.wav");
	seEnter->SetVolume(0.1f);
	return true;
}

/*
プレイヤーの入力を処理する
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

	// ボタンの入力受付時間があれば、下の処理は実行しない
	if (buttonTimer > 0)
		return;

	seCursor = Audio::Engine::Instance().Prepare("Res/Audio/cursor.mp3");
	// ボタンの選択を変える処理
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
シーンを更新する

@param deltaTime 前回の更新からの経過時間（秒）
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
	 選択中のボタンをわかりやすく表示するために
	 拡大縮小させる
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
	 ボタンを押したときに次のシーンに遷移する処理
	*/
	if (nextScene)
	{
		// フェードの処理
		fadeTimer += deltaTime;
		sprFade.SetColor(glm::vec4(1, 1, 1, fadeTimer));
		spriteRenderer.AddVertices(sprFade);
		// メインゲームに移行するときはロード中の画像を表示
		if (fadeTimer >= 0.9f && select == Select::continues)
		{
			spriteRenderer.AddVertices(sprLoad);
		}
		// フェードが終わったらシーンに遷移
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
シーンを描画する
*/
void GameOverScene::Render()
{
	const GLFWEW::Window& window = GLFWEW::Window::Instance();
	const glm::vec2 screenSize(window.Width(), window.Height());
	spriteRenderer.Draw(screenSize);
}

// 選択ボタンを変えた時に入力受付時間を設ける
void GameOverScene::SelectCoolDown(Select s)
{
	buttonTimer = 0.2f;
	selectTimer = 0;
	flashingTimer = 0;
	select = s;
}