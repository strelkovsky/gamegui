
#include "stdafx.h"
#include "image.h"
#include "renderer.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4355)
#endif

namespace gui
{



Renderer::Renderer(void) :
	GuiZInitialValue(1.0f),
	GuiZElementStep(0.001f),
	GuiZLayerStep(0.0001f),
	m_isQueueing(true),
	m_hdpi(96),
	m_vdpi(96),
	m_maxTextureSize(4096),
	m_originalsize(1.f, 1.f),
	m_texmanager(*this),
	m_autoScale(true),
	m_num_quads(0),
	m_num_batches(0),
	m_currentCapturing(0)
{
	m_quads.resize(10000);
	m_batches.resize(1000);
	resetZValue();
}

Renderer::~Renderer(void)
{
}

void Renderer::immediateDraw(const Image& img, const Rect& dest_rect, float z, const Rect& clip_rect,const ColorRect& colors)
{
	// get the rect area that we will actually draw to (i.e. perform clipping)
	Rect final_rect(dest_rect.getIntersection(clip_rect));

	// check if rect was totally clipped
	if (final_rect.getWidth() != 0)
	{
		const Rect& source_rect = img.pixel_rect;

        const float x_scale = 1.f;
        const float y_scale = 1.f;

		float tex_per_pix_x = source_rect.getWidth() / dest_rect.getWidth();
		float tex_per_pix_y = source_rect.getHeight() / dest_rect.getHeight();

		// calculate final, clipped, texture co-ordinates
		Rect  tex_rect((source_rect.m_left + ((final_rect.m_left - dest_rect.m_left) * tex_per_pix_x)) * x_scale,
			(source_rect.m_top + ((final_rect.m_top - dest_rect.m_top) * tex_per_pix_y)) * y_scale,
			(source_rect.m_right + ((final_rect.m_right - dest_rect.m_right) * tex_per_pix_x)) * x_scale,
			(source_rect.m_bottom + ((final_rect.m_bottom - dest_rect.m_bottom) * tex_per_pix_y)) * y_scale);

		tex_rect *= img.texture.getSize();

		// queue a quad to be rendered
		QuadInfo quad;
		fillQuad(quad, final_rect, tex_rect, z, img, colors);
		renderQuadDirect(quad);
	}
}

void Renderer::draw(const Image& img, const Rect& dest_rect, float z, const Rect& clip_rect,const ColorRect& colors, Image::ImageOps horz, Image::ImageOps vert)
{
	const Rect& source_rect = img.pixel_rect;
	Size imgSz = source_rect.getSize();
    
	unsigned int horzTiles = 1;
	unsigned int vertTiles = 1;

	float xpos = dest_rect.m_left;
	float ypos = dest_rect.m_top;

	switch (horz)
    {		
	case Image::Tile:
		if(dest_rect.getWidth() <= 0.f)
			return;
		horzTiles = (unsigned int)((dest_rect.getWidth() + (imgSz.width - 1)) / imgSz.width);
		break;
	case Image::Stretch:
	default:
		imgSz.width = dest_rect.getWidth();
		break;
	}

	switch (vert)
    {
	case Image::Tile:
		if(dest_rect.getHeight() <= 0.f)
			return;
		vertTiles = (unsigned int)((dest_rect.getHeight() + (imgSz.height - 1)) / imgSz.height);
		break;
	case Image::Stretch:
	default:
		imgSz.height = dest_rect.getHeight();
		break;
	}

	// perform final rendering (actually is now a caching of the images which will be drawn)
    Rect finalRect;
    Rect finalClipper(clip_rect.getIntersection(dest_rect));
    finalRect.m_top = ypos;
    finalRect.m_bottom = ypos + imgSz.height;

    for (unsigned int row = 0; row < vertTiles; ++row)
    {
        finalRect.m_left = xpos;
        finalRect.m_right = xpos + imgSz.width;

        for (unsigned int col = 0; col < horzTiles; ++col)
        {
            // add image to the rendering cache for the target window.
            draw(img, finalRect, z, finalClipper, colors/*,  quad_split_mode*/);

            finalRect.m_left += imgSz.width;
            finalRect.m_right += imgSz.width;
        }

        finalRect.m_top += imgSz.height;
        finalRect.m_bottom += imgSz.height;
    }
}

void Renderer::drawLine(const Image& img, const vec2* p, size_t size, float z, const Rect& clip_rect, const Color& color, float width)
{
	ColorRect color_rect(color);

	vec2 p0, p1, p2, p3;

	Rect source_rect = img.pixel_rect;
	source_rect *= img.texture.getSize();

	for(size_t i = 0; i < size - 1; ++i)
	{
		const vec2& v1 = p[i];
		const vec2& v2 = p[i+1];

		vec2 dir = make_normal(v2-v1);

		vec2 nv(-dir.y, dir.x);
		nv *= width*0.5f;

		p0 = v1 + nv;
		p1 = v2 + nv;

		p2 = v1 - nv;
		p3 = v2 - nv;

		addQuad(p0, p1, p2, p3, source_rect, z, img, color_rect);
	}
}

void Renderer::draw(const Image& img, const Rect& dest_rect, float z, const Rect& clip_rect,const ColorRect& colors)
{
	// get the rect area that we will actually draw to (i.e. perform clipping)
	Rect final_rect(dest_rect.getIntersection(clip_rect));

	// check if rect was totally clipped
	if (final_rect.getWidth() != 0)
	{
		const Rect& source_rect = img.pixel_rect;

		float tex_per_pix_x = source_rect.getWidth() / dest_rect.getWidth();
		float tex_per_pix_y = source_rect.getHeight() / dest_rect.getHeight();

		// calculate final, clipped, texture co-ordinates
		Rect  tex_rect(
			(source_rect.m_left + ((final_rect.m_left - dest_rect.m_left) * tex_per_pix_x)),
			(source_rect.m_top + ((final_rect.m_top - dest_rect.m_top) * tex_per_pix_y)),
			(source_rect.m_right + ((final_rect.m_right - dest_rect.m_right) * tex_per_pix_x)),
			(source_rect.m_bottom + ((final_rect.m_bottom - dest_rect.m_bottom) * tex_per_pix_y))
			);
		
		tex_rect *= img.texture.getSize();

		// queue a quad to be rendered
		addQuad(final_rect, tex_rect, z, img, colors);
	}

}
void Renderer::clearCache(BaseWindow* window)
{
	if (window)
	{
		QuadCacheMap::iterator i = m_mapQuadList.find(window);
		if (i != m_mapQuadList.end())
			i->second.num = 0;//erase(window);
		//m_mapQuadList[window].m_vec.resize(0);
	}
	else
		m_mapQuadList.clear();
}

bool Renderer::isExistInCache(BaseWindow* window) const
{
	QuadCacheMap::const_iterator i = m_mapQuadList.find(window);
	return i != m_mapQuadList.end() && i->second.num > 0;
}

void Renderer::startCaptureForCache(BaseWindow* window)
{
	QuadCacheMap::iterator i = m_mapQuadList.find(window);
	if (i == m_mapQuadList.end())
	{
		QuadCacheRecord& v = m_mapQuadList[window];
		v.m_vec.resize(100);
		v.num = 0;
		m_currentCapturing = &v;
	}
	else
	{		
		m_currentCapturing = &(i->second);
		m_currentCapturing->num = 0;
	}
		
}
void Renderer::endCaptureForCache(BaseWindow* window)
{
	m_currentCapturing = NULL;
}


void Renderer::clearRenderList(void)
{
	//m_quadlist.clear();
	m_num_quads = 0;
	m_num_batches = 0;
}

void Renderer::beginBatching()
{
	clearRenderList();
}

void Renderer::endBatching()
{ 
	// �������� ��������� ����
	if (!m_num_batches) return;
	m_batches[m_num_batches - 1].numQuads = m_num_quads - m_batches[m_num_batches - 1].startQuad;
}


void Renderer::sortQuads(void)
{
}

void Renderer::OnLostDevice(void)
{
	m_texmanager.onDeviceLost();
}

void Renderer::OnResetDevice(void)
{
	m_texmanager.onDeviceReset();
}

const Size Renderer::getSize(void)
{
	if(m_autoScale)
		return getOriginalSize();
	else
		return getViewportSize();
}

void Renderer::cleanup(bool complete)
{
	m_num_quads = 0;
	m_num_batches = 0;
	clearCache();
	if(complete)
	{
		m_texmanager.cleanup();
	}	
}

void Renderer::computeVirtualDivRealFactor(Size& coefOut) const
{
	if (!m_autoScale)
	{
		coefOut.width = 1.0f;
		coefOut.height = 1.0f;
		return;
	}
	const Size& original = getOriginalSize(); 
	Size& current = getViewportSize();
	coefOut = current/original;	
}
Rect Renderer::virtualToRealCoord( const Rect& virtualRect ) const
{
	Rect result = virtualRect;
	Size k;
	computeVirtualDivRealFactor(k);
	result.m_left *= k.width;
	result.m_right *= k.width;
	result.m_top *= k.height;
	result.m_bottom *= k.height;
	return result;
}

Rect Renderer::realToVirtualCoord( const Rect& realRect ) const
{
	Size k;
	computeVirtualDivRealFactor(k);
	Rect result = realRect;
	result.m_left /= k.width;
	result.m_right /= k.width;
	result.m_top /= k.height;
	result.m_bottom /= k.height;
	return result;
}

void Renderer::fillQuad(QuadInfo& quad, const Rect& rc, const Rect& uv, float z, const Image& img, const ColorRect& colors)
{
	quad.positions[0].x	= quad.positions[2].x = rc.m_left;
	quad.positions[0].y	= quad.positions[1].y = rc.m_top;
	quad.positions[1].x	= quad.positions[3].x = rc.m_right;
	quad.positions[2].y	= quad.positions[3].y = rc.m_bottom;

	quad.z				= z;
	quad.texture		= &img.texture;
	quad.texPosition	= uv;
	quad.topLeftCol		= colors.m_top_left.getARGB();
	quad.topRightCol	= colors.m_top_right.getARGB();
	quad.bottomLeftCol	= colors.m_bottom_left.getARGB();
	quad.bottomRightCol	= colors.m_bottom_right.getARGB();
}
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif
