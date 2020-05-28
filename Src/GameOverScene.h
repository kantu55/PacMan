/*
@file GameOverScene.h
*/
#ifndef GAMEOVERSCENE_H_INCLUDED
#define GAMEOVERSCENE_H_INCLUDED
#include "Scene.h"
#include "Font.h"
#include "Audio/Audio.h"
#include <vector>

/*
タイトル画面
*/
class GameOverScene : public Scene
{
public:
	GameOverScene() : Scene("GameOverScene"){}
	virtual ~GameOverScene() = default;

	virtual bool Initialize() override;
	virtual void ProcessInput() override;
	virtual void Update(float) override;
	virtual void Render() override;
	virtual void Finalize() override {}

	enum Select
	{
		continues,
		quit,
	};
	Select select = Select::continues;

	void SelectCoolDown(Select s);

private:
	std::vector<Sprite> sprites;
	SpriteRenderer spriteRenderer;
	Sprite sprBackGround;
	Sprite sprFade;
	Sprite sprGameOver;
	Sprite sprContinue;
	Sprite sprQuit;
	Sprite sprLoad;

	// サウンド
	Audio::SoundPtr seEnter;
	Audio::SoundPtr seCursor;

	bool nextScene = false;

	float fadeTimer = 0;
	float selectTimer = 0;
	int flashingTimer = 0;
	float buttonTimer = 0;
	float scaleTimer = 0;
};

#endif // !GAMEOVERSCENE_H_INCLUDED

