#pragma once
#include "window.h"
#include "image.h"

namespace gui
{
	class Imageset;
	typedef boost::shared_ptr<Imageset> ImagesetPtr;

	class  StaticImage : public BaseWindow
	{
	public:
		typedef StaticImage Self;
		StaticImage(System& sys, const std::string& name = "");
		virtual ~StaticImage();
		
		static const char* GetType() { return "StaticImage"; }
		virtual const char* getType() { return Self::GetType(); }

		void setImageset(const std::string& set);
		const std::string getImageset() const;

		void setImage(const std::string& image);
		const std::string getImage() const;

		void setVertFormat(const std::string& format)
		{
			m_vformat = StringToImageOps(format);
			invalidate();
		}
		const std::string getVertFormat() const { return ImageOpsToString(m_vformat); }

		void setHorzFormat(const std::string& format)
		{
			m_hformat = StringToImageOps(format);
			invalidate();
		}
		const std::string getHorzFormat() const { return ImageOpsToString(m_hformat); }

		virtual void render(const Rect& finalRect, const Rect& finalClip);

		virtual void init(xml::node& node);

	protected:
		ImagesetPtr m_imgset;
		const Image* m_img;
		Image::ImageOps	m_vformat;
		Image::ImageOps	m_hformat;
	};
}