#include <cvt/util/SIMD.h>
#include <cvt/util/Time.h>
#include <cvt/util/CVTTest.h>
#include <cvt/math/Math.h>
#include <sstream>

using namespace cvt;

#define AATEST( op, p1, p2, p3, n, op2 ) \
	for( size_t i = 0; i < n; i++ ) {																\
		p2[ i ] = Math::rand( -1e3f, 1e3f );														\
		p3[ i ] = Math::rand( -1e3f, 1e3f );														\
	}																								\
	for( int st = SIMD_BASE; st < SIMD_BEST; st++ ) {												\
		std::stringstream ss;																		\
		SIMD* simd = SIMD::get( ( SIMDType ) st );													\
		bool fail = false;																			\
		for( size_t i = 0; i < n; i++ )																\
			p1[ i ] = 0.0f;																			\
		simd->op( p1, p2, p3, n );																	\
		for( size_t i = 0; i < TESTSIZE; i++ ) {                                                    \
			if( Math::abs( p1[ i ] - ( p2[ i ] op2 p3[ i ] ) ) > Math::EPSILONF ) {					\
				fail = true;																		\
				break;																				\
			}																						\
		}																							\
		ss << simd->name();																			\
		ss << " "#op" array array" ;																\
		delete simd;																				\
		CVTTEST_PRINT( ss.str(), !fail );															\
	}

#define AC1TEST( op, p1, p2, c, n, op2 ) \
	for( size_t i = 0; i < n; i++ ) {																\
		p2[ i ] = Math::rand( -1e3f, 1e3f );														\
		c = Math::rand( -1e3f, 1e3f );																\
	}																								\
	for( int st = SIMD_BASE; st < SIMD_BEST; st++ ) {												\
		std::stringstream ss;																		\
		SIMD* simd = SIMD::get( ( SIMDType ) st );													\
		bool fail = false;																			\
		for( size_t i = 0; i < n; i++ )																\
			p1[ i ] = 0.0f;																			\
		simd->op( p1, p2, c, n );																	\
		for( size_t i = 0; i < TESTSIZE; i++ ) {                                                    \
			if( Math::abs( p1[ i ] - ( p2[ i ] op2 c ) ) > Math::EPSILONF ) {						\
				std::cout << "[ "<< i << " ] : " << p1[ i ] << " <-> " << ( p2[ i ] op2 c ) << std::endl;					\
				fail = true;																		\
				break;																				\
			}																						\
		}																							\
		ss << simd->name();																			\
		ss << " "#op" array const" ;																\
		delete simd;																				\
		CVTTEST_PRINT( ss.str(), !fail );															\
	}

BEGIN_CVTTEST( simd )
		float* fdst;
		float* fsrc1;
		float* fsrc2;
		float val1;
		Time tmr;
		double t;

#define TESTSIZE ( 32 + 3 )
		fdst = new float[ TESTSIZE ];
		fsrc1 = new float[ TESTSIZE ];
		fsrc2 = new float[ TESTSIZE ];

		AATEST( Add, fdst, fsrc1, fsrc2, TESTSIZE, + )
		AATEST( Sub, fdst, fsrc1, fsrc2, TESTSIZE, - )
		AATEST( Mul, fdst, fsrc1, fsrc2, TESTSIZE, * )
		AATEST( Div, fdst, fsrc1, fsrc2, TESTSIZE, / )

		AC1TEST( Add, fdst, fsrc1, val1, TESTSIZE, + )
		AC1TEST( Sub, fdst, fsrc1, val1, TESTSIZE, - )
		AC1TEST( Mul, fdst, fsrc1, val1, TESTSIZE, * )
		AC1TEST( Div, fdst, fsrc1, val1, TESTSIZE, / )

		delete[] fdst;
		delete[] fsrc1;
		delete[] fsrc2;
#undef TESTSIZE


#define TESTSIZE ( 2048 * 2048 )
		fdst = new float[ TESTSIZE ];
		fsrc1 = new float[ TESTSIZE ];
		fsrc2 = new float[ TESTSIZE ];

		for( int st = SIMD_BASE; st < SIMD_BEST; st++ ) {
			SIMD* simd = SIMD::get( ( SIMDType ) st );
			t = 0;
			for( int iter = 0; iter < 100; iter++ ) {
				tmr.reset();
				simd->Add( fdst, fsrc1, fsrc2, TESTSIZE );
				t += tmr.elapsedMilliSeconds();
			}
			t /= 100.0;
			std::cout << simd->name() << " Add "  << t  << " ms" << std::endl;
			delete simd;
		}

		for( int st = SIMD_BASE; st < SIMD_BEST; st++ ) {
			SIMD* simd = SIMD::get( ( SIMDType ) st );
			t = 0;
			for( int iter = 0; iter < 100; iter++ ) {
				tmr.reset();
				simd->Sub( fdst, fsrc1, fsrc2, TESTSIZE );
				t += tmr.elapsedMilliSeconds();
			}
			t /= 100.0;
			std::cout << simd->name() << " Sub "  << t  << " ms" << std::endl;
			delete simd;
		}

		for( int st = SIMD_BASE; st < SIMD_BEST; st++ ) {
			SIMD* simd = SIMD::get( ( SIMDType ) st );
			t = 0;
			for( int iter = 0; iter < 100; iter++ ) {
				tmr.reset();
				simd->Mul( fdst, fsrc1, fsrc2, TESTSIZE );
				t += tmr.elapsedMilliSeconds();
			}
			t /= 100.0;
			std::cout << simd->name() << " Mul "  << t  << " ms" << std::endl;
			delete simd;
		}
            
        for( int st = SIMD_BASE; st < SIMD_BEST; st++ ) {
            SIMD* simd = SIMD::get( ( SIMDType ) st );
            t = 0;
            for( int iter = 0; iter < 100; iter++ ) {
                tmr.reset();
                simd->MulAdd( fdst, fsrc1, 200.0f, TESTSIZE );
                t += tmr.elapsedMilliSeconds();
            }
            t /= 100.0;
            std::cout << simd->name() << " MulAdd "  << t  << " ms" << std::endl;
            delete simd;
        }
            
        for( int st = SIMD_BASE; st < SIMD_BEST; st++ ) {
            SIMD* simd = SIMD::get( ( SIMDType ) st );
            t = 0;
            for( int iter = 0; iter < 100; iter++ ) {
                tmr.reset();
                simd->MulSub( fdst, fsrc1, 200.0f, TESTSIZE );
                t += tmr.elapsedMilliSeconds();
            }
            t /= 100.0;
            std::cout << simd->name() << " MulSub "  << t  << " ms" << std::endl;
            delete simd;
        }

		for( int st = SIMD_BASE; st < SIMD_BEST; st++ ) {
			SIMD* simd = SIMD::get( ( SIMDType ) st );
			t = 0;
			for( int iter = 0; iter < 100; iter++ ) {
				tmr.reset();
				simd->Div( fdst, fsrc1, fsrc2, TESTSIZE );
				t += tmr.elapsedMilliSeconds();
			}
			t /= 100.0;
			std::cout << simd->name() << " Div "  << t  << " ms" << std::endl;
			delete simd;
		}

		for( int st = SIMD_BASE; st < SIMD_BEST; st++ ) {
			SIMD* simd = SIMD::get( ( SIMDType ) st );
			t = 0;
			for( int iter = 0; iter < 100; iter++ ) {
				tmr.reset();
				simd->Add( fdst, fsrc1, 10.0f, TESTSIZE );
				t += tmr.elapsedMilliSeconds();
			}
			t /= 100.0;
			std::cout << simd->name() << " Add Const "  << t  << " ms" << std::endl;
			delete simd;
		}

		for( int st = SIMD_BASE; st < SIMD_BEST; st++ ) {
			SIMD* simd = SIMD::get( ( SIMDType ) st );
			t = 0;
			for( int iter = 0; iter < 100; iter++ ) {
				tmr.reset();
				simd->Sub( fdst, fsrc1, 10.0f, TESTSIZE );
				t += tmr.elapsedMilliSeconds();
			}
			t /= 100.0;
			std::cout << simd->name() << " Sub Const "  << t  << " ms" << std::endl;
			delete simd;
		}

		for( int st = SIMD_BASE; st < SIMD_BEST; st++ ) {
			SIMD* simd = SIMD::get( ( SIMDType ) st );
			t = 0;
			for( int iter = 0; iter < 100; iter++ ) {
				tmr.reset();
				simd->Mul( fdst, fsrc1, 10.0f, TESTSIZE );
				t += tmr.elapsedMilliSeconds();
			}
			t /= 100.0;
			std::cout << simd->name() << " Mul Const "  << t  << " ms" << std::endl;
			delete simd;
		}

		for( int st = SIMD_BASE; st < SIMD_BEST; st++ ) {
			SIMD* simd = SIMD::get( ( SIMDType ) st );
			t = 0;
			for( int iter = 0; iter < 100; iter++ ) {
				tmr.reset();
				simd->Div( fdst, fsrc1, 10.0f, TESTSIZE );
				t += tmr.elapsedMilliSeconds();
			}
			t /= 100.0;
			std::cout << simd->name() << " Div Const "  << t  << " ms" << std::endl;
			delete simd;
		}
        
            uint8_t * usrc0 = new uint8_t[ TESTSIZE ];
            for( int x = 0; x < TESTSIZE; x++ )
                usrc0[ x ] = Math::max( x, 255 );
        
            for( int st = SIMD_BASE; st < SIMD_BEST; st++ ) {
                SIMD* simd = SIMD::get( ( SIMDType ) st );
                t = 0;
                for( int iter = 0; iter < 100; iter++ ) {
                    tmr.reset();
                    simd->prefixSum1( fdst, 0, usrc0, 0, TESTSIZE, 1 );
                    t += tmr.elapsedMilliSeconds();
                }
                t /= 100.0;
                std::cout << simd->name() << " prefixSum1 Const "  << t  << " ms" << std::endl;
                
                for( int x = 0; x < 100; x++ )
                    std::cout << fdst[ x ] << " ";
                std::cout << std::endl;
                
                delete simd;
            }
        
		
		
		uint8_t * udst = new uint8_t[ TESTSIZE ];
		for( int st = SIMD_BASE; st < SIMD_BEST; st++ ) {
			SIMD* simd = SIMD::get( ( SIMDType ) st );
			t = 0;
			tmr.reset();
			for( int iter = 0; iter < 100; iter++ ) {				
				simd->SAD( udst, usrc0, TESTSIZE );
			}
			t += tmr.elapsedMilliSeconds();
			t /= 100.0;
			std::cout << simd->name() << " SAD "  << t  << " ms" << std::endl;
			delete simd;
		}
		delete[] usrc0;
		delete[] udst;	
			
		delete[] fdst;
		delete[] fsrc1;
		delete[] fsrc2;
#undef TESTSIZE       

		return true;
	END_CVTTEST
