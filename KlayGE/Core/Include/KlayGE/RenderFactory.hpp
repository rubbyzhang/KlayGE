#ifndef _RENDERFACTORY_HPP
#define _RENDERFACTORY_HPP

#include <KlayGE/Texture.hpp>
#include <KlayGE/RenderBuffer.hpp>
#include <KlayGE/RenderEffect.hpp>
#include <KlayGE/Font.hpp>

#include <boost/utility.hpp>

namespace KlayGE
{
	class RenderFactory
	{
	public:
		virtual ~RenderFactory()
			{ }

		virtual std::wstring const & Name() const = 0;

		virtual RenderEngine& RenderEngineInstance() = 0;
		virtual TexturePtr MakeTexture(uint32_t width, uint32_t height, uint16_t numMipMaps,
			PixelFormat format, Texture::TextureUsage usage = Texture::TU_Default) = 0;
		virtual RenderTexturePtr MakeRenderTexture(uint32_t width, uint32_t height) = 0;

		FontPtr MakeFont(std::string const & fontName, uint32_t fontHeight = 12, uint32_t flags = 0)
		{
			return FontPtr(new Font(fontName, fontHeight, flags));
		}

		virtual RenderEffectPtr MakeRenderEffect(std::string const & srcData, uint32_t flags = 0) = 0;

		// sizeElement表示流中每个元素的大小，比如Position流是size(float)
		// numElement表示一个顶点有几个元素表示，比如Position流是由(x, y, z)组成，所以为3
		virtual VertexStreamPtr MakeVertexStream(VertexStreamType type,
			uint8_t sizeElement, uint8_t numElement, bool staticStream = false) = 0;
		virtual IndexStreamPtr MakeIndexStream(bool staticStream = false) = 0;
	};

	template <typename RenderEngineType, typename TextureType, typename RenderTextureType,
		typename RenderEffectType>
	class ConcreteRenderFactory : boost::noncopyable, public RenderFactory
	{
	public:
		ConcreteRenderFactory(std::wstring const & name)
				: name_(name)
			{ }
		virtual ~ConcreteRenderFactory()
			{ }

		std::wstring const & Name() const
			{ return name_; }

		RenderEngine& RenderEngineInstance()
		{
			static RenderEngineType renderEngine;
			return renderEngine;
		}

	private:
		std::wstring const name_;
	};
}

#endif			// _RENDERFACTORY_HPP
