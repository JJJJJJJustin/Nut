#pragma once

// Here we defined VertexBuffer、IndexBuffer、BufferLayout

namespace Nut
{
	//////////////////////////////////////////////////////
	/////////////////// BufferLayout /////////////////////
	//////////////////////////////////////////////////////
	enum class ShaderDataType : uint8_t
	{
		None, Float, Float2, Float3, Float4, Int, Int2, Int3, Int4, Mat3, Mat4, Bool
	};

	static  uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type) {
		case ShaderDataType::Float:		return 4;
		case ShaderDataType::Float2:	return 4 * 2;
		case ShaderDataType::Float3:	return 4 * 3;
		case ShaderDataType::Float4:	return 4 * 4;
		case ShaderDataType::Int:		return 4;
		case ShaderDataType::Int2:		return 4 * 2;
		case ShaderDataType::Int3:		return 4 * 3;
		case ShaderDataType::Int4:		return 4 * 4;
		case ShaderDataType::Mat3:		return 4 * 3 * 3;
		case ShaderDataType::Mat4:		return 4 * 4 * 4;
		case ShaderDataType::Bool:		return 1;
		}
		NUT_CORE_ASSERT(false, "Unknown ShaderDataType !");
		return 0;
	}

	struct LayoutElement {
		std::string Name;
		ShaderDataType Type;
		uint32_t Size;
		uint32_t Offset;
		uint32_t Count;
		bool Normalized;
		uint32_t GLType;

		LayoutElement() {}

		LayoutElement(ShaderDataType type, const std::string& name, bool normalized = false)
			:Type(type), Name(name), Size(ShaderDataTypeSize(type)), Offset(0), Count(GetComponentCount()), Normalized(normalized), GLType(GetTypeToGLType(type))
			//这里的 Size 是某一个顶点属性中完整的一个分量的字节大小（x1,y1,z1）
		{}

		uint32_t GetComponentCount() const {
			switch (Type) {
			case ShaderDataType::Float:		return 1;
			case ShaderDataType::Float2:	return 2;
			case ShaderDataType::Float3:	return 3;
			case ShaderDataType::Float4:	return 4;
			case ShaderDataType::Int:		return 1;
			case ShaderDataType::Int2:		return 2;
			case ShaderDataType::Int3:		return 3;
			case ShaderDataType::Int4:		return 4;
			case ShaderDataType::Mat3:		return 3 * 3;
			case ShaderDataType::Mat4:		return 4 * 4;
			case ShaderDataType::Bool:		return 1;
			}
			NUT_CORE_ASSERT(false, "Unknown ShaderDataType !");
			return 0;
		}

		uint32_t GetTypeToGLType(ShaderDataType type) {
			switch (type) {
			case ShaderDataType::Float:		return 0x1406;				//GL_FLOAT == 0x1406
			case ShaderDataType::Float2:	return 0x1406;
			case ShaderDataType::Float3:	return 0x1406;
			case ShaderDataType::Float4:	return 0x1406;
			case ShaderDataType::Int:		return 0x1404;				//GL_INT == 0x1404
			case ShaderDataType::Int2:		return 0x1404;
			case ShaderDataType::Int3:		return 0x1404;
			case ShaderDataType::Int4:		return 0x1404;
			case ShaderDataType::Mat3:		return 0x1406;
			case ShaderDataType::Mat4:		return 0x1406;
			case ShaderDataType::Bool:		return 0x8B56;				// GL_BOOL == 0x8B56
			}
			NUT_CORE_ASSERT(false, "Unknown ShaderDataType !");
			return 0;
		}

	};

	class BufferLayout
	{
	public:
		BufferLayout() {}

		BufferLayout(std::initializer_list<LayoutElement> elements)
			:m_Elements(elements)
		{
			CalcOffsetAndStride();
		}

		inline const std::vector<LayoutElement>& GetElements() const { return m_Elements; }
		inline const uint32_t& GetStride() const { return m_Stride; }

		std::vector<LayoutElement>::iterator begin() { return m_Elements.begin(); };			//方便在 for each 中将 m_Element(layout) 作为 range
		std::vector<LayoutElement>::iterator end() { return m_Elements.end(); };				//！！！！函数名（迭代器名称）记得是小写 :"end"
		std::vector<LayoutElement>::const_iterator begin() const { return m_Elements.begin(); }	//如果 layout 是 const，则需要 const 类型的迭代器协助 layout 作为 range
		std::vector<LayoutElement>::const_iterator end() const { return m_Elements.end(); }
	private:
		std::vector<LayoutElement> m_Elements;
		uint32_t m_Stride;
	private:
		void CalcOffsetAndStride()
		{
			m_Stride = 0;
			uint32_t offset = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = offset;			//第一个布局的 offset 为 0，后续累加
				offset += element.Size;
				m_Stride += element.Size;			// Size 在 LayoutElement 的构造函数中被计算了
			}
		}

	};

	//////////////////////////////////////////////////////
	/////////////////// VertexBuffer /////////////////////
	//////////////////////////////////////////////////////
	class VertexBuffer
	{
	public:
		virtual  ~VertexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;

		static VertexBuffer* Create(float* vertices, uint32_t size);
	};

	//////////////////////////////////////////////////////
	/////////////////// IndexBuffer //////////////////////
	//////////////////////////////////////////////////////
	class IndexBuffer
	{
	public:
		virtual  ~IndexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const  = 0;
		virtual uint32_t GetCount() const = 0;

		static IndexBuffer* Create(uint32_t* indices, uint32_t count);
	};


}