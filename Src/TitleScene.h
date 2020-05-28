/*
@file TitleScene.h
*/
#ifndef TITLESCENE_H_INCLUDED
#define TITLESCENE_H_INCLUDED
#include "Scene.h"
#include "Sprite.h"
#include "Font.h"
#include "Audio/Audio.h"
#include <vector>

/*
ƒ^ƒCƒgƒ‹‰æ–Ê
*/
class TitleScene : public Scene
{
public:
	TitleScene() : Scene("TitleScene"){}
	virtual ~TitleScene() = default;

	virtual bool Initialize() override;
	virtual void ProcessInput() override;
	virtual void Update(float) override;
	virtual void Render() override;
	virtual void Finalize() override {}

	enum Select
	{
		gamestart,
		tutorial,
		exit,
	};
	Select select = Select::gamestart;

	void SelectCoolDown(Select s);

private:
	std::vector<Sprite> sprites;
	SpriteRenderer spriteRenderer;
	FontRenderer fontRenderer;
	Sprite sprFade;
	Sprite sprGameStart;
	Sprite sprExit;
	Sprite sprTutorialBtn;
	Sprite sprTutorial;
	Sprite sprTitle;
	Sprite sprTitleBg;
	Sprite sprLoad;
	Sprite sprMap;
	Sprite sprTutorialText;
	Audio::SoundPtr bgm;
	Audio::SoundPtr seEnter;
	Audio::SoundPtr seCursor;
	float timer = 0;
	float fadeTimer = 0;
	float selectTimer = 0;
	int flashingTimer = 0;
	float buttonTimer = 0;
	float scaleTimer = 0;
	bool isNext = false;
	bool once = false;
	bool isTutorial = false;
	int Count = 0;
};

#endif // !TITLESCENE_H_INCLUDED