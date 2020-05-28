/*
 @file GameClearScene.cpp
*/
#include "GameClearScene.h"
#include "TitleScene.h"
#include "GLFWEW.h"
#include <Windows.h>

bool GameClearScene::Initialize()
{
	spriteRenderer.Init(1000, "Res/Sprite.vert", "Res/Sprite.frag");
	sprites.reserve(100);
	// バックグラウンド用の画像
	sprBackGround.SetTexture(Texture::Image2D::Create("Res/White.tga"));
	sprites.push_back(sprBackGround);
	// フェード用の画像
	sprFade.SetTexture(Texture::Image2D::Create("Res/black.tga"));
	sprFade.SetScale(glm::vec2(10.0f));
	sprFade.SetColor(glm::vec4(1, 1, 1, fadeTimer));
	sprites.push_back(sprFade);
	// ゲームクリアの画像
	sprGameClear.SetTexture(Texture::Image2D::Create("Res/GameClear.tga"));
	sprGameClear.SetPosition(glm::vec3(0, 100, 0));
	sprGameClear.SetScale(glm::vec2(3.0f));
	// ダイアログの画像
	sprAnyButton.SetTexture(Texture::Image2D::Create("Res/AnyButton.tga"));
	sprAnyButton.SetPosition(glm::vec3(0, -150, 0));
	seEnter = Audio::Engine::Instance().Prepare("Res/Audio/Start.wav");
	seEnter->SetVolume(0.1f);
	return true;
}

void GameClearScene::ProcessInput()
{
	GLFWEW::Window& window = GLFWEW::Window::Instance();
	const GamePad gamepad = GLFWEW::Window::Instance().GetGamePad();
	// ボタンが押されたらタイトルに戻るフラグをTrueにする
	if (isAnyButton || gamepad.buttons)
	{
		seEnter->Play();
		isExit = true;
	}
}

void GameClearScene::Update(float deltaTime)
{
	spriteRenderer.BeginUpdate();
	spriteRenderer.AddVertices(sprBackGround);
	spriteRenderer.AddVertices(sprGameClear);
	spriteRenderer.AddVertices(sprAnyButton);
	// タイトルに戻るフラグが立ったらフェードの処理を入れる
	if (isExit)
	{
		// フェードの処理
		fadeTimer += deltaTime;
		sprFade.SetColor(glm::vec4(1, 1, 1, fadeTimer));
		spriteRenderer.AddVertices(sprFade);
		// フェードしたら(画面が黒くなったら)タイトルへ戻る
		if (fadeTimer >= 1.0f)
		{
			SceneStack::Instance().Replace(std::make_shared<TitleScene>());
			return;
		}
	}
	spriteRenderer.EndUpdate();
}

/*
シーンを描画する
*/
void GameClearScene::Render()
{
	const GLFWEW::Window& window = GLFWEW::Window::Instance();
	const glm::vec2 screenSize(window.Width(), window.Height());
	spriteRenderer.Draw(screenSize);
}