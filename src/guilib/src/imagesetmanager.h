#pragma once

#include "rect.h"
#include "Size.h"
namespace xml
{
	class node;
};

namespace gui
{
	struct SubImage
	{
		size_t m_ordinal; // ordinal to texture in atlas
		Rect m_src; // area in atlas
		Rect m_crop; // for a cropped images, offset, and original size
		point m_offset; // offset in result image
	};

	class System;
	class Imageset1;
	class Texture;
	typedef boost::shared_ptr<Texture> TexturePtr;
	class Image1
	{
	public:
		typedef std::vector<SubImage> SubImages;
		/// @brief - Image with regions, if data.size() != 0
		/// @param parent - Ordinals in the sub-images will point to textures in this imageset
		/// @param sz - full image size
		/// @param data - sub-images
		Image1(Imageset1* parent, const Size& sz, SubImages& data);
		/// @brief - An empty image
		Image1();

		Size GetSize() const;
		Imageset1* GetParent() const;

	private:
		Size m_size;
		SubImages m_data;
		Imageset1* m_parent;
	};

	inline Size Image1::GetSize() const
	{
		return m_size;
	}

	inline Imageset1* Image1::GetParent() const
	{
		return m_parent;
	}

	/// sample XML:
	//<Imageset Name="CoolImageset!">
	//	<Textures>
	//		<Texture Name="UberTex" Filename="test.tga"/>
	//		<Texture Filename="test1.tga"/>
	//	</Textures>
	//	<Images>
	//		<Image>
	//			<Rect Texture="UberTex" SrcLeft="0" SrcTop="0" SrcRight="100" SrcBottom="100" XPos="0" YPos="0" />
	//			<Rect Texture="test1.tga" SrcLeft="0" SrcTop="0" SrcRight="100" SrcBottom="100" XPos="0" YPos="100"
	//				CropLeft="5" CropTop="5" OrigWidth="110" OrigHeight="110" />
	//		</Image>
	//	</Images>
	//</Imageset>

	class Imageset1
	{
	public:
		/// @brief - General way to create and load imageset
		/// @param sys - System reference(load texture, log)
		/// @param name - debug only name
		/// @param imgset - pointer to xml node named "imageset", optional
		Imageset1(System& sys, const std::string& name, xml::node* imgset = 0);

		/// @brief - New texture pushed back. No texture deletion support!
		/// @param tex - texture ptr
		/// @returns - ordinal to texture, for a SubImage
		size_t AppendTexture(TexturePtr tex);

		/// @brief - Suppose to be used in manual image creation
		/// @param name - image name, used for an access to image
		/// @param sz - full image size
		/// @param data - subimage data - texture coords, and size, output size() == 0 is success
		/// @returns - true if success
		bool DefineImage(const std::string& name, const Size& sz, Image1::SubImages& data);
		
		/// @brief - Used to load from loaded xml
		/// @param imgset - xml node named "imageset"
		/// @param name - debug only name
		/// @returns - true if success
		bool Load(xml::node* imgset, const std::string& name, System& sys);

	private:
		typedef std::vector<TexturePtr> Textures;
		Textures m_textures;
		typedef boost::unordered_map<std::string, Image1> Images;
		Images m_images;
	};

	typedef boost::shared_ptr<Imageset1> Imageset1Ptr;
	typedef boost::weak_ptr<Imageset1> Imageset1WeakPtr;

	/// @brief - producing and loading imagesets
	class ImagesetManager
	{
	public:
		/// @brief - Produce an empty imageset using name
		/// @param name - must be unique, otherwise returned previous
		/// @returns - ptr
		Imageset1Ptr MakeEmpty(System& sys, const std::string& name);
		/// @brief - Produse an imageset using loaded xml
		/// @param imgset - xml node named "imageset"
		Imageset1Ptr Make(System& sys, xml::node* imgset);
	private:
		Imageset1Ptr Produce(System& sys, const std::string& name, xml::node* imgset);

	private:
		typedef boost::unordered_map<std::string, Imageset1WeakPtr> ImagesetRegistry;
		ImagesetRegistry m_registry;
	};
}