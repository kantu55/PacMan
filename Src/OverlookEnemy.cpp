#define _CRT_SECURE_NO_WARNINGS
#include "OverlookEnemy.h"

OverlookEnemy::OverlookEnemy(const Terrain::HeightMap* hm, const Mesh::Buffer& buffer,
	const glm::vec3& pos, const glm::vec3& rot)
	:EnemyActor::EnemyActor(hm, buffer, pos, rot)
{
	strcpy(&meshName, "oni_small");
	colLocal = Collision::CreateSphere(glm::vec3(0, 0.7f, 0), 0.8f);
	task = Task::reserve;
	state = State::overlook;
	isVisual = false;
}

void OverlookEnemy::Update(float deltaTime)
{
	EnemyActor::Update(deltaTime);
	if (RayChack(front, 6))
	{
		warningTimer = 5;
		mode = Mode::worning;
		velocity = glm::vec3(0);
		state = State::vigilance;
		task = Task::reserve;
		isAnimation = false;
	}
	
	switch (state)
	{
	case EnemyActor::State::wait:
		if (Wait(deltaTime))
		{
			//プレイヤーが視認できているかでタスクが切り替わる
			if (task == Task::end)
			{
				task = Task::reserve;
				state = State::overlook;
			}
		}
		break;
	case EnemyActor::State::vigilance:
		if (Warning(deltaTime))
		{
			if (task == Task::end)
			{
				mode = Mode::normal;
				waitTimer = 1.0f;
				state = State::wait;
				rotation.y = glm::radians(90.0f);
			}
		}
		break;
	case EnemyActor::State::overlook:
		if (Vigilance())
		{
			task = Task::reserve;
			waitTimer = 1.0f;
			state = State::wait;
		}
		break;
	}
}

void OverlookEnemy::OnHit(const ActorPtr& b, const glm::vec3& p)
{
	EnemyActor::OnHit(b, p);
}