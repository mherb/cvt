#include <cvt/vision/AGAST.h>

namespace cvt
{
	void AGAST::oast9_16( const uint8_t* im, size_t stride, size_t xsize, size_t ysize, std::vector<Feature2D> & corners )
	{
		register int y;
		register int x;
		register int xsizeB=xsize - 4;
		register int ysizeB=ysize - 3;
		int	nExpectedCorners=512;
		int total=0;
		corners.reserve( nExpectedCorners );

		init9_16_pattern( stride );

		for(y=3; y < ysizeB; y++)
		{
			x=2;
			while(1)
			{
				x++;
				if(x>xsizeB)
					break;
				register const uint8_t* const p = im + y * stride + x;
				register const int cb = *p + _threshold;
				register const int c_b = *p - _threshold;
				if(p[_offset0] > cb) 
					if(p[_offset2] > cb) 
						if(p[_offset4] > cb) 
							if(p[_offset5] > cb) 
								if(p[_offset7] > cb) 
									if(p[_offset3] > cb) 
										if(p[_offset1] > cb) 
											if(p[_offset6] > cb) 
												if(p[_offset8] > cb) 
												{}
												else 
													if(p[_offset15] > cb) 
													{}
													else 
														continue;
											else 
												if(p[_offset13] > cb) 
													if(p[_offset14] > cb) 
														if(p[_offset15] > cb) 
														{}
														else 
															continue;
													else 
														continue;
												else 
													continue;
										else 
											if(p[_offset8] > cb) 
												if(p[_offset9] > cb) 
													if(p[_offset10] > cb) 
														if(p[_offset6] > cb) 
														{}
														else 
															if(p[_offset11] > cb) 
																if(p[_offset12] > cb) 
																	if(p[_offset13] > cb) 
																		if(p[_offset14] > cb) 
																			if(p[_offset15] > cb) 
																			{}
																			else 
																				continue;
																		else 
																			continue;
																	else 
																		continue;
																else 
																	continue;
															else 
																continue;
													else 
														continue;
												else 
													continue;
											else 
												continue;
									else 
										if(p[_offset10] > cb) 
											if(p[_offset11] > cb) 
												if(p[_offset12] > cb) 
													if(p[_offset8] > cb) 
														if(p[_offset9] > cb) 
															if(p[_offset6] > cb) 
															{}
															else 
																if(p[_offset13] > cb) 
																	if(p[_offset14] > cb) 
																		if(p[_offset15] > cb) 
																		{}
																		else 
																			continue;
																	else 
																		continue;
																else 
																	continue;
														else 
															if(p[_offset1] > cb) 
																if(p[_offset13] > cb) 
																	if(p[_offset14] > cb) 
																		if(p[_offset15] > cb) 
																		{}
																		else 
																			continue;
																	else 
																		continue;
																else 
																	continue;
															else 
																continue;
													else 
														if(p[_offset1] > cb) 
															if(p[_offset13] > cb) 
																if(p[_offset14] > cb) 
																	if(p[_offset15] > cb) 
																	{}
																	else 
																		continue;
																else 
																	continue;
															else 
																continue;
														else 
															continue;
												else 
													continue;
											else 
												continue;
										else 
											continue;
								else if(p[_offset7] < c_b) 
									if(p[_offset14] > cb) 
										if(p[_offset15] > cb) 
											if(p[_offset1] > cb) 
												if(p[_offset3] > cb) 
													if(p[_offset6] > cb) 
													{}
													else 
														if(p[_offset13] > cb) 
														{}
														else 
															continue;
												else 
													if(p[_offset10] > cb) 
														if(p[_offset11] > cb) 
															if(p[_offset12] > cb) 
																if(p[_offset13] > cb) 
																{}
																else 
																	continue;
															else 
																continue;
														else 
															continue;
													else 
														continue;
											else 
												if(p[_offset8] > cb) 
													if(p[_offset9] > cb) 
														if(p[_offset10] > cb) 
															if(p[_offset11] > cb) 
																if(p[_offset12] > cb) 
																	if(p[_offset13] > cb) 
																	{}
																	else 
																		continue;
																else 
																	continue;
															else 
																continue;
														else 
															continue;
													else 
														continue;
												else 
													continue;
										else 
											continue;
									else if(p[_offset14] < c_b) 
										if(p[_offset8] < c_b) 
											if(p[_offset9] < c_b) 
												if(p[_offset10] < c_b) 
													if(p[_offset11] < c_b) 
														if(p[_offset12] < c_b) 
															if(p[_offset13] < c_b) 
																if(p[_offset6] < c_b) 
																{}
																else 
																	if(p[_offset15] < c_b) 
																	{}
																	else 
																		continue;
															else 
																continue;
														else 
															continue;
													else 
														continue;
												else 
													continue;
											else 
												continue;
										else 
											continue;
									else 
										continue;
								else 
									if(p[_offset14] > cb) 
										if(p[_offset15] > cb) 
											if(p[_offset1] > cb) 
												if(p[_offset3] > cb) 
													if(p[_offset6] > cb) 
													{}
													else 
														if(p[_offset13] > cb) 
														{}
														else 
															continue;
												else 
													if(p[_offset10] > cb) 
														if(p[_offset11] > cb) 
															if(p[_offset12] > cb) 
																if(p[_offset13] > cb) 
																{}
																else 
																	continue;
															else 
																continue;
														else 
															continue;
													else 
														continue;
											else 
												if(p[_offset8] > cb) 
													if(p[_offset9] > cb) 
														if(p[_offset10] > cb) 
															if(p[_offset11] > cb) 
																if(p[_offset12] > cb) 
																	if(p[_offset13] > cb) 
																	{}
																	else 
																		continue;
																else 
																	continue;
															else 
																continue;
														else 
															continue;
													else 
														continue;
												else 
													continue;
										else 
											continue;
									else 
										continue;
							else if(p[_offset5] < c_b) 
								if(p[_offset12] > cb) 
									if(p[_offset13] > cb) 
										if(p[_offset14] > cb) 
											if(p[_offset15] > cb) 
												if(p[_offset1] > cb) 
													if(p[_offset3] > cb) 
													{}
													else 
														if(p[_offset10] > cb) 
															if(p[_offset11] > cb) 
															{}
															else 
																continue;
														else 
															continue;
												else 
													if(p[_offset8] > cb) 
														if(p[_offset9] > cb) 
															if(p[_offset10] > cb) 
																if(p[_offset11] > cb) 
																{}
																else 
																	continue;
															else 
																continue;
														else 
															continue;
													else 
														continue;
											else 
												if(p[_offset6] > cb) 
													if(p[_offset7] > cb) 
														if(p[_offset8] > cb) 
															if(p[_offset9] > cb) 
																if(p[_offset10] > cb) 
																	if(p[_offset11] > cb) 
																	{}
																	else 
																		continue;
																else 
																	continue;
															else 
																continue;
														else 
															continue;
													else 
														continue;
												else 
													continue;
										else 
											continue;
									else 
										continue;
								else if(p[_offset12] < c_b) 
									if(p[_offset7] < c_b) 
										if(p[_offset8] < c_b) 
											if(p[_offset9] < c_b) 
												if(p[_offset10] < c_b) 
													if(p[_offset11] < c_b) 
														if(p[_offset13] < c_b) 
															if(p[_offset6] < c_b) 
															{}
															else 
																if(p[_offset14] < c_b) 
																	if(p[_offset15] < c_b) 
																	{}
																	else 
																		continue;
																else 
																	continue;
														else 
															continue;
													else 
														continue;
												else 
													continue;
											else 
												continue;
										else 
											continue;
									else 
										continue;
								else 
									continue;
							else 
								if(p[_offset12] > cb) 
									if(p[_offset13] > cb) 
										if(p[_offset14] > cb) 
											if(p[_offset15] > cb) 
												if(p[_offset1] > cb) 
													if(p[_offset3] > cb) 
													{}
													else 
														if(p[_offset10] > cb) 
															if(p[_offset11] > cb) 
															{}
															else 
																continue;
														else 
															continue;
												else 
													if(p[_offset8] > cb) 
														if(p[_offset9] > cb) 
															if(p[_offset10] > cb) 
																if(p[_offset11] > cb) 
																{}
																else 
																	continue;
															else 
																continue;
														else 
															continue;
													else 
														continue;
											else 
												if(p[_offset6] > cb) 
													if(p[_offset7] > cb) 
														if(p[_offset8] > cb) 
															if(p[_offset9] > cb) 
																if(p[_offset10] > cb) 
																	if(p[_offset11] > cb) 
																	{}
																	else 
																		continue;
																else 
																	continue;
															else 
																continue;
														else 
															continue;
													else 
														continue;
												else 
													continue;
										else 
											continue;
									else 
										continue;
								else if(p[_offset12] < c_b) 
									if(p[_offset7] < c_b) 
										if(p[_offset8] < c_b) 
											if(p[_offset9] < c_b) 
												if(p[_offset10] < c_b) 
													if(p[_offset11] < c_b) 
														if(p[_offset13] < c_b) 
															if(p[_offset14] < c_b) 
																if(p[_offset6] < c_b) 
																{}
																else 
																	if(p[_offset15] < c_b) 
																	{}
																	else 
																		continue;
															else 
																continue;
														else 
															continue;
													else 
														continue;
												else 
													continue;
											else 
												continue;
										else 
											continue;
									else 
										continue;
								else 
									continue;
						else if(p[_offset4] < c_b) 
							if(p[_offset11] > cb) 
								if(p[_offset12] > cb) 
									if(p[_offset13] > cb) 
										if(p[_offset10] > cb) 
											if(p[_offset14] > cb) 
												if(p[_offset15] > cb) 
													if(p[_offset1] > cb) 
													{}
													else 
														if(p[_offset8] > cb) 
															if(p[_offset9] > cb) 
															{}
															else 
																continue;
														else 
															continue;
												else 
													if(p[_offset6] > cb) 
														if(p[_offset7] > cb) 
															if(p[_offset8] > cb) 
																if(p[_offset9] > cb) 
																{}
																else 
																	continue;
															else 
																continue;
														else 
															continue;
													else 
														continue;
											else 
												if(p[_offset5] > cb) 
													if(p[_offset6] > cb) 
														if(p[_offset7] > cb) 
															if(p[_offset8] > cb) 
																if(p[_offset9] > cb) 
																{}
																else 
																	continue;
															else 
																continue;
														else 
															continue;
													else 
														continue;
												else 
													continue;
										else 
											if(p[_offset1] > cb) 
												if(p[_offset3] > cb) 
													if(p[_offset14] > cb) 
														if(p[_offset15] > cb) 
														{}
														else 
															continue;
													else 
														continue;
												else 
													continue;
											else 
												continue;
									else 
										continue;
								else 
									continue;
							else if(p[_offset11] < c_b) 
								if(p[_offset7] < c_b) 
									if(p[_offset8] < c_b) 
										if(p[_offset9] < c_b) 
											if(p[_offset10] < c_b) 
												if(p[_offset6] < c_b) 
													if(p[_offset5] < c_b) 
														if(p[_offset3] < c_b) 
														{}
														else 
															if(p[_offset12] < c_b) 
															{}
															else 
																continue;
													else 
														if(p[_offset12] < c_b) 
															if(p[_offset13] < c_b) 
																if(p[_offset14] < c_b) 
																{}
																else 
																	continue;
															else 
																continue;
														else 
															continue;
												else 
													if(p[_offset12] < c_b) 
														if(p[_offset13] < c_b) 
															if(p[_offset14] < c_b) 
																if(p[_offset15] < c_b) 
																{}
																else 
																	continue;
															else 
																continue;
														else 
															continue;
													else 
														continue;
											else 
												continue;
										else 
											continue;
									else 
										continue;
								else 
									continue;
							else 
								continue;
						else 
							if(p[_offset11] > cb) 
								if(p[_offset12] > cb) 
									if(p[_offset13] > cb) 
										if(p[_offset10] > cb) 
											if(p[_offset14] > cb) 
												if(p[_offset15] > cb) 
													if(p[_offset1] > cb) 
													{}
													else 
														if(p[_offset8] > cb) 
															if(p[_offset9] > cb) 
															{}
															else 
																continue;
														else 
															continue;
												else 
													if(p[_offset6] > cb) 
														if(p[_offset7] > cb) 
															if(p[_offset8] > cb) 
																if(p[_offset9] > cb) 
																{}
																else 
																	continue;
															else 
																continue;
														else 
															continue;
													else 
														continue;
											else 
												if(p[_offset5] > cb) 
													if(p[_offset6] > cb) 
														if(p[_offset7] > cb) 
															if(p[_offset8] > cb) 
																if(p[_offset9] > cb) 
																{}
																else 
																	continue;
															else 
																continue;
														else 
															continue;
													else 
														continue;
												else 
													continue;
										else 
											if(p[_offset1] > cb) 
												if(p[_offset3] > cb) 
													if(p[_offset14] > cb) 
														if(p[_offset15] > cb) 
														{}
														else 
															continue;
													else 
														continue;
												else 
													continue;
											else 
												continue;
									else 
										continue;
								else 
									continue;
							else if(p[_offset11] < c_b) 
								if(p[_offset7] < c_b) 
									if(p[_offset8] < c_b) 
										if(p[_offset9] < c_b) 
											if(p[_offset10] < c_b) 
												if(p[_offset12] < c_b) 
													if(p[_offset13] < c_b) 
														if(p[_offset6] < c_b) 
															if(p[_offset5] < c_b) 
															{}
															else 
																if(p[_offset14] < c_b) 
																{}
																else 
																	continue;
														else 
															if(p[_offset14] < c_b) 
																if(p[_offset15] < c_b) 
																{}
																else 
																	continue;
															else 
																continue;
													else 
														continue;
												else 
													continue;
											else 
												continue;
										else 
											continue;
									else 
										continue;
								else 
									continue;
							else 
								continue;
					else if(p[_offset2] < c_b) 
						if(p[_offset9] > cb) 
							if(p[_offset10] > cb) 
								if(p[_offset11] > cb) 
									if(p[_offset8] > cb) 
										if(p[_offset12] > cb) 
											if(p[_offset13] > cb) 
												if(p[_offset14] > cb) 
													if(p[_offset15] > cb) 
													{}
													else 
														if(p[_offset6] > cb) 
															if(p[_offset7] > cb) 
															{}
															else 
																continue;
														else 
															continue;
												else 
													if(p[_offset5] > cb) 
														if(p[_offset6] > cb) 
															if(p[_offset7] > cb) 
															{}
															else 
																continue;
														else 
															continue;
													else 
														continue;
											else 
												if(p[_offset4] > cb) 
													if(p[_offset5] > cb) 
														if(p[_offset6] > cb) 
															if(p[_offset7] > cb) 
															{}
															else 
																continue;
														else 
															continue;
													else 
														continue;
												else 
													continue;
										else 
											if(p[_offset3] > cb) 
												if(p[_offset4] > cb) 
													if(p[_offset5] > cb) 
														if(p[_offset6] > cb) 
															if(p[_offset7] > cb) 
															{}
															else 
																continue;
														else 
															continue;
													else 
														continue;
												else 
													continue;
											else 
												continue;
									else 
										if(p[_offset1] > cb) 
											if(p[_offset12] > cb) 
												if(p[_offset13] > cb) 
													if(p[_offset14] > cb) 
														if(p[_offset15] > cb) 
														{}
														else 
															continue;
													else 
														continue;
												else 
													continue;
											else 
												continue;
										else 
											continue;
								else 
									continue;
							else 
								continue;
						else if(p[_offset9] < c_b) 
							if(p[_offset7] < c_b) 
								if(p[_offset8] < c_b) 
									if(p[_offset6] < c_b) 
										if(p[_offset5] < c_b) 
											if(p[_offset4] < c_b) 
												if(p[_offset3] < c_b) 
													if(p[_offset1] < c_b) 
													{}
													else 
														if(p[_offset10] < c_b) 
														{}
														else 
															continue;
												else 
													if(p[_offset10] < c_b) 
														if(p[_offset11] < c_b) 
															if(p[_offset12] < c_b) 
															{}
															else 
																continue;
														else 
															continue;
													else 
														continue;
											else 
												if(p[_offset10] < c_b) 
													if(p[_offset11] < c_b) 
														if(p[_offset12] < c_b) 
															if(p[_offset13] < c_b) 
															{}
															else 
																continue;
														else 
															continue;
													else 
														continue;
												else 
													continue;
										else 
											if(p[_offset10] < c_b) 
												if(p[_offset11] < c_b) 
													if(p[_offset12] < c_b) 
														if(p[_offset13] < c_b) 
															if(p[_offset14] < c_b) 
															{}
															else 
																continue;
														else 
															continue;
													else 
														continue;
												else 
													continue;
											else 
												continue;
									else 
										if(p[_offset10] < c_b) 
											if(p[_offset11] < c_b) 
												if(p[_offset12] < c_b) 
													if(p[_offset13] < c_b) 
														if(p[_offset14] < c_b) 
															if(p[_offset15] < c_b) 
															{}
															else 
																continue;
														else 
															continue;
													else 
														continue;
												else 
													continue;
											else 
												continue;
										else 
											continue;
								else 
									continue;
							else 
								continue;
						else 
							continue;
					else 
						if(p[_offset9] > cb) 
							if(p[_offset10] > cb) 
								if(p[_offset11] > cb) 
									if(p[_offset8] > cb) 
										if(p[_offset12] > cb) 
											if(p[_offset13] > cb) 
												if(p[_offset14] > cb) 
													if(p[_offset15] > cb) 
													{}
													else 
														if(p[_offset6] > cb) 
															if(p[_offset7] > cb) 
															{}
															else 
																continue;
														else 
															continue;
												else 
													if(p[_offset5] > cb) 
														if(p[_offset6] > cb) 
															if(p[_offset7] > cb) 
															{}
															else 
																continue;
														else 
															continue;
													else 
														continue;
											else 
												if(p[_offset4] > cb) 
													if(p[_offset5] > cb) 
														if(p[_offset6] > cb) 
															if(p[_offset7] > cb) 
															{}
															else 
																continue;
														else 
															continue;
													else 
														continue;
												else 
													continue;
										else 
											if(p[_offset3] > cb) 
												if(p[_offset4] > cb) 
													if(p[_offset5] > cb) 
														if(p[_offset6] > cb) 
															if(p[_offset7] > cb) 
															{}
															else 
																continue;
														else 
															continue;
													else 
														continue;
												else 
													continue;
											else 
												continue;
									else 
										if(p[_offset1] > cb) 
											if(p[_offset12] > cb) 
												if(p[_offset13] > cb) 
													if(p[_offset14] > cb) 
														if(p[_offset15] > cb) 
														{}
														else 
															continue;
													else 
														continue;
												else 
													continue;
											else 
												continue;
										else 
											continue;
								else 
									continue;
							else 
								continue;
						else if(p[_offset9] < c_b) 
							if(p[_offset7] < c_b) 
								if(p[_offset8] < c_b) 
									if(p[_offset10] < c_b) 
										if(p[_offset11] < c_b) 
											if(p[_offset6] < c_b) 
												if(p[_offset5] < c_b) 
													if(p[_offset4] < c_b) 
														if(p[_offset3] < c_b) 
														{}
														else 
															if(p[_offset12] < c_b) 
															{}
															else 
																continue;
													else 
														if(p[_offset12] < c_b) 
															if(p[_offset13] < c_b) 
															{}
															else 
																continue;
														else 
															continue;
												else 
													if(p[_offset12] < c_b) 
														if(p[_offset13] < c_b) 
															if(p[_offset14] < c_b) 
															{}
															else 
																continue;
														else 
															continue;
													else 
														continue;
											else 
												if(p[_offset12] < c_b) 
													if(p[_offset13] < c_b) 
														if(p[_offset14] < c_b) 
															if(p[_offset15] < c_b) 
															{}
															else 
																continue;
														else 
															continue;
													else 
														continue;
												else 
													continue;
										else 
											continue;
									else 
										continue;
								else 
									continue;
							else 
								continue;
						else 
							continue;
				else if(p[_offset0] < c_b) 
					if(p[_offset2] > cb) 
						if(p[_offset9] > cb) 
							if(p[_offset7] > cb) 
								if(p[_offset8] > cb) 
									if(p[_offset6] > cb) 
										if(p[_offset5] > cb) 
											if(p[_offset4] > cb) 
												if(p[_offset3] > cb) 
													if(p[_offset1] > cb) 
													{}
													else 
														if(p[_offset10] > cb) 
														{}
														else 
															continue;
												else 
													if(p[_offset10] > cb) 
														if(p[_offset11] > cb) 
															if(p[_offset12] > cb) 
															{}
															else 
																continue;
														else 
															continue;
													else 
														continue;
											else 
												if(p[_offset10] > cb) 
													if(p[_offset11] > cb) 
														if(p[_offset12] > cb) 
															if(p[_offset13] > cb) 
															{}
															else 
																continue;
														else 
															continue;
													else 
														continue;
												else 
													continue;
										else 
											if(p[_offset10] > cb) 
												if(p[_offset11] > cb) 
													if(p[_offset12] > cb) 
														if(p[_offset13] > cb) 
															if(p[_offset14] > cb) 
															{}
															else 
																continue;
														else 
															continue;
													else 
														continue;
												else 
													continue;
											else 
												continue;
									else 
										if(p[_offset10] > cb) 
											if(p[_offset11] > cb) 
												if(p[_offset12] > cb) 
													if(p[_offset13] > cb) 
														if(p[_offset14] > cb) 
															if(p[_offset15] > cb) 
															{}
															else 
																continue;
														else 
															continue;
													else 
														continue;
												else 
													continue;
											else 
												continue;
										else 
											continue;
								else 
									continue;
							else 
								continue;
						else if(p[_offset9] < c_b) 
							if(p[_offset10] < c_b) 
								if(p[_offset11] < c_b) 
									if(p[_offset8] < c_b) 
										if(p[_offset12] < c_b) 
											if(p[_offset13] < c_b) 
												if(p[_offset14] < c_b) 
													if(p[_offset15] < c_b) 
													{}
													else 
														if(p[_offset6] < c_b) 
															if(p[_offset7] < c_b) 
															{}
															else 
																continue;
														else 
															continue;
												else 
													if(p[_offset5] < c_b) 
														if(p[_offset6] < c_b) 
															if(p[_offset7] < c_b) 
															{}
															else 
																continue;
														else 
															continue;
													else 
														continue;
											else 
												if(p[_offset4] < c_b) 
													if(p[_offset5] < c_b) 
														if(p[_offset6] < c_b) 
															if(p[_offset7] < c_b) 
															{}
															else 
																continue;
														else 
															continue;
													else 
														continue;
												else 
													continue;
										else 
											if(p[_offset3] < c_b) 
												if(p[_offset4] < c_b) 
													if(p[_offset5] < c_b) 
														if(p[_offset6] < c_b) 
															if(p[_offset7] < c_b) 
															{}
															else 
																continue;
														else 
															continue;
													else 
														continue;
												else 
													continue;
											else 
												continue;
									else 
										if(p[_offset1] < c_b) 
											if(p[_offset12] < c_b) 
												if(p[_offset13] < c_b) 
													if(p[_offset14] < c_b) 
														if(p[_offset15] < c_b) 
														{}
														else 
															continue;
													else 
														continue;
												else 
													continue;
											else 
												continue;
										else 
											continue;
								else 
									continue;
							else 
								continue;
						else 
							continue;
					else if(p[_offset2] < c_b) 
						if(p[_offset4] > cb) 
							if(p[_offset11] > cb) 
								if(p[_offset7] > cb) 
									if(p[_offset8] > cb) 
										if(p[_offset9] > cb) 
											if(p[_offset10] > cb) 
												if(p[_offset6] > cb) 
													if(p[_offset5] > cb) 
														if(p[_offset3] > cb) 
														{}
														else 
															if(p[_offset12] > cb) 
															{}
															else 
																continue;
													else 
														if(p[_offset12] > cb) 
															if(p[_offset13] > cb) 
																if(p[_offset14] > cb) 
																{}
																else 
																	continue;
															else 
																continue;
														else 
															continue;
												else 
													if(p[_offset12] > cb) 
														if(p[_offset13] > cb) 
															if(p[_offset14] > cb) 
																if(p[_offset15] > cb) 
																{}
																else 
																	continue;
															else 
																continue;
														else 
															continue;
													else 
														continue;
											else 
												continue;
										else 
											continue;
									else 
										continue;
								else 
									continue;
							else if(p[_offset11] < c_b) 
								if(p[_offset12] < c_b) 
									if(p[_offset13] < c_b) 
										if(p[_offset10] < c_b) 
											if(p[_offset14] < c_b) 
												if(p[_offset15] < c_b) 
													if(p[_offset1] < c_b) 
													{}
													else 
														if(p[_offset8] < c_b) 
															if(p[_offset9] < c_b) 
															{}
															else 
																continue;
														else 
															continue;
												else 
													if(p[_offset6] < c_b) 
														if(p[_offset7] < c_b) 
															if(p[_offset8] < c_b) 
																if(p[_offset9] < c_b) 
																{}
																else 
																	continue;
															else 
																continue;
														else 
															continue;
													else 
														continue;
											else 
												if(p[_offset5] < c_b) 
													if(p[_offset6] < c_b) 
														if(p[_offset7] < c_b) 
															if(p[_offset8] < c_b) 
																if(p[_offset9] < c_b) 
																{}
																else 
																	continue;
															else 
																continue;
														else 
															continue;
													else 
														continue;
												else 
													continue;
										else 
											if(p[_offset1] < c_b) 
												if(p[_offset3] < c_b) 
													if(p[_offset14] < c_b) 
														if(p[_offset15] < c_b) 
														{}
														else 
															continue;
													else 
														continue;
												else 
													continue;
											else 
												continue;
									else 
										continue;
								else 
									continue;
							else 
								continue;
						else if(p[_offset4] < c_b) 
							if(p[_offset5] > cb) 
								if(p[_offset12] > cb) 
									if(p[_offset7] > cb) 
										if(p[_offset8] > cb) 
											if(p[_offset9] > cb) 
												if(p[_offset10] > cb) 
													if(p[_offset11] > cb) 
														if(p[_offset13] > cb) 
															if(p[_offset6] > cb) 
															{}
															else 
																if(p[_offset14] > cb) 
																	if(p[_offset15] > cb) 
																	{}
																	else 
																		continue;
																else 
																	continue;
														else 
															continue;
													else 
														continue;
												else 
													continue;
											else 
												continue;
										else 
											continue;
									else 
										continue;
								else if(p[_offset12] < c_b) 
									if(p[_offset13] < c_b) 
										if(p[_offset14] < c_b) 
											if(p[_offset15] < c_b) 
												if(p[_offset1] < c_b) 
													if(p[_offset3] < c_b) 
													{}
													else 
														if(p[_offset10] < c_b) 
															if(p[_offset11] < c_b) 
															{}
															else 
																continue;
														else 
															continue;
												else 
													if(p[_offset8] < c_b) 
														if(p[_offset9] < c_b) 
															if(p[_offset10] < c_b) 
																if(p[_offset11] < c_b) 
																{}
																else 
																	continue;
															else 
																continue;
														else 
															continue;
													else 
														continue;
											else 
												if(p[_offset6] < c_b) 
													if(p[_offset7] < c_b) 
														if(p[_offset8] < c_b) 
															if(p[_offset9] < c_b) 
																if(p[_offset10] < c_b) 
																	if(p[_offset11] < c_b) 
																	{}
																	else 
																		continue;
																else 
																	continue;
															else 
																continue;
														else 
															continue;
													else 
														continue;
												else 
													continue;
										else 
											continue;
									else 
										continue;
								else 
									continue;
							else if(p[_offset5] < c_b) 
								if(p[_offset7] > cb) 
									if(p[_offset14] > cb) 
										if(p[_offset8] > cb) 
											if(p[_offset9] > cb) 
												if(p[_offset10] > cb) 
													if(p[_offset11] > cb) 
														if(p[_offset12] > cb) 
															if(p[_offset13] > cb) 
																if(p[_offset6] > cb) 
																{}
																else 
																	if(p[_offset15] > cb) 
																	{}
																	else 
																		continue;
															else 
																continue;
														else 
															continue;
													else 
														continue;
												else 
													continue;
											else 
												continue;
										else 
											continue;
									else if(p[_offset14] < c_b) 
										if(p[_offset15] < c_b) 
											if(p[_offset1] < c_b) 
												if(p[_offset3] < c_b) 
													if(p[_offset6] < c_b) 
													{}
													else 
														if(p[_offset13] < c_b) 
														{}
														else 
															continue;
												else 
													if(p[_offset10] < c_b) 
														if(p[_offset11] < c_b) 
															if(p[_offset12] < c_b) 
																if(p[_offset13] < c_b) 
																{}
																else 
																	continue;
															else 
																continue;
														else 
															continue;
													else 
														continue;
											else 
												if(p[_offset8] < c_b) 
													if(p[_offset9] < c_b) 
														if(p[_offset10] < c_b) 
															if(p[_offset11] < c_b) 
																if(p[_offset12] < c_b) 
																	if(p[_offset13] < c_b) 
																	{}
																	else 
																		continue;
																else 
																	continue;
															else 
																continue;
														else 
															continue;
													else 
														continue;
												else 
													continue;
										else 
											continue;
									else 
										continue;
								else if(p[_offset7] < c_b) 
									if(p[_offset3] < c_b) 
										if(p[_offset1] < c_b) 
											if(p[_offset6] < c_b) 
												if(p[_offset8] < c_b) 
												{}
												else 
													if(p[_offset15] < c_b) 
													{}
													else 
														continue;
											else 
												if(p[_offset13] < c_b) 
													if(p[_offset14] < c_b) 
														if(p[_offset15] < c_b) 
														{}
														else 
															continue;
													else 
														continue;
												else 
													continue;
										else 
											if(p[_offset8] < c_b) 
												if(p[_offset9] < c_b) 
													if(p[_offset10] < c_b) 
														if(p[_offset6] < c_b) 
														{}
														else 
															if(p[_offset11] < c_b) 
																if(p[_offset12] < c_b) 
																	if(p[_offset13] < c_b) 
																		if(p[_offset14] < c_b) 
																			if(p[_offset15] < c_b) 
																			{}
																			else 
																				continue;
																		else 
																			continue;
																	else 
																		continue;
																else 
																	continue;
															else 
																continue;
													else 
														continue;
												else 
													continue;
											else 
												continue;
									else 
										if(p[_offset10] < c_b) 
											if(p[_offset11] < c_b) 
												if(p[_offset12] < c_b) 
													if(p[_offset8] < c_b) 
														if(p[_offset9] < c_b) 
															if(p[_offset6] < c_b) 
															{}
															else 
																if(p[_offset13] < c_b) 
																	if(p[_offset14] < c_b) 
																		if(p[_offset15] < c_b) 
																		{}
																		else 
																			continue;
																	else 
																		continue;
																else 
																	continue;
														else 
															if(p[_offset1] < c_b) 
																if(p[_offset13] < c_b) 
																	if(p[_offset14] < c_b) 
																		if(p[_offset15] < c_b) 
																		{}
																		else 
																			continue;
																	else 
																		continue;
																else 
																	continue;
															else 
																continue;
													else 
														if(p[_offset1] < c_b) 
															if(p[_offset13] < c_b) 
																if(p[_offset14] < c_b) 
																	if(p[_offset15] < c_b) 
																	{}
																	else 
																		continue;
																else 
																	continue;
															else 
																continue;
														else 
															continue;
												else 
													continue;
											else 
												continue;
										else 
											continue;
								else 
									if(p[_offset14] < c_b) 
										if(p[_offset15] < c_b) 
											if(p[_offset1] < c_b) 
												if(p[_offset3] < c_b) 
													if(p[_offset6] < c_b) 
													{}
													else 
														if(p[_offset13] < c_b) 
														{}
														else 
															continue;
												else 
													if(p[_offset10] < c_b) 
														if(p[_offset11] < c_b) 
															if(p[_offset12] < c_b) 
																if(p[_offset13] < c_b) 
																{}
																else 
																	continue;
															else 
																continue;
														else 
															continue;
													else 
														continue;
											else 
												if(p[_offset8] < c_b) 
													if(p[_offset9] < c_b) 
														if(p[_offset10] < c_b) 
															if(p[_offset11] < c_b) 
																if(p[_offset12] < c_b) 
																	if(p[_offset13] < c_b) 
																	{}
																	else 
																		continue;
																else 
																	continue;
															else 
																continue;
														else 
															continue;
													else 
														continue;
												else 
													continue;
										else 
											continue;
									else 
										continue;
							else 
								if(p[_offset12] > cb) 
									if(p[_offset7] > cb) 
										if(p[_offset8] > cb) 
											if(p[_offset9] > cb) 
												if(p[_offset10] > cb) 
													if(p[_offset11] > cb) 
														if(p[_offset13] > cb) 
															if(p[_offset14] > cb) 
																if(p[_offset6] > cb) 
																{}
																else 
																	if(p[_offset15] > cb) 
																	{}
																	else 
																		continue;
															else 
																continue;
														else 
															continue;
													else 
														continue;
												else 
													continue;
											else 
												continue;
										else 
											continue;
									else 
										continue;
								else if(p[_offset12] < c_b) 
									if(p[_offset13] < c_b) 
										if(p[_offset14] < c_b) 
											if(p[_offset15] < c_b) 
												if(p[_offset1] < c_b) 
													if(p[_offset3] < c_b) 
													{}
													else 
														if(p[_offset10] < c_b) 
															if(p[_offset11] < c_b) 
															{}
															else 
																continue;
														else 
															continue;
												else 
													if(p[_offset8] < c_b) 
														if(p[_offset9] < c_b) 
															if(p[_offset10] < c_b) 
																if(p[_offset11] < c_b) 
																{}
																else 
																	continue;
															else 
																continue;
														else 
															continue;
													else 
														continue;
											else 
												if(p[_offset6] < c_b) 
													if(p[_offset7] < c_b) 
														if(p[_offset8] < c_b) 
															if(p[_offset9] < c_b) 
																if(p[_offset10] < c_b) 
																	if(p[_offset11] < c_b) 
																	{}
																	else 
																		continue;
																else 
																	continue;
															else 
																continue;
														else 
															continue;
													else 
														continue;
												else 
													continue;
										else 
											continue;
									else 
										continue;
								else 
									continue;
						else 
							if(p[_offset11] > cb) 
								if(p[_offset7] > cb) 
									if(p[_offset8] > cb) 
										if(p[_offset9] > cb) 
											if(p[_offset10] > cb) 
												if(p[_offset12] > cb) 
													if(p[_offset13] > cb) 
														if(p[_offset6] > cb) 
															if(p[_offset5] > cb) 
															{}
															else 
																if(p[_offset14] > cb) 
																{}
																else 
																	continue;
														else 
															if(p[_offset14] > cb) 
																if(p[_offset15] > cb) 
																{}
																else 
																	continue;
															else 
																continue;
													else 
														continue;
												else 
													continue;
											else 
												continue;
										else 
											continue;
									else 
										continue;
								else 
									continue;
							else if(p[_offset11] < c_b) 
								if(p[_offset12] < c_b) 
									if(p[_offset13] < c_b) 
										if(p[_offset10] < c_b) 
											if(p[_offset14] < c_b) 
												if(p[_offset15] < c_b) 
													if(p[_offset1] < c_b) 
													{}
													else 
														if(p[_offset8] < c_b) 
															if(p[_offset9] < c_b) 
															{}
															else 
																continue;
														else 
															continue;
												else 
													if(p[_offset6] < c_b) 
														if(p[_offset7] < c_b) 
															if(p[_offset8] < c_b) 
																if(p[_offset9] < c_b) 
																{}
																else 
																	continue;
															else 
																continue;
														else 
															continue;
													else 
														continue;
											else 
												if(p[_offset5] < c_b) 
													if(p[_offset6] < c_b) 
														if(p[_offset7] < c_b) 
															if(p[_offset8] < c_b) 
																if(p[_offset9] < c_b) 
																{}
																else 
																	continue;
															else 
																continue;
														else 
															continue;
													else 
														continue;
												else 
													continue;
										else 
											if(p[_offset1] < c_b) 
												if(p[_offset3] < c_b) 
													if(p[_offset14] < c_b) 
														if(p[_offset15] < c_b) 
														{}
														else 
															continue;
													else 
														continue;
												else 
													continue;
											else 
												continue;
									else 
										continue;
								else 
									continue;
							else 
								continue;
					else 
						if(p[_offset9] > cb) 
							if(p[_offset7] > cb) 
								if(p[_offset8] > cb) 
									if(p[_offset10] > cb) 
										if(p[_offset11] > cb) 
											if(p[_offset6] > cb) 
												if(p[_offset5] > cb) 
													if(p[_offset4] > cb) 
														if(p[_offset3] > cb) 
														{}
														else 
															if(p[_offset12] > cb) 
															{}
															else 
																continue;
													else 
														if(p[_offset12] > cb) 
															if(p[_offset13] > cb) 
															{}
															else 
																continue;
														else 
															continue;
												else 
													if(p[_offset12] > cb) 
														if(p[_offset13] > cb) 
															if(p[_offset14] > cb) 
															{}
															else 
																continue;
														else 
															continue;
													else 
														continue;
											else 
												if(p[_offset12] > cb) 
													if(p[_offset13] > cb) 
														if(p[_offset14] > cb) 
															if(p[_offset15] > cb) 
															{}
															else 
																continue;
														else 
															continue;
													else 
														continue;
												else 
													continue;
										else 
											continue;
									else 
										continue;
								else 
									continue;
							else 
								continue;
						else if(p[_offset9] < c_b) 
							if(p[_offset10] < c_b) 
								if(p[_offset11] < c_b) 
									if(p[_offset8] < c_b) 
										if(p[_offset12] < c_b) 
											if(p[_offset13] < c_b) 
												if(p[_offset14] < c_b) 
													if(p[_offset15] < c_b) 
													{}
													else 
														if(p[_offset6] < c_b) 
															if(p[_offset7] < c_b) 
															{}
															else 
																continue;
														else 
															continue;
												else 
													if(p[_offset5] < c_b) 
														if(p[_offset6] < c_b) 
															if(p[_offset7] < c_b) 
															{}
															else 
																continue;
														else 
															continue;
													else 
														continue;
											else 
												if(p[_offset4] < c_b) 
													if(p[_offset5] < c_b) 
														if(p[_offset6] < c_b) 
															if(p[_offset7] < c_b) 
															{}
															else 
																continue;
														else 
															continue;
													else 
														continue;
												else 
													continue;
										else 
											if(p[_offset3] < c_b) 
												if(p[_offset4] < c_b) 
													if(p[_offset5] < c_b) 
														if(p[_offset6] < c_b) 
															if(p[_offset7] < c_b) 
															{}
															else 
																continue;
														else 
															continue;
													else 
														continue;
												else 
													continue;
											else 
												continue;
									else 
										if(p[_offset1] < c_b) 
											if(p[_offset12] < c_b) 
												if(p[_offset13] < c_b) 
													if(p[_offset14] < c_b) 
														if(p[_offset15] < c_b) 
														{}
														else 
															continue;
													else 
														continue;
												else 
													continue;
											else 
												continue;
										else 
											continue;
								else 
									continue;
							else 
								continue;
						else 
							continue;
				else 
					if(p[_offset7] > cb) 
						if(p[_offset8] > cb) 
							if(p[_offset9] > cb) 
								if(p[_offset6] > cb) 
									if(p[_offset5] > cb) 
										if(p[_offset4] > cb) 
											if(p[_offset3] > cb) 
												if(p[_offset2] > cb) 
													if(p[_offset1] > cb) 
													{}
													else 
														if(p[_offset10] > cb) 
														{}
														else 
															continue;
												else 
													if(p[_offset10] > cb) 
														if(p[_offset11] > cb) 
														{}
														else 
															continue;
													else 
														continue;
											else 
												if(p[_offset10] > cb) 
													if(p[_offset11] > cb) 
														if(p[_offset12] > cb) 
														{}
														else 
															continue;
													else 
														continue;
												else 
													continue;
										else 
											if(p[_offset10] > cb) 
												if(p[_offset11] > cb) 
													if(p[_offset12] > cb) 
														if(p[_offset13] > cb) 
														{}
														else 
															continue;
													else 
														continue;
												else 
													continue;
											else 
												continue;
									else 
										if(p[_offset10] > cb) 
											if(p[_offset11] > cb) 
												if(p[_offset12] > cb) 
													if(p[_offset13] > cb) 
														if(p[_offset14] > cb) 
														{}
														else 
															continue;
													else 
														continue;
												else 
													continue;
											else 
												continue;
										else 
											continue;
								else 
									if(p[_offset10] > cb) 
										if(p[_offset11] > cb) 
											if(p[_offset12] > cb) 
												if(p[_offset13] > cb) 
													if(p[_offset14] > cb) 
														if(p[_offset15] > cb) 
														{}
														else 
															continue;
													else 
														continue;
												else 
													continue;
											else 
												continue;
										else 
											continue;
									else 
										continue;
							else 
								continue;
						else 
							continue;
					else if(p[_offset7] < c_b) 
						if(p[_offset8] < c_b) 
							if(p[_offset9] < c_b) 
								if(p[_offset6] < c_b) 
									if(p[_offset5] < c_b) 
										if(p[_offset4] < c_b) 
											if(p[_offset3] < c_b) 
												if(p[_offset2] < c_b) 
													if(p[_offset1] < c_b) 
													{}
													else 
														if(p[_offset10] < c_b) 
														{}
														else 
															continue;
												else 
													if(p[_offset10] < c_b) 
														if(p[_offset11] < c_b) 
														{}
														else 
															continue;
													else 
														continue;
											else 
												if(p[_offset10] < c_b) 
													if(p[_offset11] < c_b) 
														if(p[_offset12] < c_b) 
														{}
														else 
															continue;
													else 
														continue;
												else 
													continue;
										else 
											if(p[_offset10] < c_b) 
												if(p[_offset11] < c_b) 
													if(p[_offset12] < c_b) 
														if(p[_offset13] < c_b) 
														{}
														else 
															continue;
													else 
														continue;
												else 
													continue;
											else 
												continue;
									else 
										if(p[_offset10] < c_b) 
											if(p[_offset11] < c_b) 
												if(p[_offset12] < c_b) 
													if(p[_offset13] < c_b) 
														if(p[_offset14] < c_b) 
														{}
														else 
															continue;
													else 
														continue;
												else 
													continue;
											else 
												continue;
										else 
											continue;
								else 
									if(p[_offset10] < c_b) 
										if(p[_offset11] < c_b) 
											if(p[_offset12] < c_b) 
												if(p[_offset13] < c_b) 
													if(p[_offset14] < c_b) 
														if(p[_offset15] < c_b) 
														{}
														else 
															continue;
													else 
														continue;
												else 
													continue;
											else 
												continue;
										else 
											continue;
									else 
										continue;
							else 
								continue;
						else 
							continue;
					else 
						continue;
				if(total == nExpectedCorners)
				{
					nExpectedCorners = nExpectedCorners << 1;
					corners.reserve( nExpectedCorners );
				}

				corners.push_back( Feature2D() );
				corners.back()[ 0 ] = x;
				corners.back()[ 1 ] = y;
				total++;
			}
		}
	}

	int AGAST::score_oast9_16( const uint8_t * p )
	{
		int bmin = _threshold;
		int bmax = 255;
		int b = (bmax + bmin)/2;

		while(1)
		{
			register const int cb = *p + b;
			register const int c_b = *p - b;
			if(p[_offset0] > cb)
				if(p[_offset2] > cb)
					if(p[_offset4] > cb)
						if(p[_offset5] > cb)
							if(p[_offset7] > cb)
								if(p[_offset3] > cb)
									if(p[_offset1] > cb)
										if(p[_offset6] > cb)
											if(p[_offset8] > cb)
												goto is_a_corner;
											else
												if(p[_offset15] > cb)
													goto is_a_corner;
												else
													goto is_not_a_corner;
										else
											if(p[_offset13] > cb)
												if(p[_offset14] > cb)
													if(p[_offset15] > cb)
														goto is_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
									else
										if(p[_offset8] > cb)
											if(p[_offset9] > cb)
												if(p[_offset10] > cb)
													if(p[_offset6] > cb)
														goto is_a_corner;
													else
														if(p[_offset11] > cb)
															if(p[_offset12] > cb)
																if(p[_offset13] > cb)
																	if(p[_offset14] > cb)
																		if(p[_offset15] > cb)
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
										else
											goto is_not_a_corner;
								else
									if(p[_offset10] > cb)
										if(p[_offset11] > cb)
											if(p[_offset12] > cb)
												if(p[_offset8] > cb)
													if(p[_offset9] > cb)
														if(p[_offset6] > cb)
															goto is_a_corner;
														else
															if(p[_offset13] > cb)
																if(p[_offset14] > cb)
																	if(p[_offset15] > cb)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
													else
														if(p[_offset1] > cb)
															if(p[_offset13] > cb)
																if(p[_offset14] > cb)
																	if(p[_offset15] > cb)
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
														if(p[_offset13] > cb)
															if(p[_offset14] > cb)
																if(p[_offset15] > cb)
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
							else if(p[_offset7] < c_b)
								if(p[_offset14] > cb)
									if(p[_offset15] > cb)
										if(p[_offset1] > cb)
											if(p[_offset3] > cb)
												if(p[_offset6] > cb)
													goto is_a_corner;
												else
													if(p[_offset13] > cb)
														goto is_a_corner;
													else
														goto is_not_a_corner;
											else
												if(p[_offset10] > cb)
													if(p[_offset11] > cb)
														if(p[_offset12] > cb)
															if(p[_offset13] > cb)
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
											if(p[_offset8] > cb)
												if(p[_offset9] > cb)
													if(p[_offset10] > cb)
														if(p[_offset11] > cb)
															if(p[_offset12] > cb)
																if(p[_offset13] > cb)
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
								else if(p[_offset14] < c_b)
									if(p[_offset8] < c_b)
										if(p[_offset9] < c_b)
											if(p[_offset10] < c_b)
												if(p[_offset11] < c_b)
													if(p[_offset12] < c_b)
														if(p[_offset13] < c_b)
															if(p[_offset6] < c_b)
																goto is_a_corner;
															else
																if(p[_offset15] < c_b)
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
								else
									goto is_not_a_corner;
							else
								if(p[_offset14] > cb)
									if(p[_offset15] > cb)
										if(p[_offset1] > cb)
											if(p[_offset3] > cb)
												if(p[_offset6] > cb)
													goto is_a_corner;
												else
													if(p[_offset13] > cb)
														goto is_a_corner;
													else
														goto is_not_a_corner;
											else
												if(p[_offset10] > cb)
													if(p[_offset11] > cb)
														if(p[_offset12] > cb)
															if(p[_offset13] > cb)
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
											if(p[_offset8] > cb)
												if(p[_offset9] > cb)
													if(p[_offset10] > cb)
														if(p[_offset11] > cb)
															if(p[_offset12] > cb)
																if(p[_offset13] > cb)
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
								else
									goto is_not_a_corner;
						else if(p[_offset5] < c_b)
							if(p[_offset12] > cb)
								if(p[_offset13] > cb)
									if(p[_offset14] > cb)
										if(p[_offset15] > cb)
											if(p[_offset1] > cb)
												if(p[_offset3] > cb)
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
													if(p[_offset9] > cb)
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
												if(p[_offset7] > cb)
													if(p[_offset8] > cb)
														if(p[_offset9] > cb)
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
								else
									goto is_not_a_corner;
							else if(p[_offset12] < c_b)
								if(p[_offset7] < c_b)
									if(p[_offset8] < c_b)
										if(p[_offset9] < c_b)
											if(p[_offset10] < c_b)
												if(p[_offset11] < c_b)
													if(p[_offset13] < c_b)
														if(p[_offset6] < c_b)
															goto is_a_corner;
														else
															if(p[_offset14] < c_b)
																if(p[_offset15] < c_b)
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
								else
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else
							if(p[_offset12] > cb)
								if(p[_offset13] > cb)
									if(p[_offset14] > cb)
										if(p[_offset15] > cb)
											if(p[_offset1] > cb)
												if(p[_offset3] > cb)
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
													if(p[_offset9] > cb)
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
												if(p[_offset7] > cb)
													if(p[_offset8] > cb)
														if(p[_offset9] > cb)
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
								else
									goto is_not_a_corner;
							else if(p[_offset12] < c_b)
								if(p[_offset7] < c_b)
									if(p[_offset8] < c_b)
										if(p[_offset9] < c_b)
											if(p[_offset10] < c_b)
												if(p[_offset11] < c_b)
													if(p[_offset13] < c_b)
														if(p[_offset14] < c_b)
															if(p[_offset6] < c_b)
																goto is_a_corner;
															else
																if(p[_offset15] < c_b)
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
								else
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
					else if(p[_offset4] < c_b)
						if(p[_offset11] > cb)
							if(p[_offset12] > cb)
								if(p[_offset13] > cb)
									if(p[_offset10] > cb)
										if(p[_offset14] > cb)
											if(p[_offset15] > cb)
												if(p[_offset1] > cb)
													goto is_a_corner;
												else
													if(p[_offset8] > cb)
														if(p[_offset9] > cb)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if(p[_offset6] > cb)
													if(p[_offset7] > cb)
														if(p[_offset8] > cb)
															if(p[_offset9] > cb)
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
											if(p[_offset5] > cb)
												if(p[_offset6] > cb)
													if(p[_offset7] > cb)
														if(p[_offset8] > cb)
															if(p[_offset9] > cb)
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
											if(p[_offset3] > cb)
												if(p[_offset14] > cb)
													if(p[_offset15] > cb)
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
						else if(p[_offset11] < c_b)
							if(p[_offset7] < c_b)
								if(p[_offset8] < c_b)
									if(p[_offset9] < c_b)
										if(p[_offset10] < c_b)
											if(p[_offset6] < c_b)
												if(p[_offset5] < c_b)
													if(p[_offset3] < c_b)
														goto is_a_corner;
													else
														if(p[_offset12] < c_b)
															goto is_a_corner;
														else
															goto is_not_a_corner;
												else
													if(p[_offset12] < c_b)
														if(p[_offset13] < c_b)
															if(p[_offset14] < c_b)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if(p[_offset12] < c_b)
													if(p[_offset13] < c_b)
														if(p[_offset14] < c_b)
															if(p[_offset15] < c_b)
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
							else
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
					else
						if(p[_offset11] > cb)
							if(p[_offset12] > cb)
								if(p[_offset13] > cb)
									if(p[_offset10] > cb)
										if(p[_offset14] > cb)
											if(p[_offset15] > cb)
												if(p[_offset1] > cb)
													goto is_a_corner;
												else
													if(p[_offset8] > cb)
														if(p[_offset9] > cb)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if(p[_offset6] > cb)
													if(p[_offset7] > cb)
														if(p[_offset8] > cb)
															if(p[_offset9] > cb)
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
											if(p[_offset5] > cb)
												if(p[_offset6] > cb)
													if(p[_offset7] > cb)
														if(p[_offset8] > cb)
															if(p[_offset9] > cb)
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
											if(p[_offset3] > cb)
												if(p[_offset14] > cb)
													if(p[_offset15] > cb)
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
						else if(p[_offset11] < c_b)
							if(p[_offset7] < c_b)
								if(p[_offset8] < c_b)
									if(p[_offset9] < c_b)
										if(p[_offset10] < c_b)
											if(p[_offset12] < c_b)
												if(p[_offset13] < c_b)
													if(p[_offset6] < c_b)
														if(p[_offset5] < c_b)
															goto is_a_corner;
														else
															if(p[_offset14] < c_b)
																goto is_a_corner;
															else
																goto is_not_a_corner;
													else
														if(p[_offset14] < c_b)
															if(p[_offset15] < c_b)
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
							else
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
				else if(p[_offset2] < c_b)
					if(p[_offset9] > cb)
						if(p[_offset10] > cb)
							if(p[_offset11] > cb)
								if(p[_offset8] > cb)
									if(p[_offset12] > cb)
										if(p[_offset13] > cb)
											if(p[_offset14] > cb)
												if(p[_offset15] > cb)
													goto is_a_corner;
												else
													if(p[_offset6] > cb)
														if(p[_offset7] > cb)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if(p[_offset5] > cb)
													if(p[_offset6] > cb)
														if(p[_offset7] > cb)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
										else
											if(p[_offset4] > cb)
												if(p[_offset5] > cb)
													if(p[_offset6] > cb)
														if(p[_offset7] > cb)
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
												if(p[_offset5] > cb)
													if(p[_offset6] > cb)
														if(p[_offset7] > cb)
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
										if(p[_offset12] > cb)
											if(p[_offset13] > cb)
												if(p[_offset14] > cb)
													if(p[_offset15] > cb)
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
					else if(p[_offset9] < c_b)
						if(p[_offset7] < c_b)
							if(p[_offset8] < c_b)
								if(p[_offset6] < c_b)
									if(p[_offset5] < c_b)
										if(p[_offset4] < c_b)
											if(p[_offset3] < c_b)
												if(p[_offset1] < c_b)
													goto is_a_corner;
												else
													if(p[_offset10] < c_b)
														goto is_a_corner;
													else
														goto is_not_a_corner;
											else
												if(p[_offset10] < c_b)
													if(p[_offset11] < c_b)
														if(p[_offset12] < c_b)
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
													if(p[_offset12] < c_b)
														if(p[_offset13] < c_b)
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
										if(p[_offset10] < c_b)
											if(p[_offset11] < c_b)
												if(p[_offset12] < c_b)
													if(p[_offset13] < c_b)
														if(p[_offset14] < c_b)
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
									if(p[_offset10] < c_b)
										if(p[_offset11] < c_b)
											if(p[_offset12] < c_b)
												if(p[_offset13] < c_b)
													if(p[_offset14] < c_b)
														if(p[_offset15] < c_b)
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
						else
							goto is_not_a_corner;
					else
						goto is_not_a_corner;
				else
					if(p[_offset9] > cb)
						if(p[_offset10] > cb)
							if(p[_offset11] > cb)
								if(p[_offset8] > cb)
									if(p[_offset12] > cb)
										if(p[_offset13] > cb)
											if(p[_offset14] > cb)
												if(p[_offset15] > cb)
													goto is_a_corner;
												else
													if(p[_offset6] > cb)
														if(p[_offset7] > cb)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if(p[_offset5] > cb)
													if(p[_offset6] > cb)
														if(p[_offset7] > cb)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
										else
											if(p[_offset4] > cb)
												if(p[_offset5] > cb)
													if(p[_offset6] > cb)
														if(p[_offset7] > cb)
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
												if(p[_offset5] > cb)
													if(p[_offset6] > cb)
														if(p[_offset7] > cb)
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
										if(p[_offset12] > cb)
											if(p[_offset13] > cb)
												if(p[_offset14] > cb)
													if(p[_offset15] > cb)
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
					else if(p[_offset9] < c_b)
						if(p[_offset7] < c_b)
							if(p[_offset8] < c_b)
								if(p[_offset10] < c_b)
									if(p[_offset11] < c_b)
										if(p[_offset6] < c_b)
											if(p[_offset5] < c_b)
												if(p[_offset4] < c_b)
													if(p[_offset3] < c_b)
														goto is_a_corner;
													else
														if(p[_offset12] < c_b)
															goto is_a_corner;
														else
															goto is_not_a_corner;
												else
													if(p[_offset12] < c_b)
														if(p[_offset13] < c_b)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if(p[_offset12] < c_b)
													if(p[_offset13] < c_b)
														if(p[_offset14] < c_b)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
										else
											if(p[_offset12] < c_b)
												if(p[_offset13] < c_b)
													if(p[_offset14] < c_b)
														if(p[_offset15] < c_b)
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
						else
							goto is_not_a_corner;
					else
						goto is_not_a_corner;
			else if(p[_offset0] < c_b)
				if(p[_offset2] > cb)
					if(p[_offset9] > cb)
						if(p[_offset7] > cb)
							if(p[_offset8] > cb)
								if(p[_offset6] > cb)
									if(p[_offset5] > cb)
										if(p[_offset4] > cb)
											if(p[_offset3] > cb)
												if(p[_offset1] > cb)
													goto is_a_corner;
												else
													if(p[_offset10] > cb)
														goto is_a_corner;
													else
														goto is_not_a_corner;
											else
												if(p[_offset10] > cb)
													if(p[_offset11] > cb)
														if(p[_offset12] > cb)
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
													if(p[_offset12] > cb)
														if(p[_offset13] > cb)
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
										if(p[_offset10] > cb)
											if(p[_offset11] > cb)
												if(p[_offset12] > cb)
													if(p[_offset13] > cb)
														if(p[_offset14] > cb)
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
									if(p[_offset10] > cb)
										if(p[_offset11] > cb)
											if(p[_offset12] > cb)
												if(p[_offset13] > cb)
													if(p[_offset14] > cb)
														if(p[_offset15] > cb)
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
						else
							goto is_not_a_corner;
					else if(p[_offset9] < c_b)
						if(p[_offset10] < c_b)
							if(p[_offset11] < c_b)
								if(p[_offset8] < c_b)
									if(p[_offset12] < c_b)
										if(p[_offset13] < c_b)
											if(p[_offset14] < c_b)
												if(p[_offset15] < c_b)
													goto is_a_corner;
												else
													if(p[_offset6] < c_b)
														if(p[_offset7] < c_b)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if(p[_offset5] < c_b)
													if(p[_offset6] < c_b)
														if(p[_offset7] < c_b)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
										else
											if(p[_offset4] < c_b)
												if(p[_offset5] < c_b)
													if(p[_offset6] < c_b)
														if(p[_offset7] < c_b)
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
												if(p[_offset5] < c_b)
													if(p[_offset6] < c_b)
														if(p[_offset7] < c_b)
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
										if(p[_offset12] < c_b)
											if(p[_offset13] < c_b)
												if(p[_offset14] < c_b)
													if(p[_offset15] < c_b)
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
					else
						goto is_not_a_corner;
				else if(p[_offset2] < c_b)
					if(p[_offset4] > cb)
						if(p[_offset11] > cb)
							if(p[_offset7] > cb)
								if(p[_offset8] > cb)
									if(p[_offset9] > cb)
										if(p[_offset10] > cb)
											if(p[_offset6] > cb)
												if(p[_offset5] > cb)
													if(p[_offset3] > cb)
														goto is_a_corner;
													else
														if(p[_offset12] > cb)
															goto is_a_corner;
														else
															goto is_not_a_corner;
												else
													if(p[_offset12] > cb)
														if(p[_offset13] > cb)
															if(p[_offset14] > cb)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if(p[_offset12] > cb)
													if(p[_offset13] > cb)
														if(p[_offset14] > cb)
															if(p[_offset15] > cb)
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
							else
								goto is_not_a_corner;
						else if(p[_offset11] < c_b)
							if(p[_offset12] < c_b)
								if(p[_offset13] < c_b)
									if(p[_offset10] < c_b)
										if(p[_offset14] < c_b)
											if(p[_offset15] < c_b)
												if(p[_offset1] < c_b)
													goto is_a_corner;
												else
													if(p[_offset8] < c_b)
														if(p[_offset9] < c_b)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if(p[_offset6] < c_b)
													if(p[_offset7] < c_b)
														if(p[_offset8] < c_b)
															if(p[_offset9] < c_b)
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
											if(p[_offset5] < c_b)
												if(p[_offset6] < c_b)
													if(p[_offset7] < c_b)
														if(p[_offset8] < c_b)
															if(p[_offset9] < c_b)
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
											if(p[_offset3] < c_b)
												if(p[_offset14] < c_b)
													if(p[_offset15] < c_b)
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
					else if(p[_offset4] < c_b)
						if(p[_offset5] > cb)
							if(p[_offset12] > cb)
								if(p[_offset7] > cb)
									if(p[_offset8] > cb)
										if(p[_offset9] > cb)
											if(p[_offset10] > cb)
												if(p[_offset11] > cb)
													if(p[_offset13] > cb)
														if(p[_offset6] > cb)
															goto is_a_corner;
														else
															if(p[_offset14] > cb)
																if(p[_offset15] > cb)
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
								else
									goto is_not_a_corner;
							else if(p[_offset12] < c_b)
								if(p[_offset13] < c_b)
									if(p[_offset14] < c_b)
										if(p[_offset15] < c_b)
											if(p[_offset1] < c_b)
												if(p[_offset3] < c_b)
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
													if(p[_offset9] < c_b)
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
												if(p[_offset7] < c_b)
													if(p[_offset8] < c_b)
														if(p[_offset9] < c_b)
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
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else if(p[_offset5] < c_b)
							if(p[_offset7] > cb)
								if(p[_offset14] > cb)
									if(p[_offset8] > cb)
										if(p[_offset9] > cb)
											if(p[_offset10] > cb)
												if(p[_offset11] > cb)
													if(p[_offset12] > cb)
														if(p[_offset13] > cb)
															if(p[_offset6] > cb)
																goto is_a_corner;
															else
																if(p[_offset15] > cb)
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
								else if(p[_offset14] < c_b)
									if(p[_offset15] < c_b)
										if(p[_offset1] < c_b)
											if(p[_offset3] < c_b)
												if(p[_offset6] < c_b)
													goto is_a_corner;
												else
													if(p[_offset13] < c_b)
														goto is_a_corner;
													else
														goto is_not_a_corner;
											else
												if(p[_offset10] < c_b)
													if(p[_offset11] < c_b)
														if(p[_offset12] < c_b)
															if(p[_offset13] < c_b)
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
											if(p[_offset8] < c_b)
												if(p[_offset9] < c_b)
													if(p[_offset10] < c_b)
														if(p[_offset11] < c_b)
															if(p[_offset12] < c_b)
																if(p[_offset13] < c_b)
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
								else
									goto is_not_a_corner;
							else if(p[_offset7] < c_b)
								if(p[_offset3] < c_b)
									if(p[_offset1] < c_b)
										if(p[_offset6] < c_b)
											if(p[_offset8] < c_b)
												goto is_a_corner;
											else
												if(p[_offset15] < c_b)
													goto is_a_corner;
												else
													goto is_not_a_corner;
										else
											if(p[_offset13] < c_b)
												if(p[_offset14] < c_b)
													if(p[_offset15] < c_b)
														goto is_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
									else
										if(p[_offset8] < c_b)
											if(p[_offset9] < c_b)
												if(p[_offset10] < c_b)
													if(p[_offset6] < c_b)
														goto is_a_corner;
													else
														if(p[_offset11] < c_b)
															if(p[_offset12] < c_b)
																if(p[_offset13] < c_b)
																	if(p[_offset14] < c_b)
																		if(p[_offset15] < c_b)
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
										else
											goto is_not_a_corner;
								else
									if(p[_offset10] < c_b)
										if(p[_offset11] < c_b)
											if(p[_offset12] < c_b)
												if(p[_offset8] < c_b)
													if(p[_offset9] < c_b)
														if(p[_offset6] < c_b)
															goto is_a_corner;
														else
															if(p[_offset13] < c_b)
																if(p[_offset14] < c_b)
																	if(p[_offset15] < c_b)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
													else
														if(p[_offset1] < c_b)
															if(p[_offset13] < c_b)
																if(p[_offset14] < c_b)
																	if(p[_offset15] < c_b)
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
														if(p[_offset13] < c_b)
															if(p[_offset14] < c_b)
																if(p[_offset15] < c_b)
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
							else
								if(p[_offset14] < c_b)
									if(p[_offset15] < c_b)
										if(p[_offset1] < c_b)
											if(p[_offset3] < c_b)
												if(p[_offset6] < c_b)
													goto is_a_corner;
												else
													if(p[_offset13] < c_b)
														goto is_a_corner;
													else
														goto is_not_a_corner;
											else
												if(p[_offset10] < c_b)
													if(p[_offset11] < c_b)
														if(p[_offset12] < c_b)
															if(p[_offset13] < c_b)
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
											if(p[_offset8] < c_b)
												if(p[_offset9] < c_b)
													if(p[_offset10] < c_b)
														if(p[_offset11] < c_b)
															if(p[_offset12] < c_b)
																if(p[_offset13] < c_b)
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
								else
									goto is_not_a_corner;
						else
							if(p[_offset12] > cb)
								if(p[_offset7] > cb)
									if(p[_offset8] > cb)
										if(p[_offset9] > cb)
											if(p[_offset10] > cb)
												if(p[_offset11] > cb)
													if(p[_offset13] > cb)
														if(p[_offset14] > cb)
															if(p[_offset6] > cb)
																goto is_a_corner;
															else
																if(p[_offset15] > cb)
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
								else
									goto is_not_a_corner;
							else if(p[_offset12] < c_b)
								if(p[_offset13] < c_b)
									if(p[_offset14] < c_b)
										if(p[_offset15] < c_b)
											if(p[_offset1] < c_b)
												if(p[_offset3] < c_b)
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
													if(p[_offset9] < c_b)
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
												if(p[_offset7] < c_b)
													if(p[_offset8] < c_b)
														if(p[_offset9] < c_b)
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
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
					else
						if(p[_offset11] > cb)
							if(p[_offset7] > cb)
								if(p[_offset8] > cb)
									if(p[_offset9] > cb)
										if(p[_offset10] > cb)
											if(p[_offset12] > cb)
												if(p[_offset13] > cb)
													if(p[_offset6] > cb)
														if(p[_offset5] > cb)
															goto is_a_corner;
														else
															if(p[_offset14] > cb)
																goto is_a_corner;
															else
																goto is_not_a_corner;
													else
														if(p[_offset14] > cb)
															if(p[_offset15] > cb)
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
							else
								goto is_not_a_corner;
						else if(p[_offset11] < c_b)
							if(p[_offset12] < c_b)
								if(p[_offset13] < c_b)
									if(p[_offset10] < c_b)
										if(p[_offset14] < c_b)
											if(p[_offset15] < c_b)
												if(p[_offset1] < c_b)
													goto is_a_corner;
												else
													if(p[_offset8] < c_b)
														if(p[_offset9] < c_b)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if(p[_offset6] < c_b)
													if(p[_offset7] < c_b)
														if(p[_offset8] < c_b)
															if(p[_offset9] < c_b)
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
											if(p[_offset5] < c_b)
												if(p[_offset6] < c_b)
													if(p[_offset7] < c_b)
														if(p[_offset8] < c_b)
															if(p[_offset9] < c_b)
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
											if(p[_offset3] < c_b)
												if(p[_offset14] < c_b)
													if(p[_offset15] < c_b)
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
				else
					if(p[_offset9] > cb)
						if(p[_offset7] > cb)
							if(p[_offset8] > cb)
								if(p[_offset10] > cb)
									if(p[_offset11] > cb)
										if(p[_offset6] > cb)
											if(p[_offset5] > cb)
												if(p[_offset4] > cb)
													if(p[_offset3] > cb)
														goto is_a_corner;
													else
														if(p[_offset12] > cb)
															goto is_a_corner;
														else
															goto is_not_a_corner;
												else
													if(p[_offset12] > cb)
														if(p[_offset13] > cb)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if(p[_offset12] > cb)
													if(p[_offset13] > cb)
														if(p[_offset14] > cb)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
										else
											if(p[_offset12] > cb)
												if(p[_offset13] > cb)
													if(p[_offset14] > cb)
														if(p[_offset15] > cb)
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
						else
							goto is_not_a_corner;
					else if(p[_offset9] < c_b)
						if(p[_offset10] < c_b)
							if(p[_offset11] < c_b)
								if(p[_offset8] < c_b)
									if(p[_offset12] < c_b)
										if(p[_offset13] < c_b)
											if(p[_offset14] < c_b)
												if(p[_offset15] < c_b)
													goto is_a_corner;
												else
													if(p[_offset6] < c_b)
														if(p[_offset7] < c_b)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if(p[_offset5] < c_b)
													if(p[_offset6] < c_b)
														if(p[_offset7] < c_b)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
										else
											if(p[_offset4] < c_b)
												if(p[_offset5] < c_b)
													if(p[_offset6] < c_b)
														if(p[_offset7] < c_b)
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
												if(p[_offset5] < c_b)
													if(p[_offset6] < c_b)
														if(p[_offset7] < c_b)
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
										if(p[_offset12] < c_b)
											if(p[_offset13] < c_b)
												if(p[_offset14] < c_b)
													if(p[_offset15] < c_b)
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
					else
						goto is_not_a_corner;
			else
				if(p[_offset7] > cb)
					if(p[_offset8] > cb)
						if(p[_offset9] > cb)
							if(p[_offset6] > cb)
								if(p[_offset5] > cb)
									if(p[_offset4] > cb)
										if(p[_offset3] > cb)
											if(p[_offset2] > cb)
												if(p[_offset1] > cb)
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
											if(p[_offset10] > cb)
												if(p[_offset11] > cb)
													if(p[_offset12] > cb)
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
												if(p[_offset12] > cb)
													if(p[_offset13] > cb)
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
									if(p[_offset10] > cb)
										if(p[_offset11] > cb)
											if(p[_offset12] > cb)
												if(p[_offset13] > cb)
													if(p[_offset14] > cb)
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
								if(p[_offset10] > cb)
									if(p[_offset11] > cb)
										if(p[_offset12] > cb)
											if(p[_offset13] > cb)
												if(p[_offset14] > cb)
													if(p[_offset15] > cb)
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
					else
						goto is_not_a_corner;
				else if(p[_offset7] < c_b)
					if(p[_offset8] < c_b)
						if(p[_offset9] < c_b)
							if(p[_offset6] < c_b)
								if(p[_offset5] < c_b)
									if(p[_offset4] < c_b)
										if(p[_offset3] < c_b)
											if(p[_offset2] < c_b)
												if(p[_offset1] < c_b)
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
											if(p[_offset10] < c_b)
												if(p[_offset11] < c_b)
													if(p[_offset12] < c_b)
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
												if(p[_offset12] < c_b)
													if(p[_offset13] < c_b)
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
									if(p[_offset10] < c_b)
										if(p[_offset11] < c_b)
											if(p[_offset12] < c_b)
												if(p[_offset13] < c_b)
													if(p[_offset14] < c_b)
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
								if(p[_offset10] < c_b)
									if(p[_offset11] < c_b)
										if(p[_offset12] < c_b)
											if(p[_offset13] < c_b)
												if(p[_offset14] < c_b)
													if(p[_offset15] < c_b)
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


	int* AGAST::score9_16( const uint8_t * i, size_t stride, std::vector<Feature2D> & corners )
	{
		int* scores = new int[ corners.size() ];

		init9_16_pattern( stride );

		for(size_t n = 0; n < corners.size(); n++)
			scores[n] = score_oast9_16( i + corners[ n ][ 1 ] * stride + corners[ n ][ 0 ]);

		return scores;
	}
}
