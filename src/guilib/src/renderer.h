#pragma once

#include "rect.h"
#include "colorRect.h"
#include "texture.h"
#include "texmanager.h"
#include "imageops.h"

namespace gui
{
class base_window;
typedef boost::function <void (base_window* wnd, const Rect& dest, const Rect& clip)> AfterRenderCallbackFunc;

struct filesystem;
typedef boost::shared_ptr<filesystem> filesystem_ptr;

struct log;

struct RenderImageInfo;

class Font;
typedef boost::shared_ptr<Font> FontPtr;

enum OrientationFlags
{
	FlipHorizontal		= 1,	//!< Horizontal flip the image
	FlipVertical		= 2,	//!< Vertical flip the image
	RotateRightAngle	= 4		//!< Rotate the image anticlockwise 90 degree
};

class Image;

class  Renderer
{
public:
	struct RenderCallbackInfo
	{
		AfterRenderCallbackFunc afterRenderCallback;
		base_window* window;
		Rect dest;
		Rect clip;
	};

	struct QuadInfo 
	{
		struct vec2 {float x, y;};
		Texture*			texture;

		// order: 0 ---> 1
		//		  2 ---> 3
		vec2				positions[4];

		float				z;
		Rect				texPosition;
		unsigned long		topLeftCol;
		unsigned long		topRightCol;
		unsigned long		bottomLeftCol;
		unsigned long		bottomRightCol;

		inline bool operator<(const QuadInfo& other) const
		{
			// this is intentionally reversed.
			return z > other.z;
		}
	};

	struct BatchInfo
	{
		Texture* texture;
		std::size_t startQuad;
		std::size_t numQuads;
		//QuadInfo* quads;

		RenderCallbackInfo callbackInfo;
	};

	Renderer(filesystem_ptr fs);
	virtual ~Renderer();

	virtual void	addCallback( AfterRenderCallbackFunc callback,
								 base_window* window, const Rect& dest, const Rect& clip) = 0;

	void	drawLine(const Image& img, const vec2* p, size_t size, float z, const Rect& clip_rect, const Color& color, float width);
	
	void	draw(const Image& img, const Rect& dest_rect, float z, const Rect& clip_rect, const ColorRect& colors);
	void	draw(const Image& img, const Rect& dest_rect, float z, const Rect& clip_rect, const ColorRect& colors, ImageOps horz, ImageOps vert);
	void	immediateDraw(const Image& img, const Rect& dest_rect, float z, const Rect& clip_rect, const ColorRect& colors);

	virtual	void	doRender() = 0;
	virtual	void	clearRenderList();	

	virtual void	beginBatching();
	virtual void	endBatching();
	void			clearCache(base_window* window = 0);
	bool			isExistInCache(base_window* window) const;
	
	virtual void	setQueueingEnabled(bool setting)  { m_isQueueing = setting; }
	bool	isQueueingEnabled(void) const { return m_isQueueing; }

	virtual	TexturePtr	createTexture(const std::string& filename) = 0;
	virtual	TexturePtr	createTexture(const void* data, unsigned int width, unsigned int height, Texture::PixelFormat format) = 0;
	virtual	TexturePtr	createTexture(unsigned int width, unsigned int height, Texture::PixelFormat format) = 0;
	virtual TexturePtr	reloadTexture(TexturePtr p, const void* data, unsigned int width, unsigned int height, Texture::PixelFormat format) = 0;

	// font managment TODO: remove it!
	virtual FontPtr		createFont(const std::string& name, const std::string& fontname, unsigned int size) = 0;

	virtual void startCaptureForCache(base_window* window) ;
	virtual void endCaptureForCache(base_window* window) ;
	virtual void drawFromCache(base_window* window) {window;}

	const Size&	getOriginalSize(void) const	{ return m_originalsize; }
	const Size	getSize(void);
	unsigned int getMaxTextureSize(void) const { return m_maxTextureSize; }
	unsigned int getHorzScreenDPI(void) const { return m_hdpi; }
	unsigned int getVertScreenDPI(void) const { return m_vdpi; }

	void	setOriginalSize(const Size& sz) { m_originalsize = sz; }

	void	resetZValue()				{m_current_z = GuiZInitialValue;}
	void	advanceZValue()				{m_current_z -= GuiZElementStep;}
	float	getCurrentZ() const			{return m_current_z;}
	float	getZLayer(unsigned int layer) const {return m_current_z - ((float)layer * GuiZLayerStep);}

	virtual void	OnResetDevice();
	virtual void	OnLostDevice();

	void setAutoScale(bool status) { m_autoScale = status; }
	bool isAutoScale() const { return m_autoScale; }
	virtual Size getViewportSize() const = 0;

	Rect virtualToRealCoord( const Rect& virtualRect ) const;
	Rect realToVirtualCoord( const Rect& realRect ) const;

	void cleanup(bool complete);

	filesystem_ptr get_filesystem() {return m_filesystem;}

protected:
	virtual	void addQuad(const Rect& dest_rect, const Rect& tex_rect, float z, const RenderImageInfo& img, const ColorRect& colours) = 0;
	virtual void addQuad(const vec2& p0, const vec2& p1, const vec2& p2, const vec2& p3, const Rect& tex_rect, float z, const RenderImageInfo& img, const ColorRect& colours) = 0;
	virtual void renderQuadDirect(const QuadInfo& q) = 0;

	void fillQuad(QuadInfo& quad, const Rect& rc, const Rect& uv, float z, const RenderImageInfo& img, const ColorRect& colors);
	void sortQuads();	
	
	friend TextureManager;
	virtual	TexturePtr	createTextureInstance(const std::string& filename) = 0;

protected:
	Renderer& operator=(const Renderer&) { return *this; }
	void computeVirtualDivRealFactor(Size& coefOut) const;

protected:
	TextureManager m_texmanager;
	
	typedef std::vector<QuadInfo> Quads;
	typedef std::vector<BatchInfo> Batches;

	Quads m_quads;
	Batches m_batches;

	size_t m_num_quads;
	size_t m_num_batches;
	
	Size m_originalsize;
	unsigned int m_hdpi;
	unsigned int m_vdpi;
	unsigned int m_maxTextureSize;

	bool m_isQueueing;
	bool m_autoScale;

	const float	GuiZInitialValue;
	const float	GuiZElementStep;
	const float	GuiZLayerStep;
	float	m_current_z;

	//log& m_log;

	typedef std::vector <QuadInfo> CachedQuadList;
	struct QuadCacheRecord
	{	
		CachedQuadList m_vec;
		std::size_t num;
	};
	typedef std::map <base_window*, QuadCacheRecord> QuadCacheMap;
	QuadCacheMap m_mapQuadList;
	QuadCacheRecord* m_currentCapturing;

	filesystem_ptr m_filesystem;
};

}