#pragma once
#include "window.h"

namespace gui
{
	class Image;
	class Imageset;
	typedef boost::shared_ptr<Imageset> ImagesetPtr;

	class Panel : public base_window
	{
	public:
		typedef Panel Self;
		Panel(System& sys, const std::string& name = std::string());
		virtual ~Panel(void);

		static const char* GetType() { return "Panel"; }
		virtual const char* getType() { return Self::GetType(); }

		virtual void render(const Rect& finalRect, const Rect& finalClip);
		virtual void init(xml::node& node);

	protected:
		void renderFrame(const Rect& dest, const Rect& clip);

	protected:
		ImagesetPtr m_imgset;

		// frame imagery
		const Image*	m_backImg;
		const Image*	m_topImg;
		const Image*	m_toplImg;
		const Image*	m_toprImg;
		const Image*	m_lImg;
		const Image*	m_rImg;
		const Image*	m_botImg;
		const Image*	m_botlImg;
		const Image*	m_botrImg;
	};
}