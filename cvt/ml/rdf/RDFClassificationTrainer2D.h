#ifndef CVT_RDFORESTTRAINERCLASSIFICATION2D_H
#define CVT_RDFORESTTRAINERCLASSIFICATION2D_H

#include <cvt/math/Vector.h>
#include <cvt/ml/rdf/RDFClassificationTrainer.h>

namespace cvt {

		class RDFTestLinear2D : public RDFTest<Vector2f>
		{
			public:
				RDFTestLinear2D( const Vector2f& vec, float threshold  ) : _norm( vec ), _threshold( threshold ) {}

				bool operator()( const Vector2f& other )
				{
					return ( _norm.x *  other.x + _norm.y * other.y ) < _threshold;
				}

			private:
				Vector2f _norm;
				float	 _threshold;
		};

		class RDForestTrainerClassification2D : public RDFClassificationTrainer<Vector2f,std::vector<Vector3f>,2>
		{
			public:
				RDForestTrainerClassification2D( size_t numberOfClasses ) : _numClasses( numberOfClasses )
				{
				}

				~RDForestTrainerClassification2D()
				{
				}

				virtual size_t dataSize( const std::vector<Vector3f>& data )
				{
					return data.size();
				}

				virtual RDFTest<Vector2f>*  randomTest()
				{
					float x = Math::rand( -1.0f, 1.0f );
					float y = Math::sqrt( 1.0f - Math::sqr( x ) );
					return new RDFTestLinear2D( Vector2f( x, y ), Math::rand( -2000.0f, 2000.0f  ) );
				}

				virtual size_t classLabel( const std::vector<Vector3f>& data, size_t index )
				{
					return ( size_t ) data[ index ].z;
				}


				virtual Vector2f& trainingData(  const std::vector<Vector3f>& data, size_t index ) {
					return *( ( Vector2f*) ( &data[ index ] ) );
				}

			private:
				size_t _numClasses;
		};

}

#endif
