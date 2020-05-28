#define _CRT_SECURE_NO_WARNINGS
#include "HuntEnemy.h"

HuntEnemy::HuntEnemy(const Terrain::HeightMap* hm, const Mesh::Buffer& buffer,
	const glm::vec3& pos, const glm::vec3& rot)
	:EnemyActor::EnemyActor(hm, buffer, pos, rot)
{
	colLocal = Collision::CreateSphere(glm::vec3(0, 0.7f, 0), 0.8f);
	mode = Mode::freeze;
	state = State::wait;
	waitTimer = 150.0f;
	moveSpeed = 3.0f;
	isVisual = false;
}

void HuntEnemy::Update(float deltaTime)
{
	EnemyActor::Update(deltaTime);

	if (NearPlayer() && mode != Mode::chace)
	{
		mode = Mode::chace;
		state = State::attack;
		nodePoint = 0;
		moveCount = 0;
		isAnimation = false;
		task = Task::reserve;
	}

	// �^�X�N�̍X�V
	switch (state)
	{
	case EnemyActor::State::wait:

		if (Wait(deltaTime))
		{
			//�v���C���[�����F�ł��Ă��邩�Ń^�X�N���؂�ւ��
			if (task == Task::end)
			{
				waitTimer = 150.0f;
				task = Task::reserve;
				state = State::wait;
			}
		}
		if (mode == Mode::hunt)
		{
			isAnimation = false;
			task = Task::reserve;
			state = State::approach;
		}
		break;

	case EnemyActor::State::approach:
		if (MoveTo(targetActor->position, 1.7f))
		{
			if (task == Task::end)
			{
				task = Task::reserve;
				state = State::attack;
			}
		}
		break;

	case EnemyActor::State::attack:
		attackTimer += deltaTime;
		if (Attack(deltaTime))
		{
			if (task == Task::end)
			{
				task = Task::reserve;
				state = State::approach;
			}
		}
		break;
	}
	
}

void HuntEnemy::OnHit(const ActorPtr& b, const glm::vec3& p)
{
	EnemyActor::OnHit(b, p);
}