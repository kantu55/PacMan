#define _CRT_SECURE_NO_WARNINGS
#include"GatekeeperEnemy.h"

GateKeeperEnemy::GateKeeperEnemy(const Terrain::HeightMap* hm, const Mesh::Buffer& buffer,
	const glm::vec3& pos, const glm::vec3& rot)
	:EnemyActor::EnemyActor(hm, buffer,pos, rot)
{
	colLocal = Collision::CreateSphere(glm::vec3(0, 0.7f, 0), 0.8f);
	state = State::wait;
	waitTimer = 150.0f;
	moveSpeed = 4.0f;
	startPos = glm::vec3(104, 0, 87);
	isVisual = true;
}

void GateKeeperEnemy::Update(float deltaTime)
{
	EnemyActor::Update(deltaTime);

	switch (state)
	{
	case EnemyActor::State::wait:
		if (Wait(deltaTime))
		{
			if (task == Task::end)
			{
				if (mode == Mode::vigilance)
				{
					task = Task::reserve;
					state = State::goback;
				}
				else
				{
					waitTimer = 10.0f;
					task = Task::reserve;
					state = State::wait;
				}
			}
		}
		break;
	case EnemyActor::State::vigilance:
		if (MoveTo(seenPos, 0.5f))
		{
			if (task == Task::end)
			{
				findRotation = rotation.y + glm::radians(90.0f);
				state = State::overlook;
			}
		}
		break;
	case EnemyActor::State::attack:
		attackTimer += deltaTime;
		if (Attack(deltaTime))
		{
			if (task == Task::end)
			{
				glm::vec3 v = position - targetActor->position;
				if (glm::length(v) > 8)
				{
					task = Task::reserve;
					mode = Mode::vigilance;
					state = State::vigilance;
				}
				else
				{
					task = Task::reserve;
					state = State::approach;
				}
			}
		}
		break;
	case EnemyActor::State::approach:
		if (MoveTo(targetActor->position, 1.75f))
		{
			if (task == Task::end)
			{
				task = Task::reserve;
				state = State::attack;
			}
		}
		break;
	case EnemyActor::State::overlook:
		if (Vigilance())
		{
			if (task == Task::end)
			{
				waitTimer = 5.0f;
				state = State::wait;
			}
		}
		break;
	case EnemyActor::State::goback:
		if (MoveTo(startPos, 0.9f))
		{
			if (task == Task::end)
			{
				waitTimer = 10.0f;
				task = Task::reserve;
				mode = Mode::normal;
				state = State::wait;
			}
		}
		break;
	}
}

void GateKeeperEnemy::OnHit(const ActorPtr& b, const glm::vec3& p)
{
	EnemyActor::OnHit(b, p);
}