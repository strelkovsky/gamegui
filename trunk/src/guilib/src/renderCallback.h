#pragma once

namespace gui
{
	class BaseWindow;
	typedef boost::function <void (BaseWindow* wnd, const Rect& dest, const Rect& clip)> AfterRenderCallbackFunc;
}