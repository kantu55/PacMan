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
シーンを初期化する

@retval true  初期化成功
@retval false 初期化失敗 ゲーム進行負荷につき、プログラムを終了すること
*/
bool TitleScene::Initialize()
{
	spriteRenderer.Init(1000, "Res/Sprite.vert", "Res/Sprite.frag");
	sprites.reserve(100);
	// バックグラウンドの黒部分
	Sprite spr(Texture::Image2D::Create("Res/ActionTitleBg.dds"));
	spr.SetScale(glm::vec2(1.5f));
	sprites.push_back(spr);
	// タイトルロゴの画像
	sprTitle.SetTexture(Texture::Image2D::Create("Res/Title.tga"));
	sprTitle.SetScale(glm::vec2(3.0f));
	sprTitle.SetPosition(glm::vec3(0, 200, 0));
	sprites.push_back(sprTitle);
	// スタートボタンの画像
	sprGameStart.SetTexture(Texture::Image2D::Create("Res/GameStart.tga"));
	sprGameStart.SetPosition(glm::vec3(0,-200,0));
	// チュートリアルのボタン
	sprTutorialBtn.SetTexture(Texture::Image2D::Create("Res/TutorialBtn.tga"));
	sprTutorialBtn.SetPosition(glm::vec3(0, -300, 0));
	// ゲーム終了のボタン
	sprExit.SetTexture(Texture::Image2D::Create("Res/Exit.tga"));
	sprExit.SetPosition(glm::vec3(0, -400, -200));
	// チュートリアルの画像
	sprTutorial.SetTexture(Texture::Image2D::Create("Res/Tutorial.tga"));
	sprTutorial.SetScale(glm::vec2(scaleTimer));
	// フェード用の画像
	sprFade.SetTexture(Texture::Image2D::Create("Res/black.tga"));
	sprFade.SetScale(glm::vec2(10.0f));
	sprFade.SetColor(glm::vec4(1, 1, 1, fadeTimer));
	sprites.push_back(sprFade);
	// ローディング用の画像
	sprLoad.SetTexture(Texture::Image2D::Create("Res/Load.tga"));
	sprLoad.SetPosition(glm::vec3(700, -450, 0));
	// チュートリアルのテキスト画像(ロード時)
	sprTutorialText.SetTexture(Texture::Image2D::Create("Res/Tutorial_font.tga"));
	sprTutorialText.SetPosition(glm::vec3(0, 100, 0));
	// チュートリアル時に表示するマップの画像(ロード時)
	sprMap.SetTexture(Texture::Image2D::Create("Res/Tutorial_Map.tga"));
	sprMap.SetPosition(glm::vec3(-500, 0, 0));
	sprMap.SetScale(glm::vec2(0.5f));

	// BGMを再生する
	bgm = Audio::Engine::Instance().Prepare("Res/Audio/Title.mp3");
	bgm->Play(Audio::Flag_Loop);

	// 決定ボタン音
	seEnter = Audio::Engine::Instance().Prepare("Res/Audio/Start.wav");
	seEnter->SetVolume(0.1f);
	
	fontRenderer.Init(1000);
	fontRenderer.LoadFromFile("Res/font.fnt");

	return true;
}

/*
プレイヤーの入力を処理する
*/
void TitleScene::ProcessInput()
{
	GLFWEW::Window& window = GLFWEW::Window::Instance();
	const GamePad gamepad = GLFWEW::Window::Instance().GetGamePad();



	// チュートリアル画面が表示されてたらボタン選択を受け付けない
	if (buttonTimer <= 0 && isTutorial)
	{
		// キーボードを押すとチュートリアル画面が終わる
		if (gamepad.buttons & GamePad::A)
		{
			isTutorial = false;
			buttonTimer = 0.5f;
		}
	}
	// ボタン選択と決定の処理
	if (buttonTimer <= 0 && !isTutorial)
	{
		// 選択ボタン音
		seCursor = Audio::Engine::Instance().Prepare("Res/Audio/cursor.mp3");
		if (timer > 0)
			return;

		// メニューの選択
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
シーンを更新する

@param deltaTime 前回の更新からの経過時間（秒）
*/
void TitleScene::Update(float deltaTime)
{
	const GLFWEW::Window& window = GLFWEW::Window::Instance();
	const float w = window.Width();
	const float h = window.Height();
	const float lineHenght = fontRenderer.LineHeight();

	// 選択ボタンを押下した後のインターバル
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

	// 選択されてる項目は文字を拡大縮小させる
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
	// シーン切り替え待ち
	if (timer > 0)
	{
		timer -= deltaTime;
		if (timer <= 0)
		{
			bgm->Stop();
			isNext = true;
		}
	}

	// メインゲームに入るときにフェードする
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
シーンを描画する
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