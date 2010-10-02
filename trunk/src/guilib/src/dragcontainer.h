#pragma once
#include "window.h"

namespace gui
{
	class  DragContainer : public BaseWindow
	{
	public:
		typedef DragContainer Self;
		DragContainer(System& sys, const std::string& name = "");
		virtual ~DragContainer(void);

		static const char* GetType() { return "DragContainer"; }
		virtual const char* getType() { return Self::GetType(); }

		virtual void rise();

		void update(BaseWindow* target, const point& pt);
		
		bool startDrag(BaseWindow* subj, const point& off);
		bool stopDrag(void);

		void reset(void);


	protected:
		BaseWindow* m_dragSubject;
		
		BaseWindow* m_dropTarget;
		point		m_childOffset;
		
	};
}