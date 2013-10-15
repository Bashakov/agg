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
		family = "Tahoma";
		in_tag = tag;
		gren = glyph_ren_outline;
	}


	parser_text::parser_text(path_renderer & path)
		: m_path(path)
		, m_feng(GetDC(NULL))
		, m_fman(m_feng)
		, m_curves(m_fman.path_adaptor())
		, m_contour(m_curves)
	{
		m_text.init(false);
		m_feng.gamma(agg::gamma_none());
	}
	
	void parser_text::clear_attr()
	{
		m_text.init(true);
	}

	double parse_double(const char* str);

	void parser_text::parse_attr(const char* name, const char* value)
	{
		m_path.begin_path();

		if(strcmp(name, "x") == 0)				m_text.x = parse_double(value);
		if(strcmp(name, "y") == 0)				m_text.y = parse_double(value);
		if(strcmp(name, "font-size") == 0)		m_text.width = m_text.height = parse_double(value);
		if(strcmp(name, "font-family") == 0)	m_text.family = value;
	}

	void parser_text::text_content(const char* s, int len)
	{
		m_feng.height(m_text.height);
		m_feng.width((m_text.width == m_text.height) ? 0.0 : m_text.width / 2.4);
		m_feng.flip_y(true);
		
		m_contour.width( -m_text.weight * m_text.height * 0.05);

		if( m_feng.create_font(m_text.family, m_text.gren) )
		{
			double x = m_text.x;
			double y = m_text.y;
			for (; *s && len; ++s, --len)
			{
				const agg::glyph_cache* glyph = m_fman.glyph(*s);
				if( !glyph )
					break;

				m_fman.init_embedded_adaptors(glyph, x, y);

				switch(glyph->data_type)
				{
				case agg::glyph_data_mono:
					// 					ren_bin.color(agg::srgba8(0, 0, 0));
					// 					agg::render_scanlines(m_fman.mono_adaptor(), m_fman.mono_scanline(), ren_bin);
					break;

				case agg::glyph_data_gray8:
					// 					ren_solid.color(agg::srgba8(0, 0, 0));
					// 					agg::render_scanlines(m_fman.gray8_adaptor(), 
					// 						m_fman.gray8_scanline(), 
					// 						ren_solid);
					break;

				case agg::glyph_data_outline:
					if(1 && fabs(m_text.weight) <= 0.01)
					{
						// For the sake of efficiency skip the
						// contour converter if the weight is about zero.
						//-----------------------
						m_path.concat_path(m_curves);
					}
					else
					{
						m_path.concat_path(m_contour);
					}
					break;
				}

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
		m_path.end_path();
	}

	void parser_text::text_end()
	{
		m_text.in_tag = false;
	}
}
}