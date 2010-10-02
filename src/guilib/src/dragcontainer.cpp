#include "StdAfx.h"
#include "dragcontainer.h"

#include "system.h"

namespace gui
{
	DragContainer::DragContainer(System& sys, const std::string& name) :
		BaseWindow(sys, name),
		m_dropTarget(0),
		m_dragSubject(0)
	{
		reset();
	}

	DragContainer::~DragContainer(void)
	{
	}

	namespace
	{
		struct seeker
		{
			const BaseWindow* m_ptr;
			seeker(const BaseWindow* ptr) : m_ptr(ptr){}
			bool operator()(WindowPtr obj) 
			{
				return obj ? (obj.get() == m_ptr) : false;
			}
		};
	}

	void DragContainer::rise()
	{
		if(m_parent)
		{
			ChildrenList& children = m_parent->getChildren();
			ChildrenIter it = std::find_if(children.begin(), children.end(), seeker(this));
			if(it != children.end())
			{
				children.splice(children.end(), children, it);
			}
		}
	}


	void DragContainer::update(BaseWindow* target, const point& pt)
	{
		m_dropTarget = target;
		setPosition(pt - m_childOffset);
	}

	void DragContainer::reset(void)
	{
		setVisible(false);
		setIgnoreInputEvents(true);
		m_dropTarget = 0;
		m_dragSubject = 0;
	}

	bool DragContainer::startDrag(BaseWindow* subj, const point& off)
	{
		setVisible(true);
		m_dragSubject = subj;
		
		DragEventArgs de;
		de.name = "On_DragStarted";
		de.offset = off;
		de.subj = m_dragSubject;
		subj->callHandler(&de);
		
		m_childOffset = de.offset;
		return de.handled;
	}

	bool DragContainer::stopDrag(void)
	{
		if(!m_dropTarget)
			m_dropTarget = m_system.getRootPtr().get();
		
		if(m_dropTarget->isAcceptDrop())
		{
			DragEventArgs de;
			de.name = "On_DragDropped";
			de.subj = m_dragSubject;
			m_dropTarget->callHandler(&de);
		}
		
		if (!m_system.isDragFrozen())
		{
			DragEventArgs de;
			de.name = "On_DragStopped";
			de.subj = m_dragSubject;
			m_dragSubject->callHandler(&de);

			if (de.handled)
			{
				reset();
				return true;
			}
		}

		return false;
	}
}