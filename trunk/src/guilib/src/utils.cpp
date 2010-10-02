#include "stdafx.h"
#include "utils.h"

#include <windows.h>
#include "vector.h"
#include "rect.h"
#include "color.h"

namespace gui
{
	#pragma message ("Use portable UTF conversion from http://sourceforge.net/projects/utfcpp/ !!!")

	std::wstring UTF8ToUTF16(const std::string& text)
	{
		// translate from UTF-8 to UTF-16
		std::wstring out;

		size_t len = text.length();
		char* src = new char[len+1];
		memset(src, 0, len+1);
		memcpy(src, text.c_str(), len);

		size_t needed = ::MultiByteToWideChar(CP_UTF8, NULL, src, (int)len+1, NULL, NULL);
		if(needed)
		{
			wchar_t* dst = new wchar_t[needed+1];
			memset(dst, 0, needed+1);
			if(::MultiByteToWideChar(CP_UTF8, NULL, src, (int)len+1, dst, (int)needed))
			{
				out = dst;
			}
			delete [] dst;
		}
		delete [] src;
		return out;
	}

	std::string UTF16ToUTF8(const std::wstring& wtext)
	{
		// translate from UTF-16 to UTF-8
		std::string out;
		size_t len = wtext.length();

		size_t needed = ::WideCharToMultiByte(CP_UTF8, NULL, wtext.c_str(), (int)len, NULL, NULL, NULL, NULL);
		if(needed)
		{
			char* dst = new char[needed+1];
			memset(dst, 0, needed+1);
			if(::WideCharToMultiByte(CP_UTF8, NULL, wtext.c_str(), (int)len, dst, (int)needed, NULL, NULL))
			{
				out = dst;
			}
			delete [] dst;
		}
		return out;	
	}

	point StringToPoint(const std::string& str)
	{
		int x, y;
		sscanf(str.c_str(), "%d %d", &x, &y);
		return point((float)x, (float)y);
	}

	std::string PointToString(const point& val)
	{
		char buff[128] = {0};
		_snprintf(buff, sizeof (buff), "%d %d", val.x, val.y);
		return std::string(buff);
	}

	bool StringToBool(const std::string& str)
	{
		return (str == "true" || str == "True");
	}

	const std::string& BoolToString(bool val)
	{
		static const std::string true_str("true");
		static const std::string false_str("false");
		return val ? true_str : false_str;
	}

	Size StringToSize(const std::string& str)
	{
		int w,h;
		sscanf(str.c_str(), "%d %d", &w, &h);
		return Size((float)w, (float)h);
	}

	std::string SizeToString(const Size& val)
	{
		char buff[128] = {0};
		_snprintf(buff, sizeof (buff), "%d %d", val.width, val.height);
		return std::string(buff);
	}

	Rect StringToArea(const std::string& str)
	{
		float x = 0;
		float y = 0;
		float w = 0;
		float h = 0;
		sscanf(str.c_str(), "%f %f %f %f", &x, &y, &w, &h);

		return Rect(point(x, y), Size(w, h));
	}

	Rect StringToRect(const std::string& str)
	{
		float x = 0;
		float y = 0;
		float w = 0;
		float h = 0;
		sscanf(str.c_str(), "%f %f %f %f", &x, &y, &w, &h);

		return Rect(x, y, w, h);
	}

	std::string RectToString(const Rect& val)
	{
		char buff[128] = {0};
		_snprintf(buff, sizeof (buff), "%d %d %d %d", val.m_left, val.m_top, val.m_right - val.m_left, val.m_bottom - val.m_top);

		return std::string(buff);
	}

	Align StringToAlign(const std::string& str)
	{
		if(str == "left" || str == "Left")
			return Left;
		if(str == "top" || str == "Top")
			return Top;
		if(str == "right" || str == "Right")
			return Right;
		if(str == "bottom" || str == "Bottom")
			return Bottom;
		if(str == "hcenter" || str == "HCenter")
			return HCenter;
		if(str == "vcenter" || str == "VCenter")
			return VCenter;

		return None;
	}

	std::string AlignToString(Align val)
	{
		switch(val)
		{
		case Left:
			return "left";
			break;
		case Top:
			return "top";
			break;
		case Right:
			return "right";
			break;
		case Bottom:
			return "bottom";
			break;
		case HCenter:
			return "hcenter";
			break;
		case VCenter:
			return "vcenter";
			break;
		case None:
		default:
			return "none";
			break;
		};
	}

	unsigned int StringToAlignment(const std::string& str)
	{
		unsigned int a = 0;
		if(!str.empty())
		{
			boost::char_separator<char> sep(", |");
			typedef boost::tokenizer<boost::char_separator<char> > aligntokenizer;
			aligntokenizer tok(str, sep);
			aligntokenizer::iterator i = tok.begin();
			aligntokenizer::iterator end = tok.end();
			while(i != end)
			{
				a |= StringToAlign(*i);
				++i;
			}
		}
		return a;
	}

	std::string AlignmentToString(unsigned int val)
	{
		std::string out;

		for(unsigned int i = 0; i < 8; i++)
		{
			unsigned int a = (1 << i) & val;
			if(i > 0)
				out += " | ";
			out += AlignToString((Align)a);
		}

		return out;
	}
	Color StringToColor(const std::string& str)
	{
		float r = 0.f;
		float g = 0.f;
		float b = 0.f;
		float a = 1.f;
		sscanf(str.c_str(), "%f %f %f %f", &r, &g, &b, &a);

		return Color(r,g,b,a);
	}

	std::string ColorToString(const Color& val)
	{
		char buff[128] = {0};
		_snprintf(buff, sizeof (buff), "%f %f %f %f", val.getRed(), val.getGreen(), val.getBlue(), val.getAlpha());

		return std::string(buff);
	}

	Color HexStringToColor(const std::string& str)
	{
		unsigned int val = (unsigned int)-1;
		sscanf(str.c_str(), "%8x", &val);
		val |= 0xFF000000; // ensure alpha is 1

		return Color(val);
	}

	std::string ColorToHexString(const Color& val)
	{
		char buff[128] = {0};
		_snprintf(buff, sizeof (buff), "%08x", val.getARGB());

		return std::string(buff);
	}
}