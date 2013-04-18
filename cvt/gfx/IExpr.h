#ifndef CVT_IEXPR_H
#define CVT_IEXPR_H

#include <cvt/gfx/Image.h>

namespace cvt {
	enum IExprType {
		IEXPR_ADD = 0,
		IEXPR_MUL
	};

	template<typename DERIVED>
	class IExpr
	{
		public:
		inline const DERIVED& derived() const
		{
			return static_cast<const DERIVED&>( *this );
		}
	};

	template<typename T1, typename T2, IExprType op>
	class IExprBinary
	{
		public:
			IExprBinary( const T1& op1, const T2& op2 ) : _op1( op1 ), _op2( op2 ) {}

			void eval( Image& dst ) const
			{
				Image dst1, dst2;
				_op1.eval( dst1 );
				_op2.eval( dst2 );
				IExprBinary<const Image&, const Image&,op> expr( dst1, dst2 );
				expr.eval( dst );
			}

		T1 _op1;
		T2 _op2;
	};

    template<typename T1, typename T2, IExprType op>
    std::ostream& operator<<( std::ostream& out, const IExprBinary<T1,T2,op>& expr )
    {
		char opToChar[] = { '+' , '*' };
		out << expr._op1 << opToChar[ op ] << expr._op2;
        return out;
    }

    template<>
    std::ostream& operator<<( std::ostream& out, const IExprBinary<const Image&,float,IEXPR_ADD>& expr )
    {
		out << "Image" << "+" << expr._op2;
        return out;
    }

	template<>
    std::ostream& operator<<( std::ostream& out, const IExprBinary<const Image&,float,IEXPR_MUL>& expr )
    {
		out << "Image" << "*" << expr._op2;
        return out;
    }

	template<>
	void IExprBinary<const Image&,float,IEXPR_MUL>::eval( Image& dst ) const
	{
	}

	/*
		- Image -> Image * -1
	 */
	IExprBinary<const Image&,float, IEXPR_MUL> operator-( const Image& img )
	{
		return IExprBinary<const Image&,float,IEXPR_MUL>( img, -1.0f );
	}

	/*
		Image + float
		float + Image
	 */
	IExprBinary<const Image&,float,IEXPR_ADD> operator+( const float val, const Image& img )
	{
		return IExprBinary<const Image&,float,IEXPR_ADD>( img, val );
	}

	IExprBinary<const Image&,float,IEXPR_ADD> operator+( const Image& img, const float val )
	{
		return IExprBinary<const Image&,float,IEXPR_ADD>( img, val );
	}

	/*
		Image * float
		float * Image
	 */
	IExprBinary<const Image&,float,IEXPR_MUL> operator*( const float val, const Image& img )
	{
		return IExprBinary<const Image&,float,IEXPR_MUL>( img, val );
	}

	IExprBinary<const Image&,float,IEXPR_MUL> operator*( const Image& img, const float val )
	{
		return IExprBinary<const Image&,float,IEXPR_MUL>( img, val );
	}

	/*
		( Image + float ) + float -> Image + float
		float + ( Image + float ) -> Image + float
	 */
	IExprBinary<const Image&,float,IEXPR_ADD> operator+( const IExprBinary<const Image&,float,IEXPR_ADD>& expr, const float value )
	{
		return IExprBinary<const Image&,float,IEXPR_ADD>( expr._op1, expr._op2 + value );
	}

	IExprBinary<const Image&,float,IEXPR_ADD> operator+( const float value, const IExprBinary<const Image&,float,IEXPR_ADD>& expr )
	{
		return IExprBinary<const Image&,float,IEXPR_ADD>( expr._op1, expr._op2 + value );
	}

	/*
		( Image * float ) * float -> Image * float
		float * ( Image * float ) -> Image * float
	 */
	IExprBinary<const Image&,float,IEXPR_MUL> operator*( const IExprBinary<const Image&,float,IEXPR_MUL>& expr, const float value )
	{
		return IExprBinary<const Image&,float,IEXPR_MUL>( expr._op1, expr._op2 * value );
	}

	IExprBinary<const Image&,float,IEXPR_MUL> operator*( const float value, const IExprBinary<const Image&,float,IEXPR_MUL>& expr )
	{
		return IExprBinary<const Image&,float,IEXPR_MUL>( expr._op1, expr._op2 * value );
	}


	template<typename T1_1, typename T2_1, IExprType op_1, typename T2>
	IExprBinary<IExprBinary<T1_1,T2_1,op_1>, T2, IEXPR_MUL> operator*( const IExprBinary<T1_1,T2_1,op_1>& expr1, const T2& expr2 )
	{
		return IExprBinary<IExprBinary<T1_1,T2_1,op_1>, T2,IEXPR_MUL>( expr1, expr2 );
	}

	template<typename T1_1, typename T2_1, IExprType op_1, typename T2>
	IExprBinary<IExprBinary<T1_1,T2_1,op_1>, T2, IEXPR_ADD> operator+( const IExprBinary<T1_1,T2_1,op_1>& expr1, const T2& expr2 )
	{
		return IExprBinary<IExprBinary<T1_1,T2_1,op_1>, T2,IEXPR_ADD>( expr1, expr2 );
	}

}


#endif
