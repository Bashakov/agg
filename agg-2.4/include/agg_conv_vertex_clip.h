
#ifndef AGG_CONV_VERTEX_CLIP_INCLUDED
#define AGG_CONV_VERTEX_CLIP_INCLUDED

#include "agg_basics.h"
#include "agg_clip_liang_barsky.h"
#include "agg_trans_affine.h"
#include <algorithm>

namespace agg
{


	template<class coord_type> class vertex_sl_clip
	{
		typedef rect_base<coord_type>     rect_type;

		struct point
		{
			coord_type x, y;
			point(coord_type x_ = 0, coord_type y_ = 0)					: x(x_), y(y_)		{}
			point(const trans_affine & t, coord_type x_, coord_type y_)	: x(x_), y(y_)		{ t.transform(&x, &y); }
			point(double k, const point & p1, const point & p2)			: x(p1.x + (p2.x - p1.x) * k), y(p1.y + (p2.y - p1.y) * k) {}

			point    transform(const trans_affine & t)	const { return point(t, x, y);			 }
			unsigned clippin_flag(const rect_type & cb) const { return clipping_flags(x, y, cb); }
		};

		struct piece
		{
			double end;
			bool   vsbl;

			piece(double e = 0.0, bool v_ = 0): end(e), vsbl(v_) {}
			static bool sort_fn ( const piece & lh, const piece & rh )	{ return lh.end < rh.end; }
		};

	public:
		//--------------------------------------------------------------------
		vertex_sl_clip()  
			: m_clip_box(0,0,0,0)
			, m_clipping(false) 
		{
			rewind();
		}

		//--------------------------------------------------------------------
		void reset_clipping()
		{
			m_clipping = false;
			rewind();
		}

		//--------------------------------------------------------------------
		void clip_box(const rect_type & cb)
		{
			m_clip_box = cb;
			m_clip_box.normalize();
			m_clipping = true;
			rewind();
		}

		//--------------------------------------------------------------------
		void set_transform(const trans_affine & transform)
		{
			m_transform = transform;
			rewind();
		}

		//--------------------------------------------------------------------
		void move_to(const coord_type x1, const coord_type y1)
		{
			m_pt2 = point(x1, y1);
			rewind();
		}

		//--------------------------------------------------------------------
		void rewind()
		{
			m_count_piece = 0;
			m_crrnt_piece = 0;
		}

		bool line_to(const coord_type x2, const coord_type y2)
		{
			rewind();
			m_pt1 = m_pt2;
			m_pt2 = point(x2, y2);
			if(m_clipping)
			{

				const point p1 = m_pt1.transform(m_transform);
				const point p2 = m_pt2.transform(m_transform);

				const unsigned f1 = p1.clippin_flag(m_clip_box);
				const unsigned f2 = p2.clippin_flag(m_clip_box);
				if(f1 || f2) 
				{
					unsigned ff = f1 ^ f2;
					piece * pPE = m_pieces;
					if (ff & 0x04) (pPE++)->end = resolve(p1.x, p2.x, m_clip_box.x1); // jump over x1 border
					if (ff & 0x01) (pPE++)->end = resolve(p1.x, p2.x, m_clip_box.x2); // jump over x2 border

					if (ff & 0x08) (pPE++)->end = resolve(p1.y, p2.y, m_clip_box.y1); // jump over y1 border
					if (ff & 0x02) (pPE++)->end = resolve(p1.y, p2.y, m_clip_box.y2); // jump over y2 border

					std::sort(m_pieces, pPE, piece::sort_fn);
					m_pieces[0].vsbl = !f1;
					*pPE++ = piece(1.0, !f2);

					for (piece * pc = m_pieces+1; pc < pPE-1; ++pc)
					{
						double mid = (pc[-1].end + pc->end) / 2.0;
						point p(mid, p1, p2);
						pc->vsbl = ! p.clippin_flag(m_clip_box);
					}
					m_count_piece = pPE - m_pieces;
				}
			}
			return m_count_piece == 0;
		}

		bool get_cmd(coord_type * x, coord_type * y, unsigned * cmd)
		{
			if(m_crrnt_piece >= m_count_piece)
				return false;

			do
			{
				point p(m_pieces[m_crrnt_piece].end, m_pt1, m_pt2);
				bool vsbl = m_pieces[m_crrnt_piece].vsbl;

				*x = p.x;
				*y = p.y;
				*cmd = vsbl? path_cmd_line_to: path_cmd_move_to;

				++m_crrnt_piece;
			}	while(  m_crrnt_piece < m_count_piece && (*cmd == path_cmd_move_to) && !m_pieces[m_crrnt_piece].vsbl );
			return true;
		}

	private:
		static double resolve(coord_type a1, coord_type a2, coord_type ax)
		{
			double t = (a1 == a2)? 1.0: 1.0 * (ax-a1) / (a2-a1);
			return t;
		}

	private:
		piece            m_pieces[10];
		point            m_pt1, m_pt2;
		int              m_crrnt_piece;
		int              m_count_piece;
		trans_affine     m_transform;
		rect_type        m_clip_box;
		bool             m_clipping;
	};


	//------------------------------------------------------------------------
	//------------------------------------------------------------------------

	template<class VertexSource, class coord_type = double> 
	class conv_vertex_clip
	{
		typedef agg::vertex_sl_clip<coord_type>	Cliper;

	public:
		conv_vertex_clip(VertexSource& vs) : m_source(vs), m_bClip(false) {}

		void set_transform(const trans_affine & transform)
		{
			m_cliper.set_transform(transform);
		}

		void clip_box(const rect_i& cb, double scalex, double scaleY)
		{
			rect_d clip_rect = rect_d(cb.x1, cb.y1, cb.x2, cb.y2);

			trans_affine_translation mv_rect( -(clip_rect.x1 + clip_rect.x2) / 2.0, -(clip_rect.y1 + clip_rect.y2) / 2.0 );
			trans_affine extend_rect;
			extend_rect *= mv_rect;
			extend_rect *= trans_affine_scaling(scalex, scaleY);
			extend_rect *= ~mv_rect;
			extend_rect.transform(&clip_rect.x1, &clip_rect.y1);
			extend_rect.transform(&clip_rect.x2, &clip_rect.y2);
			m_cliper.clip_box(clip_rect);
			m_bClip = true;
		}

		void rewind(unsigned path_id) 
		{ 
			m_source.rewind(path_id); 
		}

		unsigned vertex(double* x, double* y) 
		{ 
			unsigned cmd = path_cmd_stop;
			if( m_bClip && m_cliper.get_cmd(x, y, &cmd) )
				return cmd;

			cmd = m_source.vertex(x, y); 
			if( m_bClip )
			{
				if(is_move_to(cmd)) 
				{
					m_cliper.move_to(*x, *y);
				}
				else if(is_line_to(cmd))
				{
					if( !m_cliper.line_to(*x, *y))
						m_cliper.get_cmd(x, y, &cmd);
				}
				else 
				{
					_ASSERT(is_stop(cmd)); //only line_to and move_to supported, use conv_curve for transform those to these
				}
			}
			return cmd;
		}

	private:
		VertexSource	&	m_source;
		Cliper				m_cliper;
		bool				m_bClip;
	};
	

}

#endif
