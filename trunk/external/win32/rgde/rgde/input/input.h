#pragma once

#include <rgde/input/base.h>
#include <rgde/core/windows_forward.h>

namespace rgde
{
namespace input
{
	class input_impl;

	// �������� ����� ������� �����
	class system : boost::noncopyable
	{
	public:
		system(core::windows::window_ptr window, bool exclusive=false, bool foreground=true);
		~system();

        // ������� ����� ������� �����
        bool set_mode (bool exclusive=false, bool foreground=true); //�������� ����� ������ ��������� �����
        void load_from_string (const std::string &xml_str);      //��������� ���������
        void load_from_file   (const std::string &file_name); //��������� ���������
        void update();                        //������� �� ������ ��� ������� �� ��������� �����
        void save(std::string &xml_str);       //��������� ���������

        // ������ � ����������� �����
        //�������� ����������
        device* get_device (types::EDevice device_type, int indx=0);
        device* get_device (const std::wstring &device_name, int indx=0);

        //���� �� ����� ����������
        bool is_device_present (types::EDevice device_type, int indx=0);
        bool is_device_present (const std::wstring &device_name, int indx=0);

		control* get_control(types::EDevice device, int dev_index, types::EControl control);
		control* get_control(types::EDevice device, types::EControl control)
		{
			return get_control(device, 0, control);
		}

        // ������ � �������� ������� �����
        void add_command       (const std::wstring &command_name); //�������� �������
        command_ptr  get_command       (const std::wstring &command_name); //�������� �������
        bool is_command_present (const std::wstring &command_name); //���� �� ����� �������
        void detach_command    (command_ptr command);                //�������� ������� ��� ���� ���������

	private:
		std::auto_ptr<input_impl> m_impl;      //���������� ���������� ������� �����
	};
}
}