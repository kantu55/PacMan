/*
@file Light.cpp
*/
#include "Light.h"
#include <iostream>

namespace /* unnamed */
{
	const char UniformBlockName[] = "LightUniformBlock";
} // unnamed namespace

/*
���C�g�p���j�t�H�[���o�b�t�@���쐬����

@param bindingPoint ���j�t�H�[���o�b�t�@�Ɋ��蓖�Ă�Ǘ��ԍ�
*/
bool LightBuffer::Init(GLuint bindingPoint)
{
	/*
	�_�u���o�b�t�@
	GPU��CPU�����Ă���UBO�𕪗�������
	*/
	ubo[0] = UniformBuffer::Create(sizeof(data), bindingPoint, UniformBlockName);
	ubo[1] = UniformBuffer::Create(sizeof(data), bindingPoint, UniformBlockName);
	currentUboIndex = 0;

	return ubo[0] && ubo[1];
}

/*
�V�F�[�_�[�v���O�����ɃX�P���^�����b�V���̐���p�f�[�^�̃o�C���f�B���O�E�|�C���g��ݒ肷��

@param program �o�C���f�B���O�E�|�C���g�ݒ��̃V�F�[�_�[�v���O����

@return true  �ݒ萬��
@return false �ݒ莸�s
*/
bool LightBuffer::BindToShader(const Shader::ProgramPtr& program)
{
	const GLuint id = program->Get();
	// UniformBlock�̈ʒu���擾����
	const GLuint blockIndex = glGetUniformBlockIndex(id, UniformBlockName);
	if (blockIndex == GL_INVALID_INDEX)
	{
		std::cerr << "[�G���[] Uniform�u���b�N'" << UniformBlockName << "'��������܂���\n";
		return false;
	}
	// �o�C���f�B���O�E�|�C���g�ɃC���^�t�F�[�X�u���b�N�����т���
	glUniformBlockBinding(id, blockIndex, ubo[0]->BindingPoint());
	const GLenum result = glGetError();
	if (result != GL_NO_ERROR)
	{
		std::cerr << "[�G���[] Uniform�u���b�N'" << UniformBlockName << "'�̃o�C���h�Ɏ��s\n";
		return false;
	}
	return true;
}

/*
GPU�֓]�����郉�C�g�f�[�^���X�V����

@param al           // ���C�g�̃A�N�^�[���X�g
@param ambientColor // �����̖��邳
*/
void LightBuffer::Update(const ActorList& al, const glm::vec3& ambientLight)
{
	data.ambientLight.color = glm::vec4(ambientLight, 0);

	int pointLightCount = 0;
	int spotLightCount = 0;
	for (auto i = al.begin(); i != al.end(); ++i)
	{
		/*
		[*i]��LightActor���w���Ă����琬���AStaticActor���w���Ă����玸�s
		*/
		if (DirectionalLightActorPtr p =
			std::dynamic_pointer_cast<DirectionalLightActor>(*i))
		{
			data.directionalLight.color = glm::vec4(p->color, 0);
			data.directionalLight.direction = glm::vec4(p->direction, 0);
		}
		else if (PointLightActorPtr p = std::dynamic_pointer_cast<PointLightActor>(*i))
		{
			if (pointLightCount < 100)
			{
				p->index = pointLightCount;
				PointLight& light = data.pointLight[pointLightCount];
				light.color = glm::vec4(p->color, 1);
				light.position = glm::vec4(p->position, 1);
				++pointLightCount;
			}
		}
		else if (SpotLightActorPtr p = std::dynamic_pointer_cast<SpotLightActor>(*i))
		{
			if (spotLightCount < 100)
			{
				p->index = spotLightCount;
				SpotLight& light = data.spotLight[spotLightCount];
				light.color = glm::vec4(p->color, 1);
				light.dirAndCutOff = glm::vec4(p->direction, p->cutOff);
				light.posAndInnerCutOff = glm::vec4(p->position, p->innerCutOff);
				++spotLightCount;
			}
		}
	}
}

/*
���C�g�f�[�^��GPU�������ɓ]������
*/
void LightBuffer::Upload()
{
	UniformBufferPtr pUbo = ubo[currentUboIndex];
	pUbo->BufferSubData(&data, 0, sizeof(data));
	// �u�I���Z�q�v��currentUboIndex�̒l�𔽓]������
	currentUboIndex = !currentUboIndex;
}

/*
���C�g�pUBO��GL�R���e�L�X�g��UBO�p�o�C���f�B���O�E�|�C���g�Ɋ��蓖�Ă�
*/
void LightBuffer::Bind()
{
	ubo[!currentUboIndex]->BindBufferRange(0, sizeof(data));
}