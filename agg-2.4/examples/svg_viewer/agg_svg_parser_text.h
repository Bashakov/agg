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

	class parser_text
	{
		struct text_desc
		{
			double			x, y;
			double			height, width;
			double			weight;
			const char *	family;
			glyph_rendering gren;
			bool			in_tag;

			void init(bool in_tag);
		};

		typedef font_engine_win32_tt_int32				font_engine_type;
		typedef font_cache_manager<font_engine_type>	font_manager_type;

	public:
		parser_text(path_renderer & path);

		void clear_attr();
		void parse_attr(const char* name, const char* value);
		void text_content(const char* s, int len );
		void text_end();

		bool is_text_mode() const { return m_text.in_tag; };
	private:
		path_renderer	&	m_path;
		font_engine_type    m_feng;
		font_manager_type   m_fman;
		text_desc			m_text;
	};
}
}

#endif AGG_SVG_PARSER_TEXT_INCLUDED