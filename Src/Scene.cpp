/*
@file Scene.cpp
*/
#include "Scene.h"
#include <iostream>

/*
�R���X�g���N�^

@param name �V�[����
*/
Scene::Scene(const char* name) : name(name)
{
	std::cout << "Scene �R���X�g���N�^: " << name << "\n";
}

/*
�f�X�g���N�^
*/
Scene::~Scene()
{
	Finalize();
	std::cout << "Scene �f�X�g���N�^: " << name << "\n";
}

/*
�V�[����������Ԃɂ���
*/
void Scene::Play()
{
	isActive = true;
	std::cout << "Scene Play: " << name << "\n";
}

/*
�V�[�����~��Ԃɂ���
*/
void Scene::Stop()
{
	isActive = false;
	std::cout << "Scene Stop: " << name << "\n";
}

/*
�V�[����\������
*/
void Scene::Show()
{
	isVisible = true;
	std::cout << "Scene Show: " << name << "\n";
}

/*
�V�[�����\������
*/
void Scene::Hide()
{
	isActive = false;
	std::cout << "Scene Hide: " << name << "\n";
}

/*
�V�[�������擾����

@param �V�[����
*/
const std::string& Scene::Name() const
{
	return name;
}

/*
�V�[���̊�����Ԃ𒲂ׂ�

@retval true  �������Ă���
@retval false ��~���Ă���
*/
bool Scene::IsActive() const
{
	return isActive;
}

/*
�V�[����\����Ԃ𒲂ׂ�

@retval true  �\�����
@retval false ��\�����
*/
bool Scene::IsVisible() const
{
	return isVisible;
}

/*
�V�[���X�^�b�N���擾����

@retrun �V�[���X�^�b�N
*/
SceneStack& SceneStack::Instance()
{
	static SceneStack instance;
	return instance;
}

/*
�R���X�g���N�^
*/
SceneStack::SceneStack()
{
	stack.reserve(16); //�X�^�b�N�̗\��T�C�Y�i�V�[���̐��j��16�ɐݒ�d2
}

/*
�V�[�����v�b�V������

@param p �V�����V�[��
*/
void SceneStack::Push(ScenePtr p)
{
	//�V�[�����ς܂�Ă��邩���ׂ�
	if (!stack.empty())
	{
		//���݂̃V�[�����~
		Current().Stop();
	}
	//�V�[�����v�b�V��
	stack.push_back(p);
	std::cout << "[�V�[�� �v�b�V��] " << p->Name() << "\n";
	//�V�����V�[�����N��
	Current().Initialize();
	Current().Play();
}

/*
�V�[�����|�b�v����
*/
void SceneStack::Pop()
{
	//�X�^�b�N��Ԃ��m�F
	if (stack.empty())
	{
		//�l�߂�ĂȂ�������x���\��
		std::cout << "[�V�[�� �|�b�v] [�x��] �V�[���X�^�b�N����ł�." << "\n";
		return;
	}
	//���ݏ�ɐς܂�Ă���V�[�����~
	Current().Stop();
	Current().Finalize();
	const std::string sceneName = Current().Name();
	stack.pop_back();
	std::cout << "[�V�[�� �|�b�v] " << sceneName << "\n";
	//�|�b�v���āA�܂��V�[�����ς܂�Ă�������s
	if (!stack.empty())
	{
		Current().Play();
	}
}

/*
�V�[����u��������

@param p �V�����V�[��
*/
void SceneStack::Replace(ScenePtr p)
{
	std::string sceneName = "(Empty)";
	if (stack.empty())
	{
		std::cout << "[�V�[�� ���v���[�X] [�x��] �V�[���X�^�b�N����ł�." << "\n";
	}
	else
	{
		sceneName = Current().Name();
		Current().Stop();
		Current().Finalize();
		stack.pop_back();
	}
	stack.push_back(p);
	std::cout << "[�V�[�� ���v���[�X] " << sceneName << " -> " << p->Name() << "\n";
	Current().Initialize();
	Current().Play();
}

/*
���݂̃V�[�����擾����(�����o�֐�)

@retrun ���݂̃V�[��
*/
Scene& SceneStack::Current()
{
	//const�̂Ȃ�(std::vector��)back�֐����Ăяo��
	return *stack.back();
}

/*
���݂̃V�[�����擾����(�R���X�g�����o�֐�)

@retrun ���݂̃V�[��
*/
const Scene& SceneStack::Current() const
{
	//const�̂���(std::vector��)back�֐����Ăяo��
	return *stack.back();
}

/*
�V�[���̐����擾����

@retrun �X�^�b�N�ɐς܂�Ă���V�[���̐�
*/
size_t SceneStack::Size() const
{
	return stack.size();
}

/*
�X�^�b�N���󂩂ǂ������ׂ�

@retval true  �X�^�b�N�͋�
@retval false �X�^�b�N��1�ȏ�̃V�[�����ς܂�Ă���
*/
bool SceneStack::Empty() const
{
	return stack.empty();
}

/*
�V�[�����X�V����

@param deltaTime �O��̍X�V����̌o�ߎ���(�b)
*/
void SceneStack::Update(float deltaTime)
{
	if (!Empty())
	{
		Current().ProcessInput();
	}
	for (ScenePtr& e : stack)
	{
		if (e->IsActive())
		{
			e->Update(deltaTime);
		}
	}
}

/*
�V�[����`�悷��
*/
void SceneStack::Render()
{
	for (ScenePtr& e : stack)
	{
		if (e->IsVisible())
		{
			e->Render();
		}
	}
}