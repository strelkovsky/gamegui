#pragma once
#include "window.h"
#include "textformatting.h"

namespace gui
{
	class Font;
	typedef boost::shared_ptr<Font> FontPtr;

	class  StaticText :	public BaseWindow
	{
	public:
		typedef StaticText Self;
		StaticText(System& sys, const std::string& name = std::string());
		virtual ~StaticText();

		static const char* GetType() { return "StaticText"; }
		virtual const char* getType() { return Self::GetType(); }

		void setFont(const std::string& font);
		void setFont(FontPtr font) { m_font = font; }
		
		virtual void setText(const std::string& text);
		virtual const std::string getText() const { return m_text; }
		virtual void appendText(const std::string& text);

		void setTextFormatting(TextFormatting fmt)
		{
			m_format = fmt;
			invalidate();
		}
		TextFormatting getTextFormatting() const { return m_format; }

		void setSpacing(float spacing);
		float getSpacing() const { return m_spacing; }

		virtual void render(const Rect& finalRect, const Rect& finalClip);
		virtual void init(xml::node& node);

	protected:
		FontPtr m_font;
		std::string m_text;
		TextFormatting m_format;
		float m_spacing;

		bool m_centred;
	};
}
