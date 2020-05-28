#define _CRT_SECURE_NO_WARNINGS
#include "LoiteringEnemy.h"

LoiteringEnemy::LoiteringEnemy(const Terrain::HeightMap* hm, const Mesh::Buffer& buffer,
	const glm::vec3& pos, const glm::vec3& rot)
	:EnemyActor::EnemyActor(hm, buffer, pos, rot)
{
	colLocal = Collision::CreateSphere(glm::vec3(0, 0.7f, 0), 0.8f);
	moveSpeed = 2.5f;
	isVisual = true;
}

void LoiteringEnemy::Update(float deltaTime)
{
	EnemyActor::Update(deltaTime);

	// �^�X�N�̍X�V
	switch (state)
	{
	case EnemyActor::State::wait:

		if (Wait(deltaTime))
		{
			//�v���C���[�����F�ł��Ă��邩�Ń^�X�N���؂�ւ��
			if (task == Task::end)
			{
				if (EnemyActor::vigilanceMode)
				{
					task = Task::reserve;
					state = State::vigilance;
				}
				else
				{
					task = Task::reserve;
					state = State::patrol;
				}
			}
		}
		break;

	case EnemyActor::State::patrol:
		if (MoveTo(roundPoints[patrolCount], 0.9f))
		{
			if (task == Task::end)
			{
				if (patrolCount == 4)
				{
					state = State::wait;
					patrolCount = 0;
				}
				else
				{
					mode = Mode::normal;
					patrolCount += 1;
					task = Task::reserve;
				}
			}
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
				glm::vec3 v = position - targetActor->position;
				if (glm::length(v) > 8)
				{
					discovery = false;
					vigilanceMode = true;
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

	case EnemyActor::State::overlook:
		if (Vigilance())
		{
			if (task == Task::end)
			{
				vigilanceMode = false;
				state = State::wait;
			}
		}
		break;
	}
}

void LoiteringEnemy::OnHit(const ActorPtr& b, const glm::vec3& p)
{
	EnemyActor::OnHit(b, p);
}

/*
�i�[�\�ȃA�N�^�[�����m�ۂ���

@param reserveCount �A�N�^�[�z��̊m�ې�
*/
void LoiteringEnemyList::Reserve(size_t reserveCount)
{
	loiteringEnemies.reserve(reserveCount);
}

/*
�A�N�^�[��ǉ�����

@param actor �ǉ�����A�N�^�[
*/
void LoiteringEnemyList::Add(const LoiteringEnemyPtr& actor)
{
	loiteringEnemies.push_back(actor);
}

/*
�A�N�^�[���폜����

@param actor �폜����A�N�^�[
*/
bool LoiteringEnemyList::Remove(const LoiteringEnemyPtr& actor)
{
	//�|�C���^����v����A�N�^��T����
	//�������������
	for (auto itr = loiteringEnemies.begin(); itr != loiteringEnemies.end(); ++itr)
	{
		if (*itr == actor)
		{
			loiteringEnemies.erase(itr);
			return true;
		}
	}
	return false;
}

/*
�w�肳�ꂽ���W�ɑΉ�����i�q�̃C���f�b�N�X���擾����

@param pos �C���f�b�N�X�̌��ɂȂ�ʒu

@retun pos�ɑΉ�����i�q�̃C���f�b�N�X
*/
glm::ivec2 LoiteringEnemyList::CalcMapIndex(const glm::vec3& pos) const
{
	const int x = std::max(0,
		std::min(sepalationSizeX - 1, static_cast<int>(pos.x / mapGridSizeX)));
	const int y = std::max(0,
		std::min(sepalationSizeY - 1, static_cast<int>(pos.z / mapGridSizeY)));
	return glm::ivec2(x, y);
}

/*
�A�N�^�[�̏�Ԃ��X�V����

@param deltaTime �O��̍X�V����̌o�ߎ���
*/
void LoiteringEnemyList::Update(float deltaTime)
{
	//�͈�for��
	for (const LoiteringEnemyPtr& e : loiteringEnemies)
	{
		if (e && e->health > 0)
		{
			e->Update(deltaTime);
		}
	}

	for (auto i = loiteringEnemies.begin(); i != loiteringEnemies.end();)
	{
		const LoiteringEnemyPtr& e = *i;
		if (!e || e->health <= 0)
		{
			// erase => vector����v�f���폜���邽�߂Ɏg��
			// ���s��A���̗v�f���w���l��Ԃ��Ă����
			i = loiteringEnemies.erase(i);
		}
		else
		{
			++i;
		}
	}

	// �i�q��ԂɃA�N�^�[�����蓖�Ă�
	for (int y = 0; y < sepalationSizeY; ++y)
	{
		for (int x = 0; x < sepalationSizeX; ++x)
		{
			grid[y][x].clear();
		}
	}
	for (auto i = loiteringEnemies.begin(); i != loiteringEnemies.end(); ++i)
	{
		const glm::ivec2 mapIndex = CalcMapIndex((*i)->position);
		grid[mapIndex.y][mapIndex.x].push_back(*i);
	}
}



/*
�A�N�^�[��`��f�[�^���X�V����
*/
void LoiteringEnemyList::UpdateDrawData(float deltaTime)
{
	for (const LoiteringEnemyPtr& e : loiteringEnemies)
	{
		if (e && e->health > 0) {
			e->UpdateDrawData(deltaTime);
		}
	}
}

/*
�A�N�^�[��`�悷��
*/
void LoiteringEnemyList::Draw()
{
	for (const LoiteringEnemyPtr& e : loiteringEnemies)
	{
		if (e && e->health > 0) {
			e->Draw();
		}
	}
}

/*
�w�肳�ꂽ���W�̋ߖT�ɂ���A�N�^�[�̃��X�g���擾����

@param pos         �����̊�_�ƂȂ���W
@param maxDistance �ߖT�Ƃ݂Ȃ��ő勗��(��)

@return Actor::position��pos���甼�amaxDistance�ȓ��ɂ���A�N�^�[�̔z��
*/
std::vector<LoiteringEnemyPtr> LoiteringEnemyList::FindNearbyActors(
	const glm::vec3& pos, float maxDistance) const
{
	std::vector<std::pair<float, LoiteringEnemyPtr>> buffer;
	buffer.reserve(1000);

	const glm::ivec2 mapIndex = CalcMapIndex(pos);
	const glm::ivec2 min = glm::max(mapIndex - 1, 0);
	const glm::ivec2 max = glm::min(
		mapIndex + 1, glm::ivec2(sepalationSizeX - 1, sepalationSizeY - 1));
	for (int y = min.y; y <= max.y; ++y)
	{
		for (int x = min.x; x <= max.x; ++x)
		{
			const std::vector<LoiteringEnemyPtr>& list = grid[y][x];
			for (auto actor : list)
			{
				const float distance = glm::distance(glm::vec3(actor->position), pos);
				buffer.push_back(std::make_pair(distance, actor));
			}
		}
	}

	std::vector<LoiteringEnemyPtr> result;
	result.reserve(100);
	for (const auto& e : buffer)
	{
		if (e.first <= maxDistance)
		{
			result.push_back(e.second);
		}
	}
	return result;
}