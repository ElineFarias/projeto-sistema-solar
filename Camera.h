#ifndef  CAMERA_H
#define CAMERA_H

// Std. Includes
#include <vector>

// GL Includes
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 


// Define várias opções possíveis para o movimento da câmera. Usado como abstração para ficar longe dos métodos de entrada específicos do sistema de janelas
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	SCROLL_FORWARD,
	SCROLL_BACKWARD
};

// Valores padrão da câmera
const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 400.0f;
const GLfloat SENSITIVTY = 0.1f;
const GLfloat ZOOM = 45.0f;


// Uma classe de câmera abstrata que processa a entrada e calcula os Ângulos Euler, Vetores e Matrizes correspondentes para uso em OpenGL
class Camera
{
public:
// Atributos da Câmera
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	glm::vec3 LookAtPos;
// Ângulos de Euler
	GLfloat Yaw;
	GLfloat Pitch;
// Opções da câmera
	GLfloat MovementSpeed;
	GLfloat MouseSensitivity;
	GLfloat Zoom;
	bool FreeCam;

// Construtor com vetores
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
	{
		this->Position = position;
		this->WorldUp = up;
		this->Yaw = yaw;
		this->Pitch = pitch;
		this->updateCameraVectors();
	}
	// Construtor com valores escalares
	Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
	{
		this->Position = glm::vec3(posX, posY, posZ);
		this->WorldUp = glm::vec3(upX, upY, upZ);
		this->Yaw = yaw;
		this->Pitch = pitch;
		this->updateCameraVectors();
	}

// Retorna a matriz de visualização calculada usando Ângulos de Euler  e a Matriz LookAt
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
	}

	// Processa a entrada recebida de qualquer sistema de entrada tipo teclado. Aceita parâmetro de entrada na forma de ENUM definido pela câmera (para abstraí-lo de sistemas de janelas)
	void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
	{
		GLfloat velocity = this->MovementSpeed * deltaTime;

		if (FreeCam)
		{
			if (direction == FORWARD)
				this->Position += this->Front * velocity;
			if (direction == BACKWARD)
				this->Position -= this->Front * velocity;
			if (direction == LEFT)
				this->Position -= this->Right * velocity;
			if (direction == RIGHT)
				this->Position += this->Right * velocity;

			/*if (direction == SCROLL_FORWARD)
				if (this->Position.y > 70.0f)
					this->Position += this->Front * (10000.0f * deltaTime);
			if (direction == SCROLL_BACKWARD)
				this->Position -= this->Front * (15000.0f * deltaTime); */
		}
		else
		{
			if (direction == FORWARD)
				this->Position += glm::vec3(0.0f, 0.0f, 1.0f) * velocity;
			if (direction == BACKWARD)
				this->Position -= glm::vec3(0.0f, 0.0f, 1.0f) * velocity;
			if (direction == LEFT)
				this->Position -= this->Right * velocity;
			if (direction == RIGHT)
				this->Position += this->Right * velocity;
			if (direction == SCROLL_FORWARD)
				if (this->Position.y > 70.0f)
					this->Position += this->Front * (10000.0f * deltaTime);
			if (direction == SCROLL_BACKWARD)
				this->Position -= this->Front * (15000.0f * deltaTime);
		}
	}

// Processa a entrada recebida de um sistema de entrada de mouse. Espera o valor de deslocamento na direção x e y.
	void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= this->MouseSensitivity;
		yoffset *= this->MouseSensitivity;

		if (FreeCam)
		{
			this->Yaw += xoffset;
			this->Pitch += yoffset;

			// Certifique-se de que quando o tom estiver fora dos limites, a tela não seja invertida
			if (constrainPitch)
			{
				if (this->Pitch > 89.0f)
					this->Pitch = 89.0f;
				if (this->Pitch < -89.0f)
					this->Pitch = -89.0f;
			} 
		}
	// Atualiza os vetores Front, Right e Up usando os ângulos Euler atualizados
		this->updateCameraVectors();
	}

// Processa a entrada recebida de um evento de roda de rolagem do mouse. Requer apenas entrada no eixo da roda vertical.
	void ProcessMouseScroll(GLfloat yoffset)
	{
		if (this->Zoom >= 1.0f && this->Zoom <= 45.0f)
			this->Zoom -= yoffset;
		if (this->Zoom <= 1.0f)
			this->Zoom = 1.0f;
		if (this->Zoom >= 45.0f)
			this->Zoom = 45.0f;
	}

private:
	// Calcula o vetor frontal dos Ângulos Euler da Câmera (atualizados)
	void updateCameraVectors()
	{
		// Calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
		front.y = sin(glm::radians(this->Pitch));
		front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));

		this->Front = glm::normalize(front);
// Também recalcula o vetor Right e Up
		this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp)); // Normaliza os vetores, porque seu comprimento se aproxima de 0 quanto mais você olha para cima ou para baixo, o que resulta em um movimento mais lento.
		this->Up = glm::normalize(glm::cross(this->Right, this->Front));
	}
};

#endif // ! CAMERA_H
