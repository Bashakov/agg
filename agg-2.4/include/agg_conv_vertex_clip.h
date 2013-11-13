
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
		struct piece
		{
			double end;
			bool   vsbl;

			piece(double e = 0.0, bool v_ = 0): end(e), vsbl(v_) {}
			static bool sort_fn ( const piece & lh, const piece & rh )	{ return lh.end < rh.end; }
		};

	public:
		struct point
		{
			coord_type x, y;
			point(coord_type x_ = 0, coord_type y_ = 0)					: x(x_), y(y_)		{}
			point(const trans_affine & t, coord_type x_, coord_type y_)	: x(x_), y(y_)		{ t.transform(&x, &y); }
			point(double k, const point & p1, const point & p2)			: x(p1.x + (p2.x - p1.x) * k), y(p1.y + (p2.y - p1.y) * k) {}

			point    transform(const trans_affine & t)	const { return point(t, x, y);			 }
			unsigned clippin_flag(const rect_type & cb) const { return clipping_flags(x, y, cb); }
		};

	public:
		//--------------------------------------------------------------------
		vertex_sl_clip()  
			: m_clip_box(0, 0, 0, 0)
			, m_clipping(false) 
		{
			reset_pieses();
		}

		//--------------------------------------------------------------------
		void reset_clipping()
		{
			m_clipping = false;
			reset_pieses();
		}

		//--------------------------------------------------------------------
		void clip_box(const rect_type & cb)
		{
			m_clip_box = cb;
			m_clip_box.normalize();
			m_clipping = true;
			reset_pieses();
		}

		//--------------------------------------------------------------------
		void set_transform(const trans_affine & transform)
		{
			m_transform = transform;
			reset_pieses();
		}

		//--------------------------------------------------------------------
		void reset_pieses()
		{
			m_count_piece = 0;
			m_crrnt_piece = 0;
		}

		//--------------------------------------------------------------------
		point get_pt2() const 
		{
			return m_pt2; 
		}

		//--------------------------------------------------------------------
		bool line_to(const point p1, const point p2)
		{
			reset_pieses();
			m_pt1 = p1;
			m_pt2 = p2;
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

		//--------------------------------------------------------------------
		bool get_cmd(coord_type * x, coord_type * y, bool * vsbl)
		{
			if(m_crrnt_piece >= m_count_piece)
				return false;
			do
			{
				point p(m_pieces[m_crrnt_piece].end, m_pt1, m_pt2);
				*vsbl = m_pieces[m_crrnt_piece].vsbl;
				*x = p.x;
				*y = p.y;

				++m_crrnt_piece;
			}	while(  m_crrnt_piece < m_count_piece && !(*vsbl) && !m_pieces[m_crrnt_piece].vsbl );
			return true;
		}
		
		//--------------------------------------------------------------------
		void return_cmd()
		{
			if(m_crrnt_piece > 0)
				m_crrnt_piece--;
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
		typedef typename Cliper::point			point;

		void operator = (const conv_vertex_clip & rh) {}
	public:
		conv_vertex_clip(VertexSource& vs) : m_source(vs), m_bClip(false), m_bNeedCloseLine(false), m_bPrevVisible(false), m_bFirstPiece(false) {}

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
			//printf("\n =============== rewind ================= \n");
			m_source.rewind(path_id); 
		}

		unsigned vertex(coord_type* x, coord_type* y)
		{
			unsigned cmd = m_bClip? work_fn(x, y): m_source.vertex(x, y);

			//printf("OUT: %2d | %8.2lf %8.2lf\n", cmd, *x, *y);
			return cmd;
		}

	private:
		unsigned work_fn(coord_type* x, coord_type* y) 
		{ 
			while(1)
			{
				unsigned cmd = path_cmd_stop;
				if( storage(x, y, &cmd) )
					return cmd;

				cmd = m_source.vertex(x, y); 
				//printf("\nIN : %2d | %8.2lf %8.2lf\n", cmd, *x, *y);
				if(is_move_to(cmd)) 
				{
					move_to(x, y);
					return cmd;
				}
				else if(is_line_to(cmd))
				{
					if(line_to(x, y))
						return path_cmd_line_to;
				}
				else if( is_end_poly(cmd) )
				{
					if(close_poly(x, y))
						return path_cmd_line_to;
				}
				else
				{
					_ASSERT(is_stop(cmd)); //only line_to and move_to supported, use conv_curve for transform those to these
					return cmd;
				}
			}
		}
			
	private:
		bool storage(coord_type* x, coord_type* y, unsigned * cmd)
		{
			bool vsbl = false;
			while( m_cliper.get_cmd(x, y, &vsbl) )
			{
				bool prev_vis = m_bPrevVisible;
				m_bPrevVisible = vsbl;
				if(vsbl)
				{
					if( m_bFirstPiece )
					{
						m_bFirstPiece = false;
						*x = m_ptFirstPieceCorrection.x;
						*y = m_ptFirstPieceCorrection.y;
						m_cliper.return_cmd();
						*cmd = path_cmd_move_to;
						return true;
					}
					if( m_bNeedCloseLine )
					{
						m_bNeedCloseLine = false;
						*x = m_ptInvisibleLine.x;
						*y = m_ptInvisibleLine.y;
						m_cliper.return_cmd();
					}

					*cmd = path_cmd_line_to;
					m_bFirstPiece = false;
					return true;
				}
				else if(prev_vis || m_bNeedCloseLine)
				{
					m_bNeedCloseLine = true;
					m_ptInvisibleLine = point(*x, *y);
				}
				else if(!vsbl && m_bFirstPiece)
				{
					m_ptFirstPieceCorrection = point(*x, *y);
				}
			}
			return false;
		}
		void move_to(coord_type* x, coord_type* y)
		{
			m_ptFirstPieceCorrection = m_ptStartPoligon = m_ptCurrentTo = point(*x, *y);
			m_cliper.reset_pieses();
			m_bNeedCloseLine = false;
			m_bPrevVisible = false;
			m_bFirstPiece = true;
		}
		bool line_to(coord_type* x, coord_type* y)
		{
			point prev = m_ptCurrentTo;
			m_ptCurrentTo = point(*x, *y);
			bool normal = m_cliper.line_to( prev, m_ptCurrentTo);
			if(normal)
				m_bFirstPiece = false;
			return normal; 
		}
		bool close_poly(coord_type* x, coord_type* y)
		{
			*x = m_ptStartPoligon.x;
			*y = m_ptStartPoligon.y;
			return line_to(x, y);
		}

	private:
		VertexSource	&	m_source;
		Cliper		m_cliper;
		bool		m_bClip;
		point		m_ptCurrentTo;
		point		m_ptInvisibleLine;
		point		m_ptStartPoligon;
		point		m_ptFirstPieceCorrection;

		bool		m_bNeedCloseLine;
		bool		m_bPrevVisible;
		bool		m_bFirstPiece;
	};
	

}

#endif
