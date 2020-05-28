/*
@file Enemy.h
*/
#ifndef ENEMY_H_INCLUDED
#define ENEMY_H_INCLUDED
#include "GLFWEW.h"
#include "SkeletalMeshActor.h"
#include "Terrain.h"
#include "Audio/Audio.h"
#include<memory>
#include <random>
#include <stdio.h>
#include <tchar.h>
#include <map>

/*
�G�l�~�[�A�N�^�[
*/
class EnemyActor : public SkeletalMeshActor
{
public:
	EnemyActor(const Terrain::HeightMap* hm, const Mesh::Buffer& buffer,
		const glm::vec3& pos, const glm::vec3& rot = glm::vec3(0));
	virtual ~EnemyActor() = default;
	virtual void Update(float) override;
	virtual void OnHit(const ActorPtr&, const glm::vec3&);
	void SetBoardingActor(ActorPtr);
	const ActorPtr& GetAttackCollision() const { return attackCollision; }

	float horizontalSpeed = velocity.x * velocity.x + velocity.z * velocity.z;
	void TargetActor(const ActorPtr& target);
	void ObjectActor(const StaticMeshActorPtr object);
	void ObstacleActor(const ActorList& obstacleis);

	bool Wait(float);
	bool MoveTo(glm::vec3, float);
	bool Attack(float);
	bool Vigilance();
	bool Warning(float);
	bool NeraEquivalent(const glm::vec3, const glm::vec3, float);
	bool SeenTo(float, float);
	bool MapCheck(int x, int z);
	void TaskForcedTermination()
	{
		velocity = glm::vec3(0);
		nodePoint = 0;
		moveCount = 0;
		state = State::vigilance;
		task = Task::reserve;
		mode = Mode::vigilance;
		isAnimation = false;
	}
	void PlayerInfo();
	void Outside();
	virtual bool RayChack(glm::vec3, int);
	virtual const glm::vec3 ObjectChack(ActorPtr, glm::vec3);
	virtual bool NearPlayer();
	
private:
	std::mt19937 rand;
	ActorPtr boardingActor;    // ����Ă���A�N�^�[
	ActorPtr attackCollision;  // �U������
	ActorList obstacle;
	ActorPtr objects;

	Audio::SoundPtr seEnemyAttack;

	glm::vec3 forward;
	glm::vec3 targetVector;
	glm::vec3 targetNormalize;
	
	float myDot;

	bool taskStart = false;
	bool taskEnd = false;
	

	float nearPlayer = 1.5f;
	float nearGoal = 0.5f;

	glm::vec3 patrolGoalPos;
	float patrolX;
	float patrolZ;

	const Terrain::HeightMap* heightMap = nullptr;

	const int width = 25;
	const int height = 10;
	const float rightWall = 120.0f;
	const float leftWall = 85.0f;
	const float forwordWall = 100.0f;
	const float backWall = 70.0f;

protected:
	ActorPtr targetActor;
	float waitTimer = 0;
	float worningTimer = 0;
	bool isAnimation = false;
	float warningTimer = 0;
	float targetLength;
	float targetDot;
	float targetRadian;
	float moveSpeed = 5.0f;    // �ړ����x
	float forgetTimer;
	float forgetTime = 1.0f; // ���E����Player������Č���������
	char meshName;
	
	bool isVisual; // ���o��L���ɂ��邩

	// �^�X�N�̃X�e�[�g
	enum class Task
	{
		reserve,   // ����
		start,       // �J�n
		end,        // �I��
		irregular, // �C���M�����[
	};
	Task task = Task::reserve;
	float attackTimer = 0;     // �U������

public:
	glm::vec3 map[200][200];
	glm::vec3 goalPos;
	glm::vec3 startPos;
	glm::vec3 nodePos[200];
	glm::vec3 o[100];
	int nodePoint = 0;
	int moveCount = 0;
	int maxCount = 0;
	int obstacleLength;
	int x = 0;
	int z = 0;
	int loop = 0;
	float heightMap_y;
	bool costChack = false;
	int roundPoint = 0;
	glm::vec3 front;
	bool vigilanceMode = false;
	bool discovery = false;
	glm::vec3 seenPos;
	int rotateCount = 0;
	int patrolCount = 0;
	float findRotation;
	float rotationSpeed = 2.0f;

	// ���񂷂�|�C���g
	glm::vec3 roundPoints[5]
	{
		glm::vec3(100, 0, 93),
		glm::vec3(89, 0, 87),
		glm::vec3(100, 0, 93),
		glm::vec3(99, 0, 89),
		glm::vec3(100, 0, 93)
	};

	// �A�j���[�V�����̏��
	enum class State
	{
		wait,  // ��~
		patrol,  // ����
		vigilance, // �x��
		attack, //�U��
		approach, // �߂Â�
		overlook, // ���n��
		goback, // �߂�
	};
	State state = State::patrol; // ���݂̃A�j���[�V�����̏��

	enum class Mode
	{
		freeze,
		normal,
		vigilance,
		worning,
		chace,
		hunt,
	};
	Mode mode = Mode::normal;
};
using EnemyActorPtr = std::shared_ptr<EnemyActor>;

class EnemyActorList
{
public:
	using iterator = std::vector<EnemyActorPtr>::iterator;
	using const_iterator = std::vector<EnemyActorPtr>::const_iterator;

	EnemyActorList() = default;
	~EnemyActorList() = default;

	void Reserve(size_t);
	void Add(const EnemyActorPtr&);
	bool Remove(const EnemyActorPtr&);
	void Update(float);
	void UpdateDrawData(float);
	void Draw();
	bool Empty() const { return enemies.empty(); }

	// �C�e���[�^�[���擾����֐�
	iterator begin() { return enemies.begin(); }
	iterator end() { return enemies.end(); }
	const_iterator begin() const { return enemies.begin(); }
	const_iterator end() const { return enemies.end(); }

	std::vector<EnemyActorPtr> FindNearbyActors(const glm::vec3& pos, float maxDistance) const;

private:
	std::vector<EnemyActorPtr> enemies;

	static const int mapGridSizeX = 10;
	static const int mapGridSizeY = 10;
	static const int sepalationSizeX = 20;
	static const int sepalationSizeY = 20;
	std::vector<EnemyActorPtr> grid[sepalationSizeY][sepalationSizeX];
	glm::ivec2 CalcMapIndex(const glm::vec3& pos) const;
};

#endif // !ENEMY_H_INCLUDED

