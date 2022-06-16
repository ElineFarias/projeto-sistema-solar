#ifndef SPHERE_H
#define SPHERE_H

#include <glad/glad.h>

#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#define _USE_MATH_DEFINES
#include <math.h>

class Sphere
{
private:
	std::vector<float> sphere_vertices;
	std::vector<float> sphere_texcoord;
	std::vector<int> sphere_indices;
	GLuint VBO, VAO, EBO;
	float radius = 1.0f;
	int sectorCount = 36;
	int stackCount = 18;

public:

	~Sphere()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}
	Sphere(float r, int sectors, int stacks)
	{
		radius = r;
		sectorCount = sectors;
		stackCount = stacks;


/* GERAR MATRIZ DE VÉRTICES */
		float x, y, z, xy;                              // vertices posição
		float lengthInv = 1.0f / radius;    // vertices normal
		float s, t;                                     // vertices  tex coords

		float sectorStep = (float)(2 * M_PI / sectorCount);
		float stackStep = (float)(M_PI / stackCount);
		float sectorAngle, stackAngle;

		for (int i = 0; i <= stackCount; ++i)
		{
			stackAngle = (float)(M_PI / 2 - i * stackStep);        // // Começando de pi/2 to -pi/2
			xy = 1.02f * radius * cosf(stackAngle);             // r * cos(u)
			z = radius * sinf(stackAngle);              // r * sin(u)

		// adiciona (sectorCount+1) vértices por pilha
	// o primeiro e o último vértice têm a mesma posição (normal), mas diferentes tex coords
			for (int j = 0; j <= sectorCount; ++j)
			{
				sectorAngle = j * sectorStep;           // Começando de 0 to 2pi

				// vértices posição (x, y, z)
				x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
				y = xy * sinf(sectorAngle);				// r * cos(u) * sin(v)
				sphere_vertices.push_back(x);
				sphere_vertices.push_back(y);
				sphere_vertices.push_back(z);


				// vértices tex coord (s, t) intervalo entre [0, 1]
				s = (float)j / sectorCount;
				t = (float)i / stackCount;
				sphere_vertices.push_back(s);
				sphere_vertices.push_back(t);

			}
		}
/* GERAR MATRIZ DE VERTICES */
		

		/* GERAR INDIVES DE ARRAY */
		int k1, k2;
		for (int i = 0; i < stackCount; ++i)
		{
			k1 = i * (sectorCount + 1);     // início da pilha atual
			k2 = k1 + sectorCount + 1;     // início da próxima pilha

			for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
			{
			// 2 triângulos por setor excluindo a primeira e a última pilha
				// k1 => k2 => k1+1
				if (i != 0)
				{
					sphere_indices.push_back(k1);
					sphere_indices.push_back(k2);
					sphere_indices.push_back(k1 + 1);
				}

				// k1+1 => k2 => k2+1
				if (i != (stackCount - 1))
				{
					sphere_indices.push_back(k1 + 1);
					sphere_indices.push_back(k2);
					sphere_indices.push_back(k2 + 1);
				}
			}
		}
		/* GERAR INDICES DE ARRAY */
		

		/* GERAR VAO-EBO */
		//GLuint VBO, VAO, EBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		// Vincula o objeto de matriz de vértices primeiro, depois vincula e define o(s) buffer(s) de vértice e ponteiro(s) de atributo.
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, (unsigned int)sphere_vertices.size() * sizeof(float), sphere_vertices.data(), GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (unsigned int)sphere_indices.size() * sizeof(unsigned int), sphere_indices.data(), GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		/* GERAR VAO-EBO */


	}
	void Draw()
	{
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES,   
			(unsigned int)sphere_indices.size(),
			GL_UNSIGNED_INT,					
			(void*)0);
		glBindVertexArray(0);
	}
};


#endif
