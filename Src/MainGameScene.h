/*
@file MainGameScene.h
*/
#ifndef MAINGAMESCENE_H_INCLUDED
#define MAINGAMESCENE_H_INCLUDED
#include "Scene.h"
#include "TitleScene.h"
#include "Font.h"
#include "Mesh.h"
#include "Terrain.h"
#include "Actor.h"
#include "PlayerActor.h"
#include "Enemy.h"
#include "LoiteringEnemy.h"
#include "OverlookEnemy.h"
#include "GatekeeperEnemy.h"
#include "HuntEnemy.h"
#include "JizoActor.h"
#include "Light.h"
#include "FramebufferObject.h"
#include <random>
#include <vector>

/*
メインゲーム画面
*/
class MainGameScene : public Scene
{
public:
	MainGameScene() : Scene("MainGameScene") {}
	virtual ~MainGameScene() = default;

	virtual bool Initialize() override;
	virtual void ProcessInput() override;
	virtual void Update(float) override;
	virtual void Render() override;
	virtual void Finalize() override {}

	bool HandleJizoEffects(int id, const glm::vec3& pos);
	StaticMeshActorPtr CreateStoneWall(glm::vec3 pos, float rot, int axsis, int size); // 石壁を生成
	void CreateSpriteWall(glm::vec3, int axsis, int index); //ミニマップの壁用スプライトの生成
	bool EventFragPosition(glm::vec3 playerPos, glm::vec3 pos);
	bool enemyValid = false;
	bool relayFrag = false;
	bool isWayPoint = false;
	bool isHunt = false;

	//プレイヤーのカメラ位置と注視点
	glm::vec3 viewPos;
	glm::vec3 targetPos;
	glm::vec3 viewoffset;
	glm::vec3 targetoffset;
	glm::vec3 vCamera;
	glm::vec3 vTraget;
	glm::vec3 vCameraTraget;
	glm::vec3 vTragetoffset;
	glm::vec3 cameraVelocity;
	glm::vec3 TragetVelocity;

	enum GameState
	{
		play,
		clear,
		over,
	};
	GameState state = GameState::play;

	int Axsis; // 0 = x, 1 = z
	float cameraRotate;
	float cameraRadius = 0;

private:
	bool flag = false;
	std::mt19937 rand;
	int jizoId = -1;
	int jizoCount = 0;
	std::vector<Sprite> sprites;
	SpriteRenderer spriteRenderer;
	FontRenderer fontRenderer;
	Mesh::Buffer meshBuffer;
	Terrain::HeightMap heightMap;
	const int x = 0;
	const int z = 1;
	Sprite sprMiniMap;
	Sprite sprEnemy;
	glm::vec3 sprEnemyPos = glm::vec3(-415, 1340, 0);
	Sprite sprPlayer;
	Sprite sprWall;
	Sprite sprJizo;
	Sprite sprGoal;
	Sprite sprBlackFade;
	Sprite sprWhiteFade;

	Audio::SoundPtr bgm;
	Audio::SoundPtr seGetJizo;
	Audio::SoundPtr seDamage;

	float fadeTimer = 1;

	FontRenderer fntJizo;
	FontRenderer fntCount;
	FontRenderer fntLoad;

	struct Camera
	{
		glm::vec3 target = glm::vec3(100, 0, 85);
		glm::vec3 position = glm::vec3(100, 50, 86);
		glm::vec3 up = glm::vec3(0, 1, 0);
		glm::vec3 velocity = glm::vec3(0);

		// 画面パラメータ
		float width = 1280; // 画面の幅(ピクセル数)
		float height = 720; // 画面の高さ(ピクセル数)
		float near = 1;     // 最小Z値(メートル)
		float far = 500;    // 最大Z値(メートル)

		// カメラパラメータ
		float fNumber = 1.4f; // エフ・ナンバー = カメラのF値
		float fov = glm::radians(60.0f); // フィールド・オブ・ビュー = カメラの視野角(ラジアン)
		float sensorSize = 36.0f; // フォーカル・プレーン = ピントの合う距離
		int camera_z = -4;
		
		// Update関数で計算するパラメータ
		float focalLength = 50.0f; // フォーカル・レングス = 焦点距離(ミリ)
		float apertrue = 20.0f; // アパーチャー　= 開口(ミリ)
		float focalPlane = 10000.0f; // フォーカル・プレーン = ピントの合う距離

		void Update(const glm::mat4& matView);
	};
	Camera camera;

	enum CameraState
	{
		playcamera,
		debug
	};
	CameraState c_state = CameraState::playcamera;

	PlayerActorPtr player;
	LoiteringEnemyPtr L_enemy;      // 徘徊する敵
	OverlookEnemyPtr O_enemy;     // 監視する敵
	GateKeeperEnemyPtr G_enemy; // 門番
	HuntEnemyPtr H_enemy;           // 追いかけ続ける敵
	EnemyActorList enemies;
	ActorList objects;

	LightBuffer lightBuffer;
	ActorList lights;

	FramebufferObjectPtr fboMain;
};

#endif // !MAINGAMESCENE_H_INCLUDED

