/*
@file GLFWEW.h
*/
#ifndef GLFWEW_H_INCLUDED
#define GLFWEW_H_INCLUDED
#include <GL\glew.h>
#include "GamePad.h"
#include <GLFW\glfw3.h>
#include <glm\vec3.hpp>

namespace GLFWEW
{
	/*
	GLFW��GLEW�̃��b�p�[�N���X
	*/
	class Window
	{
	private:
		Window();
		~Window();
		/*
		GLFW��GLEW���Ǘ�����I�u�W�F�N�g�𕡐�����悤�ɂȂ��
		������������I���������I�u�W�F�N�g�̐��������s����Ă��܂��댯�������邽��
		Window�N���X�̗��p�҂��쐬�E�R�s�[���邱�Ƃ��֎~���Ă���
		*/
		Window(const Window&) = delete;  //�R�s�[�R���X�g���N�^
		Window& operator = (const Window&) = delete;  //�R�s�[������Z�q
		void UpdateGamePad();

		bool isGLFWInitialized = false;
		bool isInitialized = false;
		GLFWwindow* window = nullptr;
		double previousTime = 0;
		double deltaTime = 0;
		int width = 0;
		int height = 0;
		GamePad gamepad;

	public:
		/*
		Window�N���X�𗘗p�ł��Ȃ����߁Ainstance�֐���static�֐��Ƃ���
		��`���A���̊֐��̒��ŃI�u�W�F�N�g���쐬���A�Ǘ�����悤�ɂ���
		*/
		static Window& Instance();
		bool Init(int w, int h, const char* title);
		bool ShouldClose() const;
		void SwapBuffers() const;

		void InitTimer();
		void UpdateTimer();
		double DeltaTime() const;
		double Time() const;

		//��ʃT�C�Y�̏����擾���Ă���
		int Width() const { return width; }
		int Height() const { return height; }

		bool IsKeyPressed(int key) const;
		bool IsMousePressed(int mouse) const;
		void CursorPos(double mousex, double mousey) const;
		const GamePad& GetGamePad() const;
		glm::vec3 CursorPos() const;
		void CenterCursor() const;

		float windowWidth = 1920;
		float windowHeight = 1080;
	};
} // namespace GLFEEW

#endif // GLFWEW_H_INCLUDED
