/*
@file GLFWE..cpp
*/

#include "GLFWEW.h"
#include <iostream>

//GLFW��GLEW�����b�v���邽�߂̖��O���
namespace GLFWEW
{
	/*
	GLFW����̃G���[�񍐂���������

	@param error  �G���[�ԍ�
	@param desc   �G���[�̓��e
	*/
	void ErrorCallback(int error, const char* desc)
	{
		std::cerr << "ERROR: " << desc << std::endl;
	}

	/*
	�V���O���g���C���X�^���X���擾����

	@return window�̃V���O���g���C���X�^���X
	*/

	Window& Window::Instance()
	{
		static Window instance;
		return instance;
	}

	/*
	�R���X�g���N�^
	*/
	Window::Window()
	{
	}

	/*
	�f�X�g���N�^
	*/
	Window::~Window()
	{
		if (isGLFWInitialized)
		{
			glfwTerminate();
		}
	}

	/*
	GLFW/GLEW�̏�����

	@param w �E�B���h�E�̕`��͈͂̕�(�s�N�Z��)
	@param h �E�B���h�E�̕`��͈͂̍���(�s�N�Z��)
	@param title �E�B���h�E�^�C�g��(UTF-8��0�I�[������)
	
	@retval true   ����������
	@retval false  ���������s
	*/

	bool Window::Init(int w, int h, const char* title)
	{
		if (isInitialized)
		{
			std::cerr << "ERROR: GLFWEW�͂��łɏ���������Ă��܂�." << std::endl;
			return false;
		}

		if (!isGLFWInitialized)
		{
			glfwSetErrorCallback(ErrorCallback);
			//OpenGL�y��GLFW�̏��������s��
			if (glfwInit() != GL_TRUE)
			{
				return false;
			}
			isGLFWInitialized = true;
		}

		if (!window)
		{
			//�O���t�B�b�N�`��p�̃E�B���h�E���쐬����
			window = glfwCreateWindow(w, h, title, nullptr, nullptr);
			if (!window)
			{
				return false;
			}
			//�w�肵���E�B���h�E�ɑΉ�����OpenGL�R���e�L�X�g��`��Ώۂɐݒ肷��
			glfwMakeContextCurrent(window);
		}

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		if (glewInit() != GLEW_OK)
		{
			std::cerr << "ERROR: GLEW�̏������Ɏ��s���܂���." << std::endl;
			return false;
		}

		//��ʂ̕��ƍ�����ۑ�
		width = w;
		height = h;

		//OpenGL�̏����R���\�[���E�B���h�E�֏o�͂���
		/*
		glGetString -> OpenGL�R���e�L�X�g�Ɋ֘A�t����Ă��������̏����擾����֐�
		GL_RENDERER�E�E�EOpenGL��`�悷��O���t�B�b�N�X�n�[�h�E�F�A���A�܂��̓h���C�o��
		*/
		const GLubyte* renderer = glGetString(GL_RENDERER);
		std::cout << "Renderer: " << renderer << std::endl;
		//GL_VERSION�E�E�E�O���t�B�b�N�X�n�[�h�E�F�A�A�܂��̓h���C�o���Ή�����ő��OpenGL
		const GLubyte* version = glGetString(GL_VERSION);
		std::cout << "Version: " << version << std::endl;

		isInitialized = true;
		return true;
	}

	/*
	�E�B���h�E�����ׂ������ׂ�

	@retval true   ����
	@retval false  ���Ȃ�
	*/
	bool Window::ShouldClose() const
	{
		return glfwWindowShouldClose(window) != 0;
	}

	/*
	�t�����g�o�b�t�@�ƃo�b�N�o�b�t�@��؂�ւ���
	�t�����g�o�b�t�@�͕\�����̃t���[���o�b�t�@
	�o�b�N�o�b�t�@�͕`�摤�̃t���[���o�b�t�@
	*/
	void Window::SwapBuffers() const
	{
		//GLFW��OS���瑗��ꂽ�C�x���g����������֐�
		glfwPollEvents();
		/*
		�t���[���o�b�t�@�̕`�摤(�o�b�N)�ƕ\����(�t�����g)�����ւ���
		�t���[���o�b�t�@�͊G��`�悷�邽�߂̃�����
		*/
		glfwSwapBuffers(window);
	}

	/*
	�L�[��������Ă��邩���ׂ�

	@param key ���ׂ����L�[�E�R�[�h(GLFW_KEY_A�Ȃ�)

	@retval true  �L�[��������Ă���
	@retval false �L�[��������Ă��Ȃ�

	�g�p�ł���L�[�E�R�[�h�ꗗ��glfw3.h�ɂ���܂�(�ukeyboard�v�Ō���)
	*/

	bool Window::IsKeyPressed(int key) const
	{
		return glfwGetKey(window, key) == GLFW_PRESS;
	}

	bool Window::IsMousePressed(int mouse) const
	{
		return glfwGetMouseButton(window, mouse) == GLFW_PRESS;
	}


	void Window::CursorPos(double mousex, double mousey) const
	{
		return glfwGetCursorPos(window, &mousex, &mousey);
	}

	/*
	�^�C�}�[������������
	*/
	void Window::InitTimer()
	{
		glfwSetTime(0.0);
		previousTime = 0.0;
		deltaTime = 0.0;
	}

	/*
	�^�C�}�[���X�V����
	*/
	void Window::UpdateTimer()
	{
		//�o�ߎ��Ԃ��v��
		const double currentTime = glfwGetTime();
		deltaTime = currentTime - previousTime;
		previousTime = currentTime;

		//�o�ߎ��Ԃ������Ȃ肷���Ȃ��悤�ɒ���
		const float upperLimit = 0.25f; //�o�ߎ��ԂƂ��ċ��e�������
		if (deltaTime > upperLimit)
		{
			deltaTime = 0.1f;
		}
		UpdateGamePad();
	}

	/*
	�o�ߎ��Ԃ��擾����

	@retrun ���O��2���UpdateTimer()�Ăяo���̊ԂɌo�߂�������
	*/
	double Window::DeltaTime() const
	{
		return deltaTime;
	}

	/*
	���o�ߎ��Ԃ��擾����

	@return GLFW������������Ă���̌o�ߎ���(�b)
	*/
	double Window::Time() const
	{
		return glfwGetTime();
	}

	/*
	�Q�[���p�b�h�̏�Ԃ��擾����

	@retrun �Q�[���p�b�h�̏��
	*/
	const GamePad& Window::GetGamePad() const
	{
		return gamepad;
	}

	/*
	�Q�[���p�b�h�̃A�i���O���͑��uID

	@note ������XBOX360�Q�[���p�b�h�
	*/
	enum GAMEPAD_AXES
	{
		GAMEPAD_AXES_LEFT_X,   // ���X�e�B�b�N��X��
		GAMEPAD_AXES_LEFT_Y,   // ���X�e�B�b�N��Y��
		GAMEPAD_AXES_TRIGGER,  // �A�i���O�g���K�[
		GAMEPAD_AXES_RIGHT_X,  // �E�X�e�B�b�N��X��
		GAMEPAD_AXES_RIGHT_Y,  // �E�X�e�B�b�N��Y��
	};

	/*
	�Q�[���p�b�h�̃f�W�^�����͑��u

	@note XBOX360�Q�[���p�b�h����
	*/
	enum GAMEPAD_BUTTON
	{
		GAMEPAD_BUTTON_A,  // A�{�^��
		GAMEPAD_BUTTON_B,  // B�{�^��
		GAMEPAD_BUTTON_X,  // X�{�^��
		GAMEPAD_BUTTON_Y,  // Y�{�^��
		GAMEPAD_BUTTON_L,  // L�{�^��
		GAMEPAD_BUTTON_R,  // R�{�^��
		GAMEPAD_BUTTON_BACK,    // Back�{�^��
		GAMEPAD_BUTTON_START,   // Start�{�^��
		GAMEPAD_BUTTON_L_THUMB, // ���X�e�B�b�N�{�^��
		GAMEPAD_BUTTON_R_THUMB, // �E�X�e�B�b�N�{�^��
		GAMEPAD_BUTTON_UP,      // ��L�[
		GAMEPAD_BUTTON_RIGHT,    // �E�L�[
		GAMEPAD_BUTTON_DOWN,   // ���L�[
		GAMEPAD_BUTTON_LEFT,    // ���L�[
	};

	/*
	�Q�[���p�b�h�̏�Ԃ��X�V����
	*/
	void Window::UpdateGamePad()
	{
		const uint32_t prevButtos = gamepad.buttons; //buttonDown�𐶐����邽�߂ɁA�X�V�O�̓���

		//�A�i���O���͂ƃ{�^�����͂��擾
		int axesCount, buttonCount;
		const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
		const uint8_t* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);

		/*
		�����̔z��nullptr�ł͂Ȃ��A�Œ���K�v�ȃf�[�^���𖞂����Ă���΁A
		�L���ȃQ�[���p�b�h���ڑ������
		*/
		if (axes && buttons && axesCount >= 2 && buttonCount >= 8)
		{
			//�L���ȃQ�[���p�b�h���ڑ�����Ă���ꍇ

			

			//�z��C���f�b�N�X��GamePad�L�[�̑Ή��\
			static const struct
			{
				int dataIndex;
				uint32_t gamepadBit;
			}
			keyMap[] = 
			{
				{ GAMEPAD_BUTTON_A, GamePad::A },
				{ GAMEPAD_BUTTON_B, GamePad::B },
				{ GAMEPAD_BUTTON_X, GamePad::X },
				{ GAMEPAD_BUTTON_Y, GamePad::Y },
				{ GAMEPAD_BUTTON_L, GamePad::L },
				{ GAMEPAD_BUTTON_R, GamePad::R },
				{ GAMEPAD_BUTTON_START, GamePad::START },
				{ GAMEPAD_BUTTON_UP, GamePad::DPAD_UP },
				{ GAMEPAD_BUTTON_DOWN, GamePad::DPAD_DOWN },
				{ GAMEPAD_BUTTON_LEFT, GamePad::DPAD_LEFT },
				{ GAMEPAD_BUTTON_RIGHT, GamePad::DPAD_RIGHT },
			};

			for (const auto& e : keyMap)
			{
				if (buttons[e.dataIndex] == GLFW_PRESS)
				{
					gamepad.buttons |= e.gamepadBit;
				}
				else if (buttons[e.dataIndex] == GLFW_RELEASE)
				{
					gamepad.buttons &= ~e.gamepadBit;
				}
			}

			//�����L�[�̓��͏�Ԃ��������āA���X�e�B�b�N�̓��͂Œu��������
			gamepad.buttons &=
				~(GamePad::DPAD_UP | GamePad::DPAD_DOWN | GamePad::DPAD_LEFT | GamePad::DPAD_RIGHT);
			static const float digitalThreshod = 0.3f; // �f�W�^�����͂Ƃ݂Ȃ��������l
			if (axes[GAMEPAD_AXES_LEFT_Y] >= digitalThreshod)
			{
				gamepad.buttons |= GamePad::DPAD_DOWN;
			}
			else if (axes[GAMEPAD_AXES_LEFT_Y] <= -digitalThreshod)
			{
				gamepad.buttons |= GamePad::DPAD_UP;
			}
			if (axes[GAMEPAD_AXES_LEFT_X] >= digitalThreshod)
			{
				gamepad.buttons |= GamePad::DPAD_RIGHT;
			}
			else if (axes[GAMEPAD_AXES_LEFT_X] <= -digitalThreshod)
			{
				gamepad.buttons |= GamePad::DPAD_LEFT;
			}
		}
		else
		{
			//�z��C���f�b�N�X��GamePad�L�[�̑Ή��\
			static const struct
			{
				int keyCode;
				uint32_t gamepadBit;
			}
			keyMap[] =
			{
				{ GLFW_KEY_ENTER, GamePad::A },
				{ GLFW_KEY_K, GamePad::B },
				{ GLFW_KEY_U, GamePad::X },
				{ GLFW_KEY_I, GamePad::Y },
				{ GLFW_KEY_O, GamePad::L },
				{ GLFW_KEY_L, GamePad::R },
				{ GLFW_KEY_ENTER, GamePad::START },
				{ GLFW_KEY_W, GamePad::DPAD_UP },
				{ GLFW_KEY_S, GamePad::DPAD_DOWN },
				{ GLFW_KEY_A, GamePad::DPAD_LEFT },
				{ GLFW_KEY_D, GamePad::DPAD_RIGHT },
			};

			for (const auto& e : keyMap)
			{
				const int key = glfwGetKey(window, e.keyCode);
				if (key == GLFW_PRESS)
				{
					gamepad.buttons |= e.gamepadBit;
				}
				else if (key == GLFW_RELEASE)
				{
					gamepad.buttons &= ~e.gamepadBit;
				}
			}
		}

		//�O��̍X�V�ŉ�����Ă��Ȃ��āA���񉟂���Ă���L�[�̏���buttonDown�Ɋi�[
		gamepad.buttonDown = gamepad.buttons & ~prevButtos;
	}

	void Window::CenterCursor() const
	{
		glfwSetCursorPos(window, width / 2, height / 2);
	}

	glm::vec3 Window::CursorPos() const
	{
		glm::vec3 mousePos = glm::vec3(0);
		double x, z;
		glfwGetCursorPos(window, &x, &z);
		{
			mousePos.x = x * 0.05f;
			mousePos.z = z * 0.25f;
		}
		mousePos.y = 0;
		return mousePos;
	}
}//namespace GLFWEW