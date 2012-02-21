/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/math/Vector.h>
#include <cvt/util/CVTTest.h>

#include <cvt/util/DataIterator.h>

namespace cvt {

    template<>
    Vector2<int> Vector2<int>::fromString( const String& s )
    {
        Vector2<int> m;

        DataIterator it( s );
        String token;
        String deliminators("\n\r\t ");
        for( size_t i = 0; i < 2; i++ ){
            if( !it.nextToken( token, deliminators ) )
                throw CVTException( "Could not create Matrix from String!" );
            m[ i ] = token.toInteger();
        }

        return m;
    }

    template<>
    Vector2<float> Vector2<float>::fromString( const String& s )
    {
        Vector2<float> m;

        DataIterator it( s );
        String token;
        String deliminators("\n\r\t ");
        for( size_t i = 0; i < 2; i++ ){
            if( !it.nextToken( token, deliminators ) )
                throw CVTException( "Could not create Matrix from String!" );
            m[ i ] = token.toFloat();
        }

        return m;
    }

    template<>
    Vector2<double> Vector2<double>::fromString( const String& s )
    {
        Vector2<double> m;

        DataIterator it( s );
        String token;
        String deliminators("\n\r\t ");
        for( size_t i = 0; i < 2; i++ ){
            if( !it.nextToken( token, deliminators ) )
                throw CVTException( "Could not create Matrix from String!" );
            m[ i ] = token.toDouble();
        }

        return m;
    }

    template<>
    Vector3<int> Vector3<int>::fromString( const String& s )
    {
        Vector3<int> m;

        DataIterator it( s );
        String token;
        String deliminators("\n\r\t ");
        for( size_t i = 0; i < 3; i++ ){
            if( !it.nextToken( token, deliminators ) )
                throw CVTException( "Could not create Matrix from String!" );
            m[ i ] = token.toInteger();
        }

        return m;
    }

    template<>
    Vector3<float> Vector3<float>::fromString( const String& s )
    {
        Vector3<float> m;

        DataIterator it( s );
        String token;
        String deliminators("\n\r\t ");
        for( size_t i = 0; i < 3; i++ ){
            if( !it.nextToken( token, deliminators ) )
                throw CVTException( "Could not create Matrix from String!" );
            m[ i ] = token.toFloat();
        }

        return m;
    }

    template<>
    Vector3<double> Vector3<double>::fromString( const String& s )
    {
        Vector3<double> m;

        DataIterator it( s );
        String token;
        String deliminators("\n\r\t ");
        for( size_t i = 0; i < 3; i++ ){
            if( !it.nextToken( token, deliminators ) )
                throw CVTException( "Could not create Matrix from String!" );
            m[ i ] = token.toDouble();
        }

        return m;
    }

    template<>
    Vector4<int> Vector4<int>::fromString( const String& s )
    {
        Vector4<int> m;

        DataIterator it( s );
        String token;
        String deliminators("\n\r\t ");
        for( size_t i = 0; i < 4; i++ ){
            if( !it.nextToken( token, deliminators ) )
                throw CVTException( "Could not create Matrix from String!" );
            m[ i ] = token.toInteger();
        }

        return m;
    }

    template<>
    Vector4<float> Vector4<float>::fromString( const String& s )
    {
        Vector4<float> m;

        DataIterator it( s );
        String token;
        String deliminators("\n\r\t ");
        for( size_t i = 0; i < 4; i++ ){
            if( !it.nextToken( token, deliminators ) )
                throw CVTException( "Could not create Matrix from String!" );
            m[ i ] = token.toFloat();
        }

        return m;
    }

    template<>
    Vector4<double> Vector4<double>::fromString( const String& s )
    {
        Vector4<double> m;

        DataIterator it( s );
        String token;
        String deliminators("\n\r\t ");
        for( size_t i = 0; i < 4; i++ ){
            if( !it.nextToken( token, deliminators ) )
                throw CVTException( "Could not create Matrix from String!" );
            m[ i ] = token.toDouble();
        }

        return m;
    }


	BEGIN_CVTTEST(Vector2)
		Vector2f vec;
		Vector2f zero( 0.0f, 0.0f );
		bool b = true;

		vec.setZero();
		b = b && vec == zero;
		CVTTEST_PRINT( "zero()", b );

		vec.set( 0.5f, 2.0f );
		b = b && vec == Vector2f( 0.5f, 2.0f );
		CVTTEST_PRINT( "set()", b );

		vec[ 0 ] = 10.0f;
		vec[ 1 ] = 20.0f;
		b = b && vec == Vector2f( 10.0f, 20.0f );
		CVTTEST_PRINT( "operator[]", b );

		b = b && Math::abs( vec[ 0 ] - 10.0f ) < Math::EPSILONF && Math::abs( vec[ 1 ] - 20.0f ) < Math::EPSILONF;
		CVTTEST_PRINT( "const operator[]", b );

		vec.setZero();
		vec += 1.0f;
		b = b && vec == Vector2f( 1.0f, 1.0f );
		CVTTEST_PRINT( "operator+=", b );

		vec.setZero();
		vec -= 1.0f;
		b = b && vec == Vector2f( -1.0f, -1.0f );
		CVTTEST_PRINT( "operator-=", b );

		vec *= -1.0f;
		b = b && vec == Vector2f( 1.0f, 1.0f );
		CVTTEST_PRINT( "operator*=", b );

		vec /= 2.0f;
		b = b && vec == Vector2f( 0.5f, 0.5f );
		CVTTEST_PRINT( "operator/=", b );

		b = b && Math::abs( vec.lengthSqr() - vec * vec ) < Math::EPSILONF;
		CVTTEST_PRINT( "lengthSqr", b );

		b = b && Math::abs( vec.length() - Math::sqrt( vec * vec ) ) < Math::EPSILONF;
		CVTTEST_PRINT( "length", b );

        String s = vec.toString();
        Vector2f vString = Vector2f::fromString( s );
        b = b && ( vString == vec );
        CVTTEST_PRINT( "fromString", b );

		return b;
	END_CVTTEST

	BEGIN_CVTTEST(Vector3)
		Vector3f vec;
		Vector3f zero( 0.0f, 0.0f, 0.0f );
		bool b = true;

		vec.setZero();
		b = b && vec == zero;
		CVTTEST_PRINT( "zero()", b );

		vec.set( 0.5f, 2.0f, -5.0f );
		b = b && vec == Vector3f( 0.5f, 2.0f, -5.0f );
		CVTTEST_PRINT( "set()", b );

		vec[ 0 ] = 10.0f;
		vec[ 1 ] = 20.0f;
		vec[ 2 ] = 30.0f;
		b = b && vec == Vector3f( 10.0f, 20.0f, 30.0f );
		CVTTEST_PRINT( "operator[]", b );

		b = b && Math::abs( vec[ 0 ] - 10.0f ) < Math::EPSILONF
			  && Math::abs( vec[ 1 ] - 20.0f ) < Math::EPSILONF
			  && Math::abs( vec[ 2 ] - 30.0f ) < Math::EPSILONF;
		CVTTEST_PRINT( "const operator[]", b );

		vec.setZero();
		vec += 1.0f;
		b = b && vec == Vector3f( 1.0f, 1.0f, 1.0f );
		CVTTEST_PRINT( "operator+=", b );

		vec.setZero();
		vec -= 1.0f;
		b = b && vec == Vector3f( -1.0f, -1.0f, -1.0f );
		CVTTEST_PRINT( "operator-=", b );

		vec *= -1.0f;
		b = b && vec == Vector3f( 1.0f, 1.0f, 1.0f );
		CVTTEST_PRINT( "operator*=", b );

		vec /= 2.0f;
		b = b && vec == Vector3f( 0.5f, 0.5f, 0.5f );
		CVTTEST_PRINT( "operator/=", b );

		b = b && Math::abs( vec.lengthSqr() - vec * vec ) < Math::EPSILONF;
		CVTTEST_PRINT( "lengthSqr", b );

		b = b && Math::abs( vec.length() - Math::sqrt( vec * vec ) ) < Math::EPSILONF;
		CVTTEST_PRINT( "length", b );

        String s = vec.toString();
        Vector3f vString = Vector3f::fromString( s );
        b = b && ( vString == vec );
        CVTTEST_PRINT( "fromString", b );

		return b;
	END_CVTTEST

	BEGIN_CVTTEST(Vector4)
		Vector4f vec;
		Vector4f zero( 0.0f, 0.0f, 0.0f, 0.0f );
		bool b = true;

		vec.setZero();
		b = b && vec == zero;
		CVTTEST_PRINT( "zero()", b );

		vec.set( 0.5f, 2.0f, -5.0f, 8.0f );
		b = b && vec == Vector4f( 0.5f, 2.0f, -5.0f, 8.0f );
		CVTTEST_PRINT( "set()", b );

		vec[ 0 ] = 10.0f;
		vec[ 1 ] = 20.0f;
		vec[ 2 ] = 30.0f;
		vec[ 3 ] = 40.0f;
		b = b && vec == Vector4f( 10.0f, 20.0f, 30.0f, 40.0f );
		CVTTEST_PRINT( "operator[]", b );

		b = b && Math::abs( vec[ 0 ] - 10.0f ) < Math::EPSILONF
			  && Math::abs( vec[ 1 ] - 20.0f ) < Math::EPSILONF
			  && Math::abs( vec[ 2 ] - 30.0f ) < Math::EPSILONF
			  && Math::abs( vec[ 3 ] - 40.0f ) < Math::EPSILONF;
		CVTTEST_PRINT( "const operator[]", b );

		vec.setZero();
		vec += 1.0f;
		b = b && vec == Vector4f( 1.0f, 1.0f, 1.0f, 1.0f );
		CVTTEST_PRINT( "operator+=", b );

		vec.setZero();
		vec -= 1.0f;
		b = b && vec == Vector4f( -1.0f, -1.0f, -1.0f, -1.0f );
		CVTTEST_PRINT( "operator-=", b );

		vec *= -1.0f;
		b = b && vec == Vector4f( 1.0f, 1.0f, 1.0f, 1.0f );
		CVTTEST_PRINT( "operator*=", b );

		vec /= 2.0f;
		b = b && vec == Vector4f( 0.5f, 0.5f, 0.5f, 0.5f );
		CVTTEST_PRINT( "operator/=", b );

		b = b && Math::abs( vec.lengthSqr() - vec * vec ) < Math::EPSILONF;
		CVTTEST_PRINT( "lengthSqr", b );

		b = b && Math::abs( vec.length() - Math::sqrt( vec * vec ) ) < Math::EPSILONF;
		CVTTEST_PRINT( "length", b );

        String s = vec.toString();
        Vector4f vString = Vector4f::fromString( s );
        b = b && ( vString == vec );
        CVTTEST_PRINT( "fromString", b );

		return b;
	END_CVTTEST


}
