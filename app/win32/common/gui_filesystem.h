#include <rgde/log/util.h>
#include <rgde/core/math.h>
#include <rgde/core/windows.h>
#include <rgde/render/device.h>
#include <rgde/core/file_system.h>

using namespace gui;
using namespace rgde;


class gui_filesystem : public gui::filesystem
{
public:
	gui_filesystem(core::vfs::system& fs, const std::string& basedir) 
		: m_fs(fs), m_basedir(basedir) {		
	}

	~gui_filesystem() {
	}

	virtual std::string load_text(const std::string& filename) 	{
		std::string out;

		core::vfs::istream_ptr f = m_fs.open_read(m_basedir + filename);

		if (!f) return out;

		size_t size = f->get_size();

		if (size == 0) return out;

		out.resize(size);

		f->read((rgde::byte*)&out[0], size);

		return out;
	}

	virtual data_ptr load_binary(const std::string& filename) {
		static data zero_data = {0};

		data_ptr out(new data);
		*out = zero_data;

		char dir[256];
		GetCurrentDirectoryA(256, dir);

		core::vfs::istream_ptr f = m_fs.open_read(m_basedir + filename);

		if (!f) return out;

		size_t size = f->get_size();

		if (size == 0) return out;

		out->ptr = new char[size];
		out->size = size;

		f->read((rgde::byte*)out->ptr, out->size);

		return out;
	}

protected:
	core::vfs::system& m_fs;
	std::string m_basedir;
};