#include "Nut.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui/imgui.h"

class ExampleLayer : public Nut::Layer
{
public:
	ExampleLayer()
		: Layer("Example layer"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f), m_CameraRotation(0.0f)
	{
		float vertices[3 * 7] = {
				-0.5f, -0.5f, 0.0f, 1.0f, 0.2f, 0.8f, 1.0f,
				 0.5f, -0.5f, 0.0f, 1.0f, 0.3f, 0.8f, 1.0f,
				 0.0f,  0.5f, 0.0f, 1.0f, 0.8f, 0.2f, 1.0f
		};
		unsigned int indices[3] = { 0, 1, 2 };

		Nut::Ref<Nut::VertexBuffer> vertexBuffer;
		Nut::Ref<Nut::IndexBuffer> indexBuffer;
		vertexBuffer.reset(Nut::VertexBuffer::Create(vertices, sizeof(vertices)));
		indexBuffer.reset(Nut::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));

		m_VertexArray.reset(Nut::VertexArray::Create());
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

		m_Shader.reset(Nut::Shader::Create(vertexSrc, fragmentSrc));

		// -------------- Square rendering ----------------
		float squareVertices[5 * 4] =
		{
			-0.5f, -0.5f, -0.1f, 0.0f, 0.0f,
			 0.5f, -0.5f, -0.1f, 1.0f, 0.0f,
			 0.5f,  0.5f, -0.1f, 1.0f, 1.0f,
			-0.5f,  0.5f, -0.1f, 0.0f, 1.0f
		};
		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };

		m_SquareVA.reset(Nut::VertexArray::Create());

		Nut::Ref<Nut::VertexBuffer> squareVB;
		squareVB.reset(Nut::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		Nut::Ref<Nut::IndexBuffer> squareIB;
		squareIB.reset(Nut::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));

		Nut::BufferLayout squareLayout =
		{
			{Nut::ShaderDataType::Float3, "a_Position"},
			{Nut::ShaderDataType::Float2, "a_TexCoord"}
		};
		squareVB->SetLayout(squareLayout);
		m_SquareVA->AddVertexBuffer(squareVB);
		m_SquareVA->SetIndexBuffer(squareIB);

		std::string squareVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			void main()
			{
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";
		std::string squareFragSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 Color;

			uniform vec3 u_Color;

			void main()
			{
				Color = vec4(u_Color, 1.0);
			}
		)";
		m_SquareShader.reset(Nut::Shader::Create(squareVertexSrc, squareFragSrc));

		std::string textureVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec2 a_TexCoord;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec2 v_TexCoord;

			void main()
			{
				v_TexCoord = a_TexCoord;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";
		std::string textureFragSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 Color;
			
			in vec2 v_TexCoord;

			uniform sampler2D u_Texture;

			void main()
			{
				Color = texture(u_Texture, v_TexCoord);
			}
		)";
		m_TextureShader.reset(Nut::Shader::Create(textureVertexSrc, textureFragSrc));
		std::dynamic_pointer_cast<Nut::OpenGLShader>(m_TextureShader)->Bind();
		std::dynamic_pointer_cast<Nut::OpenGLShader>(m_TextureShader)->UpdateUniformInt("u_Texture", 0);
		m_EmojiTexture = Nut::Texture2D::Create("assets/textures/emoji.png");
		m_Texture = Nut::Texture2D::Create("assets/textures/rain.jpg");
	}

	void OnUpdate(Nut::Timestep& ts) override {

		//NUT_TRACE("{0}s( {1}ms ) pre frame", ts.GetSeconds(), ts.GetMilliseconds())		165HZ: 0.0061478615s( 6.1478615ms ) pre frame

		if (Nut::Input::IsKeyPressed(NUT_KEY_LEFT))
			m_CameraPosition.x += m_CameraMoveSpeed * ts;
		else if (Nut::Input::IsKeyPressed(NUT_KEY_RIGHT))
			m_CameraPosition.x -= m_CameraMoveSpeed * ts;

		if (Nut::Input::IsKeyPressed(NUT_KEY_UP))
			m_CameraPosition.y -= m_CameraMoveSpeed * ts;
		else if (Nut::Input::IsKeyPressed(NUT_KEY_DOWN))
			m_CameraPosition.y += m_CameraMoveSpeed * ts;

		if (Nut::Input::IsKeyPressed(NUT_KEY_A))
			m_CameraRotation -= m_CameraRotateSpeed * ts;
		else if (Nut::Input::IsKeyPressed(NUT_KEY_D))
			m_CameraRotation += m_CameraRotateSpeed * ts;

		Nut::RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Nut::RendererCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		Nut::Renderer::BeginScene(m_Camera);

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
		std::dynamic_pointer_cast<Nut::OpenGLShader>(m_SquareShader)->Bind();
		std::dynamic_pointer_cast<Nut::OpenGLShader>(m_SquareShader)->UpdateUniformFloat3("u_Color", m_SquareColor);
			for (int y = 0; y < 20; y++) {
				for (int x = 0; x < 20; x++) {
					glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
					glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;

					Nut::Renderer::Submit(m_SquareShader, m_SquareVA, transform);
				}
			}
		m_Texture->Bind(); 
		Nut::Renderer::Submit(m_TextureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)) );
		m_EmojiTexture->Bind();
		Nut::Renderer::Submit(m_TextureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		// Triangle Draw call
		//Nut::Renderer::Submit(m_Shader, m_VertexArray, glm::mat4(1.0f));

		Nut::Renderer::EndScene();
	}

	void OnImGuiRender() override
	{
		ImGui::Begin("Test");
		ImGui::ColorEdit3("Square Color Edit", glm::value_ptr(m_SquareColor));
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

	void OnEvent(Nut::Event& event) override
	{

	}
private:
	Nut::Ref<Nut::Shader> m_Shader;
	Nut::Ref<Nut::VertexArray> m_VertexArray;

	Nut::Ref<Nut::Shader> m_SquareShader, m_TextureShader;
	Nut::Ref<Nut::VertexArray> m_SquareVA;

	Nut::Ref<Nut::Texture2D> m_Texture,m_EmojiTexture;

	glm::vec3 m_SquareColor = { 0.5412f, 0.1686f, 0.8863f };

	Nut::OrthoGraphicCamera m_Camera;

	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 4.0f;
	float m_CameraRotation;
	float m_CameraRotateSpeed = 180.0f;
};

class Sandbox : public Nut::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
		//取消 PushOverlay(new Nut::ImGuiLayer()); ，将其作为 Hazel 运行时 固定自动添加的图层（在 application.cpp 中）
	}

	~Sandbox()
	{
	}

};

Nut::Application* Nut::CreateApplication()
{
	return new Sandbox();
}