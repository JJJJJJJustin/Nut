#include "Nut.h"
#include <Nut/Core/EntryPoint.h>
#include "Sandbox2D.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class ExampleLayer : public Nut::Layer
{
public:
	ExampleLayer()
		: Layer("Example layer"), m_CameraController(1280.0f / 720.0f, true)
	{
		float vertices[3 * 7] = {
				-0.5f, -0.5f, 0.0f, 1.0f, 0.2f, 0.8f, 1.0f,
				 0.5f, -0.5f, 0.0f, 1.0f, 0.3f, 0.8f, 1.0f,
				 0.0f,  0.5f, 0.0f, 1.0f, 0.8f, 0.2f, 1.0f
		};
		unsigned int indices[3] = { 0, 1, 2 };

		Nut::Ref<Nut::VertexBuffer> vertexBuffer;
		Nut::Ref<Nut::IndexBuffer> indexBuffer;
		vertexBuffer = Nut::VertexBuffer::Create(vertices, sizeof(vertices));
		indexBuffer  = Nut::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));

		m_VertexArray = Nut::VertexArray::Create();
		Nut::BufferLayout layout =
		{
			{ Nut::ShaderDataType::Float3, "a_Position" },
			{ Nut::ShaderDataType::Float4, "a_Color" }
		};
		vertexBuffer->SetLayout(layout);												// when we set the layout, we store the layout data in OpenGLBuffer.m_Layout by "SetLayout()" function, then delete layout.

		m_VertexArray->AddVertexBuffer(vertexBuffer);
		m_VertexArray->SetIndexBuffer(indexBuffer);

		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			out vec3 v_Position;
			out vec4 v_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position =  u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";
		std::string fragmentSrc = R"(
			#version 330 core

			in vec3 v_Position;
			in vec4 v_Color;
			layout(location = 0) out vec4 a_Color;

			void main()
			{
				a_Color = vec4(v_Position * 0.5 + 0.5, 1.0);
				a_Color = v_Color;
			}
		)";

		m_Shader = Nut::Shader::Create("TriangleShader", vertexSrc, fragmentSrc);

		// -------------- Square rendering ----------------
		float squareVertices[5 * 4] =
		{
			-0.5f, -0.5f, -0.1f, 0.0f, 0.0f,
			 0.5f, -0.5f, -0.1f, 1.0f, 0.0f,
			 0.5f,  0.5f, -0.1f, 1.0f, 1.0f,
			-0.5f,  0.5f, -0.1f, 0.0f, 1.0f
		};
		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };

		m_SquareVA = Nut::VertexArray::Create();

		Nut::Ref<Nut::VertexBuffer> squareVB;
		squareVB = Nut::VertexBuffer::Create(squareVertices, sizeof(squareVertices));
		Nut::Ref<Nut::IndexBuffer> squareIB;
		squareIB = Nut::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));

		Nut::BufferLayout squareLayout =
		{
			{Nut::ShaderDataType::Float3, "a_Position"},
			{Nut::ShaderDataType::Float2, "a_TexCoord"}
		};
		squareVB->SetLayout(squareLayout);
		m_SquareVA->AddVertexBuffer(squareVB);
		m_SquareVA->SetIndexBuffer(squareIB);

		m_SquareShader = Nut::Shader::Create("assets/shaders/FlatColorShader.glsl");
		//auto m_TextureShader = Nut::Shader::Create("assets/shaders/TextureShader.glsl");
		auto textureShader = m_ShaderLibrary.Load("assets/shaders/TextureShader.glsl");

		textureShader->Bind();
		textureShader->SetInt("u_Texture", 0);
		m_EmojiTexture = Nut::Texture2D::Create("assets/textures/emoji.png");
		m_Texture = Nut::Texture2D::Create("assets/textures/rain.jpg");
	}

	void OnUpdate(Nut::Timestep ts) override {
		// Update
		m_CameraController.OnUpdate(ts);
		// Render
		Nut::RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Nut::RendererCommand::Clear();

		Nut::Renderer::BeginScene(m_CameraController.GetCamera());

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
		m_SquareShader->Bind();
		m_SquareShader->SetFloat4("u_Color", m_SquareColor);
		for (int y = 0; y < 20; y++) {
			for (int x = 0; x < 20; x++) {
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;

				Nut::Renderer::Submit(m_SquareShader, m_SquareVA, transform);
			}
		}

		auto textureShader = m_ShaderLibrary.Get("TextureShader");
		m_Texture->Bind(); 
		Nut::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)) );
		m_EmojiTexture->Bind();
		Nut::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		// Triangle Draw call
		//Nut::Renderer::Submit(m_Shader, m_VertexArray, glm::mat4(1.0f));

		Nut::Renderer::EndScene();
	}

	void OnImGuiRender() override
	{
		ImGui::Begin("Test");
		ImGui::ColorEdit4("Square Color Edit", glm::value_ptr(m_SquareColor));
		const char* text = R"(
                         ...----....
                  .-'                  ''
                   .          .    .    .''.
             .' . .  .       .   .   .. ....::.
            .  .    .    .  .  .    .... ....:IA.
           '.  . .   .       .  . .. .....:.::IHHB.
         .:... . ."::"'.. .   .  . .:.. .. ..:IHIMMA
        .:::I .AHHHHHHAI::. .:...,:IIHMHHL:. . VMMMM
       :..V.:VHHHHHHHHHHB... . .:VPHHMHHHHHHHAI.:VMMI
       ::". ..HHHHMMHHHHHI.  . .:..I:MHHHHHMHV:':H:WM
       '.  ....:.HHHMMHV" .AVMHMA.:.'VHHHV:' .  :IHWV
       .:.   ...    .   ..HMMMHMMMA::.:::....  .:IHW'
     : .   .'' .: .. .  :HMMM:IMMMI::IIIPPHI::'.P:HM.
     'V:.. ... ...  .   'VMMV..VMMV :.:..:....::IHHHH
        :IP""HII:  .    . . .'V:. . . :.::IHIHHMMMMM"
        :"VI:VWMA.:      .   .. .:. ..IVHHHMMMHMMMMI
        :..VIIHMM.::.,:.,:!"I:!"I!"I!"MMMMMMHMMMMMM'
          V:HI:MA :AXXXIXII:IIIISSSSSSMMMHMHMMMMMM
             :: VSSSSSSSSSBSSSMMBSSSBB:MMHIMMI
            ::I. SSXXIIIIXSSBMBSSXXXIIMI:.IMM
            :.I:."SSSSSSSISISSXIIXSSSSI:..MMM.
              "V::...:.IHHHMMMMMMMMMMMMMHHMHP'
                 ...::.:::..:..::IIIIIHHMV"
                     "V::... . .I::IHHV"'

		)";
		ImGui::Text(text);
		ImGui::End();
	}

	void OnEvent(Nut::Event& e) override
	{
		m_CameraController.OnEvent(e);
	}
private:
	Nut::Ref<Nut::Shader> m_Shader;
	Nut::ShaderLibrary m_ShaderLibrary;
	Nut::Ref<Nut::VertexArray> m_VertexArray;

	Nut::Ref<Nut::Shader> m_SquareShader;
	Nut::Ref<Nut::VertexArray> m_SquareVA;

	Nut::Ref<Nut::Texture2D> m_Texture,m_EmojiTexture;

	glm::vec4 m_SquareColor = { 0.5412f, 0.1686f, 0.8863f, 1.0f };

	Nut::OrthoGraphicCameraController m_CameraController;

};

class Sandbox : public Nut::Application
{
public:
	Sandbox()
	{
		//取消在 sandbox中 PushOverlay(new Nut::ImGuiLayer()); ，将其作为 Hazel 运行时自动添加的图层:DemoWindow（在 application.cpp 中固定调用 OnImGuiRender 函数）
		//PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());
	}

	~Sandbox()
	{
	}

};

Nut::Application* Nut::CreateApplication()
{
	return new Sandbox();
}