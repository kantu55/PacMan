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
���C���Q�[�����
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
	StaticMeshActorPtr CreateStoneWall(glm::vec3 pos, float rot, int axsis, int size); // �Εǂ𐶐�
	void CreateSpriteWall(glm::vec3, int axsis, int index); //�~�j�}�b�v�̕Ǘp�X�v���C�g�̐���
	bool EventFragPosition(glm::vec3 playerPos, glm::vec3 pos);
	bool enemyValid = false;
	bool relayFrag = false;
	bool isWayPoint = false;
	bool isHunt = false;

	//�v���C���[�̃J�����ʒu�ƒ����_
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

		// ��ʃp�����[�^
		float width = 1280; // ��ʂ̕�(�s�N�Z����)
		float height = 720; // ��ʂ̍���(�s�N�Z����)
		float near = 1;     // �ŏ�Z�l(���[�g��)
		float far = 500;    // �ő�Z�l(���[�g��)

		// �J�����p�����[�^
		float fNumber = 1.4f; // �G�t�E�i���o�[ = �J������F�l
		float fov = glm::radians(60.0f); // �t�B�[���h�E�I�u�E�r���[ = �J�����̎���p(���W�A��)
		float sensorSize = 36.0f; // �t�H�[�J���E�v���[�� = �s���g�̍�������
		int camera_z = -4;
		
		// Update�֐��Ōv�Z����p�����[�^
		float focalLength = 50.0f; // �t�H�[�J���E�����O�X = �œ_����(�~��)
		float apertrue = 20.0f; // �A�p�[�`���[�@= �J��(�~��)
		float focalPlane = 10000.0f; // �t�H�[�J���E�v���[�� = �s���g�̍�������

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
	LoiteringEnemyPtr L_enemy;      // �p�j����G
	OverlookEnemyPtr O_enemy;     // �Ď�����G
	GateKeeperEnemyPtr G_enemy; // ���
	HuntEnemyPtr H_enemy;           // �ǂ�����������G
	EnemyActorList enemies;
	ActorList objects;

	LightBuffer lightBuffer;
	ActorList lights;

	FramebufferObjectPtr fboMain;
};

#endif // !MAINGAMESCENE_H_INCLUDED

