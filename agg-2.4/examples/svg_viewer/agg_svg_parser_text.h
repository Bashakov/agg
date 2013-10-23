#ifndef AGG_SVG_PARSER_TEXT_INCLUDED
#define AGG_SVG_PARSER_TEXT_INCLUDED

#include "agg_svg_path_renderer.h"
#include "agg_conv_curve.h"
#include "agg_conv_contour.h"
#include "../../font_win32_tt/agg_font_win32_tt.h"


namespace agg
{
namespace svg
{
	class ffont_to_path
	{
		typedef str_type::char_type							char_type;
		typedef font_engine_win32_tt_int32					font_engine_type;
		typedef font_cache_manager<font_engine_type>		font_manager_type;

	public:
		ffont_to_path();
		~ffont_to_path();
		
		bool set_font(const char_type* typeface, double height,	double width = 0.0, int weight = FW_REGULAR, bool italic = false);

		template<class Path>
		void write(const char_type* s, int len, double & x, double & y, Path& path)
		{
			if( len == 0 ) len = -1;
			for (; *s && len; ++s, --len)
			{
				const agg::glyph_cache* glyph = m_fman.glyph(*s);
				if( !glyph || glyph->data_type != glyph_data_outline)
					continue;

				m_fman.init_embedded_adaptors(glyph, x, y);
				path.add_path( m_fman.path_adaptor() );

				x += glyph->advance_x;
				y += glyph->advance_y;
			}
		}
		
	private:
		font_engine_type    m_feng;
		font_manager_type   m_fman;
		glyph_rendering		m_gren;
	};

	class parser_text
	{
		typedef str_type::char_type		char_type;

		struct text_desc
		{
			double				x, y, height, width, weight;
			const char_type *	family;
			glyph_rendering		gren;
			bool				in_tag;

			void init(bool in_tag);
		};

	public:
		parser_text(path_renderer & path);

		void clear_attr();
		void parse_attr(const char_type* name, const char_type* value);
		void text_content(const char_type* s, int len);
		void text_end();

		bool is_text_mode() const { return m_text.in_tag; };

	private:
		path_renderer	&	m_path;
		ffont_to_path		m_ff_font;
		gsv_text			m_agg_text;
		text_desc			m_text;
	};
}
}

#endif AGG_SVG_PARSER_TEXT_INCLUDED