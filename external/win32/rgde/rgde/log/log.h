#pragma once

#include <rgde/log/device.h>

namespace rgde
{
    namespace log
    {
        typedef boost::iostreams::stream<device::composite_dev<char> >    log_stream;
        typedef boost::iostreams::stream<device::composite_dev<wchar_t> > wlog_stream;

        //multibyte
        extern log_stream lmsg;
        extern log_stream lwrn;
        extern log_stream lerr;

        //unicode
        extern wlog_stream wlmsg;
        extern wlog_stream wlwrn;
        extern wlog_stream wlerr;

        //����� ��� ���������� ������ � �����:
        //  ���� ���������� ���� ���� ��������� ����� ������,
        //  ����������� ��������������� ����������� ������� (w)cout � (w)cerr � ���
        class logsystem
        {
        public:
            logsystem();
           ~logsystem();

        private:
            //������������� ������������� ������ ������ ���������� ����� ������
            class loghelper;

            static int counter;
            static loghelper *ploghelper;

        private:
            logsystem (const logsystem&);
            logsystem& operator= (const logsystem&);
        };
    }
}