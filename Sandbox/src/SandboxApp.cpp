#include "Nut.h"

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

		std::shared_ptr<Nut::VertexBuffer> vertexBuffer;
		std::shared_ptr<Nut::IndexBuffer> indexBuffer;
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

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position =  u_ViewProjection * vec4(a_Position, 1.0);
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

		m_Shader.reset(new Nut::Shader(vertexSrc, fragmentSrc));

		// -------------- Square rendering ----------------
		float squareVertices[3 * 4] =
		{
			-0.75f, -0.75f, -0.1f,
			 0.75f, -0.75f, -0.1f,
			 0.75f,  0.75f, -0.1f,
			-0.75f,  0.75f, -0.1f
		};
		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };

		m_SquareVA.reset(Nut::VertexArray::Create());

		std::shared_ptr<Nut::VertexBuffer> squareVB;
		squareVB.reset(Nut::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		std::shared_ptr<Nut::IndexBuffer> squareIB;
		squareIB.reset(Nut::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));

		Nut::BufferLayout squareLayout =
		{
			{Nut::ShaderDataType::Float3, "a_Position"}
		};
		squareVB->SetLayout(squareLayout);
		m_SquareVA->AddVertexBuffer(squareVB);
		m_SquareVA->SetIndexBuffer(squareIB);

		std::string squareVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;

			void main()
			{
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
			}
		)";
		std::string squareFragSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 a_Color;

			void main()
			{
				a_Color = vec4(0.2, 0.3, 0.8, 1.0);
			}
		)";
		m_SquareShader.reset(new Nut::Shader(squareVertexSrc, squareFragSrc));

    }

	void OnUpdate(Nut::Timestep& ts) override{

		//NUT_TRACE("{0}s( {1}ms ) pre frame", ts.GetSeconds(), ts.GetMilliseconds())		165HZ: 0.0061478615s( 6.1478615ms ) pre frame

		if (Nut::Input::IsKeyPressed(NUT_KEY_RIGHT)) 
			m_CameraPosition.x -= m_CameraMoveSpeed * ts;
		else if (Nut::Input::IsKeyPressed(NUT_KEY_LEFT)) 
			m_CameraPosition.x += m_CameraMoveSpeed * ts;
	
		if (Nut::Input::IsKeyPressed(NUT_KEY_UP))
			m_CameraPosition.y -= m_CameraMoveSpeed * ts;
		else if(Nut::Input::IsKeyPressed(NUT_KEY_DOWN))
			m_CameraPosition.y += m_CameraMoveSpeed * ts;

		if (Nut::Input::IsKeyPressed(NUT_KEY_A)) 
			m_CameraRotation -= m_CameraRotateSpeed * ts;
		else if (Nut::Input::IsKeyPressed(NUT_KEY_D))
			m_CameraRotation += m_CameraRotateSpeed * ts;

        Nut::RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
        Nut::RendererCommand::Clear();

        m_Camera.SetPosition( m_CameraPosition );
        m_Camera.SetRotation( m_CameraRotation );

        Nut::Renderer::BeginScene(m_Camera);

        Nut::Renderer::Submit(m_SquareShader, m_SquareVA);
        Nut::Renderer::Submit(m_Shader, m_VertexArray);

        Nut::Renderer::EndScene();
	}

	void OnImGuiRender() override
	{
		ImGui::Begin("Test");
		const char* text = R"(
                             ...----....
                         ..-:"''         ''"-..
                      .-'                      '-.
                    .'              .     .       '.
                  .'   .          .    .      .    .''.
                .'  .    .       .   .   .     .   . ..:.
              .' .   . .  .       .   .   ..  .   . ....::.
             ..   .   .      .  .    .     .  ..  . ....:IA.
            .:  .   .    .    .  .  .    .. .  .. .. ....:IA.
           .: .   .   ..   .    .     . . .. . ... ....:.:VHA.
           '..  .  .. .   .       .  . .. . .. . .....:.::IHHB.
          .:. .  . .  . .   .  .  . . . ...:.:... .......:HIHMM.
         .:.... .   . ."::"'.. .   .  . .:.:.:II;,. .. ..:IHIMMA
         ':.:..  ..::IHHHHHI::. . .  ...:.::::.,,,. . ....VIMMHM
        .:::I. .AHHHHHHHHHHAI::. .:...,:IIHHHHHHMMMHHL:. . VMMMM
       .:.:V.:IVHHHHHHHMHMHHH::..:" .:HIHHHHHHHHHHHHHMHHA. .VMMM.
       :..V.:IVHHHHHMMHHHHHHHB... . .:VPHHMHHHMMHHHHHHHHHAI.:VMMI
       ::V..:VIHHHHHHMMMHHHHHH. .   .I":IIMHHMMHHHHHHHHHHHAPI:WMM
       ::". .:.HHHHHHHHMMHHHHHI.  . .:..I:MHMMHHHHHHHHHMHV:':H:WM
       :: . :.::IIHHHHHHMMHHHHV  .ABA.:.:IMHMHMMMHMHHHHV:'. .IHWW
       '.  ..:..:.:IHHHHHMMHV" .AVMHMA.:.'VHMMMMHHHHHV:' .  :IHWV
        :.  .:...:".:.:TPP"   .AVMMHMMA.:. "VMMHHHP.:... .. :IVAI
       .:.   '... .:"'   .   ..HMMMHMMMA::. ."VHHI:::....  .:IHW'
       ...  .  . ..:IIPPIH: ..HMMMI.MMMV:I:.  .:ILLH:.. ...:I:IM
     : .   .'"' .:.V". .. .  :HMMM:IMMMI::I. ..:HHIIPPHI::'.P:HM.
     :.  .  .  .. ..:.. .    :AMMM IMMMM..:...:IV":T::I::.".:IHIMA
     'V:.. .. . .. .  .  .   'VMMV..VMMV :....:V:.:..:....::IHHHMH
       "IHH:.II:.. .:. .  . . . " :HB"" . . ..PI:.::.:::..:IHHMMV"
        :IP""HHII:.  .  .    . . .'V:. . . ..:IH:.:.::IHIHHMMMMM"
        :V:. VIMA:I..  .     .  . .. . .  .:.I:I:..:IHHHHMMHHMMM
        :"VI:.VWMA::. .:      .   .. .:. ..:.I::.:IVHHHMMMHMMMMI
        :."VIIHHMMA:.  .   .   .:  .:.. . .:.II:I:AMMMMMMHMMMMMI
        :..VIHIHMMMI...::.,:.,:!"I:!"I!"I!"V:AI:VAMMMMMMHMMMMMM'
        ':.:HIHIMHHA:"!!"I.:AXXXVVXXXXXXXA:."HPHIMMMMHHMHMMMMMV
          V:H:I:MA:W'I :AXXXIXII:IIIISSSSSSXXA.I.VMMMHMHMMMMMM
            'I::IVA ASSSSXSSSSBBSBMBSSSSSSBBMMMBS.VVMMHIMM'"'
             I:: VPAIMSSSSSSSSSBSSSMMBSSSBBMMMMXXI:MMHIMMI
            .I::. "H:XIIXBBMMMMMMMMMMMMMMMMMBXIXXMMPHIIMM'
            :::I.  ':XSSXXIIIIXSSBMBSSXXXIIIXXSMMAMI:.IMM
            :::I:.  .VSSSSSISISISSSBII:ISSSSBMMB:MI:..:MM
            ::.I:.  ':"SSSSSSSISISSXIIXSSSSBMMB:AHI:..MMM.
            ::.I:. . ..:"BBSSSSSSSSSSSSBBBMMMB:AHHI::.HMMI
            :..::.  . ..::":BBBBBSSBBBMMMB:MMMMHHII::IHHMI
            ':.I:... ....:IHHHHHMMMMMMMMMMMMMMMHHIIIIHMMV"
              "V:. ..:...:.IHHHMMMMMMMMMMMMMMMMHHHMHHMHP'
               ':. .:::.:.::III::IHHHHMMMMMHMHMMHHHHM"
                 "::....::.:::..:..::IIIIIHHHHMMMHHMV"
                   "::.::.. .. .  ...:::IIHHMMMMHMV"
                     "V::... . .I::IHHMMV"'
                       '"VHVHHHAHHHHMMV:"'
		)";
		ImGui::Text(text);
		ImGui::End();
	}

	void OnEvent(Nut::Event& event) override
	{
		
	}
private:
	std::shared_ptr<Nut::Shader> m_Shader;
	std::shared_ptr<Nut::VertexArray> m_VertexArray;

	std::shared_ptr<Nut::Shader> m_SquareShader;
	std::shared_ptr<Nut::VertexArray> m_SquareVA;

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