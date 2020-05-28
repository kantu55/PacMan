/*
@file Shader.cpp
*/
#include "Shader.h"
#include "Geometry.h"
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include <fstream>

/*
�V�F�[�_�[�Ɋւ���@�\���i�[���閼�O���
*/
namespace Shader
{
	/*
	�V�F�[�_�[�E�v���O�������R���p�C������

	@param type �V�F�[�_�[�̎��
	@param string �V�F�[�_�[�E�I�u�W�F�N�g�̍쐬�Ɏ��s
	*/

	GLuint Compile(GLenum type, const GLchar* string)
	{
		if (!string)
		{
			return 0;
		}
		//�V�F�[�_�[�E�I�u�W�F�N�g���쐬���Ă���
		GLuint shader = glCreateShader(type);
		//�V�F�[�_�[�E�I�u�W�F�N�g�ɃV�F�[�_�[�E�v���O�������֘A�t�����Ă���
		glShaderSource(shader, 1, &string, nullptr);
		//�V�F�[�_�[�E�v���O�������R���p�C������A�V�F�[�_�[�E�I�u�W�F�N�g�ɃZ�b�g�����
		glCompileShader(shader);
		GLint compiled = 0;
		//�V�F�[�_�[�E�I�u�W�F�N�g�̏�Ԃ��擾���Ă���
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		//�R���p�C���Ɏ��s�����ꍇ�A�������R���\�[���ɏo�͂���0��Ԃ�
		if (!compiled)
		{
			GLint infoLen = 0;
			//�ڍ׏��̃o�C�g�����擾���Ă���
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
			if (infoLen)
			{
				//�ڍ׏����l�����邽�߂�vector���g���Ă���
				std::vector<char> buf;
				//std::vector�̃T�C�Y��ύX���Ă���
				buf.resize(infoLen);
				if (static_cast<int>(buf.size()) >= infoLen)
				{
					glGetShaderInfoLog(shader, infoLen, NULL, buf.data());
					std::cerr << "ERROR: �V�F�[�_�[�̃R���p�C���Ɏ��s.\n" << buf.data() << std::endl;
				}
			}
			//�G���[���������ꍇ�̓V�F�[�_�[�E�I�u�W�F�N�g���폜
			glDeleteShader(shader);
			return 0;
		}
		return shader;
	}

	/*
	�v���O�����E�I�u�W�F�N�g���쐬����

	@param vsCode ���_�V�F�[�_�[�E�v���O�����ւ̃|�C���^
	@param fsCode �t���O�����g�V�F�[�_�[�E�v���O�����ւ̃|�C���^

	@retval 0���傫��  �쐬�����v���O�����E�I�u�W�F�N�g
	@retval 0            �v���O�����E�I�u�W�F�N�g�̍쐬�Ɏ��s
	*/

	GLuint Build(const GLchar* vsCode, const GLchar* fsCode)
	{
		//�V�F�[�_�[�E�v���O��������V�F�[�_�[�E�I�u�W�F�N�g���쐬
		GLuint vs = Compile(GL_VERTEX_SHADER, vsCode);
		GLuint fs = Compile(GL_FRAGMENT_SHADER, fsCode);
		if (!vs || !fs)
		{
			return 0;
		}
		//�V�F�[�_�[�E�v���O�������쐬���Ă���
		GLuint program = glCreateProgram();
		//�V�F�[�_�[�E�I�u�W�F�N�g���V�F�[�_�[�E�v���O�����Ɋ��蓖�ĂĂ���
		glAttachShader(program, fs);
		glDeleteShader(fs);
		glAttachShader(program, vs);
		glDeleteShader(vs);
		//���蓖�Ă��V�F�[�_�[�E�v���O�������m�������N���Ă���
		glLinkProgram(program);
		GLint linkStatus = GL_FALSE;
		//�����N���ʂ��擾���Ă���
		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
		if (linkStatus != GL_TRUE)
		{
			GLint infoLen = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
			if (infoLen)
			{
				std::vector<char> buf;
				buf.resize(infoLen);
				if (static_cast<int>(buf.size()) >= infoLen)
				{
					glGetProgramInfoLog(program, infoLen, NULL, buf.data());
					std::cerr << "ERROR: �V�F�[�_�[�̃����N�Ɏ��s.\n" << buf.data() << std::endl;
				}
			}
			glDeleteShader(program);
			return 0;
		}
		return program;
	}

	/*
	�t�@�C����ǂݍ���

	@param path �ǂݍ��ރt�@�C����

	@retrun �ǂݍ��񂾃f�[�^
	*/
	std::vector<GLchar> ReadFile(const char* path)
	{
		std::basic_ifstream<GLchar> ifs;
		ifs.open(path, std::ios_base::binary);
		if (!ifs.is_open())
		{
			std::cerr << "ERROR : " << path << " ���J���܂���.\n";
			return{};
		}
		ifs.seekg(0, std::ios_base::end);
		const size_t length = (size_t)ifs.tellg();
		ifs.seekg(0, std::ios_base::beg);
		std::vector<GLchar> buf(length);
		ifs.read(buf.data(), length);
		buf.push_back('\0'); //�Ō��'\0'�����Ȃ���΂Ȃ�Ȃ�
		return buf;
	}

	/*
	�t�@�C������v���O�����E�I�u�W�F�N�g���쐬����

	@param vsPath ���_�f�[�^�E�t�@�C����
	@param fsPath �t���O�����g�V�F�[�_�[�E�t�@�C����

	@retrun �쐬�����v���O�����E�I�u�W�F�N�g
	*/
	GLuint BuildFromFile(const char* vsPath, const char* fsPath)
	{
		const std::vector<GLchar> vsCode = ReadFile(vsPath);
		const std::vector<GLchar> fsCode = ReadFile(fsPath);
		return Build(vsCode.data(), fsCode.data());
	}

	/*
	�R���X�g���N�^
	*/
	Program::Program()
	{
		
	}

	/*
	�R���X�g���N�^

	@param programId �v���O�����E�I�u�W�F�N�g��ID
	*/
	Program::Program(GLuint programId)
	{
		Reset(programId);
	}

	/*
	�f�X�g���N�^

	�v���O�����E�I�u�W�F�N�g���폜����
	*/
	Program::~Program()
	{
		if (id)
		{
			glDeleteProgram(id);
		}
	}

	/*
	�v���O�����E�I�u�W�F�N�g��ݒ肷��

	@param id �v���O�����E�I�u�W�F�N�g��ID
	*/
	void Program::Reset(GLuint programId)
	{
		glDeleteProgram(id);
		id = programId;
		if (id == 0)
		{
			locMatMVP = -1;
			locMatModel = -1;
			locPointLightCount = -1;
			locPointLightIndex = -1;
			locSpotLightCount = -1;
			locSpotLightIndex = -1;
			locCameraPosition = -1;
			locTime = -1;
			locViewInfo = -1;
			locCameraInfo = -1;
			return;
		}

		locMatMVP = glGetUniformLocation(id, "matMVP");
		locMatModel = glGetUniformLocation(id, "matModel");
		locPointLightCount = glGetUniformLocation(id, "pointLightCount");
		locPointLightIndex = glGetUniformLocation(id, "pointLightIndex");
		locSpotLightCount = glGetUniformLocation(id, "spotLightCount");
		locSpotLightIndex = glGetUniformLocation(id, "spotLightIndex");
		locCameraPosition = glGetUniformLocation(id, "cameraPosition");
		locTime = glGetUniformLocation(id, "time");
		locViewInfo = glGetUniformLocation(id, "viewInfo");
		locCameraInfo = glGetUniformLocation(id, "cameraInfo");

		glUseProgram(id);
		const GLint texColorLoc = glGetUniformLocation(id, "texColor");
		if (texColorLoc >= 0)
		{
			glUniform1i(texColorLoc, 0);
		}
		
		for (GLint i = 0; i < 6; ++i)
		{
			std::string name("texColorArray[");
			name += static_cast<char>('0' + i);
			name += ']';
			const GLint texColorLoc = glGetUniformLocation(id, name.c_str());
			if (texColorLoc >= 0)
			{
				glUniform1i(texColorLoc, i);
			}
		}
		for (GLint i = 0; i < 8; ++i)
		{
			std::string name("texNormalArray[");
			name += static_cast<char>('0' + i);
			name += ']';
			const GLint texColorLoc = glGetUniformLocation(id, name.c_str());
			if (texColorLoc >= 0)
			{
				glUniform1i(texColorLoc, i + 8);
			}
		}

		const GLint locTexPointLightIndex = glGetUniformLocation(id, "texPointLightIndex");
		if (locTexPointLightIndex >= 0)
		{
			glUniform1i(locTexPointLightIndex, 4);
		}
		const GLint locTexSpotLightIndex = glGetUniformLocation(id, "texSpotLightIndex");
		if (locTexSpotLightIndex >= 0)
		{
			glUniform1i(locTexSpotLightIndex, 5);
		}
		const GLint locTexCubeMap = glGetUniformLocation(id, "texCubeMap");
		if (locTexCubeMap >= 0)
		{
			glUniform1i(locTexCubeMap, 6);
		}
		glUseProgram(0);


		const GLenum result = glGetError();
		if (result != GL_NO_ERROR) {
			std::cerr << "[�G���[]" << __func__ << "(" << result << ")\n";
		}
	}

	bool Program::IsNull() const
	{
		return id == 0;
	}

	/*
	�v���O�����E�I�u�W�F�N�g���O���t�B�b�N�E�p�C�v���C���Ɋ��蓖�Ă�
	*/
	void Program::Use()
	{
		if (id)
		{
			glUseProgram(id);
		}
	}

	/*
	�`��Ɏg�p����e�N�X�`����ݒ肷��

	@param unitNo �ݒ肷��e�N�X�`���E�C���[�W�E���j�b�g�̔ԍ�(0�`)
	@param texId  �ݒ肷��e�N�X�`��ID
	*/
	void Program::BindTexture(GLuint unitNo, GLuint texId)
	{
		glActiveTexture(GL_TEXTURE0 + unitNo);
		glBindTexture(GL_TEXTURE_2D, texId);
	}

	/*
	�`��Ɏg����r���[�E�v���W�F�N�V�����s���ݒ肷��

	@param matVP �ݒ肷��r���[�E�v���W�F�N�V�����s��
	*/
	void Program::SetViewProjectionMatrix(const glm::mat4& matVP)
	{
		this->matVP = matVP;
		if (locMatMVP >= 0)
		{
			glUniformMatrix4fv(locMatMVP, 1, GL_FALSE, &matVP[0][0]);
		}
	}

	/*
	�`��Ɏg���郂�f���s���ݒ肷��

	@param m �ݒ肷�郂�f���s��
	*/
	void Program::SetModelMatrix(const glm::mat4& m)
	{
		if (locMatModel >= 0)
		{
			glUniformMatrix4fv(locMatModel, 1, GL_FALSE, &m[0][0]);
		}
	}

	/*
	�`��Ɏg���郉�C�g��ݒ肷��

	@param count     // �`��Ɏg�p����|�C���g���C�g�̐�(0�`8)
	@param indexList // �`��Ɏg�p����|�C���g���C�g�̔ԍ��̔z��
	*/
	void Program::SetPointLightIndex(int count, const int* indexList)
	{
		if (locPointLightCount >= 0)
		{
			glUniform1i(locPointLightCount, count);
		}
		if (locPointLightIndex >= 0 && count > 0)
		{
			glUniform1iv(locPointLightIndex, count, indexList);
		}
	}

	/* 
	�`��Ɏg���郉�C�g��ݒ肷��
	
	@param count      �`��Ɏg�p����X�|�b�g���C�g�̐�(0�`8).
	@param indexList  �`��Ɏg�p����X�|�b�g���C�g�ԍ��̔z��.
	*/
	void Program::SetSpotLightIndex(int count, const int* indexList)
	{
		if (locSpotLightCount >= 0)
		{
			glUniform1i(locSpotLightCount, count);
		}
		if (locSpotLightCount >= 0 && count > 0)
		{
			glUniform1iv(locSpotLightIndex, count, indexList);
		}
	}

	/*
	�J�������W��ݒ肷��

	@param pos �J�������W
	*/
	void Program::SetCameraPosition(const glm::vec3& pos)
	{
		if (locCameraPosition >= 0)
		{
			glUniform3fv(locCameraPosition, 1, &pos.x);
		}
	}

	/*
	���o�ߎ��Ԃ�ݒ肷��

	@param time ���o�ߎ���
	*/
	void Program::SetTime(float time)
	{
		if (locTime >= 0)
		{
			glUniform1f(locTime, time);
		}
	}

	/*
	��ʂ̏���ݒ肷��

	@param w     //�E�B���h�E�̕�(�s�N�Z���P��)
	@param h      //�E�B���h�E�̍���(�s�N�Z���P��)
	@param near //�ŏ�Z����(m�P��)
	@param far   //�ő�Z����(m�P��)
	*/
	void Program::SetViewInfo(float w, float h, float near, float far)
	{
		if (locViewInfo >= 0)
		{
			glUniform4f(locViewInfo, 1.0f / w, 1.0 / h, near, far);
		}
	}

	/*
	�J�����̏���ݒ肷��

	@param focalPlane   �ŕ���(�s���g�̍����ʒu�̃����Y����̋���. mm�P��)
	@param focalLength �œ_����(����1�_�ɏW�܂�ʒu�̃����Y����̋���. mm�P��)
	@param aperture     �J��(���̎�����̃T�C�Y. mm�P��)
	@param sensorSize  �Z���T�[�T�C�Y(�����󂯂�Z���T�[�̉���. mm�P��)
	*/
	void Program::SetCameraInfo(float focalPlane, float focalLength, float aperture,
		float sensorSize)
	{
		if (locCameraInfo >= 0)
		{
			glUniform4f(locCameraInfo, focalPlane, focalLength, aperture, sensorSize);
		}
	}

	/*
	�v���O�����I�u�W�F�N�g���쐬����

	@param vsPath  ���_�V�F�[�_�[�t�@�C����
	@param fsPath  �t���O�����g�V�F�[�_�[�t�@�C����

	@return  �쐬�����v���O�����I�u�W�F�N�g
	*/
	ProgramPtr Program::Create(const char* vsPath, const char* fsPath)
	{
		return std::make_shared<Program>(BuildFromFile(vsPath, fsPath));
	}
}
//Shader namespace