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
// SVG parser.
//
//----------------------------------------------------------------------------

#include "agg_config.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <fstream>

#include "agg_svg_parser.h"
#include "agg_rounded_rect.h"
#include "expat.h"
#include "unkenc.h"

namespace agg
{
namespace svg
{
	namespace
	{
    struct named_color
    {
        str_type::char_type  name[22];
        int8u r, g, b, a;
    };

    named_color colors[] = 
    {
        { _T("aliceblue"),240,248,255, 255 },
        { _T("antiquewhite"),250,235,215, 255 },
        { _T("aqua"),0,255,255, 255 },
        { _T("aquamarine"),127,255,212, 255 },
        { _T("azure"),240,255,255, 255 },
        { _T("beige"),245,245,220, 255 },
        { _T("bisque"),255,228,196, 255 },
        { _T("black"),0,0,0, 255 },
        { _T("blanchedalmond"),255,235,205, 255 },
        { _T("blue"),0,0,255, 255 },
        { _T("blueviolet"),138,43,226, 255 },
        { _T("brown"),165,42,42, 255 },
        { _T("burlywood"),222,184,135, 255 },
        { _T("cadetblue"),95,158,160, 255 },
        { _T("chartreuse"),127,255,0, 255 },
        { _T("chocolate"),210,105,30, 255 },
        { _T("coral"),255,127,80, 255 },
        { _T("cornflowerblue"),100,149,237, 255 },
        { _T("cornsilk"),255,248,220, 255 },
        { _T("crimson"),220,20,60, 255 },
        { _T("cyan"),0,255,255, 255 },
        { _T("darkblue"),0,0,139, 255 },
        { _T("darkcyan"),0,139,139, 255 },
        { _T("darkgoldenrod"),184,134,11, 255 },
        { _T("darkgray"),169,169,169, 255 },
        { _T("darkgreen"),0,100,0, 255 },
        { _T("darkgrey"),169,169,169, 255 },
        { _T("darkkhaki"),189,183,107, 255 },
        { _T("darkmagenta"),139,0,139, 255 },
        { _T("darkolivegreen"),85,107,47, 255 },
        { _T("darkorange"),255,140,0, 255 },
        { _T("darkorchid"),153,50,204, 255 },
        { _T("darkred"),139,0,0, 255 },
        { _T("darksalmon"),233,150,122, 255 },
        { _T("darkseagreen"),143,188,143, 255 },
        { _T("darkslateblue"),72,61,139, 255 },
        { _T("darkslategray"),47,79,79, 255 },
        { _T("darkslategrey"),47,79,79, 255 },
        { _T("darkturquoise"),0,206,209, 255 },
        { _T("darkviolet"),148,0,211, 255 },
        { _T("deeppink"),255,20,147, 255 },
        { _T("deepskyblue"),0,191,255, 255 },
        { _T("dimgray"),105,105,105, 255 },
        { _T("dimgrey"),105,105,105, 255 },
        { _T("dodgerblue"),30,144,255, 255 },
        { _T("firebrick"),178,34,34, 255 },
        { _T("floralwhite"),255,250,240, 255 },
        { _T("forestgreen"),34,139,34, 255 },
        { _T("fuchsia"),255,0,255, 255 },
        { _T("gainsboro"),220,220,220, 255 },
        { _T("ghostwhite"),248,248,255, 255 },
        { _T("gold"),255,215,0, 255 },
        { _T("goldenrod"),218,165,32, 255 },
        { _T("gray"),128,128,128, 255 },
        { _T("green"),0,128,0, 255 },
        { _T("greenyellow"),173,255,47, 255 },
        { _T("grey"),128,128,128, 255 },
        { _T("honeydew"),240,255,240, 255 },
        { _T("hotpink"),255,105,180, 255 },
        { _T("indianred"),205,92,92, 255 },
        { _T("indigo"),75,0,130, 255 },
        { _T("ivory"),255,255,240, 255 },
        { _T("khaki"),240,230,140, 255 },
        { _T("lavender"),230,230,250, 255 },
        { _T("lavenderblush"),255,240,245, 255 },
        { _T("lawngreen"),124,252,0, 255 },
        { _T("lemonchiffon"),255,250,205, 255 },
        { _T("lightblue"),173,216,230, 255 },
        { _T("lightcoral"),240,128,128, 255 },
        { _T("lightcyan"),224,255,255, 255 },
        { _T("lightgoldenrodyellow"),250,250,210, 255 },
        { _T("lightgray"),211,211,211, 255 },
        { _T("lightgreen"),144,238,144, 255 },
        { _T("lightgrey"),211,211,211, 255 },
        { _T("lightpink"),255,182,193, 255 },
        { _T("lightsalmon"),255,160,122, 255 },
        { _T("lightseagreen"),32,178,170, 255 },
        { _T("lightskyblue"),135,206,250, 255 },
        { _T("lightslategray"),119,136,153, 255 },
        { _T("lightslategrey"),119,136,153, 255 },
        { _T("lightsteelblue"),176,196,222, 255 },
        { _T("lightyellow"),255,255,224, 255 },
        { _T("lime"),0,255,0, 255 },
        { _T("limegreen"),50,205,50, 255 },
        { _T("linen"),250,240,230, 255 },
        { _T("magenta"),255,0,255, 255 },
        { _T("maroon"),128,0,0, 255 },
        { _T("mediumaquamarine"),102,205,170, 255 },
        { _T("mediumblue"),0,0,205, 255 },
        { _T("mediumorchid"),186,85,211, 255 },
        { _T("mediumpurple"),147,112,219, 255 },
        { _T("mediumseagreen"),60,179,113, 255 },
        { _T("mediumslateblue"),123,104,238, 255 },
        { _T("mediumspringgreen"),0,250,154, 255 },
        { _T("mediumturquoise"),72,209,204, 255 },
        { _T("mediumvioletred"),199,21,133, 255 },
        { _T("midnightblue"),25,25,112, 255 },
        { _T("mintcream"),245,255,250, 255 },
        { _T("mistyrose"),255,228,225, 255 },
        { _T("moccasin"),255,228,181, 255 },
        { _T("navajowhite"),255,222,173, 255 },
        { _T("navy"),0,0,128, 255 },
        { _T("oldlace"),253,245,230, 255 },
        { _T("olive"),128,128,0, 255 },
        { _T("olivedrab"),107,142,35, 255 },
        { _T("orange"),255,165,0, 255 },
        { _T("orangered"),255,69,0, 255 },
        { _T("orchid"),218,112,214, 255 },
        { _T("palegoldenrod"),238,232,170, 255 },
        { _T("palegreen"),152,251,152, 255 },
        { _T("paleturquoise"),175,238,238, 255 },
        { _T("palevioletred"),219,112,147, 255 },
        { _T("papayawhip"),255,239,213, 255 },
        { _T("peachpuff"),255,218,185, 255 },
        { _T("peru"),205,133,63, 255 },
        { _T("pink"),255,192,203, 255 },
        { _T("plum"),221,160,221, 255 },
        { _T("powderblue"),176,224,230, 255 },
        { _T("purple"),128,0,128, 255 },
        { _T("red"),255,0,0, 255 },
        { _T("rosybrown"),188,143,143, 255 },
        { _T("royalblue"),65,105,225, 255 },
        { _T("saddlebrown"),139,69,19, 255 },
        { _T("salmon"),250,128,114, 255 },
        { _T("sandybrown"),244,164,96, 255 },
        { _T("seagreen"),46,139,87, 255 },
        { _T("seashell"),255,245,238, 255 },
        { _T("sienna"),160,82,45, 255 },
        { _T("silver"),192,192,192, 255 },
        { _T("skyblue"),135,206,235, 255 },
        { _T("slateblue"),106,90,205, 255 },
        { _T("slategray"),112,128,144, 255 },
        { _T("slategrey"),112,128,144, 255 },
        { _T("snow"),255,250,250, 255 },
        { _T("springgreen"),0,255,127, 255 },
        { _T("steelblue"),70,130,180, 255 },
        { _T("tan"),210,180,140, 255 },
        { _T("teal"),0,128,128, 255 },
        { _T("thistle"),216,191,216, 255 },
        { _T("tomato"),255,99,71, 255 },
        { _T("turquoise"),64,224,208, 255 },
        { _T("violet"),238,130,238, 255 },
        { _T("wheat"),245,222,179, 255 },
        { _T("white"),255,255,255, 255 },
        { _T("whitesmoke"),245,245,245, 255 },
        { _T("yellow"),255,255,0, 255 },
        { _T("yellowgreen"),154,205,50, 255 },
        { _T("zzzzzzzzzzz"),0,0,0, 0 }
    }; 
}

    //------------------------------------------------------------------------

	utf_convertor::utf_convertor()
	{
		m_buffer.resize(1024);
		m_wideBuffer.resize(1024);
	}

	utf_convertor::adapter utf_convertor::convert(const char_type * utf_str, size_t utf_len /*= 0*/)
	{
#ifndef _UNICODE 
		if( !utf_len )
			utf_len = _tcslen(utf_str);

		int wbuflen = MultiByteToWideChar(CP_UTF8, 0, utf_str, utf_len, 0, 0);
		m_wideBuffer.resize(wbuflen);
		MultiByteToWideChar(CP_UTF8, 0, utf_str, utf_len, &m_wideBuffer[0], m_wideBuffer.size());

		int buflen = WideCharToMultiByte(1251, 0, &m_wideBuffer[0], m_wideBuffer.size(), 0, 0, 0, 0);
		m_buffer.resize(buflen);
		WideCharToMultiByte(1251, 0, &m_wideBuffer[0], m_wideBuffer.size(), &m_buffer[0], m_buffer.size(), 0, 0);
		return adapter(m_buffer.c_str(), m_buffer.size());
#else
		return adapter(utf_str, utf_len); // utf-16 string almoust UCS2, pass without convertion
#endif
	}

	//------------------------------------------------------------------------
    parser::~parser()
    {
    }

    //------------------------------------------------------------------------
    parser::parser(path_renderer& path)
		: m_path(path)
        , m_tokenizer()
        , m_title_flag(false)
        , m_path_flag(false)
		, m_parser_text(m_path)
		, m_str_title(_T(""))
    {
    }

	//------------------------------------------------------------------------
	void parser::clear()
	{
		m_str_title.clear();
		m_str_attr_name.clear();
		m_str_attr_value.clear();
		m_parser_text.clear_attr();
	}

	//------------------------------------------------------------------------
	void parser::parse(const char* szSVG, size_t strLen)
	{
		clear();
		XML_Parser p = XML_ParserCreate(NULL);
		if(p == 0) 
			throw exception(_T("Couldn't allocate memory for parser"));

		XML_SetUserData(p, this);
		XML_SetElementHandler(p, start_element, end_element);
		XML_SetCharacterDataHandler(p, content);
		XML_SetUnknownEncodingHandler(p, unknownEncoding, NULL);

		
		if(!XML_Parse(p, szSVG, strLen, true))
			throw exception(_T("%s at line %d\n"), XML_ErrorString(XML_GetErrorCode(p)), XML_GetCurrentLineNumber(p));
		XML_ParserFree(p);
	}

    //------------------------------------------------------------------------
    void parser::parse(const char_type* fname)
    {
		clear();
	    XML_Parser p = XML_ParserCreate(NULL);
	    if(p == 0) 
	    {
		    throw exception(_T("Couldn't allocate memory for parser"));
	    }

        XML_SetUserData(p, this);
	    XML_SetElementHandler(p, start_element, end_element);
	    XML_SetCharacterDataHandler(p, content);
		XML_SetUnknownEncodingHandler(p, unknownEncoding, NULL);

		char buffer[buf_size];
		std::ifstream infile(fname, std::ios::binary);
        if(!infile)
		    throw exception(_T("Couldn't open file %s"), fname);

        bool done = false;
        do
        {
			infile.read(buffer, buf_size);
			size_t len = infile.gcount();
            done = len < buf_size;
            if(!XML_Parse(p, buffer, len, done))
                throw exception(_T("%s at line %d\n"), XML_ErrorString(XML_GetErrorCode(p)), XML_GetCurrentLineNumber(p));

        } while(!done);
        XML_ParserFree(p);

        char_type * ts = &m_str_title[0];
        while(*ts)
        {
            if(*ts < ' ') *ts = ' ';
            ++ts;
        }
    }


    //------------------------------------------------------------------------
    void parser::start_element(void* data, const char_type* el, const char_type** attr)
    {
        parser& self = *(parser*)data;

        if(_tcscmp(el, _T("title")) == 0)
        {
            self.m_title_flag = true;
        }
        else if(_tcscmp(el, _T("g")) == 0)
        {
            self.m_path.push_attr();
            self.parse_attr(attr);
        }
        else if(_tcscmp(el, _T("path")) == 0)
        {
            if(self.m_path_flag)
                throw exception(_T("start_element: Nested path"));

            self.m_path.begin_path();
            self.parse_path(attr);
            self.m_path.end_path();
            self.m_path_flag = true;
        }
        else if(_tcscmp(el, _T("rect")) == 0) 
        {
            self.parse_rect(attr);
        }
        else if(_tcscmp(el, _T("line")) == 0) 
        {
            self.parse_line(attr);
        }
        else if(_tcscmp(el, _T("polyline")) == 0) 
        {
            self.parse_poly(attr, false);
        }
        else if(_tcscmp(el, _T("polygon")) == 0) 
        {
            self.parse_poly(attr, true);
        }
		else if(_tcscmp(el, _T("circle")) == 0)
		{
			self.parse_circle(attr);
		}
		else if(_tcscmp(el, _T("ellipse")) == 0)
		{
			self.parse_ellipse(attr);
		}
		else if(_tcscmp(el, _T("text")) == 0)
		{
			self.parse_text_start(attr);
		}
        //else
        //if(_tcscmp(el, _T("<OTHER_ELEMENTS>")) == 0) 
        //{
        //}
        // . . .
    } 


    //------------------------------------------------------------------------
    void parser::end_element(void* data, const char_type* el)
    {
        parser& self = *(parser*)data;

        if(_tcscmp(el, _T("title")) == 0)
        {
            self.m_title_flag = false;
        }
        else
        if(_tcscmp(el, _T("g")) == 0)
        {
            self.m_path.pop_attr();
        }
        else
        if(_tcscmp(el, _T("path")) == 0)
        {
            self.m_path_flag = false;
        }
		else 
		if(_tcscmp(el, _T("text")) == 0)
		{
			self.parse_text_end();
		}
        //else
        //if(_tcscmp(el, _T("<OTHER_ELEMENTS>")) == 0) 
        //{
        //}
        // . . .
    }


    //------------------------------------------------------------------------
    void parser::content(void* data, const char_type* utf_str, int utf_len)
    {
        parser& self = *(parser*)data;
		utf_convertor::adapter str = self.m_utf_convertor.convert(utf_str, utf_len);

        // m_title_flag signals that the <title> tag is being parsed now.
        // The following code concatenates the pieces of content of the <title> tag.
        if(self.m_title_flag)
        {
			self.m_str_title.assign(str.first, str.first + str.second);
        }
		else if(self.m_parser_text.is_text_mode())
		{
			self.parse_text_content(str.first, str.second);
		}
    }


    //------------------------------------------------------------------------
    void parser::parse_attr(const char_type** attr)
    {
        int i;
        for(i = 0; attr[i]; i += 2)
        {
            if(_tcscmp(attr[i], _T("style")) == 0)
            {
                parse_style(attr[i + 1]);
            }
            else
            {
                parse_attr(attr[i], attr[i + 1]);
            }
        }
    }

    //-------------------------------------------------------------
    void parser::parse_path(const char_type** attr)
    {
        int i;

        for(i = 0; attr[i]; i += 2)
        {
            // The <path> tag can consist of the path itself (_T("d=")) 
            // as well as of other parameters like "style=", "transform=", etc.
            // In the last case we simply rely on the function of parsing 
            // attributes (see 'else' branch).
            if(_tcscmp(attr[i], _T("d")) == 0)
            {
                m_tokenizer.set_path_str(attr[i + 1]);
                m_path.parse_path(m_tokenizer);
            }
            else
            {
                // Create a temporary single pair "name-value" in order
                // to avoid multiple calls for the same attribute.
                const char_type* tmp[4];
                tmp[0] = attr[i];
                tmp[1] = attr[i + 1];
                tmp[2] = 0;
                tmp[3] = 0;
                parse_attr(tmp);
            }
        }
    }


    //-------------------------------------------------------------
    int cmp_color(const void* p1, const void* p2)
    {
        return _tcscmp(((named_color*)p1)->name, ((named_color*)p2)->name);
    }

	double parse_double(const str_type::char_type* str)
	{
		static const str_type::char_type * const nums = _T(".,+-01223456789");

		str = _tcspbrk(str, nums);
		if(!str)
			return 0.0;

		
		bool bNegativ = (*str == _T('-'))? true: false;
		if( *str == _T('+') || *str == _T('-') )
			str++;

		double dH = 0.0, dL = 0.0;
		for( ; *str && _tcschr(nums+4, *str); str++ )
		{
			dH *= 10;
			dH += (*str) - _T('0');
		}
		if( *str == _T('.') || *str == _T(',') )
			str++;

		for( double d = 1.0; *str && _tcschr(nums+4, *str); str++ )
		{
			d *= 0.1;
			dL += ((*str) - _T('0')) * d;
		}
		dH += dL;
		return bNegativ? -dH: dH;
	}

//  dot or comma locale problem
//  =====================================
// 	double parse_double1(const str_type::char_type* str)
// 	{
// 	    while(*str == _T(' ')) ++str;
// 		return _tstof(str); 
// 	}

    //-------------------------------------------------------------
    rgba8 parse_color(const str_type::char_type* str)
    {
        while(*str == _T(' ')) ++str;
        unsigned c = 0;
        if(*str == _T('#'))
        {
            _stscanf(str + 1, _T("%x"), &c);
            return rgb8_packed(c);
        }
		else if(_tcsncmp(str, _T("rgb("), 4) == 0)
		{
			int r, g, b;
			if(3 != _stscanf(str, _T("rgb(%d,%d,%d)"), &r, &g, &b))
				throw exception(_T("parse_color: false on '%s'"), str);

			return rgba8(r, g, b, 255);
		}
		else if(_tcsncmp(str, _T("rgba("), 5) == 0)
		{
			int r, g, b;
			str_type::char_type bufOp[32];

			if(4 != _stscanf(str, _T("rgba(%d,%d,%d, %s)"), &r, &g, &b, bufOp)) // failed with %f if locale settings changed
				throw exception(_T("parse_color: false on '%s'"), str);

			float a = static_cast<float>(parse_double(bufOp));
			a = min( 1.0f, max(0.0f, a));
			return rgba8(r, g, b, (int8u)(255 * a));
		}
        else
        {
            named_color c;
			unsigned len = _tcslen(str);
            if(len > sizeof(c.name) - 1)
                throw exception(_T("parse_color: Invalid color name '%s'"), str);

			_tcscpy(c.name, str);
            const void* p = bsearch(&c, colors, sizeof(colors) / sizeof(colors[0]), sizeof(colors[0]), cmp_color);
            if(p == 0)
                throw exception(_T("parse_color: Invalid color name '%s'"), str);

            const named_color* pc = (const named_color*)p;
            return rgba8(pc->r, pc->g, pc->b, pc->a);
        }
    }

    //-------------------------------------------------------------
    bool parser::parse_attr(const char_type* name, const char_type* value)
    {
        if(_tcscmp(name, _T("style")) == 0)
        {
            parse_style(value);
        }
        else
        if(_tcscmp(name, _T("fill")) == 0)
        {
            if(_tcscmp(value, _T("none")) == 0)
            {
                m_path.fill_none();
            }
            else
            {
                m_path.fill(parse_color(value));
            }
        }
        else
        if(_tcscmp(name, _T("fill-opacity")) == 0)
        {
            m_path.fill_opacity(parse_double(value));
        }
        else
        if(_tcscmp(name, _T("stroke")) == 0)
        {
            if(_tcscmp(value, _T("none")) == 0)
            {
                m_path.stroke_none();
            }
            else
            {
                m_path.stroke(parse_color(value));
            }
        }
        else
        if(_tcscmp(name, _T("stroke-width")) == 0)
        {
            m_path.stroke_width(parse_double(value));
        }
        else
        if(_tcscmp(name, _T("stroke-linecap")) == 0)
        {
            if(_tcscmp(value, _T("butt")) == 0)        m_path.line_cap(butt_cap);
            else if(_tcscmp(value, _T("round")) == 0)  m_path.line_cap(round_cap);
            else if(_tcscmp(value, _T("square")) == 0) m_path.line_cap(square_cap);
        }
        else
        if(_tcscmp(name, _T("stroke-linejoin")) == 0)
        {
            if(_tcscmp(value, _T("miter")) == 0)      m_path.line_join(miter_join);
            else if(_tcscmp(value, _T("round")) == 0) m_path.line_join(round_join);
            else if(_tcscmp(value, _T("bevel")) == 0) m_path.line_join(bevel_join);
        }
        else
        if(_tcscmp(name, _T("stroke-miterlimit")) == 0)
        {
            m_path.miter_limit(parse_double(value));
        }
		else
		if(_tcscmp(name, _T("stroke-dasharray")) == 0)
		{
			parse_stroke_dasharray(value);
		}
        else
        if(_tcscmp(name, _T("stroke-opacity")) == 0)
        {
            m_path.stroke_opacity(parse_double(value));
        }
        else
        if(_tcscmp(name, _T("transform")) == 0)
        {
            parse_transform(value);
        }
        //else
        //if(_tcscmp(el, _T("<OTHER_ATTRIBUTES>")) == 0) 
        //{
        //}
        // . . .
        else
        {
            return false;
        }
        return true;
    }

    //-------------------------------------------------------------
    bool parser::parse_name_value(const char_type* nv_start, const char_type* nv_end)
    {
        const char_type* str = nv_start;
        while(str < nv_end && *str != _T(':')) 
			++str;

        const char_type* val = str;

        // Right Trim
        while(str > nv_start && (*str == _T(':') || isspace(*str))) 
			--str;
		++str;
		m_str_attr_name.assign(nv_start, str);

        while(val < nv_end && (*val == _T(':') || isspace(*val))) ++val;
		m_str_attr_value.assign(val, nv_end);

        return parse_attr(m_str_attr_name.c_str(), m_str_attr_value.c_str());
    }

    //-------------------------------------------------------------
    void parser::parse_style(const char_type* str)
    {
        while(*str)
        {
            // Left Trim
            while(*str && isspace(*str)) ++str;
            const char_type* nv_start = str;
            while(*str && *str != _T(';')) ++str;
            const char_type* nv_end = str;

            // Right Trim
            while(nv_end > nv_start && 
                (*nv_end == _T(';') || isspace(*nv_end))) --nv_end;
            ++nv_end;

            parse_name_value(nv_start, nv_end);
            if(*str) ++str;
        }

    }


    //-------------------------------------------------------------
    void parser::parse_rect(const char_type** attr)
    {
        int i;
        double x = 0.0;
        double y = 0.0;
        double w = 0.0;
        double h = 0.0;
		double rx = 0.0;
		double ry = 0.0;

        m_path.begin_path();
        for(i = 0; attr[i]; i += 2)
        {
            if(!parse_attr(attr[i], attr[i + 1]))
            {
                if(_tcscmp(attr[i], _T("x")) == 0)      x = parse_double(attr[i + 1]);
                if(_tcscmp(attr[i], _T("y")) == 0)      y = parse_double(attr[i + 1]);
                if(_tcscmp(attr[i], _T("width")) == 0)  w = parse_double(attr[i + 1]);
                if(_tcscmp(attr[i], _T("height")) == 0) h = parse_double(attr[i + 1]);
				if(_tcscmp(attr[i], _T("rx")) == 0)    rx = parse_double(attr[i + 1]);
				if(_tcscmp(attr[i], _T("ry")) == 0)    ry = parse_double(attr[i + 1]);
            }
        }

        if(w != 0.0 && h != 0.0)
        {
			if(w < 0.0) { x += w; w = -w; }
			if(h < 0.0) { y += h; h = -h; }

			if(rx == 0.0 && ry == 0.0)
			{
				m_path.move_to(x,     y);
				m_path.line_to(x + w, y);
				m_path.line_to(x + w, y + h);
				m_path.line_to(x,     y + h);
				m_path.close_subpath();
			}
			else
			{
				rounded_rect rr(x, y, x+w, y+h, 0);
				rr.radius(rx, ry);
				rr.normalize_radius();
				m_path.add_path(rr);
			}
        }
        m_path.end_path();
    }


    //-------------------------------------------------------------
    void parser::parse_line(const char_type** attr)
    {
        int i;
        double x1 = 0.0;
        double y1 = 0.0;
        double x2 = 0.0;
        double y2 = 0.0;

        m_path.begin_path();
        for(i = 0; attr[i]; i += 2)
        {
            if(!parse_attr(attr[i], attr[i + 1]))
            {
                if(_tcscmp(attr[i], _T("x1")) == 0) x1 = parse_double(attr[i + 1]);
                if(_tcscmp(attr[i], _T("y1")) == 0) y1 = parse_double(attr[i + 1]);
                if(_tcscmp(attr[i], _T("x2")) == 0) x2 = parse_double(attr[i + 1]);
                if(_tcscmp(attr[i], _T("y2")) == 0) y2 = parse_double(attr[i + 1]);
            }
        }

        m_path.move_to(x1, y1);
        m_path.line_to(x2, y2);
        m_path.end_path();
    }


    //-------------------------------------------------------------
    void parser::parse_poly(const char_type** attr, bool close_flag)
    {
        int i;
        double x = 0.0;
        double y = 0.0;

        m_path.begin_path();
        for(i = 0; attr[i]; i += 2)
        {
            if(!parse_attr(attr[i], attr[i + 1]))
            {
                if(_tcscmp(attr[i], _T("points")) == 0) 
                {
                    m_tokenizer.set_path_str(attr[i + 1]);
                    if(!m_tokenizer.next())
                    {
                        throw exception(_T("parse_poly: Too few coordinates"));
                    }
                    x = m_tokenizer.last_number();
                    if(!m_tokenizer.next())
                    {
                        throw exception(_T("parse_poly: Too few coordinates"));
                    }
                    y = m_tokenizer.last_number();
                    m_path.move_to(x, y);
                    while(m_tokenizer.next())
                    {
                        x = m_tokenizer.last_number();
                        if(!m_tokenizer.next())
                        {
                            throw exception(_T("parse_poly: Odd number of coordinates"));
                        }
                        y = m_tokenizer.last_number();
                        m_path.line_to(x, y);
                    }
                }
            }
        }
        if(close_flag) 
        {
            m_path.close_subpath();
        }
        m_path.end_path();
    }


	//-------------------------------------------------------------
	void parser::parse_circle(const char_type** attr)
	{
		int i;
		double cx = 0.0;
		double cy = 0.0;
		double r = 0.0;

		m_path.begin_path();
		for(i = 0; attr[i]; i += 2)
		{
			if(!parse_attr(attr[i], attr[i + 1]))
			{
				if(_tcscmp(attr[i], _T("cx")) == 0) cx = parse_double(attr[i + 1]);
				if(_tcscmp(attr[i], _T("cy")) == 0) cy = parse_double(attr[i + 1]);
				if(_tcscmp(attr[i], _T("r")) == 0) r = parse_double(attr[i + 1]);
			}
		}

		m_path.move_to(cx-r, cy);
		m_path.arc(r, r, 360, true, true, 0, .0001, true);
		m_path.end_path();
	}


	void parser::parse_ellipse(const char_type** attr)
	{
		int i;
		double cx = 0.0;
		double cy = 0.0;
		double rx = 0.0;
		double ry = 0.0;

		m_path.begin_path();
		for(i = 0; attr[i]; i += 2)
		{
			if(!parse_attr(attr[i], attr[i + 1]))
			{
				if(_tcscmp(attr[i], _T("cx")) == 0) cx = parse_double(attr[i + 1]);
				if(_tcscmp(attr[i], _T("cy")) == 0) cy = parse_double(attr[i + 1]);
				if(_tcscmp(attr[i], _T("rx")) == 0) rx = parse_double(attr[i + 1]);
				if(_tcscmp(attr[i], _T("ry")) == 0) ry = parse_double(attr[i + 1]);
			}
		}

		m_path.move_to(cx-rx, cy);
		m_path.arc(rx, ry, 360, true, true, 0, .0001, true);
		m_path.end_path();
	}

	//-------------------------------------------------------------
	void parser::parse_text_start(const char_type** attr)
	{
		m_path.begin_path();
		m_parser_text.clear_attr();

		for(int i = 0; attr[i]; i += 2)
		{
			if(!parse_attr(attr[i], attr[i + 1]))
			{
				m_parser_text.parse_attr(attr[i], attr[i + 1]);
			}
		}
	}

	void parser::parse_text_content(const char_type* s, int len)
	{
		m_parser_text.text_content(s, len);
	}

	void parser::parse_text_end()
	{
		m_parser_text.text_end();
		m_path.end_path();
	}

    //-------------------------------------------------------------
    void parser::parse_transform(const char_type* str)
    {
        while(*str)
        {
            if(islower(*str))
            {
                if(_tcsncmp(str, _T("matrix"), 6) == 0)    str += parse_matrix(str);    else 
                if(_tcsncmp(str, _T("translate"), 9) == 0) str += parse_translate(str); else 
                if(_tcsncmp(str, _T("rotate"), 6) == 0)    str += parse_rotate(str);    else 
                if(_tcsncmp(str, _T("scale"), 5) == 0)     str += parse_scale(str);     else 
                if(_tcsncmp(str, _T("skewX"), 5) == 0)     str += parse_skew_x(str);    else 
                if(_tcsncmp(str, _T("skewY"), 5) == 0)     str += parse_skew_y(str);    else
                {
                    ++str;
                }
            }
            else
            {
                ++str;
            }
        }
    }

	//-------------------------------------------------------------
	void parser::parse_stroke_dasharray(const char_type* str)
	{
		m_tokenizer.set_path_str(str);
		dash_description dashes;
 		while (m_tokenizer.next())
 		{
 			double dash = m_tokenizer.last_number();
			if(!m_tokenizer.next())
				throw exception( _T("parse_stroke_dasharray: dash gap len must be pairs"));
			double gap = m_tokenizer.last_number(); 
			dashes.add_dash(dash, gap);
 		}
		if(dashes.dashes_count())
			m_path.dash(dashes);
	}

    //-------------------------------------------------------------
    static bool is_numeric(str_type::char_type c)
    {
		return _tcschr(_T("0123456789+-.eE"), c) != 0;
    }

    //-------------------------------------------------------------
    static unsigned parse_transform_args(const str_type::char_type* str, 
                                         double* args, 
                                         unsigned max_na, 
                                         unsigned* na)
    {
        *na = 0;
        const str_type::char_type* ptr = str;
        while(*ptr && *ptr != _T('(')) ++ptr;
        if(*ptr == 0)
        {
            throw exception( _T("parse_transform_args: Invalid syntax"));
        }
        const str_type::char_type* end = ptr;
        while(*end && *end != _T(')')) ++end;
        if(*end == 0)
        {
            throw exception( _T("parse_transform_args: Invalid syntax"));
        }

        while(ptr < end)
        {
            if(is_numeric(*ptr))
            {
                if(*na >= max_na)
                {
                    throw exception( _T("parse_transform_args: Too many arguments"));
                }
				args[(*na)++] = _tstof(ptr);
                while(ptr < end && is_numeric(*ptr)) ++ptr;
            }
            else
            {
                ++ptr;
            }
        }
        return unsigned(end - str);
    }

    //-------------------------------------------------------------
    unsigned parser::parse_matrix(const char_type* str)
    {
        double args[6];
        unsigned na = 0;
        unsigned len = parse_transform_args(str, args, 6, &na);
        if(na != 6)
        {
            throw exception( _T("parse_matrix: Invalid number of arguments"));
        }
        m_path.transform().premultiply(trans_affine(args[0], args[1], args[2], args[3], args[4], args[5]));
        return len;
    }

    //-------------------------------------------------------------
    unsigned parser::parse_translate(const char_type* str)
    {
        double args[2];
        unsigned na = 0;
        unsigned len = parse_transform_args(str, args, 2, &na);
        if(na == 1) args[1] = 0.0;
        m_path.transform().premultiply(trans_affine_translation(args[0], args[1]));
        return len;
    }

    //-------------------------------------------------------------
    unsigned parser::parse_rotate(const char_type* str)
    {
        double args[3];
        unsigned na = 0;
        unsigned len = parse_transform_args(str, args, 3, &na);
        if(na == 1) 
        {
            m_path.transform().premultiply(trans_affine_rotation(deg2rad(args[0])));
        }
        else if(na == 3)
        {
            trans_affine t = trans_affine_translation(-args[1], -args[2]);
            t *= trans_affine_rotation(deg2rad(args[0]));
            t *= trans_affine_translation(args[1], args[2]);
            m_path.transform().premultiply(t);
        }
        else
        {
            throw exception( _T("parse_rotate: Invalid number of arguments"));
        }
        return len;
    }

    //-------------------------------------------------------------
    unsigned parser::parse_scale(const char_type* str)
    {
        double args[2];
        unsigned na = 0;
        unsigned len = parse_transform_args(str, args, 2, &na);
        if(na == 1) args[1] = args[0];
        m_path.transform().premultiply(trans_affine_scaling(args[0], args[1]));
        return len;
    }

    //-------------------------------------------------------------
    unsigned parser::parse_skew_x(const char_type* str)
    {
        double arg;
        unsigned na = 0;
        unsigned len = parse_transform_args(str, &arg, 1, &na);
        m_path.transform().premultiply(trans_affine_skewing(deg2rad(arg), 0.0));
        return len;
    }

    //-------------------------------------------------------------
    unsigned parser::parse_skew_y(const char_type* str)
    {
        double arg;
        unsigned na = 0;
        unsigned len = parse_transform_args(str, &arg, 1, &na);
        m_path.transform().premultiply(trans_affine_skewing(0.0, deg2rad(arg)));
        return len;
    }

}
}