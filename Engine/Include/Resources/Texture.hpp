#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Maths/Vector3.h>
#include <vector>
#include <deque>


namespace Resources
{
	struct TextureDatas
	{
		//TextureDatas() = default;
		//TextureDatas(GLenum f1, GLenum f2, float in_data);

		GLenum formats[2];
		float* data = nullptr;
		bool filled = false;

		void destroy();
	};

	enum class TextureType
	{
		Default,
		CubeMap
	};
	
	class Texture
	{
	protected:


		//	Protected Internal Variables
		//	----------------------------

		std::vector<TextureDatas> m_datas;

		GLuint	m_ID = 0;

	public:

		//	Constructor & Destructor
		//	------------------------
		Texture() = default;
		Texture(const std::string& filename, const std::string& textName);

		//	Public Internal Variables
		//	-------------------------
		int m_width = 0;
		int m_height = 0;
		
		bool m_generated = false;


		std::string m_path;
		std::string m_name;

		//	Public Internal Functions
		//	-------------------------

		virtual void generateGL();
		
		void destroyTexture();

		void bind(const int bindID) const;
		GLuint getID() const { return m_ID; }

		void showImGui() const;
	};



	class TextureCubeMap : public Texture
	{
	public:

		//	Constructor & Destructor
		//	------------------------
		TextureCubeMap() = default;
		TextureCubeMap(const std::string& textName, const std::vector<std::string>& faces);


		void generateGL() override;

	};


	class TextureMap
	{
	protected:

		//	Protected Internal Variables
		//	----------------------------

		Texture* m_texture = nullptr;
		std::string m_name;

	public:

		//	Constructor & Destructor
		//	------------------------

		TextureMap() = default;
		TextureMap(Texture * in_texture);
		TextureMap(const std::string& path, const std::string& textName, const Maths::Vector3f& in_offset, const Maths::Vector3f& in_tiling);

		//	Public Internal Variables
		//	-------------------------

		Maths::Vector3f m_offset = { 0.0f, 0.0f, 0.0f };
		Maths::Vector3f m_tiling = { 1.0f, 1.0f, 1.0f };

		void bind(const int bindID) const;
		GLuint getTextureID() const;

		virtual void showImGui();
	};



	class BumpMap : public TextureMap
	{
	public:

		BumpMap() = default;
		BumpMap(const std::string& path, const std::string& textName, const Maths::Vector3f & in_offset, const Maths::Vector3f & in_tiling, const float in_multiplier);

		float m_multiplier = 1.0f;
		void showImGui() override;
	};
}