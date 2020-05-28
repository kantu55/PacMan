/*
@file Font.h
*/
#ifndef FONT_H_INCLUDED
#define FONT_H_INCLUDED
#include <GL/glew.h>
#include "Sprite.h"
#include <glm/glm.hpp>
#include<vector>
#include<string>


/*
�r�b�g�}�b�v�t�H���g�`��N���X
*/
class FontRenderer
{
public:
	FontRenderer() = default;
	~FontRenderer() = default;

	/*
	delete���w�肷�邱�ƂŁA���̌^�̕ϐ��̃R�s�[���֎~����
	*/
	FontRenderer(const FontRenderer&) = delete;
	FontRenderer& operator=(const FontRenderer&) = delete;

	bool Init(size_t maxChar);                //�t�H���g�`��N���X��������
	bool LoadFromFile(const char* path);      //�t�H���g���t�@�C���ƃe�N�X�`����ǂݍ���
	void BeginUpdate();                       //�����̒ǉ����J�n
	bool AddString(const glm::vec2&, const wchar_t*);  //�K�v�Ȃ������s���ĕ�����ǉ�
	void EndUpdate();                         //�ǉ����I��
	void Draw(const glm::vec2&) const;        //�t�H���g��`��
	void Color(const glm::vec4&) const;       //�F��ύX����
	float LineHeight() const;                 //�t�H���g�̍�����Ԃ�

private:
	SpriteRenderer spriteRenderer;  //�`��Ɏg�p����X�v���C�g�����_���[
	std::vector<Texture::Image2DPtr> textures;  //�t�H���g�̃e�N�X�`�����X�g
	float lineHeight = 0;  //�s�̍���
	float base = 0;  //�s�̏㕔����x�[�X���C���܂ł̋���

	//�������
	struct CharacterInfo
	{
		int id = -1;  //�����R�[�h
		int page = 0; //�������܂܂��t�H���g�e�N�X�`���̔ԍ�
		glm::vec2 uv = glm::vec2(0);   //�����̃e�N�X�`�����W
		glm::vec2 size = glm::vec2(0); //�����̕\���T�C�Y
		glm::vec2 offset = glm::vec2(0); //�\���ʒu�̕␳�l
		float xadvance = 0; //���̕�����\������ʒu
	};
	std::vector<CharacterInfo> characterInfoList; //������񃊃X�g
};

#endif // FONT_H_INCLUDED