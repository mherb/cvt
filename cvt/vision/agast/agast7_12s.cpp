#include <cvt/vision/AGAST.h>

namespace cvt
{
	void AGAST::init7_12s_pattern( size_t stride )
	{
		if( stride == _lastStride )
			return;
		_lastStride = stride;

		_offset0=(-2);
		_offset1=(-2)+(-1)*_lastStride;
		_offset2=(-1)+(-2)*_lastStride;
		_offset3=(0)+(-2)*_lastStride;
		_offset4=(1)+(-2)*_lastStride;
		_offset5=(2)+(-1)*_lastStride;
		_offset6=(2)+(0)*_lastStride;
		_offset7=(2)+(1)*_lastStride;
		_offset8=(1)+(2)*_lastStride;
		_offset9=(0)+(2)*_lastStride;
		_offset10=(-1)+(2)*_lastStride;
		_offset11=(-2)+(1)*_lastStride;
	}

	void AGAST::agast7_12s( const uint8_t* im, size_t stride, size_t xsize, size_t ysize, std::vector<Feature2D> & corners )
	{
		register int y;
		register int x;
		register int xsizeB=xsize - 2; //2, +1 due to faster test x>xsizeB
		register int ysizeB=ysize - 2;
		int	nExpectedCorners=512;
		int total=0;

		corners.reserve( nExpectedCorners );
		init7_12s_pattern( stride );

		for(y=2; y < ysizeB; y++)
		{
			x=2;
			while(1)
			{
homogeneous:
				{
					x++;
					if(x>xsizeB)
						break;
					register const uint8_t* const p = im + y*_lastStride + x;
					register const int cb = *p + _threshold;
					register const int c_b = *p - _threshold;
					if(p[_offset0] > cb) 
						if(p[_offset2] > cb) 
							if(p[_offset5] > cb) 
								if(p[_offset9] > cb) 
									if(p[_offset7] > cb) 
										if(p[_offset1] > cb) 
											if(p[_offset6] > cb) 
												if(p[_offset3] > cb) 
													if(p[_offset4] > cb) 
														goto success_structured;
													else 
														if(p[_offset10] > cb) 
															if(p[_offset11] > cb) 
																goto success_structured;
															else 
																goto structured;
														else 
															goto homogeneous;
												else 
													if(p[_offset8] > cb) 
														if(p[_offset10] > cb) 
															if(p[_offset4] > cb) 
																goto success_structured;
															else 
																if(p[_offset11] > cb) 
																	goto success_structured;
																else 
																	goto structured;
														else 
															goto homogeneous;
													else 
														goto homogeneous;
											else 
												if(p[_offset11] > cb) 
													if(p[_offset3] > cb) 
														if(p[_offset4] > cb) 
															goto success_structured;
														else 
															if(p[_offset10] > cb) 
																goto success_structured;
															else 
																goto homogeneous;
													else 
														if(p[_offset8] > cb) 
															if(p[_offset10] > cb) 
																goto success_structured;
															else 
																goto homogeneous;
														else 
															goto homogeneous;
												else 
													goto homogeneous;
										else 
											if(p[_offset6] > cb) 
												if(p[_offset8] > cb) 
													if(p[_offset4] > cb) 
														if(p[_offset3] > cb) 
															goto success_structured;
														else 
															if(p[_offset10] > cb) 
																goto success_structured;
															else 
																goto homogeneous;
													else 
														if(p[_offset10] > cb) 
															if(p[_offset11] > cb) 
																goto success_structured;
															else 
																goto homogeneous;
														else 
															goto homogeneous;
												else 
													goto homogeneous;
											else 
												goto homogeneous;
									else 
										if(p[_offset1] > cb) 
											if(p[_offset11] > cb) 
												if(p[_offset3] > cb) 
													if(p[_offset4] > cb) 
														goto success_homogeneous;
													else 
														if(p[_offset10] > cb) 
															goto success_homogeneous;
														else 
															goto homogeneous;
												else 
													if(p[_offset8] > cb) 
														if(p[_offset10] > cb) 
															goto success_homogeneous;
														else 
															goto homogeneous;
													else 
														goto homogeneous;
											else 
												if(p[_offset6] > cb) 
													if(p[_offset3] > cb) 
														if(p[_offset4] > cb) 
															goto success_homogeneous;
														else 
															goto homogeneous;
													else 
														goto homogeneous;
												else 
													goto homogeneous;
										else 
											goto homogeneous;
								else 
									if(p[_offset3] > cb) 
										if(p[_offset4] > cb) 
											if(p[_offset7] > cb) 
												if(p[_offset1] > cb) 
													if(p[_offset6] > cb) 
														goto success_homogeneous;
													else 
														if(p[_offset11] > cb) 
															goto success_homogeneous;
														else 
															goto homogeneous;
												else 
													if(p[_offset6] > cb) 
														if(p[_offset8] > cb) 
															goto success_homogeneous;
														else 
															goto homogeneous;
													else 
														goto homogeneous;
											else 
												if(p[_offset1] > cb) 
													if(p[_offset6] > cb) 
														goto success_homogeneous;
													else 
														if(p[_offset11] > cb) 
															goto success_homogeneous;
														else 
															goto homogeneous;
												else 
													goto homogeneous;
										else 
											goto homogeneous;
									else 
										goto homogeneous;
							else 
								if(p[_offset9] < c_b) 
									if(p[_offset7] < c_b) 
										if(p[_offset5] < c_b) 
											if(p[_offset1] > cb) 
												if(p[_offset4] > cb) 
													if(p[_offset10] > cb) 
														if(p[_offset3] > cb) 
															if(p[_offset11] > cb) 
																goto success_structured;
															else 
																goto structured;
														else 
															goto homogeneous;
													else 
														if(p[_offset6] < c_b) 
															if(p[_offset8] < c_b) 
																if(p[_offset11] < c_b) 
																	if(p[_offset10] < c_b) 
																		goto success_structured;
																	else 
																		goto structured;
																else 
																	goto structured;
															else 
																goto homogeneous;
														else 
															goto homogeneous;
												else 
													if(p[_offset6] < c_b) 
														if(p[_offset8] < c_b) 
															if(p[_offset10] < c_b) 
																if(p[_offset4] < c_b) 
																	goto success_structured;
																else 
																	if(p[_offset11] < c_b) 
																		goto success_structured;
																	else 
																		goto structured;
															else 
																if(p[_offset3] < c_b) 
																	if(p[_offset4] < c_b) 
																		goto success_structured;
																	else 
																		goto structured;
																else 
																	goto homogeneous;
														else 
															goto homogeneous;
													else 
														goto homogeneous;
											else 
												if(p[_offset6] < c_b) 
													if(p[_offset8] < c_b) 
														if(p[_offset4] < c_b) 
															if(p[_offset3] < c_b) 
																goto success_structured;
															else 
																if(p[_offset10] < c_b) 
																	goto success_structured;
																else 
																	goto homogeneous;
														else 
															if(p[_offset10] < c_b) 
																if(p[_offset11] < c_b) 
																	goto success_structured;
																else 
																	goto homogeneous;
															else 
																goto homogeneous;
													else 
														goto homogeneous;
												else 
													goto homogeneous;
										else 
											if(p[_offset1] > cb) 
												if(p[_offset3] > cb) 
													if(p[_offset4] > cb) 
														if(p[_offset10] > cb) 
															if(p[_offset11] > cb) 
																goto success_structured;
															else 
																goto homogeneous;
														else 
															goto homogeneous;
													else 
														goto homogeneous;
												else 
													goto homogeneous;
											else 
												goto homogeneous;
									else 
										if(p[_offset1] > cb) 
											if(p[_offset3] > cb) 
												if(p[_offset4] > cb) 
													if(p[_offset10] > cb) 
														if(p[_offset11] > cb) 
															goto success_homogeneous;
														else 
															goto homogeneous;
													else 
														goto homogeneous;
												else 
													goto homogeneous;
											else 
												goto homogeneous;
										else 
											goto homogeneous;
								else 
									if(p[_offset10] > cb) 
										if(p[_offset11] > cb) 
											if(p[_offset9] > cb) 
												if(p[_offset7] > cb) 
													if(p[_offset1] > cb) 
														if(p[_offset3] > cb) 
															goto success_homogeneous;
														else 
															if(p[_offset8] > cb) 
																goto success_homogeneous;
															else 
																goto homogeneous;
													else 
														if(p[_offset6] > cb) 
															if(p[_offset8] > cb) 
																goto success_homogeneous;
															else 
																goto homogeneous;
														else 
															goto homogeneous;
												else 
													if(p[_offset1] > cb) 
														if(p[_offset3] > cb) 
															goto success_homogeneous;
														else 
															if(p[_offset8] > cb) 
																goto success_homogeneous;
															else 
																goto homogeneous;
													else 
														goto homogeneous;
											else 
												if(p[_offset1] > cb) 
													if(p[_offset3] > cb) 
														if(p[_offset4] > cb) 
															goto success_homogeneous;
														else 
															goto homogeneous;
													else 
														goto homogeneous;
												else 
													goto homogeneous;
										else 
											goto homogeneous;
									else 
										goto homogeneous;
						else 
							if(p[_offset7] > cb) 
								if(p[_offset9] > cb) 
									if(p[_offset8] > cb) 
										if(p[_offset5] > cb) 
											if(p[_offset1] > cb) 
												if(p[_offset10] > cb) 
													if(p[_offset11] > cb) 
														goto success_homogeneous;
													else 
														if(p[_offset6] > cb) 
															if(p[_offset4] > cb) 
																goto success_structured;
															else 
																goto homogeneous;
														else 
															goto homogeneous;
												else 
													if(p[_offset6] > cb) 
														if(p[_offset3] > cb) 
															if(p[_offset4] > cb) 
																goto success_structured;
															else 
																goto homogeneous;
														else 
															goto homogeneous;
													else 
														goto homogeneous;
											else 
												if(p[_offset6] > cb) 
													if(p[_offset4] > cb) 
														if(p[_offset3] > cb) 
															goto success_homogeneous;
														else 
															if(p[_offset10] > cb) 
																goto success_homogeneous;
															else 
																goto homogeneous;
													else 
														if(p[_offset10] > cb) 
															if(p[_offset11] > cb) 
																goto success_homogeneous;
															else 
																goto homogeneous;
														else 
															goto homogeneous;
												else 
													goto homogeneous;
										else 
											if(p[_offset10] > cb) 
												if(p[_offset11] > cb) 
													if(p[_offset1] > cb) 
														goto success_homogeneous;
													else 
														if(p[_offset6] > cb) 
															goto success_homogeneous;
														else 
															goto homogeneous;
												else 
													goto homogeneous;
											else 
												goto homogeneous;
									else 
										goto homogeneous;
								else 
									goto homogeneous;
							else 
								if(p[_offset7] < c_b) 
									if(p[_offset5] < c_b) 
										if(p[_offset2] < c_b) 
											if(p[_offset6] < c_b) 
												if(p[_offset4] < c_b) 
													if(p[_offset3] < c_b) 
														if(p[_offset1] < c_b) 
															goto success_homogeneous;
														else 
															if(p[_offset8] < c_b) 
																goto success_homogeneous;
															else 
																goto homogeneous;
													else 
														if(p[_offset9] < c_b) 
															if(p[_offset8] < c_b) 
																if(p[_offset10] < c_b) 
																	goto success_structured;
																else 
																	goto homogeneous;
															else 
																goto homogeneous;
														else 
															goto homogeneous;
												else 
													if(p[_offset9] < c_b) 
														if(p[_offset8] < c_b) 
															if(p[_offset10] < c_b) 
																if(p[_offset11] < c_b) 
																	goto success_structured;
																else 
																	goto homogeneous;
															else 
																goto homogeneous;
														else 
															goto homogeneous;
													else 
														goto homogeneous;
											else 
												goto homogeneous;
										else 
											if(p[_offset9] < c_b) 
												if(p[_offset6] < c_b) 
													if(p[_offset8] < c_b) 
														if(p[_offset4] < c_b) 
															if(p[_offset3] < c_b) 
																goto success_homogeneous;
															else 
																if(p[_offset10] < c_b) 
																	goto success_homogeneous;
																else 
																	goto homogeneous;
														else 
															if(p[_offset10] < c_b) 
																if(p[_offset11] < c_b) 
																	goto success_homogeneous;
																else 
																	goto homogeneous;
															else 
																goto homogeneous;
													else 
														goto homogeneous;
												else 
													goto homogeneous;
											else 
												goto homogeneous;
									else 
										goto homogeneous;
								else 
									goto homogeneous;
					else if(p[_offset0] < c_b) 
						if(p[_offset2] < c_b) 
							if(p[_offset9] < c_b) 
								if(p[_offset5] < c_b) 
									if(p[_offset7] < c_b) 
										if(p[_offset1] < c_b) 
											if(p[_offset6] < c_b) 
												if(p[_offset3] < c_b) 
													if(p[_offset4] < c_b) 
														goto success_structured;
													else 
														if(p[_offset10] < c_b) 
															if(p[_offset11] < c_b) 
																goto success_structured;
															else 
																goto structured;
														else 
															goto homogeneous;
												else 
													if(p[_offset8] < c_b) 
														if(p[_offset10] < c_b) 
															if(p[_offset4] < c_b) 
																goto success_structured;
															else 
																if(p[_offset11] < c_b) 
																	goto success_structured;
																else 
																	goto structured;
														else 
															goto homogeneous;
													else 
														goto homogeneous;
											else 
												if(p[_offset11] < c_b) 
													if(p[_offset3] < c_b) 
														if(p[_offset4] < c_b) 
															goto success_structured;
														else 
															if(p[_offset10] < c_b) 
																goto success_structured;
															else 
																goto homogeneous;
													else 
														if(p[_offset8] < c_b) 
															if(p[_offset10] < c_b) 
																goto success_structured;
															else 
																goto homogeneous;
														else 
															goto homogeneous;
												else 
													goto homogeneous;
										else 
											if(p[_offset6] < c_b) 
												if(p[_offset8] < c_b) 
													if(p[_offset4] < c_b) 
														if(p[_offset3] < c_b) 
															goto success_structured;
														else 
															if(p[_offset10] < c_b) 
																goto success_structured;
															else 
																goto homogeneous;
													else 
														if(p[_offset10] < c_b) 
															if(p[_offset11] < c_b) 
																goto success_structured;
															else 
																goto homogeneous;
														else 
															goto homogeneous;
												else 
													goto homogeneous;
											else 
												goto homogeneous;
									else 
										if(p[_offset1] < c_b) 
											if(p[_offset11] < c_b) 
												if(p[_offset3] < c_b) 
													if(p[_offset4] < c_b) 
														goto success_homogeneous;
													else 
														if(p[_offset10] < c_b) 
															goto success_homogeneous;
														else 
															goto homogeneous;
												else 
													if(p[_offset8] < c_b) 
														if(p[_offset10] < c_b) 
															goto success_homogeneous;
														else 
															goto homogeneous;
													else 
														goto homogeneous;
											else 
												if(p[_offset6] < c_b) 
													if(p[_offset3] < c_b) 
														if(p[_offset4] < c_b) 
															goto success_homogeneous;
														else 
															goto homogeneous;
													else 
														goto homogeneous;
												else 
													goto homogeneous;
										else 
											goto homogeneous;
								else 
									if(p[_offset10] < c_b) 
										if(p[_offset11] < c_b) 
											if(p[_offset7] < c_b) 
												if(p[_offset1] < c_b) 
													if(p[_offset3] < c_b) 
														goto success_homogeneous;
													else 
														if(p[_offset8] < c_b) 
															goto success_homogeneous;
														else 
															goto homogeneous;
												else 
													if(p[_offset6] < c_b) 
														if(p[_offset8] < c_b) 
															goto success_homogeneous;
														else 
															goto homogeneous;
													else 
														goto homogeneous;
											else 
												if(p[_offset1] < c_b) 
													if(p[_offset3] < c_b) 
														goto success_homogeneous;
													else 
														if(p[_offset8] < c_b) 
															goto success_homogeneous;
														else 
															goto homogeneous;
												else 
													goto homogeneous;
										else 
											goto homogeneous;
									else 
										goto homogeneous;
							else 
								if(p[_offset9] > cb) 
									if(p[_offset5] > cb) 
										if(p[_offset7] > cb) 
											if(p[_offset1] < c_b) 
												if(p[_offset4] < c_b) 
													if(p[_offset10] < c_b) 
														if(p[_offset3] < c_b) 
															if(p[_offset11] < c_b) 
																goto success_structured;
															else 
																goto structured;
														else 
															goto homogeneous;
													else 
														if(p[_offset6] > cb) 
															if(p[_offset8] > cb) 
																if(p[_offset11] > cb) 
																	if(p[_offset10] > cb) 
																		goto success_structured;
																	else 
																		goto structured;
																else 
																	goto structured;
															else 
																goto homogeneous;
														else 
															goto homogeneous;
												else 
													if(p[_offset6] > cb) 
														if(p[_offset8] > cb) 
															if(p[_offset10] > cb) 
																if(p[_offset4] > cb) 
																	goto success_structured;
																else 
																	if(p[_offset11] > cb) 
																		goto success_structured;
																	else 
																		goto structured;
															else 
																if(p[_offset3] > cb) 
																	if(p[_offset4] > cb) 
																		goto success_structured;
																	else 
																		goto structured;
																else 
																	goto homogeneous;
														else 
															goto homogeneous;
													else 
														goto homogeneous;
											else 
												if(p[_offset6] > cb) 
													if(p[_offset8] > cb) 
														if(p[_offset4] > cb) 
															if(p[_offset3] > cb) 
																goto success_structured;
															else 
																if(p[_offset10] > cb) 
																	goto success_structured;
																else 
																	goto homogeneous;
														else 
															if(p[_offset10] > cb) 
																if(p[_offset11] > cb) 
																	goto success_structured;
																else 
																	goto homogeneous;
															else 
																goto homogeneous;
													else 
														goto homogeneous;
												else 
													goto homogeneous;
										else 
											if(p[_offset1] < c_b) 
												if(p[_offset3] < c_b) 
													if(p[_offset4] < c_b) 
														if(p[_offset10] < c_b) 
															if(p[_offset11] < c_b) 
																goto success_structured;
															else 
																goto homogeneous;
														else 
															goto homogeneous;
													else 
														goto homogeneous;
												else 
													goto homogeneous;
											else 
												goto homogeneous;
									else 
										if(p[_offset3] < c_b) 
											if(p[_offset4] < c_b) 
												if(p[_offset5] < c_b) 
													if(p[_offset7] < c_b) 
														if(p[_offset1] < c_b) 
															if(p[_offset6] < c_b) 
																goto success_structured;
															else 
																if(p[_offset11] < c_b) 
																	goto success_structured;
																else 
																	goto homogeneous;
														else 
															if(p[_offset6] < c_b) 
																if(p[_offset8] < c_b) 
																	goto success_structured;
																else 
																	goto homogeneous;
															else 
																goto homogeneous;
													else 
														if(p[_offset1] < c_b) 
															if(p[_offset6] < c_b) 
																goto success_homogeneous;
															else 
																if(p[_offset11] < c_b) 
																	goto success_homogeneous;
																else 
																	goto homogeneous;
														else 
															goto homogeneous;
												else 
													if(p[_offset1] < c_b) 
														if(p[_offset10] < c_b) 
															if(p[_offset11] < c_b) 
																goto success_homogeneous;
															else 
																goto homogeneous;
														else 
															goto homogeneous;
													else 
														goto homogeneous;
											else 
												goto homogeneous;
										else 
											goto homogeneous;
								else 
									if(p[_offset3] < c_b) 
										if(p[_offset4] < c_b) 
											if(p[_offset5] < c_b) 
												if(p[_offset7] < c_b) 
													if(p[_offset1] < c_b) 
														if(p[_offset6] < c_b) 
															goto success_homogeneous;
														else 
															if(p[_offset11] < c_b) 
																goto success_homogeneous;
															else 
																goto homogeneous;
													else 
														if(p[_offset6] < c_b) 
															if(p[_offset8] < c_b) 
																goto success_homogeneous;
															else 
																goto homogeneous;
														else 
															goto homogeneous;
												else 
													if(p[_offset1] < c_b) 
														if(p[_offset6] < c_b) 
															goto success_homogeneous;
														else 
															if(p[_offset11] < c_b) 
																goto success_homogeneous;
															else 
																goto homogeneous;
													else 
														goto homogeneous;
											else 
												if(p[_offset1] < c_b) 
													if(p[_offset10] < c_b) 
														if(p[_offset11] < c_b) 
															goto success_homogeneous;
														else 
															goto homogeneous;
													else 
														goto homogeneous;
												else 
													goto homogeneous;
										else 
											goto homogeneous;
									else 
										goto homogeneous;
						else 
							if(p[_offset7] > cb) 
								if(p[_offset5] > cb) 
									if(p[_offset2] > cb) 
										if(p[_offset6] > cb) 
											if(p[_offset4] > cb) 
												if(p[_offset3] > cb) 
													if(p[_offset1] > cb) 
														goto success_homogeneous;
													else 
														if(p[_offset8] > cb) 
															goto success_homogeneous;
														else 
															goto homogeneous;
												else 
													if(p[_offset9] > cb) 
														if(p[_offset8] > cb) 
															if(p[_offset10] > cb) 
																goto success_structured;
															else 
																goto homogeneous;
														else 
															goto homogeneous;
													else 
														goto homogeneous;
											else 
												if(p[_offset9] > cb) 
													if(p[_offset8] > cb) 
														if(p[_offset10] > cb) 
															if(p[_offset11] > cb) 
																goto success_structured;
															else 
																goto homogeneous;
														else 
															goto homogeneous;
													else 
														goto homogeneous;
												else 
													goto homogeneous;
										else 
											goto homogeneous;
									else 
										if(p[_offset9] > cb) 
											if(p[_offset6] > cb) 
												if(p[_offset8] > cb) 
													if(p[_offset4] > cb) 
														if(p[_offset3] > cb) 
															goto success_homogeneous;
														else 
															if(p[_offset10] > cb) 
																goto success_homogeneous;
															else 
																goto homogeneous;
													else 
														if(p[_offset10] > cb) 
															if(p[_offset11] > cb) 
																goto success_homogeneous;
															else 
																goto homogeneous;
														else 
															goto homogeneous;
												else 
													goto homogeneous;
											else 
												goto homogeneous;
										else 
											goto homogeneous;
								else 
									goto homogeneous;
							else 
								if(p[_offset7] < c_b) 
									if(p[_offset9] < c_b) 
										if(p[_offset8] < c_b) 
											if(p[_offset5] < c_b) 
												if(p[_offset1] < c_b) 
													if(p[_offset10] < c_b) 
														if(p[_offset11] < c_b) 
															goto success_homogeneous;
														else 
															if(p[_offset6] < c_b) 
																if(p[_offset4] < c_b) 
																	goto success_structured;
																else 
																	goto homogeneous;
															else 
																goto homogeneous;
													else 
														if(p[_offset6] < c_b) 
															if(p[_offset3] < c_b) 
																if(p[_offset4] < c_b) 
																	goto success_structured;
																else 
																	goto homogeneous;
															else 
																goto homogeneous;
														else 
															goto homogeneous;
												else 
													if(p[_offset6] < c_b) 
														if(p[_offset4] < c_b) 
															if(p[_offset3] < c_b) 
																goto success_homogeneous;
															else 
																if(p[_offset10] < c_b) 
																	goto success_homogeneous;
																else 
																	goto homogeneous;
														else 
															if(p[_offset10] < c_b) 
																if(p[_offset11] < c_b) 
																	goto success_homogeneous;
																else 
																	goto homogeneous;
															else 
																goto homogeneous;
													else 
														goto homogeneous;
											else 
												if(p[_offset10] < c_b) 
													if(p[_offset11] < c_b) 
														if(p[_offset1] < c_b) 
															goto success_homogeneous;
														else 
															if(p[_offset6] < c_b) 
																goto success_homogeneous;
															else 
																goto homogeneous;
													else 
														goto homogeneous;
												else 
													goto homogeneous;
										else 
											goto homogeneous;
									else 
										goto homogeneous;
								else 
									goto homogeneous;
					else 
						if(p[_offset5] > cb) 
							if(p[_offset7] > cb) 
								if(p[_offset9] > cb) 
									if(p[_offset6] > cb) 
										if(p[_offset4] > cb) 
											if(p[_offset3] > cb) 
												if(p[_offset8] > cb) 
													goto success_homogeneous;
												else 
													if(p[_offset1] > cb) 
														if(p[_offset2] > cb) 
															goto success_homogeneous;
														else 
															goto homogeneous;
													else 
														goto homogeneous;
											else 
												if(p[_offset8] > cb) 
													if(p[_offset10] > cb) 
														goto success_homogeneous;
													else 
														goto homogeneous;
												else 
													goto homogeneous;
										else 
											if(p[_offset11] > cb) 
												if(p[_offset8] > cb) 
													if(p[_offset10] > cb) 
														goto success_homogeneous;
													else 
														goto homogeneous;
												else 
													goto homogeneous;
											else 
												goto homogeneous;
									else 
										goto homogeneous;
								else 
									if(p[_offset2] > cb) 
										if(p[_offset3] > cb) 
											if(p[_offset4] > cb) 
												if(p[_offset1] > cb) 
													if(p[_offset6] > cb) 
														goto success_homogeneous;
													else 
														goto homogeneous;
												else 
													if(p[_offset6] > cb) 
														if(p[_offset8] > cb) 
															goto success_homogeneous;
														else 
															goto homogeneous;
													else 
														goto homogeneous;
											else 
												goto homogeneous;
										else 
											goto homogeneous;
									else 
										goto homogeneous;
							else 
								goto homogeneous;
						else 
							if(p[_offset5] < c_b) 
								if(p[_offset7] < c_b) 
									if(p[_offset9] < c_b) 
										if(p[_offset6] < c_b) 
											if(p[_offset4] < c_b) 
												if(p[_offset3] < c_b) 
													if(p[_offset8] < c_b) 
														goto success_homogeneous;
													else 
														if(p[_offset1] < c_b) 
															if(p[_offset2] < c_b) 
																goto success_homogeneous;
															else 
																goto homogeneous;
														else 
															goto homogeneous;
												else 
													if(p[_offset8] < c_b) 
														if(p[_offset10] < c_b) 
															goto success_homogeneous;
														else 
															goto homogeneous;
													else 
														goto homogeneous;
											else 
												if(p[_offset11] < c_b) 
													if(p[_offset8] < c_b) 
														if(p[_offset10] < c_b) 
															goto success_homogeneous;
														else 
															goto homogeneous;
													else 
														goto homogeneous;
												else 
													goto homogeneous;
										else 
											goto homogeneous;
									else 
										if(p[_offset2] < c_b) 
											if(p[_offset3] < c_b) 
												if(p[_offset4] < c_b) 
													if(p[_offset1] < c_b) 
														if(p[_offset6] < c_b) 
															goto success_homogeneous;
														else 
															goto homogeneous;
													else 
														if(p[_offset6] < c_b) 
															if(p[_offset8] < c_b) 
																goto success_homogeneous;
															else 
																goto homogeneous;
														else 
															goto homogeneous;
												else 
													goto homogeneous;
											else 
												goto homogeneous;
										else 
											goto homogeneous;
								else 
									goto homogeneous;
							else 
								goto homogeneous;
				}
structured:
				{
					x++;
					if(x>xsizeB)
						break;
					register const uint8_t* const p = im + y*_lastStride + x;
					register const int cb = *p + _threshold;
					register const int c_b = *p - _threshold;
					if(p[_offset0] > cb) 
						if(p[_offset2] > cb) 
							if(p[_offset5] > cb) 
								if(p[_offset9] > cb) 
									if(p[_offset7] > cb) 
										if(p[_offset1] > cb) 
											if(p[_offset6] > cb) 
												if(p[_offset3] > cb) 
													if(p[_offset4] > cb) 
														goto success_structured;
													else 
														if(p[_offset10] > cb) 
															if(p[_offset11] > cb) 
																goto success_structured;
															else 
																goto structured;
														else 
															goto structured;
												else 
													if(p[_offset8] > cb) 
														if(p[_offset10] > cb) 
															if(p[_offset4] > cb) 
																goto success_structured;
															else 
																if(p[_offset11] > cb) 
																	goto success_structured;
																else 
																	goto structured;
														else 
															goto structured;
													else 
														goto structured;
											else 
												if(p[_offset11] > cb) 
													if(p[_offset3] > cb) 
														if(p[_offset4] > cb) 
															goto success_structured;
														else 
															if(p[_offset10] > cb) 
																goto success_structured;
															else 
																goto structured;
													else 
														if(p[_offset8] > cb) 
															if(p[_offset10] > cb) 
																goto success_structured;
															else 
																goto structured;
														else 
															goto structured;
												else 
													goto structured;
										else 
											if(p[_offset6] > cb) 
												if(p[_offset8] > cb) 
													if(p[_offset4] > cb) 
														if(p[_offset3] > cb) 
															goto success_structured;
														else 
															if(p[_offset10] > cb) 
																goto success_structured;
															else 
																goto structured;
													else 
														if(p[_offset10] > cb) 
															if(p[_offset11] > cb) 
																goto success_structured;
															else 
																goto structured;
														else 
															goto structured;
												else 
													goto structured;
											else 
												goto structured;
									else 
										if(p[_offset1] > cb) 
											if(p[_offset11] > cb) 
												if(p[_offset3] > cb) 
													if(p[_offset4] > cb) 
														goto success_structured;
													else 
														if(p[_offset10] > cb) 
															goto success_structured;
														else 
															goto structured;
												else 
													if(p[_offset8] > cb) 
														if(p[_offset10] > cb) 
															goto success_structured;
														else 
															goto structured;
													else 
														goto structured;
											else 
												if(p[_offset6] > cb) 
													if(p[_offset3] > cb) 
														if(p[_offset4] > cb) 
															goto success_structured;
														else 
															goto structured;
													else 
														goto structured;
												else 
													goto structured;
										else 
											goto structured;
								else 
									if(p[_offset3] > cb) 
										if(p[_offset4] > cb) 
											if(p[_offset7] > cb) 
												if(p[_offset1] > cb) 
													if(p[_offset6] > cb) 
														goto success_structured;
													else 
														if(p[_offset11] > cb) 
															goto success_structured;
														else 
															goto structured;
												else 
													if(p[_offset6] > cb) 
														if(p[_offset8] > cb) 
															goto success_structured;
														else 
															goto structured;
													else 
														goto structured;
											else 
												if(p[_offset1] > cb) 
													if(p[_offset6] > cb) 
														goto success_structured;
													else 
														if(p[_offset11] > cb) 
															goto success_structured;
														else 
															goto structured;
												else 
													goto structured;
										else 
											goto structured;
									else 
										goto structured;
							else 
								if(p[_offset7] < c_b) 
									if(p[_offset9] < c_b) 
										if(p[_offset5] < c_b) 
											if(p[_offset1] > cb) 
												if(p[_offset4] > cb) 
													if(p[_offset10] > cb) 
														if(p[_offset3] > cb) 
															if(p[_offset11] > cb) 
																goto success_structured;
															else 
																goto structured;
														else 
															goto structured;
													else 
														if(p[_offset6] < c_b) 
															if(p[_offset8] < c_b) 
																if(p[_offset11] < c_b) 
																	if(p[_offset10] < c_b) 
																		goto success_structured;
																	else 
																		goto structured;
																else 
																	goto structured;
															else 
																goto structured;
														else 
															goto structured;
												else 
													if(p[_offset6] < c_b) 
														if(p[_offset8] < c_b) 
															if(p[_offset10] < c_b) 
																if(p[_offset4] < c_b) 
																	goto success_structured;
																else 
																	if(p[_offset11] < c_b) 
																		goto success_structured;
																	else 
																		goto structured;
															else 
																if(p[_offset3] < c_b) 
																	if(p[_offset4] < c_b) 
																		goto success_structured;
																	else 
																		goto structured;
																else 
																	goto structured;
														else 
															goto structured;
													else 
														goto structured;
											else 
												if(p[_offset6] < c_b) 
													if(p[_offset8] < c_b) 
														if(p[_offset4] < c_b) 
															if(p[_offset3] < c_b) 
																goto success_structured;
															else 
																if(p[_offset10] < c_b) 
																	goto success_structured;
																else 
																	goto structured;
														else 
															if(p[_offset10] < c_b) 
																if(p[_offset11] < c_b) 
																	goto success_structured;
																else 
																	goto structured;
															else 
																goto structured;
													else 
														goto structured;
												else 
													goto structured;
										else 
											if(p[_offset1] > cb) 
												if(p[_offset3] > cb) 
													if(p[_offset4] > cb) 
														if(p[_offset10] > cb) 
															if(p[_offset11] > cb) 
																goto success_structured;
															else 
																goto structured;
														else 
															goto structured;
													else 
														goto structured;
												else 
													goto structured;
											else 
												goto structured;
									else 
										if(p[_offset10] > cb) 
											if(p[_offset11] > cb) 
												if(p[_offset9] > cb) 
													if(p[_offset1] > cb) 
														if(p[_offset3] > cb) 
															goto success_structured;
														else 
															if(p[_offset8] > cb) 
																goto success_structured;
															else 
																goto structured;
													else 
														goto structured;
												else 
													if(p[_offset1] > cb) 
														if(p[_offset3] > cb) 
															if(p[_offset4] > cb) 
																goto success_structured;
															else 
																goto structured;
														else 
															goto structured;
													else 
														goto structured;
											else 
												goto structured;
										else 
											goto structured;
								else 
									if(p[_offset10] > cb) 
										if(p[_offset11] > cb) 
											if(p[_offset9] > cb) 
												if(p[_offset1] > cb) 
													if(p[_offset3] > cb) 
														goto success_structured;
													else 
														if(p[_offset8] > cb) 
															goto success_structured;
														else 
															goto structured;
												else 
													if(p[_offset6] > cb) 
														if(p[_offset8] > cb) 
															if(p[_offset7] > cb) 
																goto success_structured;
															else 
																goto structured;
														else 
															goto structured;
													else 
														goto structured;
											else 
												if(p[_offset1] > cb) 
													if(p[_offset3] > cb) 
														if(p[_offset4] > cb) 
															goto success_structured;
														else 
															goto structured;
													else 
														goto structured;
												else 
													goto structured;
										else 
											goto structured;
									else 
										goto structured;
						else 
							if(p[_offset7] > cb) 
								if(p[_offset9] > cb) 
									if(p[_offset8] > cb) 
										if(p[_offset5] > cb) 
											if(p[_offset1] > cb) 
												if(p[_offset10] > cb) 
													if(p[_offset11] > cb) 
														goto success_structured;
													else 
														if(p[_offset6] > cb) 
															if(p[_offset4] > cb) 
																goto success_structured;
															else 
																goto structured;
														else 
															goto structured;
												else 
													if(p[_offset6] > cb) 
														if(p[_offset3] > cb) 
															if(p[_offset4] > cb) 
																goto success_structured;
															else 
																goto structured;
														else 
															goto structured;
													else 
														goto structured;
											else 
												if(p[_offset6] > cb) 
													if(p[_offset4] > cb) 
														if(p[_offset3] > cb) 
															goto success_structured;
														else 
															if(p[_offset10] > cb) 
																goto success_structured;
															else 
																goto structured;
													else 
														if(p[_offset10] > cb) 
															if(p[_offset11] > cb) 
																goto success_structured;
															else 
																goto structured;
														else 
															goto structured;
												else 
													goto structured;
										else 
											if(p[_offset10] > cb) 
												if(p[_offset11] > cb) 
													if(p[_offset1] > cb) 
														goto success_structured;
													else 
														if(p[_offset6] > cb) 
															goto success_structured;
														else 
															goto structured;
												else 
													goto structured;
											else 
												goto structured;
									else 
										goto structured;
								else 
									goto structured;
							else 
								if(p[_offset7] < c_b) 
									if(p[_offset5] < c_b) 
										if(p[_offset2] < c_b) 
											if(p[_offset6] < c_b) 
												if(p[_offset4] < c_b) 
													if(p[_offset3] < c_b) 
														if(p[_offset1] < c_b) 
															goto success_structured;
														else 
															if(p[_offset8] < c_b) 
																goto success_structured;
															else 
																goto structured;
													else 
														if(p[_offset9] < c_b) 
															if(p[_offset8] < c_b) 
																if(p[_offset10] < c_b) 
																	goto success_structured;
																else 
																	goto structured;
															else 
																goto structured;
														else 
															goto structured;
												else 
													if(p[_offset9] < c_b) 
														if(p[_offset8] < c_b) 
															if(p[_offset10] < c_b) 
																if(p[_offset11] < c_b) 
																	goto success_structured;
																else 
																	goto structured;
															else 
																goto structured;
														else 
															goto structured;
													else 
														goto structured;
											else 
												goto structured;
										else 
											if(p[_offset9] < c_b) 
												if(p[_offset6] < c_b) 
													if(p[_offset8] < c_b) 
														if(p[_offset4] < c_b) 
															if(p[_offset3] < c_b) 
																goto success_structured;
															else 
																if(p[_offset10] < c_b) 
																	goto success_structured;
																else 
																	goto structured;
														else 
															if(p[_offset10] < c_b) 
																if(p[_offset11] < c_b) 
																	goto success_structured;
																else 
																	goto structured;
															else 
																goto structured;
													else 
														goto structured;
												else 
													goto structured;
											else 
												goto structured;
									else 
										goto structured;
								else 
									goto structured;
					else if(p[_offset0] < c_b) 
						if(p[_offset2] < c_b) 
							if(p[_offset11] < c_b) 
								if(p[_offset3] < c_b) 
									if(p[_offset5] < c_b) 
										if(p[_offset9] < c_b) 
											if(p[_offset7] < c_b) 
												if(p[_offset1] < c_b) 
													if(p[_offset4] < c_b) 
														goto success_structured;
													else 
														if(p[_offset10] < c_b) 
															goto success_structured;
														else 
															goto structured;
												else 
													if(p[_offset6] < c_b) 
														if(p[_offset8] < c_b) 
															if(p[_offset4] < c_b) 
																goto success_structured;
															else 
																if(p[_offset10] < c_b) 
																	goto success_structured;
																else 
																	goto structured;
														else 
															goto structured;
													else 
														goto structured;
											else 
												if(p[_offset1] < c_b) 
													if(p[_offset4] < c_b) 
														goto success_structured;
													else 
														if(p[_offset10] < c_b) 
															goto success_structured;
														else 
															goto structured;
												else 
													goto structured;
										else 
											if(p[_offset4] < c_b) 
												if(p[_offset7] < c_b) 
													if(p[_offset1] < c_b) 
														goto success_structured;
													else 
														if(p[_offset6] < c_b) 
															if(p[_offset8] < c_b) 
																goto success_structured;
															else 
																goto structured;
														else 
															goto structured;
												else 
													if(p[_offset1] < c_b) 
														goto success_structured;
													else 
														goto structured;
											else 
												goto structured;
									else 
										if(p[_offset10] < c_b) 
											if(p[_offset9] < c_b) 
												if(p[_offset7] < c_b) 
													if(p[_offset1] < c_b) 
														goto success_structured;
													else 
														if(p[_offset6] < c_b) 
															if(p[_offset8] < c_b) 
																goto success_structured;
															else 
																goto structured;
														else 
															goto structured;
												else 
													if(p[_offset1] < c_b) 
														goto success_structured;
													else 
														goto structured;
											else 
												if(p[_offset1] < c_b) 
													if(p[_offset4] < c_b) 
														goto success_structured;
													else 
														goto structured;
												else 
													goto structured;
										else 
											if(p[_offset7] > cb) 
												if(p[_offset9] > cb) 
													if(p[_offset5] > cb) 
														if(p[_offset4] > cb) 
															if(p[_offset6] > cb) 
																if(p[_offset8] > cb) 
																	if(p[_offset10] > cb) 
																		goto success_structured;
																	else 
																		goto structured;
																else 
																	goto structured;
															else 
																goto structured;
														else 
															goto structured;
													else 
														goto structured;
												else 
													goto structured;
											else 
												goto structured;
								else 
									if(p[_offset9] < c_b) 
										if(p[_offset8] < c_b) 
											if(p[_offset10] < c_b) 
												if(p[_offset7] < c_b) 
													if(p[_offset1] < c_b) 
														goto success_structured;
													else 
														if(p[_offset6] < c_b) 
															goto success_structured;
														else 
															goto structured;
												else 
													if(p[_offset1] < c_b) 
														goto success_structured;
													else 
														goto structured;
											else 
												goto structured;
										else 
											goto structured;
									else 
										if(p[_offset5] > cb) 
											if(p[_offset7] > cb) 
												if(p[_offset9] > cb) 
													if(p[_offset4] > cb) 
														if(p[_offset6] > cb) 
															if(p[_offset8] > cb) 
																if(p[_offset3] > cb) 
																	goto success_structured;
																else 
																	if(p[_offset10] > cb) 
																		goto success_structured;
																	else 
																		goto structured;
															else 
																goto structured;
														else 
															goto structured;
													else 
														goto structured;
												else 
													goto structured;
											else 
												goto structured;
										else 
											goto structured;
							else 
								if(p[_offset4] < c_b) 
									if(p[_offset5] < c_b) 
										if(p[_offset7] < c_b) 
											if(p[_offset6] < c_b) 
												if(p[_offset3] < c_b) 
													if(p[_offset1] < c_b) 
														goto success_structured;
													else 
														if(p[_offset8] < c_b) 
															goto success_structured;
														else 
															goto structured;
												else 
													if(p[_offset9] < c_b) 
														if(p[_offset8] < c_b) 
															if(p[_offset10] < c_b) 
																goto success_structured;
															else 
																goto structured;
														else 
															goto structured;
													else 
														goto structured;
											else 
												goto structured;
										else 
											if(p[_offset1] < c_b) 
												if(p[_offset6] < c_b) 
													if(p[_offset3] < c_b) 
														goto success_structured;
													else 
														goto structured;
												else 
													goto structured;
											else 
												goto structured;
									else 
										if(p[_offset7] > cb) 
											if(p[_offset9] > cb) 
												if(p[_offset5] > cb) 
													if(p[_offset6] > cb) 
														if(p[_offset8] > cb) 
															if(p[_offset10] > cb) 
																if(p[_offset11] > cb) 
																	goto success_structured;
																else 
																	goto structured;
															else 
																goto structured;
														else 
															goto structured;
													else 
														goto structured;
												else 
													goto structured;
											else 
												goto structured;
										else 
											goto structured;
								else 
									if(p[_offset5] > cb) 
										if(p[_offset7] > cb) 
											if(p[_offset9] > cb) 
												if(p[_offset6] > cb) 
													if(p[_offset8] > cb) 
														if(p[_offset10] > cb) 
															if(p[_offset4] > cb) 
																goto success_structured;
															else 
																if(p[_offset11] > cb) 
																	goto success_structured;
																else 
																	goto homogeneous;
														else 
															if(p[_offset3] > cb) 
																if(p[_offset4] > cb) 
																	goto success_structured;
																else 
																	goto structured;
															else 
																goto structured;
													else 
														goto structured;
												else 
													goto structured;
											else 
												goto structured;
										else 
											goto structured;
									else 
										goto structured;
						else 
							if(p[_offset7] > cb) 
								if(p[_offset5] > cb) 
									if(p[_offset2] > cb) 
										if(p[_offset6] > cb) 
											if(p[_offset4] > cb) 
												if(p[_offset3] > cb) 
													if(p[_offset1] > cb) 
														goto success_structured;
													else 
														if(p[_offset8] > cb) 
															goto success_structured;
														else 
															goto structured;
												else 
													if(p[_offset9] > cb) 
														if(p[_offset8] > cb) 
															if(p[_offset10] > cb) 
																goto success_structured;
															else 
																goto structured;
														else 
															goto structured;
													else 
														goto structured;
											else 
												if(p[_offset9] > cb) 
													if(p[_offset8] > cb) 
														if(p[_offset10] > cb) 
															if(p[_offset11] > cb) 
																goto success_structured;
															else 
																goto structured;
														else 
															goto structured;
													else 
														goto structured;
												else 
													goto structured;
										else 
											goto structured;
									else 
										if(p[_offset9] > cb) 
											if(p[_offset6] > cb) 
												if(p[_offset8] > cb) 
													if(p[_offset4] > cb) 
														if(p[_offset3] > cb) 
															goto success_structured;
														else 
															if(p[_offset10] > cb) 
																goto success_structured;
															else 
																goto structured;
													else 
														if(p[_offset10] > cb) 
															if(p[_offset11] > cb) 
																goto success_structured;
															else 
																goto structured;
														else 
															goto structured;
												else 
													goto structured;
											else 
												goto structured;
										else 
											goto structured;
								else 
									goto structured;
							else 
								if(p[_offset7] < c_b) 
									if(p[_offset9] < c_b) 
										if(p[_offset8] < c_b) 
											if(p[_offset5] < c_b) 
												if(p[_offset1] < c_b) 
													if(p[_offset10] < c_b) 
														if(p[_offset11] < c_b) 
															goto success_structured;
														else 
															if(p[_offset6] < c_b) 
																if(p[_offset4] < c_b) 
																	goto success_structured;
																else 
																	goto structured;
															else 
																goto structured;
													else 
														if(p[_offset6] < c_b) 
															if(p[_offset3] < c_b) 
																if(p[_offset4] < c_b) 
																	goto success_structured;
																else 
																	goto structured;
															else 
																goto structured;
														else 
															goto structured;
												else 
													if(p[_offset6] < c_b) 
														if(p[_offset4] < c_b) 
															if(p[_offset3] < c_b) 
																goto success_structured;
															else 
																if(p[_offset10] < c_b) 
																	goto success_structured;
																else 
																	goto structured;
														else 
															if(p[_offset10] < c_b) 
																if(p[_offset11] < c_b) 
																	goto success_structured;
																else 
																	goto structured;
															else 
																goto structured;
													else 
														goto structured;
											else 
												if(p[_offset10] < c_b) 
													if(p[_offset11] < c_b) 
														if(p[_offset1] < c_b) 
															goto success_structured;
														else 
															if(p[_offset6] < c_b) 
																goto success_structured;
															else 
																goto structured;
													else 
														goto structured;
												else 
													goto structured;
										else 
											goto structured;
									else 
										goto structured;
								else 
									goto structured;
					else 
						if(p[_offset5] > cb) 
							if(p[_offset7] > cb) 
								if(p[_offset9] > cb) 
									if(p[_offset6] > cb) 
										if(p[_offset4] > cb) 
											if(p[_offset3] > cb) 
												if(p[_offset8] > cb) 
													goto success_structured;
												else 
													if(p[_offset1] > cb) 
														if(p[_offset2] > cb) 
															goto success_structured;
														else 
															goto structured;
													else 
														goto structured;
											else 
												if(p[_offset8] > cb) 
													if(p[_offset10] > cb) 
														goto success_structured;
													else 
														goto structured;
												else 
													goto structured;
										else 
											if(p[_offset11] > cb) 
												if(p[_offset8] > cb) 
													if(p[_offset10] > cb) 
														goto success_structured;
													else 
														goto structured;
												else 
													goto structured;
											else 
												goto structured;
									else 
										goto structured;
								else 
									if(p[_offset2] > cb) 
										if(p[_offset3] > cb) 
											if(p[_offset4] > cb) 
												if(p[_offset1] > cb) 
													if(p[_offset6] > cb) 
														goto success_structured;
													else 
														goto structured;
												else 
													if(p[_offset6] > cb) 
														if(p[_offset8] > cb) 
															goto success_structured;
														else 
															goto structured;
													else 
														goto structured;
											else 
												goto structured;
										else 
											goto structured;
									else 
										goto structured;
							else 
								goto structured;
						else 
							if(p[_offset5] < c_b) 
								if(p[_offset7] < c_b) 
									if(p[_offset9] < c_b) 
										if(p[_offset6] < c_b) 
											if(p[_offset4] < c_b) 
												if(p[_offset3] < c_b) 
													if(p[_offset8] < c_b) 
														goto success_structured;
													else 
														if(p[_offset1] < c_b) 
															if(p[_offset2] < c_b) 
																goto success_structured;
															else 
																goto structured;
														else 
															goto structured;
												else 
													if(p[_offset8] < c_b) 
														if(p[_offset10] < c_b) 
															goto success_structured;
														else 
															goto structured;
													else 
														goto structured;
											else 
												if(p[_offset11] < c_b) 
													if(p[_offset8] < c_b) 
														if(p[_offset10] < c_b) 
															goto success_structured;
														else 
															goto structured;
													else 
														goto structured;
												else 
													goto structured;
										else 
											goto structured;
									else 
										if(p[_offset2] < c_b) 
											if(p[_offset3] < c_b) 
												if(p[_offset4] < c_b) 
													if(p[_offset1] < c_b) 
														if(p[_offset6] < c_b) 
															goto success_structured;
														else 
															goto structured;
													else 
														if(p[_offset6] < c_b) 
															if(p[_offset8] < c_b) 
																goto success_structured;
															else 
																goto structured;
														else 
															goto structured;
												else 
													goto structured;
											else 
												goto structured;
										else 
											goto structured;
								else 
									goto structured;
							else 
								goto homogeneous;
				}
success_homogeneous:
				if(total == nExpectedCorners)
				{
					nExpectedCorners = nExpectedCorners << 1;
					corners.reserve( nExpectedCorners );
				}
				corners.push_back( Feature2D() );
				corners.back()[ 0 ] = x;
				corners.back()[ 1 ] = y;
				total++;

				goto homogeneous;
success_structured:
				if(total == nExpectedCorners)
				{
					nExpectedCorners = nExpectedCorners << 1;
					corners.reserve( nExpectedCorners );
				}
				corners.push_back( Feature2D() );
				corners.back()[ 0 ] = x;
				corners.back()[ 1 ] = y;
				total++;

				goto structured;
			}
		}
	}

int AGAST::score_agast7_12s( const uint8_t* p )
{
    int bmin = _threshold;
    int bmax = 255;
    int b = (bmax + bmin)/2;

	while(1)
	{
		register const int cb = *p + b;
		register const int c_b = *p - b;
		if(p[_offset0] > cb)
		  if(p[_offset5] > cb)
		    if(p[_offset2] < c_b)
		      if(p[_offset7] > cb)
		        if(p[_offset9] < c_b)
		          goto is_not_a_corner;
		        else
		          if(p[_offset9] > cb)
		            if(p[_offset1] < c_b)
		              if(p[_offset6] < c_b)
		                goto is_not_a_corner;
		              else
		                if(p[_offset6] > cb)
		                  if(p[_offset8] > cb)
		                    if(p[_offset4] > cb)
		                      if(p[_offset3] > cb)
		                        goto is_a_corner;
		                      else
		                        if(p[_offset10] > cb)
		                          goto is_a_corner;
		                        else
		                          goto is_not_a_corner;
		                    else
		                      if(p[_offset10] > cb)
		                        if(p[_offset11] > cb)
		                          goto is_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                  else
		                    goto is_not_a_corner;
		                else
		                  goto is_not_a_corner;
		            else
		              if(p[_offset1] > cb)
		                if(p[_offset6] < c_b)
		                  if(p[_offset8] > cb)
		                    if(p[_offset10] > cb)
		                      if(p[_offset11] > cb)
		                        goto is_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                  else
		                    goto is_not_a_corner;
		                else
		                  if(p[_offset6] > cb)
		                    if(p[_offset8] > cb)
		                      if(p[_offset4] > cb)
		                        if(p[_offset3] > cb)
		                          goto is_a_corner;
		                        else
		                          if(p[_offset10] > cb)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        if(p[_offset10] > cb)
		                          if(p[_offset11] > cb)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                  else
		                    if(p[_offset8] > cb)
		                      if(p[_offset10] > cb)
		                        if(p[_offset11] > cb)
		                          goto is_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		              else
		                if(p[_offset6] < c_b)
		                  goto is_not_a_corner;
		                else
		                  if(p[_offset6] > cb)
		                    if(p[_offset8] > cb)
		                      if(p[_offset4] > cb)
		                        if(p[_offset3] > cb)
		                          goto is_a_corner;
		                        else
		                          if(p[_offset10] > cb)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        if(p[_offset10] > cb)
		                          if(p[_offset11] > cb)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                  else
		                    goto is_not_a_corner;
		          else
		            goto is_not_a_corner;
		      else
		        goto is_not_a_corner;
		    else
		      if(p[_offset2] > cb)
		        if(p[_offset7] < c_b)
		          if(p[_offset9] < c_b)
		            if(p[_offset1] < c_b)
		              goto is_not_a_corner;
		            else
		              if(p[_offset1] > cb)
		                if(p[_offset6] > cb)
		                  if(p[_offset3] > cb)
		                    if(p[_offset4] > cb)
		                      goto is_a_corner;
		                    else
		                      goto is_not_a_corner;
		                  else
		                    goto is_not_a_corner;
		                else
		                  if(p[_offset6] < c_b)
		                    if(p[_offset3] > cb)
		                      if(p[_offset4] > cb)
		                        if(p[_offset11] > cb)
		                          goto is_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                  else
		                    if(p[_offset3] > cb)
		                      if(p[_offset4] > cb)
		                        if(p[_offset11] > cb)
		                          goto is_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		              else
		                goto is_not_a_corner;
		          else
		            if(p[_offset9] > cb)
		              if(p[_offset1] < c_b)
		                goto is_not_a_corner;
		              else
		                if(p[_offset1] > cb)
		                  if(p[_offset6] < c_b)
		                    if(p[_offset11] > cb)
		                      if(p[_offset3] > cb)
		                        if(p[_offset4] > cb)
		                          goto is_a_corner;
		                        else
		                          if(p[_offset10] > cb)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        if(p[_offset8] > cb)
		                          if(p[_offset10] > cb)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                  else
		                    if(p[_offset6] > cb)
		                      if(p[_offset3] > cb)
		                        if(p[_offset4] > cb)
		                          goto is_a_corner;
		                        else
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        if(p[_offset8] > cb)
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                    else
		                      if(p[_offset11] > cb)
		                        if(p[_offset3] > cb)
		                          if(p[_offset4] > cb)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset10] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          if(p[_offset8] > cb)
		                            if(p[_offset10] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                else
		                  goto is_not_a_corner;
		            else
		              if(p[_offset1] < c_b)
		                goto is_not_a_corner;
		              else
		                if(p[_offset1] > cb)
		                  if(p[_offset6] > cb)
		                    if(p[_offset3] > cb)
		                      if(p[_offset4] > cb)
		                        goto is_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      if(p[_offset3] > cb)
		                        if(p[_offset4] > cb)
		                          if(p[_offset11] > cb)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset3] > cb)
		                        if(p[_offset4] > cb)
		                          if(p[_offset11] > cb)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                else
		                  goto is_not_a_corner;
		        else
		          if(p[_offset9] < c_b)
		            if(p[_offset7] > cb)
		              if(p[_offset1] < c_b)
		                if(p[_offset6] < c_b)
		                  goto is_not_a_corner;
		                else
		                  if(p[_offset6] > cb)
		                    if(p[_offset3] > cb)
		                      if(p[_offset4] > cb)
		                        if(p[_offset8] > cb)
		                          goto is_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                  else
		                    goto is_not_a_corner;
		              else
		                if(p[_offset1] > cb)
		                  if(p[_offset6] > cb)
		                    if(p[_offset3] > cb)
		                      if(p[_offset4] > cb)
		                        goto is_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      if(p[_offset3] > cb)
		                        if(p[_offset4] > cb)
		                          if(p[_offset11] > cb)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset3] > cb)
		                        if(p[_offset4] > cb)
		                          if(p[_offset11] > cb)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                else
		                  if(p[_offset6] < c_b)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] > cb)
		                      if(p[_offset3] > cb)
		                        if(p[_offset4] > cb)
		                          if(p[_offset8] > cb)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		            else
		              if(p[_offset1] < c_b)
		                goto is_not_a_corner;
		              else
		                if(p[_offset1] > cb)
		                  if(p[_offset6] > cb)
		                    if(p[_offset3] > cb)
		                      if(p[_offset4] > cb)
		                        goto is_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      if(p[_offset3] > cb)
		                        if(p[_offset4] > cb)
		                          if(p[_offset11] > cb)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset3] > cb)
		                        if(p[_offset4] > cb)
		                          if(p[_offset11] > cb)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                else
		                  goto is_not_a_corner;
		          else
		            if(p[_offset7] > cb)
		              if(p[_offset9] > cb)
		                if(p[_offset1] < c_b)
		                  if(p[_offset6] < c_b)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] > cb)
		                      if(p[_offset8] > cb)
		                        if(p[_offset4] > cb)
		                          if(p[_offset3] > cb)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset10] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                else
		                  if(p[_offset1] > cb)
		                    if(p[_offset6] < c_b)
		                      if(p[_offset11] > cb)
		                        if(p[_offset3] > cb)
		                          if(p[_offset4] > cb)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset10] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          if(p[_offset8] > cb)
		                            if(p[_offset10] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset6] > cb)
		                        if(p[_offset3] > cb)
		                          if(p[_offset4] > cb)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset10] > cb)
		                              if(p[_offset11] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          if(p[_offset8] > cb)
		                            if(p[_offset10] > cb)
		                              if(p[_offset4] > cb)
		                                goto is_a_corner;
		                              else
		                                if(p[_offset11] > cb)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        if(p[_offset11] > cb)
		                          if(p[_offset3] > cb)
		                            if(p[_offset4] > cb)
		                              goto is_a_corner;
		                            else
		                              if(p[_offset10] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            if(p[_offset8] > cb)
		                              if(p[_offset10] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] > cb)
		                        if(p[_offset8] > cb)
		                          if(p[_offset4] > cb)
		                            if(p[_offset3] > cb)
		                              goto is_a_corner;
		                            else
		                              if(p[_offset10] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            if(p[_offset10] > cb)
		                              if(p[_offset11] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		              else
		                if(p[_offset1] < c_b)
		                  if(p[_offset6] < c_b)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] > cb)
		                      if(p[_offset3] > cb)
		                        if(p[_offset4] > cb)
		                          if(p[_offset8] > cb)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                else
		                  if(p[_offset1] > cb)
		                    if(p[_offset6] > cb)
		                      if(p[_offset3] > cb)
		                        if(p[_offset4] > cb)
		                          goto is_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset3] > cb)
		                          if(p[_offset4] > cb)
		                            if(p[_offset11] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset3] > cb)
		                          if(p[_offset4] > cb)
		                            if(p[_offset11] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] > cb)
		                        if(p[_offset3] > cb)
		                          if(p[_offset4] > cb)
		                            if(p[_offset8] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		            else
		              if(p[_offset9] > cb)
		                if(p[_offset1] < c_b)
		                  goto is_not_a_corner;
		                else
		                  if(p[_offset1] > cb)
		                    if(p[_offset6] < c_b)
		                      if(p[_offset11] > cb)
		                        if(p[_offset3] > cb)
		                          if(p[_offset4] > cb)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset10] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          if(p[_offset8] > cb)
		                            if(p[_offset10] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset6] > cb)
		                        if(p[_offset3] > cb)
		                          if(p[_offset4] > cb)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset10] > cb)
		                              if(p[_offset11] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          if(p[_offset8] > cb)
		                            if(p[_offset10] > cb)
		                              if(p[_offset11] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        if(p[_offset11] > cb)
		                          if(p[_offset3] > cb)
		                            if(p[_offset4] > cb)
		                              goto is_a_corner;
		                            else
		                              if(p[_offset10] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            if(p[_offset8] > cb)
		                              if(p[_offset10] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                  else
		                    goto is_not_a_corner;
		              else
		                if(p[_offset1] < c_b)
		                  goto is_not_a_corner;
		                else
		                  if(p[_offset1] > cb)
		                    if(p[_offset6] > cb)
		                      if(p[_offset3] > cb)
		                        if(p[_offset4] > cb)
		                          goto is_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset3] > cb)
		                          if(p[_offset4] > cb)
		                            if(p[_offset11] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset3] > cb)
		                          if(p[_offset4] > cb)
		                            if(p[_offset11] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                  else
		                    goto is_not_a_corner;
		      else
		        if(p[_offset7] > cb)
		          if(p[_offset9] < c_b)
		            goto is_not_a_corner;
		          else
		            if(p[_offset9] > cb)
		              if(p[_offset1] < c_b)
		                if(p[_offset6] < c_b)
		                  goto is_not_a_corner;
		                else
		                  if(p[_offset6] > cb)
		                    if(p[_offset8] > cb)
		                      if(p[_offset4] > cb)
		                        if(p[_offset3] > cb)
		                          goto is_a_corner;
		                        else
		                          if(p[_offset10] > cb)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        if(p[_offset10] > cb)
		                          if(p[_offset11] > cb)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                  else
		                    goto is_not_a_corner;
		              else
		                if(p[_offset1] > cb)
		                  if(p[_offset6] < c_b)
		                    if(p[_offset8] > cb)
		                      if(p[_offset10] > cb)
		                        if(p[_offset11] > cb)
		                          goto is_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                  else
		                    if(p[_offset6] > cb)
		                      if(p[_offset8] > cb)
		                        if(p[_offset4] > cb)
		                          if(p[_offset3] > cb)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset10] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset8] > cb)
		                        if(p[_offset10] > cb)
		                          if(p[_offset11] > cb)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                else
		                  if(p[_offset6] < c_b)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] > cb)
		                      if(p[_offset8] > cb)
		                        if(p[_offset4] > cb)
		                          if(p[_offset3] > cb)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset10] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		            else
		              goto is_not_a_corner;
		        else
		          goto is_not_a_corner;
		  else
		    if(p[_offset5] < c_b)
		      if(p[_offset9] < c_b)
		        if(p[_offset7] > cb)
		          if(p[_offset2] < c_b)
		            goto is_not_a_corner;
		          else
		            if(p[_offset2] > cb)
		              if(p[_offset1] < c_b)
		                goto is_not_a_corner;
		              else
		                if(p[_offset1] > cb)
		                  if(p[_offset6] > cb)
		                    if(p[_offset3] > cb)
		                      if(p[_offset4] > cb)
		                        if(p[_offset10] > cb)
		                          if(p[_offset11] > cb)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      if(p[_offset3] > cb)
		                        if(p[_offset4] > cb)
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset3] > cb)
		                        if(p[_offset4] > cb)
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                else
		                  goto is_not_a_corner;
		            else
		              goto is_not_a_corner;
		        else
		          if(p[_offset7] < c_b)
		            if(p[_offset2] < c_b)
		              if(p[_offset1] > cb)
		                if(p[_offset6] > cb)
		                  goto is_not_a_corner;
		                else
		                  if(p[_offset6] < c_b)
		                    if(p[_offset8] < c_b)
		                      if(p[_offset4] < c_b)
		                        if(p[_offset3] < c_b)
		                          goto is_a_corner;
		                        else
		                          if(p[_offset10] < c_b)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        if(p[_offset10] < c_b)
		                          if(p[_offset11] < c_b)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                  else
		                    goto is_not_a_corner;
		              else
		                if(p[_offset1] < c_b)
		                  if(p[_offset6] > cb)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      if(p[_offset4] < c_b)
		                        if(p[_offset3] < c_b)
		                          goto is_a_corner;
		                        else
		                          if(p[_offset8] < c_b)
		                            if(p[_offset10] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        if(p[_offset8] < c_b)
		                          if(p[_offset10] < c_b)
		                            if(p[_offset11] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                else
		                  if(p[_offset6] > cb)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      if(p[_offset8] < c_b)
		                        if(p[_offset4] < c_b)
		                          if(p[_offset3] < c_b)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset10] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          if(p[_offset10] < c_b)
		                            if(p[_offset11] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		            else
		              if(p[_offset2] > cb)
		                if(p[_offset1] < c_b)
		                  if(p[_offset6] > cb)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      if(p[_offset8] < c_b)
		                        if(p[_offset4] < c_b)
		                          if(p[_offset3] < c_b)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset10] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          if(p[_offset10] < c_b)
		                            if(p[_offset11] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                else
		                  if(p[_offset1] > cb)
		                    if(p[_offset6] > cb)
		                      if(p[_offset3] > cb)
		                        if(p[_offset4] > cb)
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset4] > cb)
		                          if(p[_offset10] > cb)
		                            if(p[_offset3] > cb)
		                              if(p[_offset11] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            if(p[_offset8] < c_b)
		                              if(p[_offset11] < c_b)
		                                if(p[_offset10] < c_b)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          if(p[_offset8] < c_b)
		                            if(p[_offset10] < c_b)
		                              if(p[_offset4] < c_b)
		                                goto is_a_corner;
		                              else
		                                if(p[_offset11] < c_b)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                            else
		                              if(p[_offset3] < c_b)
		                                if(p[_offset4] < c_b)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        if(p[_offset3] > cb)
		                          if(p[_offset4] > cb)
		                            if(p[_offset10] > cb)
		                              if(p[_offset11] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                  else
		                    if(p[_offset6] > cb)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset8] < c_b)
		                          if(p[_offset4] < c_b)
		                            if(p[_offset3] < c_b)
		                              goto is_a_corner;
		                            else
		                              if(p[_offset10] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            if(p[_offset10] < c_b)
		                              if(p[_offset11] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		              else
		                if(p[_offset1] > cb)
		                  if(p[_offset6] > cb)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      if(p[_offset8] < c_b)
		                        if(p[_offset4] < c_b)
		                          if(p[_offset3] < c_b)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset10] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          if(p[_offset10] < c_b)
		                            if(p[_offset11] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                else
		                  if(p[_offset1] < c_b)
		                    if(p[_offset6] > cb)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset8] < c_b)
		                          if(p[_offset4] < c_b)
		                            if(p[_offset3] < c_b)
		                              goto is_a_corner;
		                            else
		                              if(p[_offset10] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            if(p[_offset10] < c_b)
		                              if(p[_offset11] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                  else
		                    if(p[_offset6] > cb)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset8] < c_b)
		                          if(p[_offset4] < c_b)
		                            if(p[_offset3] < c_b)
		                              goto is_a_corner;
		                            else
		                              if(p[_offset10] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            if(p[_offset10] < c_b)
		                              if(p[_offset11] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		          else
		            if(p[_offset2] < c_b)
		              goto is_not_a_corner;
		            else
		              if(p[_offset2] > cb)
		                if(p[_offset1] < c_b)
		                  goto is_not_a_corner;
		                else
		                  if(p[_offset1] > cb)
		                    if(p[_offset6] > cb)
		                      if(p[_offset3] > cb)
		                        if(p[_offset4] > cb)
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset3] > cb)
		                          if(p[_offset4] > cb)
		                            if(p[_offset10] > cb)
		                              if(p[_offset11] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset3] > cb)
		                          if(p[_offset4] > cb)
		                            if(p[_offset10] > cb)
		                              if(p[_offset11] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                  else
		                    goto is_not_a_corner;
		              else
		                goto is_not_a_corner;
		      else
		        if(p[_offset9] > cb)
		          if(p[_offset7] < c_b)
		            if(p[_offset2] > cb)
		              if(p[_offset1] < c_b)
		                goto is_not_a_corner;
		              else
		                if(p[_offset1] > cb)
		                  if(p[_offset6] > cb)
		                    if(p[_offset10] > cb)
		                      if(p[_offset11] > cb)
		                        if(p[_offset3] > cb)
		                          goto is_a_corner;
		                        else
		                          if(p[_offset8] > cb)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      if(p[_offset10] > cb)
		                        if(p[_offset11] > cb)
		                          if(p[_offset3] > cb)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset8] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset10] > cb)
		                        if(p[_offset11] > cb)
		                          if(p[_offset3] > cb)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset8] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                else
		                  goto is_not_a_corner;
		            else
		              if(p[_offset2] < c_b)
		                if(p[_offset1] < c_b)
		                  if(p[_offset6] > cb)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      if(p[_offset3] < c_b)
		                        if(p[_offset4] < c_b)
		                          goto is_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                else
		                  if(p[_offset1] > cb)
		                    if(p[_offset6] > cb)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset3] < c_b)
		                          if(p[_offset4] < c_b)
		                            if(p[_offset8] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                  else
		                    if(p[_offset6] > cb)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset3] < c_b)
		                          if(p[_offset4] < c_b)
		                            if(p[_offset8] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		              else
		                goto is_not_a_corner;
		          else
		            if(p[_offset7] > cb)
		              if(p[_offset2] < c_b)
		                if(p[_offset1] < c_b)
		                  if(p[_offset6] < c_b)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] > cb)
		                      if(p[_offset8] > cb)
		                        if(p[_offset10] > cb)
		                          if(p[_offset11] > cb)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                else
		                  if(p[_offset1] > cb)
		                    if(p[_offset6] > cb)
		                      if(p[_offset8] > cb)
		                        if(p[_offset10] > cb)
		                          if(p[_offset11] > cb)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset8] > cb)
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset8] > cb)
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] > cb)
		                        if(p[_offset8] > cb)
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		              else
		                if(p[_offset2] > cb)
		                  if(p[_offset1] < c_b)
		                    if(p[_offset6] < c_b)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] > cb)
		                        if(p[_offset8] > cb)
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                  else
		                    if(p[_offset1] > cb)
		                      if(p[_offset6] > cb)
		                        if(p[_offset10] > cb)
		                          if(p[_offset11] > cb)
		                            if(p[_offset3] > cb)
		                              goto is_a_corner;
		                            else
		                              if(p[_offset8] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset6] < c_b)
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              if(p[_offset3] > cb)
		                                goto is_a_corner;
		                              else
		                                if(p[_offset8] > cb)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              if(p[_offset3] > cb)
		                                goto is_a_corner;
		                              else
		                                if(p[_offset8] > cb)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        goto is_not_a_corner;
		                      else
		                        if(p[_offset6] > cb)
		                          if(p[_offset8] > cb)
		                            if(p[_offset10] > cb)
		                              if(p[_offset11] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                else
		                  if(p[_offset1] < c_b)
		                    if(p[_offset6] < c_b)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] > cb)
		                        if(p[_offset8] > cb)
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                  else
		                    if(p[_offset1] > cb)
		                      if(p[_offset6] > cb)
		                        if(p[_offset8] > cb)
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset6] < c_b)
		                          if(p[_offset8] > cb)
		                            if(p[_offset10] > cb)
		                              if(p[_offset11] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          if(p[_offset8] > cb)
		                            if(p[_offset10] > cb)
		                              if(p[_offset11] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        goto is_not_a_corner;
		                      else
		                        if(p[_offset6] > cb)
		                          if(p[_offset8] > cb)
		                            if(p[_offset10] > cb)
		                              if(p[_offset11] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		            else
		              if(p[_offset2] < c_b)
		                goto is_not_a_corner;
		              else
		                if(p[_offset2] > cb)
		                  if(p[_offset1] < c_b)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset1] > cb)
		                      if(p[_offset6] > cb)
		                        if(p[_offset10] > cb)
		                          if(p[_offset11] > cb)
		                            if(p[_offset3] > cb)
		                              goto is_a_corner;
		                            else
		                              if(p[_offset8] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset6] < c_b)
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              if(p[_offset3] > cb)
		                                goto is_a_corner;
		                              else
		                                if(p[_offset8] > cb)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              if(p[_offset3] > cb)
		                                goto is_a_corner;
		                              else
		                                if(p[_offset8] > cb)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                else
		                  goto is_not_a_corner;
		        else
		          if(p[_offset2] < c_b)
		            if(p[_offset7] > cb)
		              goto is_not_a_corner;
		            else
		              if(p[_offset7] < c_b)
		                if(p[_offset1] < c_b)
		                  if(p[_offset6] > cb)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      if(p[_offset3] < c_b)
		                        if(p[_offset4] < c_b)
		                          goto is_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                else
		                  if(p[_offset1] > cb)
		                    if(p[_offset6] > cb)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset3] < c_b)
		                          if(p[_offset4] < c_b)
		                            if(p[_offset8] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                  else
		                    if(p[_offset6] > cb)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset3] < c_b)
		                          if(p[_offset4] < c_b)
		                            if(p[_offset8] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		              else
		                goto is_not_a_corner;
		          else
		            if(p[_offset2] > cb)
		              if(p[_offset7] > cb)
		                if(p[_offset1] < c_b)
		                  goto is_not_a_corner;
		                else
		                  if(p[_offset1] > cb)
		                    if(p[_offset6] > cb)
		                      if(p[_offset3] > cb)
		                        if(p[_offset4] > cb)
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset3] > cb)
		                          if(p[_offset4] > cb)
		                            if(p[_offset10] > cb)
		                              if(p[_offset11] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset3] > cb)
		                          if(p[_offset4] > cb)
		                            if(p[_offset10] > cb)
		                              if(p[_offset11] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                  else
		                    goto is_not_a_corner;
		              else
		                if(p[_offset7] < c_b)
		                  if(p[_offset1] < c_b)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset1] > cb)
		                      if(p[_offset6] > cb)
		                        if(p[_offset3] > cb)
		                          if(p[_offset4] > cb)
		                            if(p[_offset10] > cb)
		                              if(p[_offset11] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset6] < c_b)
		                          if(p[_offset3] > cb)
		                            if(p[_offset4] > cb)
		                              if(p[_offset10] > cb)
		                                if(p[_offset11] > cb)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          if(p[_offset3] > cb)
		                            if(p[_offset4] > cb)
		                              if(p[_offset10] > cb)
		                                if(p[_offset11] > cb)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                else
		                  if(p[_offset1] < c_b)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset1] > cb)
		                      if(p[_offset6] > cb)
		                        if(p[_offset3] > cb)
		                          if(p[_offset4] > cb)
		                            if(p[_offset10] > cb)
		                              if(p[_offset11] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset6] < c_b)
		                          if(p[_offset3] > cb)
		                            if(p[_offset4] > cb)
		                              if(p[_offset10] > cb)
		                                if(p[_offset11] > cb)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          if(p[_offset3] > cb)
		                            if(p[_offset4] > cb)
		                              if(p[_offset10] > cb)
		                                if(p[_offset11] > cb)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		            else
		              goto is_not_a_corner;
		    else
		      if(p[_offset2] < c_b)
		        if(p[_offset7] > cb)
		          if(p[_offset9] < c_b)
		            goto is_not_a_corner;
		          else
		            if(p[_offset9] > cb)
		              if(p[_offset1] < c_b)
		                if(p[_offset6] < c_b)
		                  goto is_not_a_corner;
		                else
		                  if(p[_offset6] > cb)
		                    if(p[_offset8] > cb)
		                      if(p[_offset10] > cb)
		                        if(p[_offset11] > cb)
		                          goto is_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                  else
		                    goto is_not_a_corner;
		              else
		                if(p[_offset1] > cb)
		                  if(p[_offset6] > cb)
		                    if(p[_offset8] > cb)
		                      if(p[_offset10] > cb)
		                        if(p[_offset11] > cb)
		                          goto is_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      if(p[_offset8] > cb)
		                        if(p[_offset10] > cb)
		                          if(p[_offset11] > cb)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset8] > cb)
		                        if(p[_offset10] > cb)
		                          if(p[_offset11] > cb)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                else
		                  if(p[_offset6] < c_b)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] > cb)
		                      if(p[_offset8] > cb)
		                        if(p[_offset10] > cb)
		                          if(p[_offset11] > cb)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		            else
		              goto is_not_a_corner;
		        else
		          goto is_not_a_corner;
		      else
		        if(p[_offset2] > cb)
		          if(p[_offset7] < c_b)
		            if(p[_offset9] < c_b)
		              if(p[_offset1] < c_b)
		                goto is_not_a_corner;
		              else
		                if(p[_offset1] > cb)
		                  if(p[_offset6] > cb)
		                    if(p[_offset3] > cb)
		                      if(p[_offset4] > cb)
		                        if(p[_offset10] > cb)
		                          if(p[_offset11] > cb)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      if(p[_offset3] > cb)
		                        if(p[_offset4] > cb)
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset3] > cb)
		                        if(p[_offset4] > cb)
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                else
		                  goto is_not_a_corner;
		            else
		              if(p[_offset9] > cb)
		                if(p[_offset1] < c_b)
		                  goto is_not_a_corner;
		                else
		                  if(p[_offset1] > cb)
		                    if(p[_offset6] > cb)
		                      if(p[_offset10] > cb)
		                        if(p[_offset11] > cb)
		                          if(p[_offset3] > cb)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset8] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset10] > cb)
		                          if(p[_offset11] > cb)
		                            if(p[_offset3] > cb)
		                              goto is_a_corner;
		                            else
		                              if(p[_offset8] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset10] > cb)
		                          if(p[_offset11] > cb)
		                            if(p[_offset3] > cb)
		                              goto is_a_corner;
		                            else
		                              if(p[_offset8] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                  else
		                    goto is_not_a_corner;
		              else
		                if(p[_offset1] < c_b)
		                  goto is_not_a_corner;
		                else
		                  if(p[_offset1] > cb)
		                    if(p[_offset6] > cb)
		                      if(p[_offset3] > cb)
		                        if(p[_offset4] > cb)
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset3] > cb)
		                          if(p[_offset4] > cb)
		                            if(p[_offset10] > cb)
		                              if(p[_offset11] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset3] > cb)
		                          if(p[_offset4] > cb)
		                            if(p[_offset10] > cb)
		                              if(p[_offset11] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                  else
		                    goto is_not_a_corner;
		          else
		            if(p[_offset9] < c_b)
		              if(p[_offset7] > cb)
		                if(p[_offset1] < c_b)
		                  goto is_not_a_corner;
		                else
		                  if(p[_offset1] > cb)
		                    if(p[_offset6] > cb)
		                      if(p[_offset3] > cb)
		                        if(p[_offset4] > cb)
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset3] > cb)
		                          if(p[_offset4] > cb)
		                            if(p[_offset10] > cb)
		                              if(p[_offset11] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset3] > cb)
		                          if(p[_offset4] > cb)
		                            if(p[_offset10] > cb)
		                              if(p[_offset11] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                  else
		                    goto is_not_a_corner;
		              else
		                if(p[_offset1] < c_b)
		                  goto is_not_a_corner;
		                else
		                  if(p[_offset1] > cb)
		                    if(p[_offset6] > cb)
		                      if(p[_offset3] > cb)
		                        if(p[_offset4] > cb)
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset3] > cb)
		                          if(p[_offset4] > cb)
		                            if(p[_offset10] > cb)
		                              if(p[_offset11] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset3] > cb)
		                          if(p[_offset4] > cb)
		                            if(p[_offset10] > cb)
		                              if(p[_offset11] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                  else
		                    goto is_not_a_corner;
		            else
		              if(p[_offset7] > cb)
		                if(p[_offset9] > cb)
		                  if(p[_offset1] < c_b)
		                    if(p[_offset6] < c_b)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] > cb)
		                        if(p[_offset8] > cb)
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                  else
		                    if(p[_offset1] > cb)
		                      if(p[_offset6] > cb)
		                        if(p[_offset10] > cb)
		                          if(p[_offset11] > cb)
		                            if(p[_offset3] > cb)
		                              goto is_a_corner;
		                            else
		                              if(p[_offset8] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset6] < c_b)
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              if(p[_offset3] > cb)
		                                goto is_a_corner;
		                              else
		                                if(p[_offset8] > cb)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              if(p[_offset3] > cb)
		                                goto is_a_corner;
		                              else
		                                if(p[_offset8] > cb)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        goto is_not_a_corner;
		                      else
		                        if(p[_offset6] > cb)
		                          if(p[_offset8] > cb)
		                            if(p[_offset10] > cb)
		                              if(p[_offset11] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                else
		                  if(p[_offset1] < c_b)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset1] > cb)
		                      if(p[_offset6] > cb)
		                        if(p[_offset3] > cb)
		                          if(p[_offset4] > cb)
		                            if(p[_offset10] > cb)
		                              if(p[_offset11] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset6] < c_b)
		                          if(p[_offset3] > cb)
		                            if(p[_offset4] > cb)
		                              if(p[_offset10] > cb)
		                                if(p[_offset11] > cb)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          if(p[_offset3] > cb)
		                            if(p[_offset4] > cb)
		                              if(p[_offset10] > cb)
		                                if(p[_offset11] > cb)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		              else
		                if(p[_offset9] > cb)
		                  if(p[_offset1] < c_b)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset1] > cb)
		                      if(p[_offset6] > cb)
		                        if(p[_offset10] > cb)
		                          if(p[_offset11] > cb)
		                            if(p[_offset3] > cb)
		                              goto is_a_corner;
		                            else
		                              if(p[_offset8] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset6] < c_b)
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              if(p[_offset3] > cb)
		                                goto is_a_corner;
		                              else
		                                if(p[_offset8] > cb)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              if(p[_offset3] > cb)
		                                goto is_a_corner;
		                              else
		                                if(p[_offset8] > cb)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                else
		                  if(p[_offset1] < c_b)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset1] > cb)
		                      if(p[_offset6] > cb)
		                        if(p[_offset3] > cb)
		                          if(p[_offset4] > cb)
		                            if(p[_offset10] > cb)
		                              if(p[_offset11] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset6] < c_b)
		                          if(p[_offset3] > cb)
		                            if(p[_offset4] > cb)
		                              if(p[_offset10] > cb)
		                                if(p[_offset11] > cb)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          if(p[_offset3] > cb)
		                            if(p[_offset4] > cb)
		                              if(p[_offset10] > cb)
		                                if(p[_offset11] > cb)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		        else
		          if(p[_offset7] > cb)
		            if(p[_offset9] < c_b)
		              goto is_not_a_corner;
		            else
		              if(p[_offset9] > cb)
		                if(p[_offset1] < c_b)
		                  if(p[_offset6] < c_b)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] > cb)
		                      if(p[_offset8] > cb)
		                        if(p[_offset10] > cb)
		                          if(p[_offset11] > cb)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                else
		                  if(p[_offset1] > cb)
		                    if(p[_offset6] > cb)
		                      if(p[_offset8] > cb)
		                        if(p[_offset10] > cb)
		                          if(p[_offset11] > cb)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset8] > cb)
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset8] > cb)
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] > cb)
		                        if(p[_offset8] > cb)
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		              else
		                goto is_not_a_corner;
		          else
		            goto is_not_a_corner;
		else if(p[_offset0] < c_b)
		  if(p[_offset5] < c_b)
		    if(p[_offset9] > cb)
		      if(p[_offset2] > cb)
		        goto is_not_a_corner;
		      else
		        if(p[_offset2] < c_b)
		          if(p[_offset7] > cb)
		            if(p[_offset1] > cb)
		              goto is_not_a_corner;
		            else
		              if(p[_offset1] < c_b)
		                if(p[_offset6] < c_b)
		                  if(p[_offset3] < c_b)
		                    if(p[_offset4] < c_b)
		                      goto is_a_corner;
		                    else
		                      goto is_not_a_corner;
		                  else
		                    goto is_not_a_corner;
		                else
		                  if(p[_offset6] > cb)
		                    if(p[_offset3] < c_b)
		                      if(p[_offset4] < c_b)
		                        if(p[_offset11] < c_b)
		                          goto is_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                  else
		                    if(p[_offset3] < c_b)
		                      if(p[_offset4] < c_b)
		                        if(p[_offset11] < c_b)
		                          goto is_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		              else
		                goto is_not_a_corner;
		          else
		            if(p[_offset7] < c_b)
		              if(p[_offset1] > cb)
		                if(p[_offset6] > cb)
		                  goto is_not_a_corner;
		                else
		                  if(p[_offset6] < c_b)
		                    if(p[_offset3] < c_b)
		                      if(p[_offset4] < c_b)
		                        if(p[_offset8] < c_b)
		                          goto is_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                  else
		                    goto is_not_a_corner;
		              else
		                if(p[_offset1] < c_b)
		                  if(p[_offset6] < c_b)
		                    if(p[_offset3] < c_b)
		                      if(p[_offset4] < c_b)
		                        goto is_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                  else
		                    if(p[_offset6] > cb)
		                      if(p[_offset3] < c_b)
		                        if(p[_offset4] < c_b)
		                          if(p[_offset11] < c_b)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset3] < c_b)
		                        if(p[_offset4] < c_b)
		                          if(p[_offset11] < c_b)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                else
		                  if(p[_offset6] > cb)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      if(p[_offset3] < c_b)
		                        if(p[_offset4] < c_b)
		                          if(p[_offset8] < c_b)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		            else
		              if(p[_offset1] > cb)
		                goto is_not_a_corner;
		              else
		                if(p[_offset1] < c_b)
		                  if(p[_offset6] < c_b)
		                    if(p[_offset3] < c_b)
		                      if(p[_offset4] < c_b)
		                        goto is_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                  else
		                    if(p[_offset6] > cb)
		                      if(p[_offset3] < c_b)
		                        if(p[_offset4] < c_b)
		                          if(p[_offset11] < c_b)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset3] < c_b)
		                        if(p[_offset4] < c_b)
		                          if(p[_offset11] < c_b)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                else
		                  goto is_not_a_corner;
		        else
		          goto is_not_a_corner;
		    else
		      if(p[_offset9] < c_b)
		        if(p[_offset7] > cb)
		          if(p[_offset2] > cb)
		            goto is_not_a_corner;
		          else
		            if(p[_offset2] < c_b)
		              if(p[_offset1] > cb)
		                goto is_not_a_corner;
		              else
		                if(p[_offset1] < c_b)
		                  if(p[_offset6] > cb)
		                    if(p[_offset11] < c_b)
		                      if(p[_offset3] < c_b)
		                        if(p[_offset4] < c_b)
		                          goto is_a_corner;
		                        else
		                          if(p[_offset10] < c_b)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        if(p[_offset8] < c_b)
		                          if(p[_offset10] < c_b)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      if(p[_offset3] < c_b)
		                        if(p[_offset4] < c_b)
		                          goto is_a_corner;
		                        else
		                          if(p[_offset10] < c_b)
		                            if(p[_offset11] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        if(p[_offset8] < c_b)
		                          if(p[_offset10] < c_b)
		                            if(p[_offset11] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                    else
		                      if(p[_offset11] < c_b)
		                        if(p[_offset3] < c_b)
		                          if(p[_offset4] < c_b)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset10] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          if(p[_offset8] < c_b)
		                            if(p[_offset10] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                else
		                  goto is_not_a_corner;
		            else
		              goto is_not_a_corner;
		        else
		          if(p[_offset7] < c_b)
		            if(p[_offset2] > cb)
		              if(p[_offset1] > cb)
		                if(p[_offset6] > cb)
		                  goto is_not_a_corner;
		                else
		                  if(p[_offset6] < c_b)
		                    if(p[_offset8] < c_b)
		                      if(p[_offset4] < c_b)
		                        if(p[_offset3] < c_b)
		                          goto is_a_corner;
		                        else
		                          if(p[_offset10] < c_b)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        if(p[_offset10] < c_b)
		                          if(p[_offset11] < c_b)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                  else
		                    goto is_not_a_corner;
		              else
		                if(p[_offset1] < c_b)
		                  if(p[_offset6] > cb)
		                    if(p[_offset8] < c_b)
		                      if(p[_offset10] < c_b)
		                        if(p[_offset11] < c_b)
		                          goto is_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      if(p[_offset8] < c_b)
		                        if(p[_offset4] < c_b)
		                          if(p[_offset3] < c_b)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset10] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          if(p[_offset10] < c_b)
		                            if(p[_offset11] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset8] < c_b)
		                        if(p[_offset10] < c_b)
		                          if(p[_offset11] < c_b)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                else
		                  if(p[_offset6] > cb)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      if(p[_offset8] < c_b)
		                        if(p[_offset4] < c_b)
		                          if(p[_offset3] < c_b)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset10] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          if(p[_offset10] < c_b)
		                            if(p[_offset11] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		            else
		              if(p[_offset2] < c_b)
		                if(p[_offset1] > cb)
		                  if(p[_offset6] > cb)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      if(p[_offset8] < c_b)
		                        if(p[_offset4] < c_b)
		                          if(p[_offset3] < c_b)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset10] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          if(p[_offset10] < c_b)
		                            if(p[_offset11] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                else
		                  if(p[_offset1] < c_b)
		                    if(p[_offset6] > cb)
		                      if(p[_offset11] < c_b)
		                        if(p[_offset3] < c_b)
		                          if(p[_offset4] < c_b)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset10] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          if(p[_offset8] < c_b)
		                            if(p[_offset10] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset3] < c_b)
		                          if(p[_offset4] < c_b)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset10] < c_b)
		                              if(p[_offset11] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          if(p[_offset8] < c_b)
		                            if(p[_offset10] < c_b)
		                              if(p[_offset4] < c_b)
		                                goto is_a_corner;
		                              else
		                                if(p[_offset11] < c_b)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        if(p[_offset11] < c_b)
		                          if(p[_offset3] < c_b)
		                            if(p[_offset4] < c_b)
		                              goto is_a_corner;
		                            else
		                              if(p[_offset10] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            if(p[_offset8] < c_b)
		                              if(p[_offset10] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                  else
		                    if(p[_offset6] > cb)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset8] < c_b)
		                          if(p[_offset4] < c_b)
		                            if(p[_offset3] < c_b)
		                              goto is_a_corner;
		                            else
		                              if(p[_offset10] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            if(p[_offset10] < c_b)
		                              if(p[_offset11] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		              else
		                if(p[_offset1] > cb)
		                  if(p[_offset6] > cb)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      if(p[_offset8] < c_b)
		                        if(p[_offset4] < c_b)
		                          if(p[_offset3] < c_b)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset10] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          if(p[_offset10] < c_b)
		                            if(p[_offset11] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                else
		                  if(p[_offset1] < c_b)
		                    if(p[_offset6] > cb)
		                      if(p[_offset8] < c_b)
		                        if(p[_offset10] < c_b)
		                          if(p[_offset11] < c_b)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset8] < c_b)
		                          if(p[_offset4] < c_b)
		                            if(p[_offset3] < c_b)
		                              goto is_a_corner;
		                            else
		                              if(p[_offset10] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            if(p[_offset10] < c_b)
		                              if(p[_offset11] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset8] < c_b)
		                          if(p[_offset10] < c_b)
		                            if(p[_offset11] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                  else
		                    if(p[_offset6] > cb)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset8] < c_b)
		                          if(p[_offset4] < c_b)
		                            if(p[_offset3] < c_b)
		                              goto is_a_corner;
		                            else
		                              if(p[_offset10] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            if(p[_offset10] < c_b)
		                              if(p[_offset11] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		          else
		            if(p[_offset2] > cb)
		              goto is_not_a_corner;
		            else
		              if(p[_offset2] < c_b)
		                if(p[_offset1] > cb)
		                  goto is_not_a_corner;
		                else
		                  if(p[_offset1] < c_b)
		                    if(p[_offset6] > cb)
		                      if(p[_offset11] < c_b)
		                        if(p[_offset3] < c_b)
		                          if(p[_offset4] < c_b)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset10] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          if(p[_offset8] < c_b)
		                            if(p[_offset10] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset3] < c_b)
		                          if(p[_offset4] < c_b)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset10] < c_b)
		                              if(p[_offset11] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          if(p[_offset8] < c_b)
		                            if(p[_offset10] < c_b)
		                              if(p[_offset11] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        if(p[_offset11] < c_b)
		                          if(p[_offset3] < c_b)
		                            if(p[_offset4] < c_b)
		                              goto is_a_corner;
		                            else
		                              if(p[_offset10] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            if(p[_offset8] < c_b)
		                              if(p[_offset10] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                  else
		                    goto is_not_a_corner;
		              else
		                goto is_not_a_corner;
		      else
		        if(p[_offset2] > cb)
		          goto is_not_a_corner;
		        else
		          if(p[_offset2] < c_b)
		            if(p[_offset7] > cb)
		              if(p[_offset1] > cb)
		                goto is_not_a_corner;
		              else
		                if(p[_offset1] < c_b)
		                  if(p[_offset6] < c_b)
		                    if(p[_offset3] < c_b)
		                      if(p[_offset4] < c_b)
		                        goto is_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                  else
		                    if(p[_offset6] > cb)
		                      if(p[_offset3] < c_b)
		                        if(p[_offset4] < c_b)
		                          if(p[_offset11] < c_b)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset3] < c_b)
		                        if(p[_offset4] < c_b)
		                          if(p[_offset11] < c_b)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                else
		                  goto is_not_a_corner;
		            else
		              if(p[_offset7] < c_b)
		                if(p[_offset1] > cb)
		                  if(p[_offset6] > cb)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      if(p[_offset3] < c_b)
		                        if(p[_offset4] < c_b)
		                          if(p[_offset8] < c_b)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                else
		                  if(p[_offset1] < c_b)
		                    if(p[_offset6] < c_b)
		                      if(p[_offset3] < c_b)
		                        if(p[_offset4] < c_b)
		                          goto is_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset6] > cb)
		                        if(p[_offset3] < c_b)
		                          if(p[_offset4] < c_b)
		                            if(p[_offset11] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset3] < c_b)
		                          if(p[_offset4] < c_b)
		                            if(p[_offset11] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                  else
		                    if(p[_offset6] > cb)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset3] < c_b)
		                          if(p[_offset4] < c_b)
		                            if(p[_offset8] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		              else
		                if(p[_offset1] > cb)
		                  goto is_not_a_corner;
		                else
		                  if(p[_offset1] < c_b)
		                    if(p[_offset6] < c_b)
		                      if(p[_offset3] < c_b)
		                        if(p[_offset4] < c_b)
		                          goto is_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset6] > cb)
		                        if(p[_offset3] < c_b)
		                          if(p[_offset4] < c_b)
		                            if(p[_offset11] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset3] < c_b)
		                          if(p[_offset4] < c_b)
		                            if(p[_offset11] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                  else
		                    goto is_not_a_corner;
		          else
		            goto is_not_a_corner;
		  else
		    if(p[_offset5] > cb)
		      if(p[_offset2] > cb)
		        if(p[_offset7] < c_b)
		          if(p[_offset9] > cb)
		            goto is_not_a_corner;
		          else
		            if(p[_offset9] < c_b)
		              if(p[_offset1] > cb)
		                if(p[_offset6] > cb)
		                  goto is_not_a_corner;
		                else
		                  if(p[_offset6] < c_b)
		                    if(p[_offset8] < c_b)
		                      if(p[_offset10] < c_b)
		                        if(p[_offset11] < c_b)
		                          goto is_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                  else
		                    goto is_not_a_corner;
		              else
		                if(p[_offset1] < c_b)
		                  if(p[_offset6] > cb)
		                    if(p[_offset8] < c_b)
		                      if(p[_offset10] < c_b)
		                        if(p[_offset11] < c_b)
		                          goto is_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      if(p[_offset8] < c_b)
		                        if(p[_offset10] < c_b)
		                          if(p[_offset11] < c_b)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset8] < c_b)
		                        if(p[_offset10] < c_b)
		                          if(p[_offset11] < c_b)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                else
		                  if(p[_offset6] > cb)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      if(p[_offset8] < c_b)
		                        if(p[_offset10] < c_b)
		                          if(p[_offset11] < c_b)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		            else
		              goto is_not_a_corner;
		        else
		          if(p[_offset7] > cb)
		            if(p[_offset9] < c_b)
		              if(p[_offset1] > cb)
		                if(p[_offset6] < c_b)
		                  goto is_not_a_corner;
		                else
		                  if(p[_offset6] > cb)
		                    if(p[_offset3] > cb)
		                      if(p[_offset4] > cb)
		                        goto is_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                  else
		                    goto is_not_a_corner;
		              else
		                if(p[_offset1] < c_b)
		                  if(p[_offset6] < c_b)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] > cb)
		                      if(p[_offset3] > cb)
		                        if(p[_offset4] > cb)
		                          if(p[_offset8] > cb)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                else
		                  if(p[_offset6] < c_b)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] > cb)
		                      if(p[_offset3] > cb)
		                        if(p[_offset4] > cb)
		                          if(p[_offset8] > cb)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		            else
		              if(p[_offset9] > cb)
		                if(p[_offset1] < c_b)
		                  if(p[_offset6] < c_b)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] > cb)
		                      if(p[_offset8] > cb)
		                        if(p[_offset4] > cb)
		                          if(p[_offset3] > cb)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset10] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                else
		                  if(p[_offset1] > cb)
		                    if(p[_offset6] < c_b)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] > cb)
		                        if(p[_offset4] > cb)
		                          if(p[_offset3] > cb)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset8] > cb)
		                              if(p[_offset10] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          if(p[_offset8] > cb)
		                            if(p[_offset10] > cb)
		                              if(p[_offset11] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] > cb)
		                        if(p[_offset8] > cb)
		                          if(p[_offset4] > cb)
		                            if(p[_offset3] > cb)
		                              goto is_a_corner;
		                            else
		                              if(p[_offset10] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            if(p[_offset10] > cb)
		                              if(p[_offset11] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		              else
		                if(p[_offset1] > cb)
		                  if(p[_offset6] < c_b)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] > cb)
		                      if(p[_offset3] > cb)
		                        if(p[_offset4] > cb)
		                          goto is_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                else
		                  if(p[_offset1] < c_b)
		                    if(p[_offset6] < c_b)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] > cb)
		                        if(p[_offset3] > cb)
		                          if(p[_offset4] > cb)
		                            if(p[_offset8] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] > cb)
		                        if(p[_offset3] > cb)
		                          if(p[_offset4] > cb)
		                            if(p[_offset8] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		          else
		            goto is_not_a_corner;
		      else
		        if(p[_offset2] < c_b)
		          if(p[_offset7] < c_b)
		            if(p[_offset9] > cb)
		              if(p[_offset1] > cb)
		                goto is_not_a_corner;
		              else
		                if(p[_offset1] < c_b)
		                  if(p[_offset6] > cb)
		                    if(p[_offset3] < c_b)
		                      if(p[_offset4] < c_b)
		                        if(p[_offset10] < c_b)
		                          if(p[_offset11] < c_b)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      if(p[_offset3] < c_b)
		                        if(p[_offset4] < c_b)
		                          if(p[_offset10] < c_b)
		                            if(p[_offset11] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset3] < c_b)
		                        if(p[_offset4] < c_b)
		                          if(p[_offset10] < c_b)
		                            if(p[_offset11] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                else
		                  goto is_not_a_corner;
		            else
		              if(p[_offset9] < c_b)
		                if(p[_offset1] > cb)
		                  if(p[_offset6] > cb)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      if(p[_offset8] < c_b)
		                        if(p[_offset10] < c_b)
		                          if(p[_offset11] < c_b)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                else
		                  if(p[_offset1] < c_b)
		                    if(p[_offset6] > cb)
		                      if(p[_offset10] < c_b)
		                        if(p[_offset11] < c_b)
		                          if(p[_offset3] < c_b)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset8] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset10] < c_b)
		                          if(p[_offset11] < c_b)
		                            if(p[_offset3] < c_b)
		                              goto is_a_corner;
		                            else
		                              if(p[_offset8] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset10] < c_b)
		                          if(p[_offset11] < c_b)
		                            if(p[_offset3] < c_b)
		                              goto is_a_corner;
		                            else
		                              if(p[_offset8] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                  else
		                    if(p[_offset6] > cb)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset8] < c_b)
		                          if(p[_offset10] < c_b)
		                            if(p[_offset11] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		              else
		                if(p[_offset1] > cb)
		                  goto is_not_a_corner;
		                else
		                  if(p[_offset1] < c_b)
		                    if(p[_offset6] > cb)
		                      if(p[_offset3] < c_b)
		                        if(p[_offset4] < c_b)
		                          if(p[_offset10] < c_b)
		                            if(p[_offset11] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset3] < c_b)
		                          if(p[_offset4] < c_b)
		                            if(p[_offset10] < c_b)
		                              if(p[_offset11] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset3] < c_b)
		                          if(p[_offset4] < c_b)
		                            if(p[_offset10] < c_b)
		                              if(p[_offset11] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                  else
		                    goto is_not_a_corner;
		          else
		            if(p[_offset7] > cb)
		              if(p[_offset9] < c_b)
		                if(p[_offset1] > cb)
		                  goto is_not_a_corner;
		                else
		                  if(p[_offset1] < c_b)
		                    if(p[_offset6] > cb)
		                      if(p[_offset10] < c_b)
		                        if(p[_offset11] < c_b)
		                          if(p[_offset3] < c_b)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset8] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset10] < c_b)
		                          if(p[_offset11] < c_b)
		                            if(p[_offset3] < c_b)
		                              goto is_a_corner;
		                            else
		                              if(p[_offset8] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset10] < c_b)
		                          if(p[_offset11] < c_b)
		                            if(p[_offset3] < c_b)
		                              goto is_a_corner;
		                            else
		                              if(p[_offset8] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                  else
		                    goto is_not_a_corner;
		              else
		                if(p[_offset9] > cb)
		                  if(p[_offset1] > cb)
		                    if(p[_offset6] < c_b)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] > cb)
		                        if(p[_offset8] > cb)
		                          if(p[_offset4] > cb)
		                            if(p[_offset3] > cb)
		                              goto is_a_corner;
		                            else
		                              if(p[_offset10] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            if(p[_offset10] > cb)
		                              if(p[_offset11] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                  else
		                    if(p[_offset1] < c_b)
		                      if(p[_offset6] < c_b)
		                        if(p[_offset3] < c_b)
		                          if(p[_offset4] < c_b)
		                            if(p[_offset10] < c_b)
		                              if(p[_offset11] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset6] > cb)
		                          if(p[_offset4] < c_b)
		                            if(p[_offset10] > cb)
		                              if(p[_offset8] > cb)
		                                if(p[_offset11] > cb)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              if(p[_offset3] < c_b)
		                                if(p[_offset11] < c_b)
		                                  if(p[_offset10] < c_b)
		                                    goto is_a_corner;
		                                  else
		                                    goto is_not_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            if(p[_offset8] > cb)
		                              if(p[_offset10] > cb)
		                                if(p[_offset4] > cb)
		                                  goto is_a_corner;
		                                else
		                                  if(p[_offset11] > cb)
		                                    goto is_a_corner;
		                                  else
		                                    goto is_not_a_corner;
		                              else
		                                if(p[_offset3] > cb)
		                                  if(p[_offset4] > cb)
		                                    goto is_a_corner;
		                                  else
		                                    goto is_not_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          if(p[_offset3] < c_b)
		                            if(p[_offset4] < c_b)
		                              if(p[_offset10] < c_b)
		                                if(p[_offset11] < c_b)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        goto is_not_a_corner;
		                      else
		                        if(p[_offset6] > cb)
		                          if(p[_offset8] > cb)
		                            if(p[_offset4] > cb)
		                              if(p[_offset3] > cb)
		                                goto is_a_corner;
		                              else
		                                if(p[_offset10] > cb)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                            else
		                              if(p[_offset10] > cb)
		                                if(p[_offset11] > cb)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                else
		                  if(p[_offset1] > cb)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset1] < c_b)
		                      if(p[_offset6] > cb)
		                        if(p[_offset3] < c_b)
		                          if(p[_offset4] < c_b)
		                            if(p[_offset10] < c_b)
		                              if(p[_offset11] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset6] < c_b)
		                          if(p[_offset3] < c_b)
		                            if(p[_offset4] < c_b)
		                              if(p[_offset10] < c_b)
		                                if(p[_offset11] < c_b)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          if(p[_offset3] < c_b)
		                            if(p[_offset4] < c_b)
		                              if(p[_offset10] < c_b)
		                                if(p[_offset11] < c_b)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		            else
		              if(p[_offset9] > cb)
		                if(p[_offset1] > cb)
		                  goto is_not_a_corner;
		                else
		                  if(p[_offset1] < c_b)
		                    if(p[_offset6] > cb)
		                      if(p[_offset3] < c_b)
		                        if(p[_offset4] < c_b)
		                          if(p[_offset10] < c_b)
		                            if(p[_offset11] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset3] < c_b)
		                          if(p[_offset4] < c_b)
		                            if(p[_offset10] < c_b)
		                              if(p[_offset11] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset3] < c_b)
		                          if(p[_offset4] < c_b)
		                            if(p[_offset10] < c_b)
		                              if(p[_offset11] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                  else
		                    goto is_not_a_corner;
		              else
		                if(p[_offset9] < c_b)
		                  if(p[_offset1] > cb)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset1] < c_b)
		                      if(p[_offset6] > cb)
		                        if(p[_offset10] < c_b)
		                          if(p[_offset11] < c_b)
		                            if(p[_offset3] < c_b)
		                              goto is_a_corner;
		                            else
		                              if(p[_offset8] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset6] < c_b)
		                          if(p[_offset10] < c_b)
		                            if(p[_offset11] < c_b)
		                              if(p[_offset3] < c_b)
		                                goto is_a_corner;
		                              else
		                                if(p[_offset8] < c_b)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          if(p[_offset10] < c_b)
		                            if(p[_offset11] < c_b)
		                              if(p[_offset3] < c_b)
		                                goto is_a_corner;
		                              else
		                                if(p[_offset8] < c_b)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                else
		                  if(p[_offset1] > cb)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset1] < c_b)
		                      if(p[_offset6] > cb)
		                        if(p[_offset3] < c_b)
		                          if(p[_offset4] < c_b)
		                            if(p[_offset10] < c_b)
		                              if(p[_offset11] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset6] < c_b)
		                          if(p[_offset3] < c_b)
		                            if(p[_offset4] < c_b)
		                              if(p[_offset10] < c_b)
		                                if(p[_offset11] < c_b)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          if(p[_offset3] < c_b)
		                            if(p[_offset4] < c_b)
		                              if(p[_offset10] < c_b)
		                                if(p[_offset11] < c_b)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		        else
		          if(p[_offset7] > cb)
		            if(p[_offset9] < c_b)
		              goto is_not_a_corner;
		            else
		              if(p[_offset9] > cb)
		                if(p[_offset1] > cb)
		                  if(p[_offset6] < c_b)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] > cb)
		                      if(p[_offset8] > cb)
		                        if(p[_offset4] > cb)
		                          if(p[_offset3] > cb)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset10] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                else
		                  if(p[_offset1] < c_b)
		                    if(p[_offset6] < c_b)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] > cb)
		                        if(p[_offset8] > cb)
		                          if(p[_offset4] > cb)
		                            if(p[_offset3] > cb)
		                              goto is_a_corner;
		                            else
		                              if(p[_offset10] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            if(p[_offset10] > cb)
		                              if(p[_offset11] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] > cb)
		                        if(p[_offset8] > cb)
		                          if(p[_offset4] > cb)
		                            if(p[_offset3] > cb)
		                              goto is_a_corner;
		                            else
		                              if(p[_offset10] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            if(p[_offset10] > cb)
		                              if(p[_offset11] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		              else
		                goto is_not_a_corner;
		          else
		            if(p[_offset9] < c_b)
		              if(p[_offset7] < c_b)
		                if(p[_offset1] > cb)
		                  if(p[_offset6] > cb)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      if(p[_offset8] < c_b)
		                        if(p[_offset10] < c_b)
		                          if(p[_offset11] < c_b)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                else
		                  if(p[_offset1] < c_b)
		                    if(p[_offset6] > cb)
		                      if(p[_offset8] < c_b)
		                        if(p[_offset10] < c_b)
		                          if(p[_offset11] < c_b)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset8] < c_b)
		                          if(p[_offset10] < c_b)
		                            if(p[_offset11] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset8] < c_b)
		                          if(p[_offset10] < c_b)
		                            if(p[_offset11] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                  else
		                    if(p[_offset6] > cb)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset8] < c_b)
		                          if(p[_offset10] < c_b)
		                            if(p[_offset11] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		              else
		                goto is_not_a_corner;
		            else
		              goto is_not_a_corner;
		    else
		      if(p[_offset2] > cb)
		        if(p[_offset7] < c_b)
		          if(p[_offset9] > cb)
		            goto is_not_a_corner;
		          else
		            if(p[_offset9] < c_b)
		              if(p[_offset1] > cb)
		                if(p[_offset6] > cb)
		                  goto is_not_a_corner;
		                else
		                  if(p[_offset6] < c_b)
		                    if(p[_offset8] < c_b)
		                      if(p[_offset10] < c_b)
		                        if(p[_offset11] < c_b)
		                          goto is_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                  else
		                    goto is_not_a_corner;
		              else
		                if(p[_offset1] < c_b)
		                  if(p[_offset6] > cb)
		                    if(p[_offset8] < c_b)
		                      if(p[_offset10] < c_b)
		                        if(p[_offset11] < c_b)
		                          goto is_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      if(p[_offset8] < c_b)
		                        if(p[_offset10] < c_b)
		                          if(p[_offset11] < c_b)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset8] < c_b)
		                        if(p[_offset10] < c_b)
		                          if(p[_offset11] < c_b)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                else
		                  if(p[_offset6] > cb)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      if(p[_offset8] < c_b)
		                        if(p[_offset10] < c_b)
		                          if(p[_offset11] < c_b)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		            else
		              goto is_not_a_corner;
		        else
		          goto is_not_a_corner;
		      else
		        if(p[_offset2] < c_b)
		          if(p[_offset7] > cb)
		            if(p[_offset9] > cb)
		              if(p[_offset1] > cb)
		                goto is_not_a_corner;
		              else
		                if(p[_offset1] < c_b)
		                  if(p[_offset6] > cb)
		                    if(p[_offset3] < c_b)
		                      if(p[_offset4] < c_b)
		                        if(p[_offset10] < c_b)
		                          if(p[_offset11] < c_b)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      if(p[_offset3] < c_b)
		                        if(p[_offset4] < c_b)
		                          if(p[_offset10] < c_b)
		                            if(p[_offset11] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset3] < c_b)
		                        if(p[_offset4] < c_b)
		                          if(p[_offset10] < c_b)
		                            if(p[_offset11] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                else
		                  goto is_not_a_corner;
		            else
		              if(p[_offset9] < c_b)
		                if(p[_offset1] > cb)
		                  goto is_not_a_corner;
		                else
		                  if(p[_offset1] < c_b)
		                    if(p[_offset6] > cb)
		                      if(p[_offset10] < c_b)
		                        if(p[_offset11] < c_b)
		                          if(p[_offset3] < c_b)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset8] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset10] < c_b)
		                          if(p[_offset11] < c_b)
		                            if(p[_offset3] < c_b)
		                              goto is_a_corner;
		                            else
		                              if(p[_offset8] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset10] < c_b)
		                          if(p[_offset11] < c_b)
		                            if(p[_offset3] < c_b)
		                              goto is_a_corner;
		                            else
		                              if(p[_offset8] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                  else
		                    goto is_not_a_corner;
		              else
		                if(p[_offset1] > cb)
		                  goto is_not_a_corner;
		                else
		                  if(p[_offset1] < c_b)
		                    if(p[_offset6] > cb)
		                      if(p[_offset3] < c_b)
		                        if(p[_offset4] < c_b)
		                          if(p[_offset10] < c_b)
		                            if(p[_offset11] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset3] < c_b)
		                          if(p[_offset4] < c_b)
		                            if(p[_offset10] < c_b)
		                              if(p[_offset11] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset3] < c_b)
		                          if(p[_offset4] < c_b)
		                            if(p[_offset10] < c_b)
		                              if(p[_offset11] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                  else
		                    goto is_not_a_corner;
		          else
		            if(p[_offset9] > cb)
		              if(p[_offset7] < c_b)
		                if(p[_offset1] > cb)
		                  goto is_not_a_corner;
		                else
		                  if(p[_offset1] < c_b)
		                    if(p[_offset6] > cb)
		                      if(p[_offset3] < c_b)
		                        if(p[_offset4] < c_b)
		                          if(p[_offset10] < c_b)
		                            if(p[_offset11] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset3] < c_b)
		                          if(p[_offset4] < c_b)
		                            if(p[_offset10] < c_b)
		                              if(p[_offset11] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset3] < c_b)
		                          if(p[_offset4] < c_b)
		                            if(p[_offset10] < c_b)
		                              if(p[_offset11] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                  else
		                    goto is_not_a_corner;
		              else
		                if(p[_offset1] > cb)
		                  goto is_not_a_corner;
		                else
		                  if(p[_offset1] < c_b)
		                    if(p[_offset6] > cb)
		                      if(p[_offset3] < c_b)
		                        if(p[_offset4] < c_b)
		                          if(p[_offset10] < c_b)
		                            if(p[_offset11] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset3] < c_b)
		                          if(p[_offset4] < c_b)
		                            if(p[_offset10] < c_b)
		                              if(p[_offset11] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset3] < c_b)
		                          if(p[_offset4] < c_b)
		                            if(p[_offset10] < c_b)
		                              if(p[_offset11] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                  else
		                    goto is_not_a_corner;
		            else
		              if(p[_offset7] < c_b)
		                if(p[_offset9] < c_b)
		                  if(p[_offset1] > cb)
		                    if(p[_offset6] > cb)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset8] < c_b)
		                          if(p[_offset10] < c_b)
		                            if(p[_offset11] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                  else
		                    if(p[_offset1] < c_b)
		                      if(p[_offset6] > cb)
		                        if(p[_offset10] < c_b)
		                          if(p[_offset11] < c_b)
		                            if(p[_offset3] < c_b)
		                              goto is_a_corner;
		                            else
		                              if(p[_offset8] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset6] < c_b)
		                          if(p[_offset10] < c_b)
		                            if(p[_offset11] < c_b)
		                              if(p[_offset3] < c_b)
		                                goto is_a_corner;
		                              else
		                                if(p[_offset8] < c_b)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          if(p[_offset10] < c_b)
		                            if(p[_offset11] < c_b)
		                              if(p[_offset3] < c_b)
		                                goto is_a_corner;
		                              else
		                                if(p[_offset8] < c_b)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                    else
		                      if(p[_offset6] > cb)
		                        goto is_not_a_corner;
		                      else
		                        if(p[_offset6] < c_b)
		                          if(p[_offset8] < c_b)
		                            if(p[_offset10] < c_b)
		                              if(p[_offset11] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                else
		                  if(p[_offset1] > cb)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset1] < c_b)
		                      if(p[_offset6] > cb)
		                        if(p[_offset3] < c_b)
		                          if(p[_offset4] < c_b)
		                            if(p[_offset10] < c_b)
		                              if(p[_offset11] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset6] < c_b)
		                          if(p[_offset3] < c_b)
		                            if(p[_offset4] < c_b)
		                              if(p[_offset10] < c_b)
		                                if(p[_offset11] < c_b)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          if(p[_offset3] < c_b)
		                            if(p[_offset4] < c_b)
		                              if(p[_offset10] < c_b)
		                                if(p[_offset11] < c_b)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		              else
		                if(p[_offset9] < c_b)
		                  if(p[_offset1] > cb)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset1] < c_b)
		                      if(p[_offset6] > cb)
		                        if(p[_offset10] < c_b)
		                          if(p[_offset11] < c_b)
		                            if(p[_offset3] < c_b)
		                              goto is_a_corner;
		                            else
		                              if(p[_offset8] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset6] < c_b)
		                          if(p[_offset10] < c_b)
		                            if(p[_offset11] < c_b)
		                              if(p[_offset3] < c_b)
		                                goto is_a_corner;
		                              else
		                                if(p[_offset8] < c_b)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          if(p[_offset10] < c_b)
		                            if(p[_offset11] < c_b)
		                              if(p[_offset3] < c_b)
		                                goto is_a_corner;
		                              else
		                                if(p[_offset8] < c_b)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                else
		                  if(p[_offset1] > cb)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset1] < c_b)
		                      if(p[_offset6] > cb)
		                        if(p[_offset3] < c_b)
		                          if(p[_offset4] < c_b)
		                            if(p[_offset10] < c_b)
		                              if(p[_offset11] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset6] < c_b)
		                          if(p[_offset3] < c_b)
		                            if(p[_offset4] < c_b)
		                              if(p[_offset10] < c_b)
		                                if(p[_offset11] < c_b)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          if(p[_offset3] < c_b)
		                            if(p[_offset4] < c_b)
		                              if(p[_offset10] < c_b)
		                                if(p[_offset11] < c_b)
		                                  goto is_a_corner;
		                                else
		                                  goto is_not_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		        else
		          if(p[_offset7] < c_b)
		            if(p[_offset9] > cb)
		              goto is_not_a_corner;
		            else
		              if(p[_offset9] < c_b)
		                if(p[_offset1] > cb)
		                  if(p[_offset6] > cb)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      if(p[_offset8] < c_b)
		                        if(p[_offset10] < c_b)
		                          if(p[_offset11] < c_b)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                else
		                  if(p[_offset1] < c_b)
		                    if(p[_offset6] > cb)
		                      if(p[_offset8] < c_b)
		                        if(p[_offset10] < c_b)
		                          if(p[_offset11] < c_b)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset8] < c_b)
		                          if(p[_offset10] < c_b)
		                            if(p[_offset11] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        if(p[_offset8] < c_b)
		                          if(p[_offset10] < c_b)
		                            if(p[_offset11] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                  else
		                    if(p[_offset6] > cb)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset8] < c_b)
		                          if(p[_offset10] < c_b)
		                            if(p[_offset11] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		              else
		                goto is_not_a_corner;
		          else
		            goto is_not_a_corner;
		else
		  if(p[_offset5] < c_b)
		    if(p[_offset7] > cb)
		      goto is_not_a_corner;
		    else
		      if(p[_offset7] < c_b)
		        if(p[_offset2] > cb)
		          if(p[_offset9] > cb)
		            goto is_not_a_corner;
		          else
		            if(p[_offset9] < c_b)
		              if(p[_offset1] > cb)
		                if(p[_offset6] > cb)
		                  goto is_not_a_corner;
		                else
		                  if(p[_offset6] < c_b)
		                    if(p[_offset8] < c_b)
		                      if(p[_offset4] < c_b)
		                        if(p[_offset3] < c_b)
		                          goto is_a_corner;
		                        else
		                          if(p[_offset10] < c_b)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        if(p[_offset10] < c_b)
		                          if(p[_offset11] < c_b)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                  else
		                    goto is_not_a_corner;
		              else
		                if(p[_offset1] < c_b)
		                  if(p[_offset6] > cb)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      if(p[_offset8] < c_b)
		                        if(p[_offset4] < c_b)
		                          if(p[_offset3] < c_b)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset10] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          if(p[_offset10] < c_b)
		                            if(p[_offset11] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                else
		                  if(p[_offset6] > cb)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      if(p[_offset8] < c_b)
		                        if(p[_offset4] < c_b)
		                          if(p[_offset3] < c_b)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset10] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          if(p[_offset10] < c_b)
		                            if(p[_offset11] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		            else
		              goto is_not_a_corner;
		        else
		          if(p[_offset2] < c_b)
		            if(p[_offset9] > cb)
		              if(p[_offset1] < c_b)
		                if(p[_offset6] > cb)
		                  goto is_not_a_corner;
		                else
		                  if(p[_offset6] < c_b)
		                    if(p[_offset3] < c_b)
		                      if(p[_offset4] < c_b)
		                        goto is_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                  else
		                    goto is_not_a_corner;
		              else
		                if(p[_offset1] > cb)
		                  if(p[_offset6] > cb)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      if(p[_offset3] < c_b)
		                        if(p[_offset4] < c_b)
		                          if(p[_offset8] < c_b)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                else
		                  if(p[_offset6] > cb)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      if(p[_offset3] < c_b)
		                        if(p[_offset4] < c_b)
		                          if(p[_offset8] < c_b)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		            else
		              if(p[_offset9] < c_b)
		                if(p[_offset1] > cb)
		                  if(p[_offset6] > cb)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      if(p[_offset8] < c_b)
		                        if(p[_offset4] < c_b)
		                          if(p[_offset3] < c_b)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset10] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          if(p[_offset10] < c_b)
		                            if(p[_offset11] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                else
		                  if(p[_offset1] < c_b)
		                    if(p[_offset6] > cb)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset4] < c_b)
		                          if(p[_offset3] < c_b)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset8] < c_b)
		                              if(p[_offset10] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          if(p[_offset8] < c_b)
		                            if(p[_offset10] < c_b)
		                              if(p[_offset11] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                  else
		                    if(p[_offset6] > cb)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset8] < c_b)
		                          if(p[_offset4] < c_b)
		                            if(p[_offset3] < c_b)
		                              goto is_a_corner;
		                            else
		                              if(p[_offset10] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            if(p[_offset10] < c_b)
		                              if(p[_offset11] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		              else
		                if(p[_offset1] < c_b)
		                  if(p[_offset6] > cb)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      if(p[_offset3] < c_b)
		                        if(p[_offset4] < c_b)
		                          goto is_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                else
		                  if(p[_offset1] > cb)
		                    if(p[_offset6] > cb)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset3] < c_b)
		                          if(p[_offset4] < c_b)
		                            if(p[_offset8] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                  else
		                    if(p[_offset6] > cb)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset3] < c_b)
		                          if(p[_offset4] < c_b)
		                            if(p[_offset8] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		          else
		            if(p[_offset9] > cb)
		              goto is_not_a_corner;
		            else
		              if(p[_offset9] < c_b)
		                if(p[_offset1] > cb)
		                  if(p[_offset6] > cb)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      if(p[_offset8] < c_b)
		                        if(p[_offset4] < c_b)
		                          if(p[_offset3] < c_b)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset10] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          if(p[_offset10] < c_b)
		                            if(p[_offset11] < c_b)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                else
		                  if(p[_offset1] < c_b)
		                    if(p[_offset6] > cb)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset8] < c_b)
		                          if(p[_offset4] < c_b)
		                            if(p[_offset3] < c_b)
		                              goto is_a_corner;
		                            else
		                              if(p[_offset10] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            if(p[_offset10] < c_b)
		                              if(p[_offset11] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                  else
		                    if(p[_offset6] > cb)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] < c_b)
		                        if(p[_offset8] < c_b)
		                          if(p[_offset4] < c_b)
		                            if(p[_offset3] < c_b)
		                              goto is_a_corner;
		                            else
		                              if(p[_offset10] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            if(p[_offset10] < c_b)
		                              if(p[_offset11] < c_b)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		              else
		                goto is_not_a_corner;
		      else
		        goto is_not_a_corner;
		  else
		    if(p[_offset5] > cb)
		      if(p[_offset7] > cb)
		        if(p[_offset2] < c_b)
		          if(p[_offset9] < c_b)
		            goto is_not_a_corner;
		          else
		            if(p[_offset9] > cb)
		              if(p[_offset1] > cb)
		                if(p[_offset6] < c_b)
		                  goto is_not_a_corner;
		                else
		                  if(p[_offset6] > cb)
		                    if(p[_offset8] > cb)
		                      if(p[_offset4] > cb)
		                        if(p[_offset3] > cb)
		                          goto is_a_corner;
		                        else
		                          if(p[_offset10] > cb)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        if(p[_offset10] > cb)
		                          if(p[_offset11] > cb)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                  else
		                    goto is_not_a_corner;
		              else
		                if(p[_offset1] < c_b)
		                  if(p[_offset6] < c_b)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] > cb)
		                      if(p[_offset8] > cb)
		                        if(p[_offset4] > cb)
		                          if(p[_offset3] > cb)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset10] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                else
		                  if(p[_offset6] < c_b)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] > cb)
		                      if(p[_offset8] > cb)
		                        if(p[_offset4] > cb)
		                          if(p[_offset3] > cb)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset10] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		            else
		              goto is_not_a_corner;
		        else
		          if(p[_offset2] > cb)
		            if(p[_offset9] < c_b)
		              if(p[_offset1] > cb)
		                if(p[_offset6] < c_b)
		                  goto is_not_a_corner;
		                else
		                  if(p[_offset6] > cb)
		                    if(p[_offset3] > cb)
		                      if(p[_offset4] > cb)
		                        goto is_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                  else
		                    goto is_not_a_corner;
		              else
		                if(p[_offset1] < c_b)
		                  if(p[_offset6] < c_b)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] > cb)
		                      if(p[_offset3] > cb)
		                        if(p[_offset4] > cb)
		                          if(p[_offset8] > cb)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                else
		                  if(p[_offset6] < c_b)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] > cb)
		                      if(p[_offset3] > cb)
		                        if(p[_offset4] > cb)
		                          if(p[_offset8] > cb)
		                            goto is_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		            else
		              if(p[_offset9] > cb)
		                if(p[_offset1] < c_b)
		                  if(p[_offset6] < c_b)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] > cb)
		                      if(p[_offset8] > cb)
		                        if(p[_offset4] > cb)
		                          if(p[_offset3] > cb)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset10] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                else
		                  if(p[_offset1] > cb)
		                    if(p[_offset6] < c_b)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] > cb)
		                        if(p[_offset4] > cb)
		                          if(p[_offset3] > cb)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset8] > cb)
		                              if(p[_offset10] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          if(p[_offset8] > cb)
		                            if(p[_offset10] > cb)
		                              if(p[_offset11] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] > cb)
		                        if(p[_offset8] > cb)
		                          if(p[_offset4] > cb)
		                            if(p[_offset3] > cb)
		                              goto is_a_corner;
		                            else
		                              if(p[_offset10] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            if(p[_offset10] > cb)
		                              if(p[_offset11] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		              else
		                if(p[_offset1] > cb)
		                  if(p[_offset6] < c_b)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] > cb)
		                      if(p[_offset3] > cb)
		                        if(p[_offset4] > cb)
		                          goto is_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                else
		                  if(p[_offset1] < c_b)
		                    if(p[_offset6] < c_b)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] > cb)
		                        if(p[_offset3] > cb)
		                          if(p[_offset4] > cb)
		                            if(p[_offset8] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] > cb)
		                        if(p[_offset3] > cb)
		                          if(p[_offset4] > cb)
		                            if(p[_offset8] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		          else
		            if(p[_offset9] < c_b)
		              goto is_not_a_corner;
		            else
		              if(p[_offset9] > cb)
		                if(p[_offset1] > cb)
		                  if(p[_offset6] < c_b)
		                    goto is_not_a_corner;
		                  else
		                    if(p[_offset6] > cb)
		                      if(p[_offset8] > cb)
		                        if(p[_offset4] > cb)
		                          if(p[_offset3] > cb)
		                            goto is_a_corner;
		                          else
		                            if(p[_offset10] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          if(p[_offset10] > cb)
		                            if(p[_offset11] > cb)
		                              goto is_a_corner;
		                            else
		                              goto is_not_a_corner;
		                          else
		                            goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                    else
		                      goto is_not_a_corner;
		                else
		                  if(p[_offset1] < c_b)
		                    if(p[_offset6] < c_b)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] > cb)
		                        if(p[_offset8] > cb)
		                          if(p[_offset4] > cb)
		                            if(p[_offset3] > cb)
		                              goto is_a_corner;
		                            else
		                              if(p[_offset10] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            if(p[_offset10] > cb)
		                              if(p[_offset11] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		                  else
		                    if(p[_offset6] < c_b)
		                      goto is_not_a_corner;
		                    else
		                      if(p[_offset6] > cb)
		                        if(p[_offset8] > cb)
		                          if(p[_offset4] > cb)
		                            if(p[_offset3] > cb)
		                              goto is_a_corner;
		                            else
		                              if(p[_offset10] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                          else
		                            if(p[_offset10] > cb)
		                              if(p[_offset11] > cb)
		                                goto is_a_corner;
		                              else
		                                goto is_not_a_corner;
		                            else
		                              goto is_not_a_corner;
		                        else
		                          goto is_not_a_corner;
		                      else
		                        goto is_not_a_corner;
		              else
		                goto is_not_a_corner;
		      else
		        goto is_not_a_corner;
		    else
		      goto is_not_a_corner;
		is_a_corner:
			bmin=b;
			goto end;

		is_not_a_corner:
			bmax=b;
			goto end;

		end:

		if(bmin == bmax - 1 || bmin == bmax)
			return bmin;
		b = (bmin + bmax) / 2;
	}
}


int* AGAST::score7_12s( const uint8_t* i, size_t stride, std::vector<Feature2D> & corners )
{
	int* scores = new int[ corners.size() ];

	init7_12s_pattern( stride );

    for(size_t n = 0; n < corners.size(); n++)
        scores[n] = score_agast7_12s( i + corners[ n ][ 1 ] * stride + corners[ n ][ 0 ]);

	return scores;
}

}
