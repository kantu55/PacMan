/*
@file MainGameScene.cpp
*/
#include "GLFWEW.h"
#include "MainGameScene.h"
#include "GameClearScene.h"
#include "GameOverScene.h"
#include "SkeletalMeshActor.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <random>

/*
�Փ˂���������

@param a �Փ˂����A�N�^�[����1
@param b �Փ˂����A�N�^�[����2
@param p �Փˈʒu
*/
void PlayerCollisionHandler(const ActorPtr& a, const ActorPtr& b, const glm::vec3& p)
{
	const glm::vec3 v = a->colWorld.s.center - p;
	// �Փˈʒu�Ƌ������߂����Ȃ������ׂ�
	if (dot(v, v) > FLT_EPSILON)
	{
		// a��b���d�Ȃ�Ȃ��ʒu�܂ňړ�
		const glm::vec3 vn = normalize(v);
		float radiusSum = a->colWorld.s.r;
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
		a->position += vn * distance;
		a->colWorld.s.center += vn * distance;
		//�v���C���[���Փ˔���`��̏�ɏ������Y�̈ړ��x�N�g����0�ɂ���
		//����60�x�͈͓̔��������画�肷��
		if (a->velocity.y < 0 && vn.y >= glm::cos(glm::radians(60.0f)))
		{
			a->velocity.y = 0;
		}
	}
	else
	{
		// �ړ���������(�������߂�����ꍇ�̗�O����)
		const float deltaTime = static_cast<float>(GLFWEW::Window::Instance().DeltaTime());
		const glm::vec3 deltaVelocity = a->velocity * deltaTime;
		a->position -= deltaVelocity;
		a->colWorld.s.center -= deltaVelocity;
	}
}

void MainGameScene::CreateSpriteWall(glm::vec3 position,int axis, int index)
{
	if (axis == 0)
	{
		index *= 21.0f;
		float x = 615;
		float y = 205;
		sprWall.SetPosition(glm::vec3(x + position.x + index, y + position.y, 0));
		sprWall.SetScale(glm::vec2(1.5f, 0.07f));
	}
	else if (axis == 1)
	{
		index *= 22.75f;
		float x = 615;
		float y = 214;
		sprWall.SetPosition(glm::vec3(x + position.x, y + position.y + index, 0));
		sprWall.SetScale(glm::vec2(0.4f, 0.35f));
	}
	sprites.push_back(sprWall);
}

StaticMeshActorPtr MainGameScene::CreateStoneWall(glm::vec3 pos, float rot, int axsis, int size)
{
	size *= 2;
	if (axsis == 0)
	{
		const Mesh::FilePtr meshStoneWall = meshBuffer.GetFile("Res/wall_stone.gltf");
		glm::vec3 position = glm::vec3(pos.x + size, 0, pos.z);
		position.y = heightMap.Height(position);
		StaticMeshActorPtr p = std::make_shared<StaticMeshActor>(
			meshStoneWall, "StoneWall", 100, position, glm::vec3(0));
		p->scale.y = 0.7f;
		p->scale.x = 0.5f;
		p->scale.z = 0.5f;
		p->colLocal = Collision::CreateOBB(glm::vec3(0, 0, 0),
			glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, -1), glm::vec3(0.6f, 1, 0.25f));
		p->rotation = glm::vec3(0, glm::radians(rot), 0);
		return p;
	}
	if (axsis == 1)
	{
		const Mesh::FilePtr meshStoneWall = meshBuffer.GetFile("Res/wall_stone.gltf");
		glm::vec3 position = glm::vec3(pos.x, 0, pos.z + size);
		position.y = heightMap.Height(position);
		StaticMeshActorPtr p = std::make_shared<StaticMeshActor>(
			meshStoneWall, "StoneWall", 100, position, glm::vec3(0));
		p->scale.y = 0.8f;
		p->scale.x = 0.5f;
		p->scale.z = 0.5f;
		p->colLocal = Collision::CreateOBB(glm::vec3(0, 0, 0),
			glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, -1), glm::vec3(0.25f, 1, 0.6f));
		p->rotation = glm::vec3(0, glm::radians(rot), 0);
		return p;
	}
	return NULL;
}

bool MainGameScene::Initialize()
{
	GLFWEW::Window& window = GLFWEW::Window::Instance();
	spriteRenderer.Init(1000, "Res/Sprite.vert", "Res/Sprite.frag");
	sprites.reserve(100);
	Sprite spr(Texture::Image2D::Create("Res/Sora.tga"));
	spr.SetPosition(glm::vec3(0, 0, 500.0f));
	spr.SetScale(glm::vec2(2));
	sprites.push_back(spr);
	// �v���C���[�̉摜
	sprPlayer.SetTexture(Texture::Image2D::Create("Res/Player.tga"));
	sprPlayer.SetScale(glm::vec2(0.125f));
	// �G�̉摜
	sprEnemy.SetTexture(Texture::Image2D::Create("Res/Oni.tga"));
	sprEnemy.SetScale(glm::vec2(0.015f));
	// �n���̉摜
	sprJizo.SetTexture(Texture::Image2D::Create("Res/Jizo.tga"));
	sprJizo.SetScale(glm::vec2(0.05f));
	// �~�j�}�b�v�̉摜
	sprMiniMap.SetTexture(Texture::Image2D::Create("Res/MiniMap.tga"));
	sprMiniMap.SetPosition(glm::vec3(750, 350, 0));
	sprMiniMap.SetScale(glm::vec2(0.8f));
	sprMiniMap.SetColor(glm::vec4(1,1,1,0.8f));
	sprites.push_back(sprMiniMap);
	// �S�[���̉摜
	sprGoal.SetTexture(Texture::Image2D::Create("Res/Goal.tga"));
	sprGoal.SetPosition(glm::vec3(885, 505, 0));
	sprGoal.SetScale(glm::vec2(0.03f));
	sprGoal.SetColor(glm::vec4(1, 1, 1, 0.8f));
	sprites.push_back(sprGoal);
	// �ǂ̉摜
	sprWall.SetTexture(Texture::Image2D::Create("Res/Wall.tga"));
	sprWall.SetColor(glm::vec4(1, 1, 1, 0.7f));
	// �t�F�[�h�p�̉摜(��)
	spriteRenderer.BeginUpdate();
	sprBlackFade.SetTexture(Texture::Image2D::Create("Res/black.tga"));
	sprBlackFade.SetScale(glm::vec2(10.0f));
	spriteRenderer.AddVertices(sprBlackFade);
	sprBlackFade.SetColor(glm::vec4(1, 1, 1, 1));
	spriteRenderer.EndUpdate();
	// �t�F�[�h�p�̉摜(��)
	sprWhiteFade.SetTexture(Texture::Image2D::Create("Res/White.tga"));

	bgm = Audio::Engine::Instance().Prepare("Res/Audio/MainGameBGM.mp3");
	bgm->Play(Audio::Flag_Loop);
	seDamage = Audio::Engine::Instance().Prepare("Res/Audio/Damage.mp3");
	
	fontRenderer.Init(1000);
	fontRenderer.LoadFromFile("Res/font.fnt");
	fntJizo.Init(1000);
	fntJizo.LoadFromFile("Res/font.fnt");
	fntCount.Init(1000);
	fntCount.LoadFromFile("Res/font.fnt");

	meshBuffer.Init(1'000'000 * sizeof(Mesh::Vertex), 3'000'000 * sizeof(GLushort));

	meshBuffer.LoadMesh("Res/red_pine_tree.gltf");
	meshBuffer.LoadMesh("Res/wall_stone.gltf");
	meshBuffer.LoadMesh("Res/jizo_statue.gltf");
	meshBuffer.LoadSkeletalMesh("Res/bikuni.gltf");
	meshBuffer.LoadSkeletalMesh("Res/oni_small.gltf");

	//�n�C�g�}�b�v���쐬����
	if (!heightMap.LoadFromFile("Res/Terrain.tga", 20.0f, 0.5f))
	{
		return false;
	}
	if (!heightMap.CreateMesh(meshBuffer, "Terrain"))
	{
		return false;
	}
	lightBuffer.Init(1);
	lightBuffer.BindToShader(meshBuffer.GetStaticMeshShader());
	lightBuffer.BindToShader(meshBuffer.GetTerrainShader());
	glm::vec3 startPos(89.5f, 0, 95);
	startPos.y = heightMap.Height(startPos);
	player = std::make_shared<PlayerActor>( &heightMap, meshBuffer, startPos);
	cameraRadius = glm::radians(180.0f);
	cameraRotate = player->rotation.y;
	
	objects.Reserve(500);
	rand.seed(0);

	// ���n����z�u
	for(int i = 0; i < 4; i++)
	{
		if (i == 0)
		{
			glm::vec3 position(89, 0, 92);
			position.y = heightMap.Height(position);
			JizoActorPtr p = std::make_shared<JizoActor>(
				meshBuffer.GetFile("Res/jizo_statue.gltf"), position, i, this);
			p->rotation.y = p->rotation.y + glm::radians(180.0f);
			p->colLocal = Collision::CreateOBB(glm::vec3(0, 0, 0),
				glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, -1), glm::vec3(1, 1, 0.5f));
			p->scale = glm::vec3(3);  // �����₷���悤�Ɋg��
			objects.Add(p);
		}
		if (i == 1)
		{
			glm::vec3 position(113.5f, 0, 88);
			position.y = heightMap.Height(position);
			JizoActorPtr p = std::make_shared<JizoActor>(
				meshBuffer.GetFile("Res/jizo_statue.gltf"), position, i, this);
			p->colLocal = Collision::CreateOBB(glm::vec3(0, 0, 0),
				glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, -1), glm::vec3(1, 1, 0.5f));
			p->scale = glm::vec3(3);  // �����₷���悤�Ɋg��
			objects.Add(p);
		}
		if (i == 2)
		{
			glm::vec3 position(88.5f, 0, 72.0f);
			position.y = heightMap.Height(position);
			JizoActorPtr p = std::make_shared<JizoActor>(
				meshBuffer.GetFile("Res/jizo_statue.gltf"), position, i, this);
			p->colLocal = Collision::CreateOBB(glm::vec3(0, 0, 0),
				glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, -1), glm::vec3(1, 1, 0.5f));
			p->scale = glm::vec3(3);  // �����₷���悤�Ɋg��
			objects.Add(p);
		}
		if (i == 3)
		{
			glm::vec3 position(113.5f, 0, 86);
			position.y = heightMap.Height(position);
			JizoActorPtr p = std::make_shared<JizoActor>(
				meshBuffer.GetFile("Res/jizo_statue.gltf"), position, i, this);
			p->rotation.y = p->rotation.y + glm::radians(180.0f);
			p->colLocal = Collision::CreateOBB(glm::vec3(0, 0, 0),
				glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, -1), glm::vec3(1, 1, 0.5f));
			p->scale = glm::vec3(3);  // �����₷���悤�Ɋg��
			objects.Add(p);
		}
	}
	sprJizo.SetPosition(glm::vec3(610, 100, 0));
	sprJizo.SetScale(glm::vec2(0.1f));
	sprites.push_back(sprJizo);

	// �ǂ�z�u
	{
		size_t outerWallCount = 11;
		const size_t wallDirectoin = 1;
		objects.Reserve(outerWallCount);
		float sprSlide = 0;
		sprWall.SetPosition(glm::vec3(737, 503, 0));
		sprWall.SetScale(glm::vec2(18, 0.125f));
		sprites.push_back(sprWall);
		for (size_t east = 0; east < outerWallCount + 2; ++east)
		{
			glm::vec3 position = glm::vec3(87.75f, 0, 70.5f);
			position.y = heightMap.Height(position);
			StaticMeshActorPtr p = CreateStoneWall(position, 0, x, east);
			objects.Add(p);
		}
		

		sprSlide = 0;
		sprWall.SetPosition(glm::vec3(750, 197.5f, 0));
		sprWall.SetScale(glm::vec2(19.5, 0.125f));
		sprites.push_back(sprWall);
		for (size_t west = 0; west < outerWallCount + 3; ++west)
		{
			glm::vec3 position = glm::vec3(87.75f, 0, 97.25f);
			position.y = heightMap.Height(position);
			StaticMeshActorPtr p = CreateStoneWall(position, 0, x, west);
			objects.Add(p);
			sprSlide += 21;
		}

		outerWallCount = 13;
		sprWall.SetPosition(glm::vec3(600, 350, 0));
		sprWall.SetScale(glm::vec2(0.5f, 4.5f));
		sprites.push_back(sprWall);
		for (size_t north = 0; north < outerWallCount; ++north)
		{
			glm::vec3 position = glm::vec3(87.0f, 0, 72.0f);
			position.y = heightMap.Height(position);
			StaticMeshActorPtr p = CreateStoneWall(position, 90.0f, z, north);
			objects.Add(p);
			sprSlide += 42.5f;
		}

		sprWall.SetPosition(glm::vec3(900, 350, 0));
		sprWall.SetScale(glm::vec2(0.5f, 4.5f));
		sprites.push_back(sprWall);
		for (size_t south = 0; south < outerWallCount; ++south)
		{
			glm::vec3 position = glm::vec3(115.0f, 0, 72.5f);
			position.y = heightMap.Height(position);
			StaticMeshActorPtr p = CreateStoneWall(position, 90.0f, z, south);
			objects.Add(p);
			sprSlide += 42.5f;
		}
	}

	{
		size_t left = 6;
		size_t right = 5;
		float sprSlide = 0;
		glm::vec3 position = glm::vec3(92.0f, 0, 87.0f);
		position.y = heightMap.Height(position);
		for (size_t i = 0; i < left; i++)
		{
			StaticMeshActorPtr p = CreateStoneWall(position, 0, x, i);
			objects.Add(p);
			CreateSpriteWall(glm::vec3(30, 110, 0), x, i);
		}

		for (size_t i = 0; i < right; i++)
		{
			position = glm::vec3(106.0f, 0, 87.0f);
			position.y = heightMap.Height(position);
			StaticMeshActorPtr p = CreateStoneWall(position, 0, x, i);
			objects.Add(p);
			CreateSpriteWall(glm::vec3(187, 110, 0), x, i);
		}
	}

	{
		float sprSlide = 0;
		glm::vec3 position;

		for (int i = 0; i < 3; i++)
		{
			position = glm::vec3(95, 0, 91);
			position.y = heightMap.Height(position);
			StaticMeshActorPtr p = CreateStoneWall(position, 0, x, i);
			objects.Add(p);
			CreateSpriteWall(glm::vec3(67, 20, 0), x, i);
		}

		for (int i = 0; i < 3; i++)
		{
			position = glm::vec3(95.0f, 0, 94.75f);
			position.y = heightMap.Height(position);
			StaticMeshActorPtr p = CreateStoneWall(position, 0, x, i);
			objects.Add(p);
			CreateSpriteWall(glm::vec3(67, 68, 0), x, i);
		}

		for (int i = 0; i < 2; i++)
		{
			position = glm::vec3(88.25f, 0, 93.0f);
			StaticMeshActorPtr p = CreateStoneWall(position, 0, x, i);
			objects.Add(p);
			CreateSpriteWall(glm::vec3(-8, 45, 0), x, i);
		}

		for (int i = 0; i < 2; i++)
		{
			position = glm::vec3(111.5f, 0, 87.75f);
			position.y = heightMap.Height(position);
			StaticMeshActorPtr p = CreateStoneWall(position, 90, z, i);
			objects.Add(p);
			CreateSpriteWall(glm::vec3(240, 68, 0), z, i);
		}



		for (int i = 0; i < 1; i++)
		{
			position = glm::vec3(111.5f, 0, 96.0f);
			position.y = heightMap.Height(position);
			StaticMeshActorPtr p = CreateStoneWall(position, 90, z, i);
			objects.Add(p);
			CreateSpriteWall(glm::vec3(240, 0, 0), z, i);
		}

		for (int i = 0; i < 4; i++)
		{
			position = glm::vec3(91.0, 0, 87.75f);
			position.y = heightMap.Height(position);
			StaticMeshActorPtr p = CreateStoneWall(position, 90, z, i);
			objects.Add(p);
			CreateSpriteWall(glm::vec3(21, 22.75f, 0), z, i);
		}

		for (int i = 0; i < 2; i++)
		{
			position = glm::vec3(105.5f, 0, 91);
			position.y = heightMap.Height(position);
			StaticMeshActorPtr p = CreateStoneWall(position, 90, z, i);
			objects.Add(p);
			CreateSpriteWall(glm::vec3(179, 35, 0), z, i);
		}

		for (int i = 0; i < 2; i++)
		{
			position = glm::vec3(102.5f, 0, 91);
			position.y = heightMap.Height(position);
			StaticMeshActorPtr p = CreateStoneWall(position, 90, z, i);
			objects.Add(p);
			CreateSpriteWall(glm::vec3(209, 35, 0), z, i);
		}

		for (int i = 0; i < 2; i++)
		{
			position = glm::vec3(108.5f, 0, 91);
			position.y = heightMap.Height(position);
			StaticMeshActorPtr p = CreateStoneWall(position, 90, z, i);
			objects.Add(p);
			CreateSpriteWall(glm::vec3(149, 35, 0), z, i);
		}
	}

	for(int i = 0; i < 2; i++)
	{
		glm::vec3 position = glm::vec3(105.25f, 0, 84);
		position.y = heightMap.Height(position);
		StaticMeshActorPtr p = CreateStoneWall(position, 90.0f, z, i);
		objects.Add(p);
		CreateSpriteWall(glm::vec3(179, 112, 0), z, i);
	}

	for (int i = 0; i < 2; i++)
	{
		glm::vec3 position = glm::vec3(102.5f, 0, 83);
		position.y = heightMap.Height(position);
		StaticMeshActorPtr p = CreateStoneWall(position, 0.0f, x, i);
		objects.Add(p);
		CreateSpriteWall(glm::vec3(150, 157, 0), x, i);
	}

	for (size_t i = 0; i < 2; ++i)
	{
		glm::vec3 position = glm::vec3(92.25f, 0, 73.25f);
		position.y = heightMap.Height(position);
		StaticMeshActorPtr p = CreateStoneWall(position, 0.0f, x, i);
		objects.Add(p); 
		CreateSpriteWall(glm::vec3(34, 270, 0), x, i);
	}

	{
		glm::vec3 position = glm::vec3(91.0f, 0, 74);
		position.y = heightMap.Height(position);
		StaticMeshActorPtr p = CreateStoneWall(position, 90.0f, z, 0);
		objects.Add(p);
		CreateSpriteWall(glm::vec3(21, 230, 0), z, 1);
	}

	{
		glm::vec3 position = glm::vec3(91.0f, 0, 78);
		position.y = heightMap.Height(position);
		StaticMeshActorPtr p = CreateStoneWall(position, 90.0f, z, 0);
		objects.Add(p);
		CreateSpriteWall(glm::vec3(21, 185.0f, 0), z, 1);
	}

	for (size_t i = 0; i < 2; ++i)
	{
		glm::vec3 position = glm::vec3(91.0f, 0, 82);
		position.y = heightMap.Height(position);
		StaticMeshActorPtr p = CreateStoneWall(position, 90.0f, z, i);
		objects.Add(p);
		CreateSpriteWall(glm::vec3(21, 140.0f, 0), z, i);
	}

	for (size_t i = 0; i < 3; ++i)
	{
		glm::vec3 position = glm::vec3(94.75f, 0, 77.5f);
		position.y = heightMap.Height(position);
		StaticMeshActorPtr p = CreateStoneWall(position, 90.0f, z, i);
		objects.Add(p);
		CreateSpriteWall(glm::vec3(62, 168.5f, 0), z, i);
	}

	for (size_t i = 0; i < 2; ++i)
	{
		glm::vec3 position = glm::vec3(98.75f, 0, 77.0f);
		position.y = heightMap.Height(position);
		StaticMeshActorPtr p = CreateStoneWall(position, 90.0f, z, i);
		objects.Add(p);
		CreateSpriteWall(glm::vec3(104, 197.0f, 0), z, i);
	}

	for (size_t i = 0; i < 3; ++i)
	{
		glm::vec3 position = glm::vec3(108.25f, 0, 77.75f);
		position.y = heightMap.Height(position);
		StaticMeshActorPtr p = CreateStoneWall(position, 90.0f, z, i);
		objects.Add(p);
		CreateSpriteWall(glm::vec3(211, 161.0f, 0), z, i);
	}

	for (size_t i = 0; i < 2; ++i)
	{
		glm::vec3 position = glm::vec3(111.75f, 0, 77.75f);
		position.y = heightMap.Height(position);
		StaticMeshActorPtr p = CreateStoneWall(position, 90.0f, z, i);
		objects.Add(p);
		CreateSpriteWall(glm::vec3(249, 184.0f, 0), z, i);
	}

	for (size_t i = 0; i < 2; ++i)
	{
		glm::vec3 position = glm::vec3(95.5f, 0, 76.25f);
		position.y = heightMap.Height(position);
		StaticMeshActorPtr p = CreateStoneWall(position, 0.0f, x, i);
		objects.Add(p);
		CreateSpriteWall(glm::vec3(70, 238, 0), x, i);
	}

	for (size_t i = 0; i < 2; ++i)
	{
		glm::vec3 position = glm::vec3(109.0f, 0, 83);
		position.y = heightMap.Height(position);
		StaticMeshActorPtr p = CreateStoneWall(position, 0.0f, x, i);
		objects.Add(p);
		CreateSpriteWall(glm::vec3(220, 157, 0), x, i);
	}

	for (size_t i = 0; i < 2; ++i)
	{
		glm::vec3 position = glm::vec3(96.0f, 0, 82.25f);
		position.y = heightMap.Height(position);
		StaticMeshActorPtr p = CreateStoneWall(position, 0.0f, x, i);
		objects.Add(p);
		CreateSpriteWall(glm::vec3(75, 167, 0), x, i);
	}

	for (size_t i = 0; i < 3; ++i)
	{
		glm::vec3 position = glm::vec3(101.75f, 0, 74.75f);
		position.y = heightMap.Height(position);
		StaticMeshActorPtr p = CreateStoneWall(position, 90.0f, z, i);
		objects.Add(p);
		CreateSpriteWall(glm::vec3(142, 200.0f, 0), z, i);
	}

	{
		glm::vec3 position = glm::vec3(105.25f, 0, 74.75f);
		position.y = heightMap.Height(position);
		StaticMeshActorPtr p = CreateStoneWall(position, 90.0f, z, 0);
		objects.Add(p);
		CreateSpriteWall(glm::vec3(179, 177.0f, 0), z, 1);
	}

	{
		glm::vec3 position = glm::vec3(105.25f, 0, 78.75f);
		position.y = heightMap.Height(position);
		StaticMeshActorPtr p = CreateStoneWall(position, 90.0f, z, 0);
		objects.Add(p);
		CreateSpriteWall(glm::vec3(179, 222.0f, 0), z, 1);
	}

	

	for (size_t i = 0; i < 2; ++i)
	{
		glm::vec3 position = glm::vec3(102.5f, 0, 79.75f);
		position.y = heightMap.Height(position);
		StaticMeshActorPtr p = CreateStoneWall(position, 0.0f, x, i);
		objects.Add(p);
		CreateSpriteWall(glm::vec3(150, 195, 0), x, i);
	}

	for (size_t i = 0; i < 2; ++i)
	{
		glm::vec3 position = glm::vec3(102.5f, 0, 73.5f);
		position.y = heightMap.Height(position);
		StaticMeshActorPtr p = CreateStoneWall(position, 0.0f, x, i);
		objects.Add(p);
		CreateSpriteWall(glm::vec3(150, 267, 0), x, i);
	}

	for (size_t i = 0; i < 2; ++i)
	{
		glm::vec3 position = glm::vec3(109.75f, 0, 73.0f);
		position.y = heightMap.Height(position);
		StaticMeshActorPtr p = CreateStoneWall(position, 0.0f, x, i);
		objects.Add(p);
		CreateSpriteWall(glm::vec3(225, 269, 0), x, i);
	}

	{
		glm::vec3 position = glm::vec3(112.5f, 0, 71.75f);
		position.y = heightMap.Height(position);
		StaticMeshActorPtr p = CreateStoneWall(position, 90.0f, z, 0);
		objects.Add(p);
		CreateSpriteWall(glm::vec3(253, 250.0f, 0), z, 1);
	}

	// ���C�g�̔z�u
	lights.Add(std::make_shared<DirectionalLightActor>(
		"DirectionalLight", glm::vec3(1.0f), glm::normalize(glm::vec3(1, -2, -1))));

	glm::vec3 color(5, 0.8f, 0.5f);
	glm::vec3 position(0);
	glm::vec3 direction(0, 5, 0);

	// �G��z�u
	{
		// ���񂷂�G
		{
			const size_t oniCount = 5;
			for (size_t i = 0; i < oniCount; ++i)
			{
				if (i == 0)
				{
					LoiteringEnemyPtr p = 
						std::make_shared<LoiteringEnemy>(&heightMap, meshBuffer, glm::vec3(93, 0, 89));
					p->roundPoints[0] = glm::vec3(100, 0, 93);
					p->roundPoints[1] = glm::vec3(101, 0, 89);
					p->roundPoints[2] = glm::vec3(93, 0, 89);
					p->roundPoints[3] = glm::vec3(93, 0, 93);
					p->roundPoints[4] = glm::vec3(96, 0, 93);
					p->position.y = heightMap.Height(p->position);
					p->ObstacleActor(objects);
					enemies.Add(p);
				}
				else if (i == 1)
				{
					LoiteringEnemyPtr p = 
						std::make_shared<LoiteringEnemy>(&heightMap, meshBuffer, glm::vec3(106, 0, 89));
					p->roundPoints[0] = glm::vec3(110, 0, 94);
					p->roundPoints[1] = glm::vec3(107, 0, 94);
					p->roundPoints[2] = glm::vec3(107, 0, 89);
					p->roundPoints[3] = glm::vec3(110, 0, 94);
					p->roundPoints[4] = glm::vec3(110, 0, 91);
					p->position.y = heightMap.Height(p->position);
					p->ObstacleActor(objects);
					enemies.Add(p);
				}
				else if (i == 2)
				{
					LoiteringEnemyPtr p = 
						std::make_shared<LoiteringEnemy>(&heightMap, meshBuffer, glm::vec3(100, 0, 72.5f));
					p->roundPoints[0] = glm::vec3(100, 0, 85);
					p->roundPoints[1] = glm::vec3(93, 0, 85);
					p->roundPoints[2] = glm::vec3(93, 0, 76);
					p->roundPoints[3] = glm::vec3(100, 0, 76);
					p->roundPoints[4] = glm::vec3(98, 0, 85);
					p->position.y = heightMap.Height(p->position);
					p->ObstacleActor(objects);
					enemies.Add(p);
				}
				else if (i == 3)
				{
					LoiteringEnemyPtr p = 
						std::make_shared<LoiteringEnemy>(&heightMap, meshBuffer, glm::vec3(107, 0, 72.5f));
					p->roundPoints[0] = glm::vec3(107, 0, 82);
					p->roundPoints[1] = glm::vec3(107, 0, 74);
					p->roundPoints[2] = glm::vec3(107, 0, 82);
					p->roundPoints[3] = glm::vec3(107, 0, 74);
					p->roundPoints[4] = glm::vec3(107, 0, 82);
					p->position.y = heightMap.Height(p->position);
					p->ObstacleActor(objects);
					enemies.Add(p);
				}
				else if (i == 4)
				{
					LoiteringEnemyPtr p = 
						std::make_shared<LoiteringEnemy>(&heightMap, meshBuffer, glm::vec3(109, 0, 74));
					p->roundPoints[0] = glm::vec3(113, 0, 82);
					p->roundPoints[1] = glm::vec3(110, 0, 80);
					p->roundPoints[2] = glm::vec3(110, 0, 75);
					p->roundPoints[3] = glm::vec3(113, 0, 75);
					p->roundPoints[4] = glm::vec3(113, 0, 82);
					p->position.y = heightMap.Height(p->position);
					p->ObstacleActor(objects);
					enemies.Add(p);
				}
			}

			// ���n���G
			OverlookEnemyPtr p;
			p = std::make_shared<OverlookEnemy>(&heightMap, meshBuffer, glm::vec3(88, 0, 80));
			p->position.y = heightMap.Height(p->position);
			p->rotation.y = p->rotation.y + glm::radians(90.0f);
			p->ObstacleActor(objects);
			enemies.Add(p);

			// ���
			for(int i = 0; i < 2; i++)
			{
				if (i == 0)
				{
					G_enemy = std::make_shared<GateKeeperEnemy>(&heightMap, meshBuffer, glm::vec3(104, 0, 87));
					G_enemy->position.y = heightMap.Height(G_enemy->position);
					G_enemy->ObstacleActor(objects);
					enemies.Add(G_enemy);
				}
				else if (i == 1)
				{
					G_enemy = std::make_shared<GateKeeperEnemy>(&heightMap, meshBuffer, glm::vec3(94, 0, 71));
					G_enemy->position.y = heightMap.Height(G_enemy->position);
					G_enemy->rotation.y = glm::radians(90.0f);
					G_enemy->ObstacleActor(objects);
					enemies.Add(G_enemy);
				}
			}

			// �ǂ�����������G
			{
				H_enemy = std::make_shared<HuntEnemy>(&heightMap, meshBuffer, glm::vec3(113, 0, 96));
				H_enemy->position.y = heightMap.Height(H_enemy->position);
				H_enemy->rotation.y = glm::radians(180.0f);
				H_enemy->ObstacleActor(objects);
				enemies.Add(H_enemy);
			}
		}
		lights.Update(0);
		lightBuffer.Update(lights, glm::vec3(0.1f, 0.05f, 0.15f));
		heightMap.UpdateLightIndex(lights);
		
#if 0
		for (size_t i = 0; i < oniCount; ++i)
		{
			//�G�̈ʒu��(50,50)-(150,150)�͈̔͂��烉���_���ɑI��
			glm::vec3 position(0);
			position.x = std::uniform_real_distribution<float>(85, 115)(rand);
			position.z = std::uniform_real_distribution<float>(70, 100)(rand);
			position.y = heightMap.Height(position);
			//�G�̌����������_���ɑI��
			glm::vec3 rotation(0);
			rotation.y = std::uniform_real_distribution<float>(0, glm::pi<float>() * 2)(rand);
			const Mesh::SkeletalMeshPtr mesh = meshBuffer.GetSkeletalMesh("oni_small");
			SkeletalMeshActorPtr p = std::make_shared<SkeletalMeshActor>(
				mesh, "Kooni", 13, position, rotation);
			p->GetMesh()->Play("Wait");

			p->colLocal = Collision::CreateCapsule(
				glm::vec3(0, 0.5f, 0), glm::vec3(0, 1, 0), 0.5f);
			enemies.Add(p);
		}
#endif
	}
	window.CenterCursor();
	return true;
}
/*
�v���C���[�̓��͂���������
*/
void MainGameScene::ProcessInput()
{
	GLFWEW::Window& window = GLFWEW::Window::Instance();
	const GamePad gamepad = GLFWEW::Window::Instance().GetGamePad();
	player->ProcessInput();

	if (gamepad.buttons & GamePad::START)
	{
		if (state == GameState::play)
		{
			// �Q�[���p�ƃf�o�b�O�p�J�����̐؂�ւ�
			if (c_state == CameraState::playcamera)
			{
				c_state = CameraState::debug;
			}
			else if (c_state == CameraState::debug)
			{
				c_state = CameraState::playcamera;
				camera.target = player->position + glm::vec3(0, 0, -5);
				camera.position = player->position + glm::vec3(0, 4, 5);
			}
		}
	}
}

/*
�V�[�����X�V����

@param deltaTime �O��̍X�V����̌o�ߎ��ԁi�b�j
*/
void MainGameScene::Update(float deltaTime)
{
	const GLFWEW::Window& window = GLFWEW::Window::Instance();
	const double w = window.Width();
	const double h = window.Height();
	const float lineHenght = fontRenderer.LineHeight();

	// �t�H���g�̕\��
	fntJizo.BeginUpdate();
	if (!relayFrag)
	{
		fntJizo.AddString(glm::vec2(580, 150), L"�ړI�F���n���ɐG���");
	}
	else
	{
		fntJizo.AddString(glm::vec2(580, 150), L"�ړI�F�S�[����ڎw��");
	}
	switch (jizoCount)
	{
	case 0:
		fntJizo.AddString(glm::vec2(650, 90), L"�c��F4");
		break;
	case 1:
		fntJizo.AddString(glm::vec2(650, 90), L"�c��F3");
		break;
	case 2:
		fntJizo.AddString(glm::vec2(650, 90), L"�c��F2");
		break;
	case 3:
		fntJizo.AddString(glm::vec2(650, 90), L"�c��F1");
		break;
	case 4:
		fntJizo.AddString(glm::vec2(650, 90), L"�c��F0");
		break;
	}
	fntJizo.EndUpdate();
	fontRenderer.BeginUpdate();

	// �Q�[���N���A�ƃQ�[���I�[�o�[����
	switch (state)
	{
	case MainGameScene::clear:
		spriteRenderer.BeginUpdate();
		fadeTimer += deltaTime;
		sprWhiteFade.SetColor(glm::vec4(1, 1, 1, fadeTimer));
		spriteRenderer.AddVertices(sprWhiteFade);
		spriteRenderer.EndUpdate();
		if (fadeTimer >= 1)
		{
			bgm->Stop();
			SceneStack::Instance().Replace(std::make_shared<GameClearScene>());
		}
		return;
	case MainGameScene::over:
		spriteRenderer.BeginUpdate();
		fadeTimer += deltaTime;
		sprBlackFade.SetColor(glm::vec4(1, 1, 1, fadeTimer));
		spriteRenderer.AddVertices(sprBlackFade);
		spriteRenderer.EndUpdate();
		if (fadeTimer >= 1)
		{
			bgm->Stop();
			SceneStack::Instance().Replace(std::make_shared<GameOverScene>());
		}
		return;
	}
	
	//�J�����̏�Ԃ��X�V����
	if(c_state == CameraState::playcamera)
	{
		player->debugFrag = true;
		camera.target = player->position + glm::vec3(0, 4, 1);
		camera.position = player->position + glm::vec3(0, 12, 8);
	}
	// �f�o�b�O�p�̃J����
	if (c_state == CameraState::debug)
	{
		player->debugFrag = true;
		camera.target = glm::vec3(100, 0, 85);
		camera.position = glm::vec3(100, 50, 86);
		camera.target.y = heightMap.Height(camera.target) + 3.0f;
		camera.position.y = heightMap.Height(camera.position) + 60.0f;
	}
	
	for (auto& e : enemies)
	{
		e->TargetActor(player);
		// �G�̓����蔻�菈��
		DetectCollision(player, e);
		DetectCollision(e, objects);
		ActorPtr enemyAttackCollision = e->GetAttackCollision();
		if (enemyAttackCollision)
		{
			bool hit = false;
			// �U�����v���[���[�ɓ���������X�e�[�g���Q�[���I�[�o�[�ɂ���
			DetectCollision(enemyAttackCollision, player,
				[this, &hit](const ActorPtr& a, const ActorPtr& b, const glm::vec3& p)
			{
				seDamage->Play();
				state = GameState::over;
			}
			);
		}
		// �ǂ�������t���O��������������ǂ�������
		if (e->mode == EnemyActor::Mode::freeze && isHunt)
		{
			e->mode = EnemyActor::Mode::hunt;
		}
		if (e->mode == EnemyActor::Mode::worning)
		{
			for (auto& e2 : enemies)
			{
				if (e2->mode == EnemyActor::Mode::normal)
				{
					glm::vec3 vector = e2->position - e->position;
					float length = glm::length(vector);
					if (length <= 10 && length > 0)
					{
						e2->TaskForcedTermination();
						e2->seenPos = e->position;
					}
				}
			}
		}
	}
	
	objects.Update(deltaTime);
	player->Update(deltaTime);
	enemies.Update(deltaTime);
	lights.Update(deltaTime);
	
	DetectCollision(player, objects);

	
	glm::vec3 goalPos = glm::vec3(113, 0, 69);
	goalPos.y = heightMap.Height(goalPos);
	if (EventFragPosition(player->position, goalPos) && relayFrag)
	{
		state = GameState::clear;
	}
	
	// �v���[���[�����Ԓn�_�ɒ�������ǂ�����������G�̒ǂ�������t���O������
	glm::vec3 wayPoint = glm::vec3(104, 0, 84);
	wayPoint.y = heightMap.Height(wayPoint);
	if (EventFragPosition(player->position, wayPoint) && !isWayPoint)
	{
		isWayPoint = true;
		isHunt = true;
	}

	// ���C�g�̍X�V
	glm::vec3 ambientColor(0.5f, 0.25f, 0.45f);
	lightBuffer.Update(lights, ambientColor);
	
	player->UpdateDrawData(deltaTime);
	enemies.UpdateDrawData(deltaTime);
	objects.UpdateDrawData(deltaTime);
	lights.UpdateDrawData(deltaTime);

	spriteRenderer.BeginUpdate();
	for (const Sprite& e : sprites)
	{
		spriteRenderer.AddVertices(e);
	}
	
	// �v���[���[���~�j�}�b�v�ɕ\��
	sprPlayer.SetPosition(sprEnemyPos + glm::vec3(player->position.x * 11.5f, player->position.z * -11.75f, 0));
	spriteRenderer.AddVertices(sprPlayer);

	// �ǂ�����������G���~�j�}�b�v�ɕ\��
	for (auto& e : enemies)
	{
		if (e->mode == EnemyActor::Mode::hunt ||
			e->mode == EnemyActor::Mode::freeze)
		{
			sprEnemy.SetRotation(e->rotation.y);
			sprEnemy.SetPosition(sprEnemyPos + glm::vec3(e->position.x * 11.5f, e->position.z * -11.75f, 0.0f));
			spriteRenderer.AddVertices(sprEnemy);
			break;
		}
	}

	// �t�F�[�h�̏���
	if (fadeTimer <= 1 && fadeTimer > 0)
	{
		fadeTimer -= deltaTime;
		sprBlackFade.SetColor(glm::vec4(1, 1, 1, fadeTimer));
		spriteRenderer.AddVertices(sprBlackFade);
	}
	else
	{
		sprBlackFade.SetColor(glm::vec4(1, 1, 1, 0));
		spriteRenderer.AddVertices(sprBlackFade);
	}

	
	fontRenderer.EndUpdate();
	spriteRenderer.EndUpdate();
}

/*
�V�[����`�悷��
*/
void MainGameScene::Render()
{
	const GLFWEW::Window& window = GLFWEW::Window::Instance();
	const glm::vec2 screenSize(window.Width(), window.Height());
	
	glEnable(GL_DEPTH_TEST);

	lightBuffer.Upload();
	lightBuffer.Bind();

	const glm::mat4 matView = glm::lookAt(camera.position, camera.target, camera.up);
	const float aspectRatio =
		static_cast<float>(window.Width()) / static_cast<float>(window.Height());
	const glm::mat4 matProj =
		glm::perspective(camera.fov * 0.5f, aspectRatio, camera.near, camera.far);
	
	glm::vec3 cubePos(100, 0, 100);
	cubePos.y = heightMap.Height(cubePos);
	
	const glm::mat4 matModel = glm::translate(glm::mat4(1), cubePos);
	meshBuffer.SetViewProjectionMatrix(matProj * matView);
	meshBuffer.SetCameraPosition(camera.position);
	meshBuffer.SetTime(window.Time());
	Mesh::Draw(meshBuffer.GetFile("Terrain"), glm::mat4(1));

	objects.Draw();
	enemies.Draw();
	player->Draw();
	spriteRenderer.Draw(screenSize);
	fntJizo.Draw(screenSize);
	
	// �f�t�H���g�̃t���[���o�b�t�@�ɕ`��
	{
		fontRenderer.Draw(screenSize);
	}
}

/*
���n���l�ɐG�ꂽ�Ƃ��̏���

@param id  ���n���l�̔ԍ�
@param pos ���n���l�̍��W

@retval true  ��������
@retval false ���łɐ퓬���̂��ߏ������Ȃ�
*/
bool MainGameScene::HandleJizoEffects(int id, const glm::vec3& pos)
{
	seGetJizo = Audio::Engine::Instance().Prepare("Res/Audio/get.mp3");
	seGetJizo->Play();
	jizoCount += 1;
	if (jizoCount >= 4)
	{
		relayFrag = true;
	}
	return true;
}

/*
�J�����̃p�����[�^���X�V����

@param matView �X�V�Ɏg�p����r���[�s��
*/
void MainGameScene::Camera::Update(const glm::mat4& matView)
{
	const glm::vec4 pos = matView * glm::vec4(target, 1);
	// �Q�[�����̒P�ʂ̓��[�g���P�ʂȂ̂�1000�{����K�v������
	focalPlane = pos.z * -1000.0f; 

	// ���a���甼�a�ɕς���
	const float imageDistance = sensorSize * 0.5f / glm::tan(fov * 0.5f);
	focalLength = 1.0f / ((1.0f / focalPlane) + (1.0f / imageDistance));
	apertrue = focalLength / fNumber;
}

bool MainGameScene::EventFragPosition(glm::vec3 playerPos, glm::vec3 pos)
{
	float length = glm::length(pos - playerPos);
	if (length <= 2.5f && length >= -2.5f)
	{
		return true;
	}
	return false;
}