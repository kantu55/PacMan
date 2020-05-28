/*
@file Enemy.cpp
*/
#define _CRT_SECURE_NO_WARNINGS
#include "Enemy.h"
#include "PlayerActor.h"
#include "SkeletalMesh.h"
#include <glm/gtc/matrix_transform.hpp>
#include <math.h>
#include "Astar.h"

#define ARRAY_NUM(a) (sizeof(a)/sizeof(a[0]))

/*
�R���X�g���N�^
*/
EnemyActor::EnemyActor(const Terrain::HeightMap* hm, const Mesh::Buffer& buffer,
	const glm::vec3& pos, const glm::vec3& rot)
	: SkeletalMeshActor(buffer.GetSkeletalMesh("oni_small"), "Enemy", 13, pos, rot), heightMap(hm)
{
	isAnimation = false;
	colLocal = Collision::CreateSphere(glm::vec3(0, 0.7f, 0), 0.8f);
}

void EnemyActor::TargetActor(const ActorPtr& target)
{
	targetActor = target;
	return;
}

void EnemyActor::ObstacleActor(const ActorList& obstacleis)
{
	obstacle = obstacleis;
	return;
}

void EnemyActor::ObjectActor(const StaticMeshActorPtr object)
{
	objects = object;
	return;
}

bool EnemyActor::NearPlayer()
{
	if (targetLength <= 1.7f)
	{
		return true;
	}
	return false;
}

bool EnemyActor::RayChack(glm::vec3 front, int seenLength)
{
	float objectMinScale = 10.0f;
	for (int i = 0; i < 3; i++)
	{
		float RotationY = rotation.y;
		float x = -1;
		x += i;
		bool wallFrag = false;
		front = glm::rotate(glm::mat4(1), RotationY,
			glm::vec3(0, 1, 0)) * glm::vec4(x, 0, 1, 1);
		for (int j = 0; j < seenLength; j++)
		{
			front = glm::rotate(glm::mat4(1), RotationY,
				glm::vec3(0, 1, 0)) * glm::vec4(x, 0, j, 1);
			front += position;
			front.y = heightMap->Height(front);
			for (auto& object : obstacle)
			{
				float targetDistance = glm::length(targetActor->position - front);
				const glm::vec3 distance = ObjectChack(object, front);
				if (glm::length(distance) <= 0.5f)
				{
					wallFrag = true;
				}
				else if (targetDistance < 1.0f && !wallFrag)
				{
					seenPos = targetActor->position;
					return true;
				}
			}
		}
	}
	return false;
}

void EnemyActor::PlayerInfo()
{
	// target�̓v���C���[
	targetVector = targetActor->position - position; // �x�N�g��
	targetNormalize = glm::normalize(targetVector);  // �x�N�g���𐳋K��
	targetLength = glm::length(targetVector); // �x�N�g���𒷂��ɕύX�i�R���W������1.45�ȓ��͂����Ȃ��j
	targetDot = glm::dot(glm::normalize(position), glm::normalize(targetVector)); // �v���C���[�ƓG(���g)�̓���
	targetRadian = std::atan2(-targetNormalize.x, targetNormalize.z); // �v���C���[�ƓG(���g)�̊p�x
}

void EnemyActor::Outside()
{
	// �����Ȃ��ǔ���
	if (position.x > rightWall || position.x < leftWall || position.z > forwordWall || position.z < backWall)
	{
		velocity = glm::vec3(0);
		if (position.x > rightWall)
		{
			position.x = rightWall;
		}
		else if (position.x < leftWall)
		{
			position.x = leftWall;
		}
		if (position.z > forwordWall)
		{
			position.z = forwordWall;
		}
		else if (position.z < backWall)
		{
			position.z = backWall;
		}
	}
}

/*
�X�V

@param deltaTime �o�ߎ���
*/
void EnemyActor::Update(float deltaTime)
{
	if (health <= 0)
		return;
	PlayerInfo();

	front = glm::rotate(glm::mat4(1), rotation.y,
		glm::vec3(0, 1, 0)) * glm::vec4(0, 0, 1, 1);
	
	// ���W�̍X�V
	SkeletalMeshActor::Update(deltaTime);
	
	if (isVisual)
	{
		// ���E�Ƀv���C���[���������猻�݂̃^�X�N���I������
		if (RayChack(front, 6) && mode != Mode::chace)
		{
			velocity = glm::vec3(0);
			nodePoint = 0;
			moveCount = 0;
			state = State::vigilance;
			task = Task::reserve;
			isAnimation = false;
		}

		// �߂��Ƀv���C���[�������狭���I�ɍU���^�X�N�ɂ͂���
		if (NearPlayer() && mode != Mode::chace)
		{
			mode = Mode::chace;
			state = State::attack;
			nodePoint = 0;
			moveCount = 0;
			isAnimation = false;
			task = Task::reserve;
		}

		// �v���C���[�ƃ`�F�C�X���Ɏ��E������������莞�Ԑ�����
		if (!RayChack(front, 6) && state == State::approach)
		{
			forgetTimer += deltaTime;
			// ��莞�Ԍo���ăv���C���[��������Ȃ������猩�������n�_�܂ňړ�����
			if (forgetTimer >= forgetTime)
			{
				velocity = glm::vec3(0);
				nodePoint = 0;
				moveCount = 0;
				mode = Mode::normal;
				state = State::vigilance;
				task = Task::reserve;
				isAnimation = false;
			}
		}
		else
		{
			forgetTimer = 0;
		}
	}

	if (attackCollision)
	{
		attackCollision->Update(deltaTime);
	}
}

/*
�ҋ@
*/
bool EnemyActor::Wait(float deltaTime)
{
	// �^�X�N�̏�����
	if (waitTimer > 0)
	{
		task = Task::start;
		//�A�j���V�����̕ύX
		if (!isAnimation)
		{
			GetMesh()->Play("Wait");
			isAnimation = true;
		}
		waitTimer -= deltaTime;
	}

	// �ҋ@���Ԃ��I��������^�X�N�I��
	if (waitTimer <= 0 && task == Task::start)
	{
		task = Task::end;
		isAnimation = false;
		return true;
	}
	return false;
}

/*
�U��
*/
bool EnemyActor::Attack(float delatTime)
{
	task = Task::start;
	velocity = glm::vec3(0);

	// �A�j���[�V�����̕ύX
	if (!isAnimation)
	{
		seEnemyAttack = Audio::Engine::Instance().Prepare("Res/Audio/EnemyAttack.mp3");
		seEnemyAttack->Play();
		GetMesh()->Play("Attack");
		attackTimer = 0;
		isAnimation = true;
	}
	glm::vec3 move = targetActor->position - position;
	if (glm::dot(move, move))
	{
		// �������X�V
		move = glm::normalize(move);
		rotation.y = std::atan2(-move.z, move.x) + glm::radians(90.0f);
	}
	// �R���W�������o��
	if (attackTimer > 0.5f && attackTimer < 0.9f)
	{
		if (!attackCollision)
		{
			static const float radian = 0.5f;
			const glm::vec3 front = glm::rotate(glm::mat4(1), rotation.y,
				glm::vec3(0, 1, 0)) * glm::vec4(0, 0, 1.0f, 1);
			attackCollision = std::make_shared<Actor>("PlayerAttackCollision", 5,
				position + front + glm::vec3(0, 1, 0), glm::vec3(0), glm::vec3(radian));
			attackCollision->colLocal = Collision::CreateSphere(glm::vec3(0), radian);
		}
	}
	else
	{
		attackCollision.reset();
	}

	// �A�j���[�V�������I��(1�b)������^�X�N�I��
	if (attackTimer >= 1)
	{
		attackTimer = 0;
		waitTimer = 1.0f;
		task = Task::end;
		isAnimation = false;
		return true;
	}
	else
	{
		return false;
	}
}

/*
�ړ�
*/
bool EnemyActor::MoveTo(glm::vec3 target, float length)
{
	// �A�j���[�V�����̕ύX
	if (!isAnimation)
	{
		GetMesh()->Play("Run");
		isAnimation = true;
	}
	
	Astar astar;
	// �^�X�N�̏�����
	if (task == Task::reserve)
	{
		/*
		�ڕW�̍��W���X�^�[�g�n�_�Ƃ��Đ����^�ɂ���
		*/
		int startPos_x = target.x;
		int startPos_z = target.z;
		/*
		�X�^�[�g�n�_���ǂƔ���Ă�����X�^�[�g�n�_�����炷
		*/
		if ((target.x - startPos_x) >= FLT_EPSILON &&
			(target.z - startPos_z) >= FLT_EPSILON)
		{
			if (MapCheck(startPos_x, startPos_z))
			{
				
			}
			else if (MapCheck(startPos_x + 1, startPos_z))
			{
				startPos_x += 1;
			}
			else if (MapCheck(startPos_x - 1, startPos_z))
			{
				startPos_x -= 1;
			}
			else if (MapCheck(startPos_x, startPos_z + 1))
			{
				startPos_z += 1;
			}
			else if (MapCheck(startPos_x, startPos_z - 1))
			{
				startPos_z -= 1;
			}
			else if (MapCheck(startPos_x + 1, startPos_z + 1))
			{
				startPos_x += 1;
				startPos_z += 1;
			}
			else if (MapCheck(startPos_x + 1, startPos_z - 1))
			{
				startPos_x += 1;
				startPos_z -= 1;
			}
			else if (MapCheck(startPos_x - 1, startPos_z - 1))
			{
				startPos_x -= 1;
				startPos_z -= 1;
			}
			else if (MapCheck(startPos_x - 1, startPos_z + 1))
			{
				startPos_x -= 1;
				startPos_z += 1;
			}
		}

		/*
		�����̍��W���S�[���n�_�Ƃ��Đ����^�ɂ���
		*/
		int goalPos_x = position.x;
		int goalPos_z = position.z;
		/*
		�S�[���n�_���ǂƔ���Ă�����S�[���n�_�����炷
		*/
		if ((position.x - goalPos_x) >= FLT_EPSILON &&
			(position.z - goalPos_z) >= FLT_EPSILON)
		{
			if (MapCheck(goalPos_x, goalPos_z))
			{

			}
			else if (MapCheck(goalPos_x + 1, goalPos_z))
			{
				goalPos_x += 1;
			}
			else if (MapCheck(goalPos_x - 1, goalPos_z))
			{
				goalPos_x -= 1;
			}
			else if (MapCheck(goalPos_x, goalPos_z + 1))
			{
				goalPos_z += 1;
			}
			else if (MapCheck(goalPos_x, goalPos_z - 1))
			{
				goalPos_z -= 1;
			}
			else if (MapCheck(goalPos_x + 1, goalPos_z + 1))
			{
				goalPos_x += 1;
				goalPos_z += 1;
			}
			else if (MapCheck(goalPos_x + 1, goalPos_z - 1))
			{
				goalPos_x += 1;
				goalPos_z -= 1;
			}
			else if (MapCheck(goalPos_x - 1, goalPos_z - 1))
			{
				goalPos_x -= 1;
				goalPos_z -= 1;
			}
			else if (MapCheck(goalPos_x - 1, goalPos_z + 1))
			{
				goalPos_x -= 1;
				goalPos_z += 1;
			}
		}

		patrolGoalPos.y = position.y;
		costChack = false;
		loop = 0;
		nodePoint = 0;
		moveCount = 0;
		astar.open.index = 0;
		astar.close.index = 0;
		for (x = leftWall; x < ARRAY_NUM(map); x++)
		{
			for (z = backWall; z <= forwordWall; z++)
			{
				// �m�[�h�̃X�^�[�g�n�_��ݒ�
				if (x == startPos_x && z == startPos_z)
				{
					map[x][z] = glm::vec3(startPos_x, position.y, startPos_z);
					map[x][z].y = heightMap->Height(map[x][z]);
					astar.s.x = x;
					astar.s.z = z;
					// �X�^�[�g�m�[�h���I�[�v�����X�g�ɒǉ�
					astar.open.node[astar.open.index++] = &astar.s;
					// �N���[�Y���X�g�͋�ɂ���
					astar.close.node[astar.close.index++] = nullptr;
				}

				// �m�[�h�̃S�[���n�_��ݒ�
				if (x == goalPos_x && z == goalPos_z)
				{
					map[x][z] = glm::vec3(x, 0, z);
					map[x][z].y = heightMap->Height(map[x][z]);
					goalPos = map[x][z]; // �S�[���n�_�Ƃ��ĕϐ��Ɋi�[
					astar.e.x = x;
					astar.e.z = z;
				}
			}
		}
		task = Task::start;
	}
	
	if (nodePoint == 0)
	{
		while (1)
		{
			Astar::NODE* n = nullptr;
			n = astar.GetMinCost(n);
			// open���烊�X�g���Ȃ��Ȃ����̂ŏI������
			if (!n)
			{
				printf("no goal...\n");
				roundPoint = 0;
				velocity = glm::vec3(0);
				task = Task::reserve;
				isAnimation = false;
				return true;
			}
			map[n->x][n->z] = glm::vec3(n->x, position.y, n->z);
			glm::vec3 v = map[n->x][n->z] - goalPos;
			// �v�Z���ɃS�[���n�_�ɒ�������v�Z�I��
			if (glm::dot(v,v) <= 1)
			{
				if (n&& !n->parent)
				{
					nodePos[nodePoint] = glm::vec3(n->x, 0, n->z);
					nodePos[nodePoint].y = heightMap->Height(nodePos[nodePoint]);
					nodePoint++;
				}

				// �X�^�[�g�n�_�̃m�[�h�������Ȃ�܂Ŋe�m�[�h�̍��W��z��Ɋi�[
				while (n->parent)
				{
					if (n->cost >= n->parent->cost)
					{
						n = n->parent;
						nodePos[nodePoint] = glm::vec3(n->x, 0, n->z);
						nodePos[nodePoint].y = heightMap->Height(nodePos[nodePoint]);
						nodePoint++;
						if (nodePoint > 55)
						{
							printf("�R�X�g���傫�����邽�ߍēx�v�Z\n");
							// �e�m�[�h�̋�ɂ���
							n->parent = nullptr;
							costChack = true;
						}
					}
					// �e�m�[�h�̃R�X�g���傫�������ꍇ�̗�O����
					else if (n->cost < n->parent->cost)
					{
						printf("�R�X�g�G���[\n");
						// �e�m�[�h�̋�ɂ���
						n->parent = nullptr;
						costChack = true;
					}
				}
				break;
			}

			// ���݂̃m�[�h���N���[�Y���X�g�Ɋi�[����
			astar.close.node[astar.close.index++] = n;

			// ���̃m�[�h������
			if (n->x > leftWall && MapCheck(n->x - 1, n->z))
			{
				astar.SearchNode(&astar.open, &astar.close, &astar.s, &astar.e, n,
					astar.CreateNode(n->x - 1, n->z, n->cost + 1));
			}

			// �E�̃m�[�h������
			if (n->x < rightWall && MapCheck(n->x + 1, n->z))
			{
				astar.SearchNode(&astar.open, &astar.close, &astar.s, &astar.e, n,
					astar.CreateNode(n->x + 1, n->z, n->cost + 1));
			}

			// �O�̃m�[�h������
			if (n->z < forwordWall && MapCheck(n->x, n->z + 1))
			{
				astar.SearchNode(&astar.open, &astar.close, &astar.s, &astar.e, n,
					astar.CreateNode(n->x, n->z + 1, n->cost + 1));
			}

			// ���̃m�[�h������
			if (n->z > backWall && MapCheck(n->x, n->z - 1))
			{
				astar.SearchNode(&astar.open, &astar.close, &astar.s, &astar.e, n,
					astar.CreateNode(n->x, n->z - 1, n->cost + 1));
			}
			// ���O�̃m�[�h������
			if (n->x > leftWall && n->z < forwordWall && MapCheck(n->x - 1, n->z + 1))
			{
				astar.SearchNode(&astar.open, &astar.close, &astar.s, &astar.e, n,
					astar.CreateNode(n->x - 1, n->z + 1, n->cost + 1));
			}

			// �E�O�̃m�[�h������
			if (n->x < leftWall && n->z < forwordWall && MapCheck(n->x + 1, n->z + 1))
			{
				astar.SearchNode(&astar.open, &astar.close, &astar.s, &astar.e, n,
					astar.CreateNode(n->x + 1, n->z + 1, n->cost + 1));
			}

			// �����̃m�[�h������
			if (n->x > leftWall && n->z > backWall && MapCheck(n->x - 1, n->z - 1))
			{
				astar.SearchNode(&astar.open, &astar.close, &astar.s, &astar.e, n,
					astar.CreateNode(n->x - 1, n->z - 1, n->cost + 1));
			}

			// �E���̃m�[�h������
			if (n->x < leftWall && n->z > backWall && MapCheck(n->x + 1, n->z - 1))
			{
				astar.SearchNode(&astar.open, &astar.close, &astar.s, &astar.e, n,
					astar.CreateNode(n->x + 1, n->z - 1, n->cost + 1));
			}

			// �S�[���ɂ��ǂ蒅�����A�i���Ƀm�[�h�̌v�Z���s�����ꍇ�̗�O����
			if (loop++ > 1000)
			{
				printf("loop error...\n");
				return true;
			}
		}
	}
	// �e�m�[�h��H��悤�ɂ���AI�𓮂���
	if (moveCount < nodePoint)
	{
		nodePos[moveCount].y = position.y;
		glm::vec3 move = nodePos[moveCount] - position;
		if (glm::dot(move, move))
		{
			// �������X�V
			move = glm::normalize(move);
			rotation.y = std::atan2(-move.z, move.x) + glm::radians(90.0f);
		}
		// ������̍��W�ɒ������玟�̃m�[�h�̍��W�Ɉڂ�
		if (NeraEquivalent(position, nodePos[moveCount], 0.5f))
		{
			moveCount++;
		}
		else
		{
			// �ړ�����
			velocity = move * (moveSpeed);
		}
	}
	// �i�[�����m�[�h�̐��̕��i�񂾂�^�X�N�I��
	if (moveCount >= nodePoint)
	{
		// �e�̃R�X�g�����������ꍇ�̗�O�����Ɉ������������ꍇ�A�^�X�N���I�������m�[�h�̍Čv�Z���s��
		if (costChack)
		{
			velocity = glm::vec3(0);
			position = nodePos[nodePoint - 1];
			task = Task::reserve;
			return false;
		}
		if (glm::length(target - nodePos[moveCount]) > length && state == State::approach)
		{
			velocity = glm::vec3(0);
			position = nodePos[nodePoint - 1];
			task = Task::reserve;
			return false;
		}
		roundPoint = 0;
		velocity = glm::vec3(0);
		task = Task::end;
		waitTimer = 2.0f;
		isAnimation = false;
		return true;
	}
	return false;
}

bool EnemyActor::Vigilance()
{
	if (task == Task::reserve)
	{
		findRotation = rotation.y + glm::radians(90.0f);
		if (!isAnimation)
		{
			GetMesh()->Play("Wait");
			isAnimation = true;
		}
		task = Task::start;
	}

	if (rotateCount == 3)
	{
		rotateCount = 0;
		return true;
	}
	else
	{
		if (rotateCount ==  0)
		{
			if ((findRotation - rotation.y) > FLT_EPSILON)
			{
				rotation.y += glm::radians(rotationSpeed);
			}
			else
			{
				rotateCount += 1;
				findRotation = rotation.y + glm::radians(-180.0f);
			}
		}
		else if (rotateCount == 1)
		{
			if ((findRotation - rotation.y) < FLT_EPSILON)
			{
				rotation.y -= glm::radians(rotationSpeed);
			}
			else
			{
				rotateCount += 1;
				findRotation += glm::radians(90.0f);
			}
		}
		else if (rotateCount == 2)
		{
			if ((findRotation - rotation.y) > FLT_EPSILON)
			{
				rotation.y += glm::radians(rotationSpeed);
			}
			else
			{
				rotateCount += 1;
			}
		}
	}
	return false;
}

bool EnemyActor::Warning(float deltaTime)
{
	// �^�X�N�̏�����
	if (warningTimer > 0)
	{
		task = Task::start;
		//�A�j���V�����̕ύX
		if (!isAnimation)
		{
			GetMesh()->Play("Wait");
			isAnimation = true;
		}
		warningTimer -= deltaTime;
	}

	// �ҋ@���Ԃ��I��������^�X�N�I��
	if (warningTimer <= 0 && task == Task::start)
	{
		task = Task::end;
		isAnimation = false;
		return true;
	}
	return false;
}

// �v���C���[�����F�ł���͈�
bool EnemyActor::SeenTo(float length, float angle)
{
	if (length <= 5.0f && angle >= -1.0f && angle <= 1.0f)
	{
		return true;
	}
	return false;
}

// �S�[�����Ɣ��f�ł���͈�
bool EnemyActor::NeraEquivalent(const glm::vec3 left, const glm::vec3 right, float acceptable)
{
	if (glm::length(right - left) <= acceptable)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/*
�Փ˃n���h��

@param b �Փˑ���̃A�N�^�[
@param p �Փ˂������������W
*/
void EnemyActor::OnHit(const ActorPtr& b, const glm::vec3& p)
{
	const glm::vec3 v = colWorld.s.center - p;
	// �Փˈʒu�Ƃ̋������߂����Ȃ������ׂ�
	if (dot(v, v) > FLT_EPSILON)
	{
		// this��b�ɏd�Ȃ�Ȃ��ʒu�܂ňړ�
		const glm::vec3 vn = normalize(v);
		float radiusSum = colWorld.s.r;
		switch (b->colWorld.type)
		{
		case Collision::Shape::Type::sphere:
			radiusSum += b->colWorld.s.r;
			break;
		case Collision::Shape::Type::capsule:
			radiusSum += b->colWorld.c.r;
			break;
		}
		const float distance = radiusSum - glm::length(v) + 0.01f;
		position += vn * distance;
		colWorld.s.center += vn * distance;
	}
	else
	{
		// �ړ���������(�������߂�����ꍇ�̗�O����)
		const float deltaTime = static_cast<float>(GLFWEW::Window::Instance().DeltaTime());
		const glm::vec3 deltaVelocity = velocity * deltaTime;
		position -= deltaVelocity;
		colWorld.s.center -= deltaVelocity;
	}
	SetBoardingActor(b);
}

/*
�G�l�~�[������Ă��镨�̂�ݒ肷��

@param p ����Ă��镨��
*/
void EnemyActor::SetBoardingActor(ActorPtr p)
{
	boardingActor = p;
}

/*
�m�[�h�̌�����ɏ�Q�������邩�`�F�b�N����

@param x �������X���W
@param z �������Z���W

@retrun true  ��Q���Ȃ�
@param false ��Q������
*/
bool EnemyActor::MapCheck(int x, int z)
{
	glm::vec3 map = glm::vec3(x , 0, z);
	map.y = heightMap->Height(map);
	for (auto& object : obstacle)
	{
		// �I�u�W�F�N�g�ƃm�[�h�̋������擾����
		const glm::vec3 distance = ObjectChack(object, map); 

		// �I�u�W�F�N�g�Ǝw�肵�����W�̋������d�Ȃ��Ă��邩�m�F����
		if (glm::length(distance) <= 0.9f)
		{
			return false;
		}
	}
	return true;
}

/*
��Q���̃I�u�W�F�N�g�Ǝw�肵�����W�̋����i�x�N�g���j�𒲂ׂ�

@param o ��Q���i���X�g�j
@param m �w�肵�����W(�m�[�h�A���C)

@return �I�u�W�F�N�g�Ǝw�肵�����W�̋����i�x�N�g���j
*/
const glm::vec3 EnemyActor::ObjectChack(ActorPtr o, glm::vec3 m)
{
	// ���X�g�����̃I�u�W�F�N�g���擾
	StaticMeshActorPtr p = std::static_pointer_cast<StaticMeshActor>(o);
	// �w�肵�����W�Ə�Q���̒��S�_�̋���(�x�N�g��)
	const glm::vec3 d = m - p->colWorld.obb.center;
	// ��Q���̒��S�_�̍��W
	glm::vec3 q = p->colWorld.obb.center;
	// x,y,z��3���𒲂ׂ�
	for (int i = 0; i < 3; i++)
	{
		// �x�N�g���Ɗe���̓��ς𒲂ׂ�
		float distance = dot(d, p->colWorld.obb.axis[i]);
		if (distance >= p->colWorld.obb.e[i]) {
			distance = p->colWorld.obb.e[i];
		}
		else if (distance <= -p->colWorld.obb.e[i]) {
			distance = -p->colWorld.obb.e[i];
		}
		q += distance * p->colWorld.obb.axis[i];
	}
	return m - q;
}

/*
�i�[�\�ȃA�N�^�[�����m�ۂ���

@param reserveCount �A�N�^�[�z��̊m�ې�
*/
void EnemyActorList::Reserve(size_t reserveCount)
{
	enemies.reserve(reserveCount);
}

/*
�A�N�^�[��ǉ�����

@param actor �ǉ�����A�N�^�[
*/
void EnemyActorList::Add(const EnemyActorPtr& actor)
{
	enemies.push_back(actor);
}

/*
�A�N�^�[���폜����

@param actor �폜����A�N�^�[
*/
bool EnemyActorList::Remove(const EnemyActorPtr& actor)
{
	//�|�C���^����v����A�N�^��T����
	//�������������
	for (auto itr = enemies.begin(); itr != enemies.end(); ++itr)
	{
		if (*itr == actor)
		{
			enemies.erase(itr);
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
glm::ivec2 EnemyActorList::CalcMapIndex(const glm::vec3& pos) const
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
void EnemyActorList::Update(float deltaTime)
{
	//�͈�for��
	for (const EnemyActorPtr& e : enemies)
	{
		if (e && e->health > 0)
		{
			e->Update(deltaTime);
		}
	}

	for (auto i = enemies.begin(); i != enemies.end();)
	{
		const EnemyActorPtr& e = *i;
		if (!e || e->health <= 0)
		{
			// erase => vector����v�f���폜���邽�߂Ɏg��
			// ���s��A���̗v�f���w���l��Ԃ��Ă����
			i = enemies.erase(i);
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
	for (auto i = enemies.begin(); i != enemies.end(); ++i)
	{
		const glm::ivec2 mapIndex = CalcMapIndex((*i)->position);
		grid[mapIndex.y][mapIndex.x].push_back(*i);
	}
}



/*
�A�N�^�[��`��f�[�^���X�V����
*/
void EnemyActorList::UpdateDrawData(float deltaTime)
{
	for (const EnemyActorPtr& e : enemies)
	{
		if (e && e->health > 0) {
			e->UpdateDrawData(deltaTime);
		}
	}
}

/*
�A�N�^�[��`�悷��
*/
void EnemyActorList::Draw()
{
	for (const EnemyActorPtr& e : enemies)
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
std::vector<EnemyActorPtr> EnemyActorList::FindNearbyActors(
	const glm::vec3& pos, float maxDistance) const
{
	std::vector<std::pair<float, EnemyActorPtr>> buffer;
	buffer.reserve(1000);

	const glm::ivec2 mapIndex = CalcMapIndex(pos);
	const glm::ivec2 min = glm::max(mapIndex - 1, 0);
	const glm::ivec2 max = glm::min(
		mapIndex + 1, glm::ivec2(sepalationSizeX - 1, sepalationSizeY - 1));
	for (int y = min.y; y <= max.y; ++y)
	{
		for (int x = min.x; x <= max.x; ++x)
		{
			const std::vector<EnemyActorPtr>& list = grid[y][x];
			for (auto actor : list)
			{
				const float distance = glm::distance(glm::vec3(actor->position), pos);
				buffer.push_back(std::make_pair(distance, actor));
			}
		}
	}

	std::vector<EnemyActorPtr> result;
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