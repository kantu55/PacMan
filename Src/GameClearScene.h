/*
 @file GameClearScene.h
*/
#ifndef GAMECLEARSCENE_H_INCLUDED
#define GAMECLEARSCENE_H_INCLUDED
#include "Scene.h"
#include "Sprite.h"
#include "Audio/Audio.h"
#include <vector>

/*
�X�e�[�^�X���
*/
class GameClearScene : public Scene
{
public:
	GameClearScene() : Scene("GameClearScene") {}
	virtual ~GameClearScene() = default;

	virtual bool Initialize() override;
	virtual void ProcessInput() override;
	virtual void Update(float) override;
	virtual void Render() override;
	virtual void Finalize() override {}

private:
	// UI
	std::vector<Sprite> sprites;
	SpriteRenderer spriteRenderer;
	Sprite sprBackGround;
	Sprite sprFade;
	Sprite sprGameClear;
	Sprite sprAnyButton;

	float fadeTimer = 0; // �t�F�[�h���鎞��
	bool isAnyButton = false;
	bool isExit = false;

	// �T�E���h
	Audio::SoundPtr seEnter;
};

#endif // !GAMECLEARSCENE_H_INCLUDED

