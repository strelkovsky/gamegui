#include "stdafx.h"
#include "imagesetmanager.h"

#include "system.h"
#include "renderer.h"

namespace gui
{
	Image1::Image1(Imageset1* parent, const Size& sz, SubImages& data)
		: m_parent(parent)
		, m_size(sz)
	{
		m_data.swap(data);
	}

	Image1::Image1()
	{
	}

	Imageset1::Imageset1(System& sys, const std::string& name, xml::node* imgset)
	{
		Load(imgset, name, sys);
	}

	size_t Imageset1::AppendTexture(TexturePtr tex)
	{
		Textures::iterator it = std::find(m_textures.begin(), m_textures.end(), tex);
		if(it != m_textures.end())
			return it - m_textures.begin();
		m_textures.push_back(tex);
		return m_textures.size() - 1;
	}

	bool Imageset1::DefineImage(const std::string& name, const Size& sz, Image1::SubImages& data)
	{
		Images::iterator it = m_images.find(name);
		if(it != m_images.end())
			return false;
		// TODO: optimize multiple copy
		Image1 img(this, sz, data);
		m_images.insert(std::make_pair(name, img));
		return true;
	}

	bool Imageset1::Load(xml::node* imgset, const std::string& name, System& sys)
	{
		if(imgset)
		{
			xml::node texsnode = imgset->child("Textures");
			if(!texsnode) // TODO: log error
			{
				sys.logEvent(LogWarning, std::string("The imageset ")
					+ name + " doesn't have any texture information. Imageset is unaffected");
				return false;
			}

			xml::node imgsnode = imgset->child("Images");
			if(!imgsnode)
			{
				sys.logEvent(LogWarning, std::string("The imageset ")
					+ name + " doesn't have any image information. Imageset is unaffected");
				return false;
			}

			typedef boost::unordered_map<std::string, size_t> TextureOrdinals;
			TextureOrdinals textureOrdinals;

			xml::node texnode = texsnode.first_child();
			while(!texnode.empty())
			{
				std::string nodename(texnode.name());
				if(nodename == "Texture")
				{
					std::string texname = texnode["Name"].value();
					std::string filename = texnode["Imagefile"].value();
					if(!texname.size())
						texname = filename;
					TextureOrdinals::iterator it = textureOrdinals.find(texname);
					if(it == textureOrdinals.end())
					{
						TexturePtr tex = sys.getRenderer().createTexture("imageset\\" + filename);
						if(tex)
						{
							m_textures.push_back(tex);
							textureOrdinals.insert(std::make_pair(texname, m_textures.size() - 1));
						}
						else
						{
							sys.logEvent(LogWarning, std::string("The imageset ")
								+ name + " unable to load texture '" + texname + "', file '"
								+ filename + "'.");
						}
					}
				}
				texnode = texnode.next_sibling();
			}

			xml::node imgnode = imgsnode.first_child();
			while(!imgnode.empty())
			{
				std::string nodename(imgnode.name());
				if(nodename == "Image")
				{
					std::string imgname = imgnode["Name"].value();
					float width = imgnode["Width"].as_float();
					float height = imgnode["Height"].as_float();

					Image1::SubImages subImages;

					xml::node rectnode = imgsnode.first_child();
					while(!rectnode.empty())
					{
						if(std::string(rectnode.name()) == "Rect")
						{
							std::string texname = rectnode["Texture"].value();
							TextureOrdinals::iterator it = textureOrdinals.find(texname);
							if(it != textureOrdinals.end())
							{
								SubImage sub;
								sub.m_ordinal = it->second;

								float left = rectnode["SrcLeft"].as_float();
								float top = rectnode["SrcTop"].as_float();
								float right = rectnode["SrcRight"].as_float();
								float bottom = rectnode["SrcBottom"].as_float();

								sub.m_src = Rect(left, top, right, bottom);

								float y = rectnode["YPos"].as_float();
								float x = rectnode["XPos"].as_float();
								sub.m_offset = point(x, y);

								if(!rectnode["CropLeft"].empty())
								{
									float cropx = rectnode["CropLeft"].as_float();
									float cropy = rectnode["CropTop"].as_float();
									float w = rectnode["OrigWidth"].as_float();
									float h = rectnode["OrigHeight"].as_float();
									sub.m_crop = Rect(point(cropx, cropy), Size(w, h));
								}
								subImages.push_back(sub);
							}
							else
							{
								sys.logEvent(LogWarning, std::string("The imageset ")
									+ name + " can't find texture '" + texname + "' for the image '"
									+ imgname + "'.");
							}
						}
						rectnode = rectnode.next_sibling();
					}
					if(subImages.size())
					{
						// TODO: optimize multiple copy
						m_images.insert(std::make_pair(imgname, Image1(this, Size(width, height), subImages)));
					}
					else
					{
						sys.logEvent(LogWarning, std::string("The imageset ")
							+ name + " can't find any rectangles for the image '"
							+ imgname + "'. Skipping this image...");
					}
				}
				imgnode = imgnode.next_sibling();
			}

			return true;
		}
		return false;
	}


	Imageset1Ptr ImagesetManager::MakeEmpty(System& sys, const std::string& name)
	{
		return Produce(sys, name, 0);
	}

	Imageset1Ptr ImagesetManager::Make(System& sys, xml::node* imgset)
	{
		if(imgset)
		{
			std::string name = (*imgset)["Name"].value();
			return Produce(sys, name, imgset);
		}
		return Imageset1Ptr();
	}

	Imageset1Ptr ImagesetManager::Produce(System& sys, const std::string& name, xml::node* imgset)
	{
		Imageset1Ptr retval;
		if(name.size())
		{
			ImagesetRegistry::iterator it = m_registry.find(name);
			if(it == m_registry.end())
			{
				retval.reset(new Imageset1(sys, name, imgset));
				Imageset1WeakPtr weak = retval;
				m_registry.insert(std::make_pair(name, weak));
				return retval;
			}

			Imageset1WeakPtr& weak = it->second;
			if(retval = weak.lock())
				return retval;
			retval.reset(new Imageset1(sys, name, imgset));
			weak = retval;
		}
		return retval;
	}

}