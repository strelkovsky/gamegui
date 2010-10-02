#pragma once

#include "window.h"

namespace gui
{
	class ScrollBar;
	class ScrollPane;

	class  ScrollPane : public BaseWindow
	{
	public:
		typedef ScrollPane Self;
		ScrollPane(System& sys, const std::string& name = "");
		virtual ~ScrollPane();

		static const char* GetType() { return "ScrollPane"; }
		virtual const char* getType() { return Self::GetType(); }

		virtual bool onLoad();
		virtual bool onSized(bool update = true);
		virtual bool onMouseWheel(int delta);
		virtual void init(xml::node& node);

		void setTarget(BaseWindow* target);

		void onTrack(const events::TrackEvent& e);
		void onTargetSized(const events::SizedEvent& e);

	protected:
		void layoutTarget();

	protected:
		ScrollBar*	m_scroll;
		BaseWindow* m_target;
	};
}