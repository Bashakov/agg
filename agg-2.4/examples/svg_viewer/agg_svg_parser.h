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

#ifndef AGG_SVG_PARSER_INCLUDED
#define AGG_SVG_PARSER_INCLUDED

#include <string>
#include <TCHAR.H>

#include "agg_svg_path_tokenizer.h"
#include "agg_svg_path_renderer.h"
#include "agg_svg_parser_text.h"


namespace agg
{
namespace svg
{
	class utf_convertor
	{
		//typedef str_type::string_type		string_type;
		typedef str_type::char_type			char_type;
	public:
		typedef std::pair<const char_type *, size_t>	adapter;

		utf_convertor();
		adapter convert(const char_type * utf_str, size_t len = 0);
	private:
		std::string		m_buffer;
		std::wstring	m_wideBuffer;
	};
	
    class parser
    {
        enum buf_size_e { buf_size = BUFSIZ };
		typedef str_type::char_type			char_type;
		typedef str_type::string_type		string_type;

    public:
        ~parser();
        parser(path_renderer& path);
		void clear();

        void parse(const char_type* fname);
		void parse(const char * stzSVG, size_t strLen);
        const char_type* title() const { return m_str_title.c_str(); }

    private:
        // XML event handlers
        static void start_element(void* data, const char_type* el, const char_type** attr);
        static void end_element(void* data, const char_type* el);
        static void content(void* data, const char_type* s, int len);

        void parse_attr(const char_type** attr);
        void parse_path(const char_type** attr);
        void parse_poly(const char_type** attr, bool close_flag);
        void parse_circle(const char_type** attr);
        void parse_ellipse(const char_type** attr);
        void parse_rect(const char_type** attr);
        void parse_line(const char_type** attr);
        void parse_style(const char_type* str);
        void parse_transform(const char_type* str);
		
		void parse_text_start(const char_type** attr);
		void parse_text_content(const char_type* s, int len);
		void parse_text_end();

        unsigned parse_matrix(const char_type* str);
        unsigned parse_translate(const char_type* str);
        unsigned parse_rotate(const char_type* str);
        unsigned parse_scale(const char_type* str);
        unsigned parse_skew_x(const char_type* str);
        unsigned parse_skew_y(const char_type* str);
        
        bool parse_attr(const char_type* name, const char_type* value);
        bool parse_name_value(const char_type* nv_start, const char_type* nv_end);
        
    private:
        path_renderer& m_path;
        path_tokenizer m_tokenizer;
        string_type    m_str_title;
        bool           m_title_flag;
        bool           m_path_flag;
        string_type    m_str_attr_name;
        string_type    m_str_attr_value;
		parser_text    m_parser_text;
		utf_convertor  m_utf_convertor;
    };

}
}

#endif