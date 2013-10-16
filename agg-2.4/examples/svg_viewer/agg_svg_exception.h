//----------------------------------------------------------------------------
// Anti-Grain Geometry - Version 2.3
// Copyright (C) 2002-2005 Maxim Shemanarev (http://www.antigrain.com)
//
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//----------------------------------------------------------------------------
// Contact: mcseem@antigrain.com
//          mcseemagg@yahoo.com
//          http://www.antigrain.com
//----------------------------------------------------------------------------
//
// SVG exception
//
//----------------------------------------------------------------------------

#ifndef AGG_SVG_EXCEPTION_INCLUDED
#define AGG_SVG_EXCEPTION_INCLUDED

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "agg_basics.h"

namespace agg 
{ 
namespace svg
{
    class exception
    {
    public:
        ~exception()
        {
        }

        exception() : m_str_msg(_T("No error")) {}
          
		exception(const str_type::char_type* fmt, ...)
        {
            if(fmt) 
            {
                m_str_msg.resize(4096);
                va_list arg;
                va_start(arg, fmt);
				_vstprintf(&m_str_msg[0], fmt, arg);
                va_end(arg);
            }
			else
			{
				m_str_msg = _T("No error");
			}
        }

        exception(const exception& exc) 
			: m_str_msg(exc.m_str_msg)
        {
        }
        
        const str_type::char_type* msg() const { return m_str_msg.c_str(); }

    private:
        str_type::string_type	m_str_msg;
    };

}
}

#endif
