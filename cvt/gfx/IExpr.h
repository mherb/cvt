#ifndef CVT_IEXPR_H
#define CVT_IEXPR_H

#include <cvt/gfx/Image.h>
#include <cvt/gfx/IMapScoped.h>

namespace cvt {
	enum IExprType {
		IEXPR_ADD = 0,
		IEXPR_SUB,
		IEXPR_MUL
	};

	template<IExprType type>
	struct IExprOperation {
		float operator()( float a, float b );
	};

	template<>
	struct IExprOperation<IEXPR_ADD> {
		float operator()( float a, float b ) { return a + b; }
	};

	template<>
	struct IExprOperation<IEXPR_SUB> {
		float operator()( float a, float b ) { return a - b; }
	};

	template<>
	struct IExprOperation<IEXPR_MUL> {
		float operator()( float a, float b ) { return a * b; }
	};


	template<typename T1, typename T2, IExprType op>
	class IExprBinary
	{
		public:
			IExprBinary( const T1& opa, const T2& opb ) : op1( opa ), op2( opb ) {}

			void eval( Image& dst )
			{
				if( !hasSizeFormat( dst.width(), dst.height(), dst.format() ) )
					throw CVTException( "Invalid image expression or assignment!" );

				map();

				IMapScoped<float> imap( dst );
				size_t height = dst.height();
				size_t width = dst.width() * dst.format().channels;
				for( size_t y = 0; y < height; y++ ) {
					setLine( y );
					imap.setLine( y );
					float* dstptr = imap.ptr();

					for( size_t x = 0; x < width; x++ )
						dstptr[ x ] = eval( x );
				}

				unmap();
			}

			void map() const
			{
				op1.map();
				op2.map();
			}

			void unmap() const
			{
				op1.unmap();
				op2.unmap();
			}

			void setLine( size_t y ) const
			{
				op1.setLine( y );
				op2.setLine( y );
			}

			float eval( size_t x ) const
			{
				IExprOperation<op> theop;
				return theop( op1.eval( x ), op2.eval( x ) );
			}

			bool hasSizeFormat( size_t width, size_t height, const IFormat& format ) const
			{
				if( !op1.hasSizeFormat( width, height, format ) )
					return false;
				return op2.hasSizeFormat( width, height, format );
			}

			T1		  op1;
			T2		  op2;
	};

	class IExprScalar
	{
		public:
			IExprScalar( float v ) : value( v ) {}

			void  map() const {}
			void  unmap() const {}
			void  setLine( size_t y ) const {}
			float eval( size_t x ) const { return value; }
			bool  hasSizeFormat( size_t width, size_t height, const IFormat& format ) const { return true; }

			float value;
	};

	class IExprImage
	{
		public:
			IExprImage( const Image& i ) : img( i ) {}

			void map() const
			{
				imap = new IMapScoped<const float>( img );
			}

			void unmap() const
			{
				delete imap;
			}

			void setLine( size_t y ) const
			{
				ptr = imap->line( y );
			}

			float eval( size_t x ) const
			{
				return ptr[ x ];
			}

			bool  hasSizeFormat( size_t width, size_t height, const IFormat& format ) const
			{
				return img.width() == width && img.height() == height && img.format() == format;
			}

			const Image&					 img;
			mutable IMapScoped<const float>* imap;
			mutable const float*			 ptr;
	};

	template<typename TX>
	struct IExprTypeFromT {
		typedef TX T;
	};

	template<>
	struct IExprTypeFromT<float> {
		typedef IExprScalar T;
	};

	template<>
	struct IExprTypeFromT<Image> {
		typedef IExprImage T;
	};


    template<typename T1, typename T2, IExprType op>
    std::ostream& operator<<( std::ostream& out, const IExprBinary<T1,T2,op>& expr )
    {
		char opToChar[] = { '+', '-' , '*' };
		out << "(" << expr.op1 << opToChar[ op ] << expr.op2 << ")";
        return out;
    }

    std::ostream& operator<<( std::ostream& out, const IExprScalar& scalar )
    {
		out << scalar.value;
        return out;
    }

    std::ostream& operator<<( std::ostream& out, const IExprImage& image )
    {
		out << "Image";
        return out;
    }


	/*
		SIMD special cases go here:
	 */
#if 0
	template<>
	void IExprBinary<IExprImage,IExprScalar,IEXPR_MUL>::eval( Image& dst )
	{
	}
#endif

	/*
		- Image -> Image * -1
	 */
	IExprBinary<IExprImage,IExprScalar, IEXPR_MUL> operator-( const Image& img )
	{
		return IExprBinary<IExprImage,IExprScalar,IEXPR_MUL>( IExprImage( img ), IExprScalar( -1.0f ) );
	}

	/*
		Image + float
		float + Image
	 */
	IExprBinary<IExprImage,IExprScalar,IEXPR_ADD> operator+( const float val, const Image& img )
	{
		return IExprBinary<IExprImage,IExprScalar,IEXPR_ADD>( IExprImage( img ), IExprScalar( val ) );
	}

	IExprBinary<IExprImage,IExprScalar,IEXPR_ADD> operator+( const Image& img, const float val )
	{
		return IExprBinary<IExprImage,IExprScalar,IEXPR_ADD>( IExprImage( img ), IExprScalar( val ) );
	}

	/*
		Image - float
		float - Image
	 */
	IExprBinary<IExprImage,IExprScalar,IEXPR_SUB> operator-( const float val, const Image& img )
	{
		return IExprBinary<IExprImage,IExprScalar,IEXPR_SUB>( IExprImage( img ), IExprScalar( val ) );
	}

	IExprBinary<IExprImage,IExprScalar,IEXPR_SUB> operator-( const Image& img, const float val )
	{
		return IExprBinary<IExprImage,IExprScalar,IEXPR_SUB>( IExprImage( img ), IExprScalar( val ) );
	}

	/*
		Image * float
		float * Image
	 */
	IExprBinary<IExprImage,IExprScalar,IEXPR_MUL> operator*( const float val, const Image& img )
	{
		return IExprBinary<IExprImage,IExprScalar,IEXPR_MUL>( IExprImage( img ), IExprScalar( val ) );
	}

	IExprBinary<IExprImage,IExprScalar,IEXPR_MUL> operator*( const Image& img, const float val )
	{
		return IExprBinary<IExprImage,IExprScalar,IEXPR_MUL>( IExprImage( img ), IExprScalar( val ) );
	}

	/*
		Image + Image
	 */
	IExprBinary<IExprImage,IExprImage,IEXPR_ADD> operator+( const Image& img1, const Image& img2 )
	{
		return IExprBinary<IExprImage, IExprImage, IEXPR_ADD>( IExprImage( img1), IExprImage( img2 ) );
	}

	/*
		Image - Image
	 */
	IExprBinary<IExprImage,IExprImage,IEXPR_SUB> operator-( const Image& img1, const Image& img2 )
	{
		return IExprBinary<IExprImage, IExprImage, IEXPR_SUB>( IExprImage( img1), IExprImage( img2 ) );
	}

	/*
		Image * Image
	 */
	IExprBinary<IExprImage,IExprImage,IEXPR_MUL> operator*( const Image& img1, const Image& img2 )
	{
		return IExprBinary<IExprImage, IExprImage, IEXPR_MUL>( IExprImage( img1), IExprImage( img2 ) );
	}

	/*
		( Expr + float ) + float -> Expr + float
	 */

	template<typename T1>
	IExprBinary<T1,IExprScalar,IEXPR_ADD> operator+( const IExprBinary<T1,IExprScalar,IEXPR_ADD>& expr1, const float val )
	{
		return IExprBinary<T1,IExprScalar,IEXPR_ADD>( expr1.op1, IExprScalar( expr1.op2.value + val ) );
	}

	/*
		( Expr + float ) - float -> Expr + float
	 */

	template<typename T1>
	IExprBinary<T1,IExprScalar,IEXPR_ADD> operator-( const IExprBinary<T1,IExprScalar,IEXPR_ADD>& expr1, const float val )
	{
		return IExprBinary<T1,IExprScalar,IEXPR_ADD>( expr1.op1, IExprScalar( expr1.op2.value - val ) );
	}


	/*
		( Expr * float ) * float -> Expr * float
	 */

	template<typename T1>
	IExprBinary<T1,IExprScalar,IEXPR_MUL> operator*( const IExprBinary<T1,IExprScalar,IEXPR_MUL>& expr1, const float val )
	{
		return IExprBinary<T1,IExprScalar,IEXPR_MUL>( expr1.op1, IExprScalar( expr1.op2.value * val ) );
	}

	/*
		( Expr1 + float ) + Expr2 -> ( Expr1 + Expr2  ) + float
	 */
	template<typename T1, typename T2>
	IExprBinary<IExprBinary<T1,typename IExprTypeFromT<T2>::T,IEXPR_ADD>, IExprScalar, IEXPR_ADD> operator+( const IExprBinary<T1,IExprScalar,IEXPR_ADD>& expr1, const T2& expr2 )
	{

		return IExprBinary<IExprBinary<T1,typename IExprTypeFromT<T2>::T,IEXPR_ADD>, IExprScalar, IEXPR_ADD>( IExprBinary<T1,typename IExprTypeFromT<T2>::T,IEXPR_ADD>( expr1.op1, typename IExprTypeFromT<T2>::T( expr2 ) ), expr1.op2 );
	}

	/*
		General: ( Expr1 + X )
	 */
	template<typename T1, typename T2, IExprType op, typename T3>
	IExprBinary<IExprBinary<T1,T2,op>,typename IExprTypeFromT<T3>::T,IEXPR_ADD> operator+( const IExprBinary<T1,T2,op>& expr1, const T3& expr2 )
	{
		return IExprBinary<IExprBinary<T1,T2,op>,typename IExprTypeFromT<T3>::T,IEXPR_ADD>( expr1, typename IExprTypeFromT<T3>::T( expr2 ) );
	}

	/*
		General: ( Expr1 - X )
	 */
	template<typename T1, typename T2, IExprType op, typename T3>
	IExprBinary<IExprBinary<T1,T2,op>,typename IExprTypeFromT<T3>::T,IEXPR_SUB> operator-( const IExprBinary<T1,T2,op>& expr1, const T3& expr2 )
	{
		return IExprBinary<IExprBinary<T1,T2,op>,typename IExprTypeFromT<T3>::T,IEXPR_SUB>( expr1, typename IExprTypeFromT<T3>::T( expr2 ) );
	}

	/*
		General: ( Expr1 * X )
	 */
	template<typename T1, typename T2, IExprType op, typename T3>
	IExprBinary<IExprBinary<T1,T2,op>,typename IExprTypeFromT<T3>::T,IEXPR_MUL> operator*( const IExprBinary<T1,T2,op>& expr1, const T3& expr2 )
	{
		return IExprBinary<IExprBinary<T1,T2,op>,typename IExprTypeFromT<T3>::T,IEXPR_MUL>( expr1, typename IExprTypeFromT<T3>::T( expr2 ) );
	}


}


#endif
