## Projeto desenvolvido para a disciplina de Computação Gráfica - Sistema Solar em 3D

**Docente:** Iális Cavalcante.

**Discentes:** Maria Eline -
               Maria Gabriele -
               Mariana Teixeira -
               Maxela Martins -
               Najala Kelly Lima -
               Nicole Fernandes.

![Imagem do Sistema Solar](SistemaSolarFinal.png)

**Bibliotecas  e APIs Utilizadas:**
* [OpenGL](https://www.opengl.org/) -  API de gráficos 3D.
* [GLAD](https://glad.dav1d.de/) - Language: C/C++, Specification: OpenGL,
Profile: Core.
* [GLFW](https://www.glfw.org/) - Utilizado para criar janelas, contextos e superfícies, além de receber entradas e eventos.
* [FREETYPE](https://www.freetype.org/) - Usado para renderizar fontes.
* [GLM](https://glm.g-truc.net/0.9.9/index.html) - Biblioteca de Matemática .

**Recursos Implementados:**
  <p>      O projeto foi desenvolvido utilizando o Visual Studio 2019. Na pasta principal existe uma arquivo executável sistemaSolar.exe, que foi gerado para computadores com suporte a SO de x64. A linguagem escolhida para o desenvolvimento do projeto foi C++.</p>
 <p>       Sol, Lua e Planetas - Sistema com Objeto no centro e os outros orbitando ao seu redor.
 <p>       Câmeras -  Duas câmeras foram implementadas, a primeira que inicia com o projeto é a câmera estática e a segunda é executada ao se selecionar um planeta(Câmera Planetas) onde também aparece algumas informações sobre o planeta selecionado. Para volta para a primeira câmera utiliza-se a tecla space.
        </p>
   <p>      Skybox - Utilizada na criação do plano de fundo, para conseguir um efeito de profundidade na cena.
        </p>
  <p>       Movimento de Câmera - É possível  movimentar pela cena através das teclas 'w', 's', 'a' e 'd'
</p>

**Comandos:**
 * Scroll do Mouse ou movimento de "pinça no touchpad"  - Aplica zoom na cena.
 * Teclas 1 a 8 - Seleciona os planetas na ordem Mercúrio, Vênus, Terra, Marte, Saturno, Urano e Netuno respectivamente.
 * Teclas W,S,A e D - Movimenta a câmera no Modo Estático.

**Link para o repositório do projeto no Github:** https://github.com/ElineFarias/ProjetoSistemaSolar.git
<p>
**Link para o repositório do projeto no Github com a tentativa de implementação do Shader de Luz:**https://github.com/ElineFarias/ProjetoSistemaSolarLuz.git
  </p>

**Link do video de Demonstração:** https://youtu.be/0eiLlNwEeRA

**Referências:**

  *https://www.solarsystemscope.com/textures/
  *https://www.glfw.org/documentation.html
  *https://glm.g-truc.net/0.9.5/api/a00176.html
  *https://github.com/JoeyDeVries/LearnOpenGL
  *https://github.com/matheustheus27/SolarSystem
  *https://cosmos-online.ru/textures
  *https://github.com/Sectah/Solar-system
  *https://www.inf.pucrs.br/~pinho/CG/Aulas/OpenGL/Texturas/MapTextures.html
  *https://learnopengl.com/Introduction
  *https://learnopengl.com/code_viewer_gh.php?code=src/4.advanced_opengl/10.3.asteroids_instanced/asteroids_instanced.cpp
  *https://ogldev.org/www/tutorial25/tutorial25.html
  *http://douc54.cs.edinboro.edu/~bennett/class/csci360/fall2016/notes/gl/two.html
  *https://www.geertarien.com/blog/2017/07/30/breakdown-of-the-lookAt-function-in-OpenGL/


<h3> DIFICULDADES ENCONTRADAS PELA EQUIPE</h3>

  * Uma das principais dificuldades encontradas pela equipe foram as manipulações das bibliotecas do OpenGL, por ser um ambiente de computação gráfica pesado, isso dificultou o desenvolvimento da aplicação. 
  
  * Além disso, outras dificuldades também encontradas foram em relação a continuidade do desenvolvimento em termos de iluminação, todos os membros da equipe tiveram dificuldades, pois no decorrer do desenvolvimento do código, vários bugs foram surgindo à medida que evoluímos.
  
  * Dificuldade de execução do programa pelo Sistema Operacional Linux, uma vez que, o Visual Studio é uma ferramenta da Microsoft e não abrange outros Sistemas Operacionais além de Windows e Mac. 
  
  * Dificuldade encontrada no uso do Shader para a implementação da iluminação, quando é feita a manipulação da luz difusa, ambiente e especular, a luz da cena fica totalmente preta e mesmo fazendo alterações nos parâmetros de cada luz a cena continua preta.
  * Como se optou trabalhar através de shaders, ao tentar implementar a luz não consegui-se realizar a mistura da luz com a textura. 
  * Com o uso do visual Studio como ferramenta de desenvolvimento, agora nessa segunda etapa do trabalho ocorreu problemas ao carregar a cena, pois ao aplicar a luz, os planetas desapareciam da cena, porém depois percebeu-se que não era problema na ferramenta, pois ao executar o arquivo executável que era criado na compilação e debugação do projeto, os planetas carregavam.
  
  
