/*
@file Sprite.h
*/
#ifndef SPRITE_H_INCLUDED
#define SPRITE_H_INCLUDED
#include "BufferObject.h"
#include "Texture.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <vector>

/*
�Z�`�\����
*/
struct Rect
{
	glm::vec2 origin; // < �������_
	glm::vec2 size;   // < �c���̕�
};

/*
�X�v���C�g�N���X
*/
class Sprite
{
public:
	Sprite() = default;
	explicit Sprite(const Texture::Image2DPtr&);
	~Sprite() = default;
	Sprite(const Sprite&) = default;
	Sprite& operator = (const Sprite&) = default;

	//���W�̐ݒ�E�擾
	void SetPosition(const glm::vec3& p) { position = p; }
	const glm::vec3& GetPosition() const { return position; }

	//��]�̐ݒ�E�擾
	void SetRotation(float r) { rotation = r; }
	float GetRotation() const { return rotation; }

	//�g�嗦�̐ݒ�E�擾
	void SetScale(const glm::vec2& s) { scale = s; }
	const glm::vec2& GetScale() const { return scale; }

	//�F�̐ݒ�E�擾
	void SetColor(const glm::vec4& c) { color = c; }
	const glm::vec4& GetColor() const { return color; }

	//�Z�`�̐ݒ�E�擾
	void SetRectangle(const Rect& r) { rect = r; }
	const Rect& GetRectangle() const { return rect; }

	//�e�N�X�`���̐ݒ�E�擾
	void SetTexture(const Texture::Image2DPtr& tex);
	const Texture::Image2DPtr& GetTexture() const { return texture; }

private:
	glm::vec3 position = glm::vec3(0);
	glm::f32 rotation = 0;
	glm::vec2 scale = glm::vec2(1);
	glm::vec4 color = glm::vec4(1);
	Rect rect = { glm::vec2(0, 0), glm::vec2(1, 1) };
	Texture::Image2DPtr texture;
};

/*
�X�v���C�g�`��N���X
*/
class SpriteRenderer
{
public:
	SpriteRenderer() = default;
	~SpriteRenderer() = default;
	SpriteRenderer(const SpriteRenderer&) = delete;
	SpriteRenderer& operator = (const SpriteRenderer&) = delete;

	bool Init(size_t maxSpriteCount, const char* vsPath, const char* fsPath);
	void BeginUpdate();
	bool AddVertices(const Sprite&);
	void EndUpdate();
	void Draw(const glm::vec2&) const;
	void Clear();

private:
	BufferObject vbo;
	BufferObject ibo;
	VertexArrayObject vao;
	Shader::ProgramPtr program;

	struct Vertex
	{
		glm::vec3 position; //���W
		glm::vec4 color;    //�F
		glm::vec2 texCoord; //�e�N�X�`��
	};
	std::vector<Vertex> vertices; //���_�f�[�^�̔z��

	struct Primitive
	{
		size_t count;
		size_t offset;
		Texture::Image2DPtr texture;
	};
	std::vector<Primitive> primitives;
};

#endif // SPRITE_H_INCULDED

