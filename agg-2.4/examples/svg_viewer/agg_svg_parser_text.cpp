#include "agg_svg_parser_text.h"

namespace agg
{
namespace svg
{

	void parser_text::text_desc::init(bool tag)
	{
		x  =  0.0;
		y  =  0.0;
		height = 18.0;
		width = 18.0;
		weight = 0.0;
		family = _T("Tahoma");
		in_tag = tag;
		gren = glyph_ren_outline;
	}


	parser_text::parser_text(path_renderer & path)
		: m_path(path)
		, m_feng(GetDC(NULL))
		, m_fman(m_feng)
		, m_curves(m_fman.path_adaptor())
	{
		m_text.init(false);
		m_feng.gamma(agg::gamma_none());
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
		m_feng.height(m_text.height);
		m_feng.width((m_text.width == m_text.height) ? 0.0 : m_text.width / 2.4);
		m_feng.flip_y(true);
		m_feng.italic(false);
		
		if( m_feng.create_font(m_text.family, m_text.gren) )
		{
			double x = m_text.x;
			double y = m_text.y;
			for (; *s && len; ++s, --len)
			{
				const agg::glyph_cache* glyph = m_fman.glyph(*s);
				if( !glyph || glyph->data_type != glyph_data_outline)
					break;

				m_fman.init_embedded_adaptors(glyph, x, y);
				//m_path.concat_path( m_curves );
				m_path.concat_path( m_fman.path_adaptor() );

				// increment pen position
				x += glyph->advance_x;
				y += glyph->advance_y;
			}
		}
		else
		{
			agg::gsv_text t;
			t.size(m_text.height);
			t.flip(true);
			t.start_point(m_text.x, m_text.y);
			t.text(s, len);

			agg::conv_stroke<agg::gsv_text> pt(t);
			pt.width(m_text.height / 10.0);
			m_path.concat_path(pt);
		}

		m_text.in_tag = false;
	}

	void parser_text::text_end()
	{
		m_text.in_tag = false;
	}
}
}