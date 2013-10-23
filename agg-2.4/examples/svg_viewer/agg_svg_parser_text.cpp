#include "agg_svg_parser_text.h"

namespace agg
{
namespace svg
{

	ffont_to_path::ffont_to_path()
		: m_feng(GetDC(NULL))
		, m_fman(m_feng)
		, m_gren(glyph_ren_outline)
	{
		m_feng.gamma(agg::gamma_none());
	}
		
	ffont_to_path::~ffont_to_path()
	{
	}

	bool ffont_to_path::set_font(const char_type* typeface, double height, double width, int weight, bool italic)
	{
		width = (width == height) ? 0.0 : width;
		m_feng.flip_y(true);
		return m_feng.create_font(typeface, m_gren, height, width, weight, italic, RUSSIAN_CHARSET);
	}

	// ========================================================================= //
	// ========================================================================= //
	// ========================================================================= //

	void parser_text::text_desc::init(bool tag)
	{
		x  =  0.0;
		y  =  0.0;
		height = 18.0;
		width = 18.0;
		weight = 0.0;
		family = _T("Tahoma");
		in_tag = tag;
	}

	// ========================================================================= //

	parser_text::parser_text(path_renderer & path)
		: m_path(path)
	{
		m_text.init(false);
	}
	
	void parser_text::clear_attr()
	{
		m_text.init(true);
	}

	double parse_double(const str_type::char_type* str);

	void parser_text::parse_attr(const char_type* name, const char_type* value)
	{
		if(_tcscmp(name, _T("x")) == 0)				m_text.x = parse_double(value);
		if(_tcscmp(name, _T("y")) == 0)				m_text.y = parse_double(value);
		if(_tcscmp(name, _T("font-size")) == 0)		m_text.width = m_text.height = parse_double(value);
		if(_tcscmp(name, _T("font-family")) == 0)	m_text.family = value;
	}

	void parser_text::text_content(const char_type* s, int len)
	{
		if( m_ff_font.set_font(m_text.family, m_text.height, m_text.width) )
		{
			m_ff_font.write(s, len, m_text.x, m_text.y, m_path);
		}
		else
		{
			m_agg_text.size(m_text.height);
			m_agg_text.flip(true);
			m_agg_text.start_point(m_text.x, m_text.y);
			m_agg_text.text(s, len);

			agg::conv_stroke<agg::gsv_text> pt(m_agg_text);
			pt.width(m_text.height / 10.0);
			m_path.add_path(pt);
		}

		m_text.in_tag = false;
	}

	void parser_text::text_end()
	{
		m_text.in_tag = false;
	}
}
}