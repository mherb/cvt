#include <cvt/vision/FAST.h>

namespace cvt {

	int FAST::score12( const uint8_t* p, const int pixel[] )
	{
		int bmin = _threshold;
		int bmax = 255;
		int b = (bmax + bmin)/2;

		/*Compute the score using binary search*/
		for(;;)
		{
			int cb = *p + b;
			int c_b= *p - b;


			if( p[pixel[0]] > cb)
				if( p[pixel[1]] > cb)
					if( p[pixel[2]] > cb)
						if( p[pixel[3]] > cb)
							if( p[pixel[4]] > cb)
								if( p[pixel[5]] > cb)
									if( p[pixel[6]] > cb)
										if( p[pixel[7]] > cb)
											if( p[pixel[8]] > cb)
												if( p[pixel[9]] > cb)
													if( p[pixel[10]] > cb)
														if( p[pixel[11]] > cb)
															goto is_a_corner;
														else
															if( p[pixel[15]] > cb)
																goto is_a_corner;
															else
																goto is_not_a_corner;
													else
														if( p[pixel[14]] > cb)
															if( p[pixel[15]] > cb)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
												else
													if( p[pixel[13]] > cb)
														if( p[pixel[14]] > cb)
															if( p[pixel[15]] > cb)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if( p[pixel[12]] > cb)
													if( p[pixel[13]] > cb)
														if( p[pixel[14]] > cb)
															if( p[pixel[15]] > cb)
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
											if( p[pixel[11]] > cb)
												if( p[pixel[12]] > cb)
													if( p[pixel[13]] > cb)
														if( p[pixel[14]] > cb)
															if( p[pixel[15]] > cb)
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
										if( p[pixel[10]] > cb)
											if( p[pixel[11]] > cb)
												if( p[pixel[12]] > cb)
													if( p[pixel[13]] > cb)
														if( p[pixel[14]] > cb)
															if( p[pixel[15]] > cb)
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
									if( p[pixel[9]] > cb)
										if( p[pixel[10]] > cb)
											if( p[pixel[11]] > cb)
												if( p[pixel[12]] > cb)
													if( p[pixel[13]] > cb)
														if( p[pixel[14]] > cb)
															if( p[pixel[15]] > cb)
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
							else if( p[pixel[4]] < c_b)
								if( p[pixel[8]] > cb)
									if( p[pixel[9]] > cb)
										if( p[pixel[10]] > cb)
											if( p[pixel[11]] > cb)
												if( p[pixel[12]] > cb)
													if( p[pixel[13]] > cb)
														if( p[pixel[14]] > cb)
															if( p[pixel[15]] > cb)
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
								else if( p[pixel[8]] < c_b)
									if( p[pixel[5]] < c_b)
										if( p[pixel[6]] < c_b)
											if( p[pixel[7]] < c_b)
												if( p[pixel[9]] < c_b)
													if( p[pixel[10]] < c_b)
														if( p[pixel[11]] < c_b)
															if( p[pixel[12]] < c_b)
																if( p[pixel[13]] < c_b)
																	if( p[pixel[14]] < c_b)
																		if( p[pixel[15]] < c_b)
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
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								if( p[pixel[8]] > cb)
									if( p[pixel[9]] > cb)
										if( p[pixel[10]] > cb)
											if( p[pixel[11]] > cb)
												if( p[pixel[12]] > cb)
													if( p[pixel[13]] > cb)
														if( p[pixel[14]] > cb)
															if( p[pixel[15]] > cb)
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
						else if( p[pixel[3]] < c_b)
							if( p[pixel[15]] > cb)
								if( p[pixel[7]] > cb)
									if( p[pixel[8]] > cb)
										if( p[pixel[9]] > cb)
											if( p[pixel[10]] > cb)
												if( p[pixel[11]] > cb)
													if( p[pixel[12]] > cb)
														if( p[pixel[13]] > cb)
															if( p[pixel[14]] > cb)
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
								else if( p[pixel[7]] < c_b)
									if( p[pixel[4]] < c_b)
										if( p[pixel[5]] < c_b)
											if( p[pixel[6]] < c_b)
												if( p[pixel[8]] < c_b)
													if( p[pixel[9]] < c_b)
														if( p[pixel[10]] < c_b)
															if( p[pixel[11]] < c_b)
																if( p[pixel[12]] < c_b)
																	if( p[pixel[13]] < c_b)
																		if( p[pixel[14]] < c_b)
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
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								if( p[pixel[4]] < c_b)
									if( p[pixel[5]] < c_b)
										if( p[pixel[6]] < c_b)
											if( p[pixel[7]] < c_b)
												if( p[pixel[8]] < c_b)
													if( p[pixel[9]] < c_b)
														if( p[pixel[10]] < c_b)
															if( p[pixel[11]] < c_b)
																if( p[pixel[12]] < c_b)
																	if( p[pixel[13]] < c_b)
																		if( p[pixel[14]] < c_b)
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
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
						else
							if( p[pixel[7]] > cb)
								if( p[pixel[8]] > cb)
									if( p[pixel[9]] > cb)
										if( p[pixel[10]] > cb)
											if( p[pixel[11]] > cb)
												if( p[pixel[12]] > cb)
													if( p[pixel[13]] > cb)
														if( p[pixel[14]] > cb)
															if( p[pixel[15]] > cb)
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
							else if( p[pixel[7]] < c_b)
								if( p[pixel[4]] < c_b)
									if( p[pixel[5]] < c_b)
										if( p[pixel[6]] < c_b)
											if( p[pixel[8]] < c_b)
												if( p[pixel[9]] < c_b)
													if( p[pixel[10]] < c_b)
														if( p[pixel[11]] < c_b)
															if( p[pixel[12]] < c_b)
																if( p[pixel[13]] < c_b)
																	if( p[pixel[14]] < c_b)
																		if( p[pixel[15]] < c_b)
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
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
					else if( p[pixel[2]] < c_b)
						if( p[pixel[6]] > cb)
							if( p[pixel[7]] > cb)
								if( p[pixel[8]] > cb)
									if( p[pixel[9]] > cb)
										if( p[pixel[10]] > cb)
											if( p[pixel[11]] > cb)
												if( p[pixel[12]] > cb)
													if( p[pixel[13]] > cb)
														if( p[pixel[14]] > cb)
															if( p[pixel[15]] > cb)
																goto is_a_corner;
															else
																if( p[pixel[3]] > cb)
																	if( p[pixel[4]] > cb)
																		if( p[pixel[5]] > cb)
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
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else if( p[pixel[6]] < c_b)
							if( p[pixel[4]] < c_b)
								if( p[pixel[5]] < c_b)
									if( p[pixel[7]] < c_b)
										if( p[pixel[8]] < c_b)
											if( p[pixel[9]] < c_b)
												if( p[pixel[10]] < c_b)
													if( p[pixel[11]] < c_b)
														if( p[pixel[12]] < c_b)
															if( p[pixel[13]] < c_b)
																if( p[pixel[3]] < c_b)
																	goto is_a_corner;
																else
																	if( p[pixel[14]] < c_b)
																		if( p[pixel[15]] < c_b)
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
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
					else
						if( p[pixel[6]] > cb)
							if( p[pixel[7]] > cb)
								if( p[pixel[8]] > cb)
									if( p[pixel[9]] > cb)
										if( p[pixel[10]] > cb)
											if( p[pixel[11]] > cb)
												if( p[pixel[12]] > cb)
													if( p[pixel[13]] > cb)
														if( p[pixel[14]] > cb)
															if( p[pixel[15]] > cb)
																goto is_a_corner;
															else
																if( p[pixel[3]] > cb)
																	if( p[pixel[4]] > cb)
																		if( p[pixel[5]] > cb)
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
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else if( p[pixel[6]] < c_b)
							if( p[pixel[4]] < c_b)
								if( p[pixel[5]] < c_b)
									if( p[pixel[7]] < c_b)
										if( p[pixel[8]] < c_b)
											if( p[pixel[9]] < c_b)
												if( p[pixel[10]] < c_b)
													if( p[pixel[11]] < c_b)
														if( p[pixel[12]] < c_b)
															if( p[pixel[13]] < c_b)
																if( p[pixel[14]] < c_b)
																	if( p[pixel[3]] < c_b)
																		goto is_a_corner;
																	else
																		if( p[pixel[15]] < c_b)
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
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
				else if( p[pixel[1]] < c_b)
					if( p[pixel[5]] > cb)
						if( p[pixel[6]] > cb)
							if( p[pixel[7]] > cb)
								if( p[pixel[8]] > cb)
									if( p[pixel[9]] > cb)
										if( p[pixel[10]] > cb)
											if( p[pixel[11]] > cb)
												if( p[pixel[12]] > cb)
													if( p[pixel[13]] > cb)
														if( p[pixel[14]] > cb)
															if( p[pixel[15]] > cb)
																goto is_a_corner;
															else
																if( p[pixel[3]] > cb)
																	if( p[pixel[4]] > cb)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
														else
															if( p[pixel[2]] > cb)
																if( p[pixel[3]] > cb)
																	if( p[pixel[4]] > cb)
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
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
					else if( p[pixel[5]] < c_b)
						if( p[pixel[4]] < c_b)
							if( p[pixel[6]] < c_b)
								if( p[pixel[7]] < c_b)
									if( p[pixel[8]] < c_b)
										if( p[pixel[9]] < c_b)
											if( p[pixel[10]] < c_b)
												if( p[pixel[11]] < c_b)
													if( p[pixel[12]] < c_b)
														if( p[pixel[3]] < c_b)
															if( p[pixel[2]] < c_b)
																goto is_a_corner;
															else
																if( p[pixel[13]] < c_b)
																	if( p[pixel[14]] < c_b)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
														else
															if( p[pixel[13]] < c_b)
																if( p[pixel[14]] < c_b)
																	if( p[pixel[15]] < c_b)
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
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
					else
						goto is_not_a_corner;
				else
					if( p[pixel[5]] > cb)
						if( p[pixel[6]] > cb)
							if( p[pixel[7]] > cb)
								if( p[pixel[8]] > cb)
									if( p[pixel[9]] > cb)
										if( p[pixel[10]] > cb)
											if( p[pixel[11]] > cb)
												if( p[pixel[12]] > cb)
													if( p[pixel[13]] > cb)
														if( p[pixel[14]] > cb)
															if( p[pixel[15]] > cb)
																goto is_a_corner;
															else
																if( p[pixel[3]] > cb)
																	if( p[pixel[4]] > cb)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
														else
															if( p[pixel[2]] > cb)
																if( p[pixel[3]] > cb)
																	if( p[pixel[4]] > cb)
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
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
					else if( p[pixel[5]] < c_b)
						if( p[pixel[4]] < c_b)
							if( p[pixel[6]] < c_b)
								if( p[pixel[7]] < c_b)
									if( p[pixel[8]] < c_b)
										if( p[pixel[9]] < c_b)
											if( p[pixel[10]] < c_b)
												if( p[pixel[11]] < c_b)
													if( p[pixel[12]] < c_b)
														if( p[pixel[13]] < c_b)
															if( p[pixel[3]] < c_b)
																if( p[pixel[2]] < c_b)
																	goto is_a_corner;
																else
																	if( p[pixel[14]] < c_b)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
															else
																if( p[pixel[14]] < c_b)
																	if( p[pixel[15]] < c_b)
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
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
					else
						goto is_not_a_corner;
			else if( p[pixel[0]] < c_b)
				if( p[pixel[1]] > cb)
					if( p[pixel[5]] > cb)
						if( p[pixel[4]] > cb)
							if( p[pixel[6]] > cb)
								if( p[pixel[7]] > cb)
									if( p[pixel[8]] > cb)
										if( p[pixel[9]] > cb)
											if( p[pixel[10]] > cb)
												if( p[pixel[11]] > cb)
													if( p[pixel[12]] > cb)
														if( p[pixel[3]] > cb)
															if( p[pixel[2]] > cb)
																goto is_a_corner;
															else
																if( p[pixel[13]] > cb)
																	if( p[pixel[14]] > cb)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
														else
															if( p[pixel[13]] > cb)
																if( p[pixel[14]] > cb)
																	if( p[pixel[15]] > cb)
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
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
					else if( p[pixel[5]] < c_b)
						if( p[pixel[6]] < c_b)
							if( p[pixel[7]] < c_b)
								if( p[pixel[8]] < c_b)
									if( p[pixel[9]] < c_b)
										if( p[pixel[10]] < c_b)
											if( p[pixel[11]] < c_b)
												if( p[pixel[12]] < c_b)
													if( p[pixel[13]] < c_b)
														if( p[pixel[14]] < c_b)
															if( p[pixel[15]] < c_b)
																goto is_a_corner;
															else
																if( p[pixel[3]] < c_b)
																	if( p[pixel[4]] < c_b)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
														else
															if( p[pixel[2]] < c_b)
																if( p[pixel[3]] < c_b)
																	if( p[pixel[4]] < c_b)
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
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
					else
						goto is_not_a_corner;
				else if( p[pixel[1]] < c_b)
					if( p[pixel[2]] > cb)
						if( p[pixel[6]] > cb)
							if( p[pixel[4]] > cb)
								if( p[pixel[5]] > cb)
									if( p[pixel[7]] > cb)
										if( p[pixel[8]] > cb)
											if( p[pixel[9]] > cb)
												if( p[pixel[10]] > cb)
													if( p[pixel[11]] > cb)
														if( p[pixel[12]] > cb)
															if( p[pixel[13]] > cb)
																if( p[pixel[3]] > cb)
																	goto is_a_corner;
																else
																	if( p[pixel[14]] > cb)
																		if( p[pixel[15]] > cb)
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
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else if( p[pixel[6]] < c_b)
							if( p[pixel[7]] < c_b)
								if( p[pixel[8]] < c_b)
									if( p[pixel[9]] < c_b)
										if( p[pixel[10]] < c_b)
											if( p[pixel[11]] < c_b)
												if( p[pixel[12]] < c_b)
													if( p[pixel[13]] < c_b)
														if( p[pixel[14]] < c_b)
															if( p[pixel[15]] < c_b)
																goto is_a_corner;
															else
																if( p[pixel[3]] < c_b)
																	if( p[pixel[4]] < c_b)
																		if( p[pixel[5]] < c_b)
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
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
					else if( p[pixel[2]] < c_b)
						if( p[pixel[3]] > cb)
							if( p[pixel[15]] < c_b)
								if( p[pixel[7]] > cb)
									if( p[pixel[4]] > cb)
										if( p[pixel[5]] > cb)
											if( p[pixel[6]] > cb)
												if( p[pixel[8]] > cb)
													if( p[pixel[9]] > cb)
														if( p[pixel[10]] > cb)
															if( p[pixel[11]] > cb)
																if( p[pixel[12]] > cb)
																	if( p[pixel[13]] > cb)
																		if( p[pixel[14]] > cb)
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
										goto is_not_a_corner;
								else if( p[pixel[7]] < c_b)
									if( p[pixel[8]] < c_b)
										if( p[pixel[9]] < c_b)
											if( p[pixel[10]] < c_b)
												if( p[pixel[11]] < c_b)
													if( p[pixel[12]] < c_b)
														if( p[pixel[13]] < c_b)
															if( p[pixel[14]] < c_b)
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
								if( p[pixel[4]] > cb)
									if( p[pixel[5]] > cb)
										if( p[pixel[6]] > cb)
											if( p[pixel[7]] > cb)
												if( p[pixel[8]] > cb)
													if( p[pixel[9]] > cb)
														if( p[pixel[10]] > cb)
															if( p[pixel[11]] > cb)
																if( p[pixel[12]] > cb)
																	if( p[pixel[13]] > cb)
																		if( p[pixel[14]] > cb)
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
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
						else if( p[pixel[3]] < c_b)
							if( p[pixel[4]] > cb)
								if( p[pixel[8]] > cb)
									if( p[pixel[5]] > cb)
										if( p[pixel[6]] > cb)
											if( p[pixel[7]] > cb)
												if( p[pixel[9]] > cb)
													if( p[pixel[10]] > cb)
														if( p[pixel[11]] > cb)
															if( p[pixel[12]] > cb)
																if( p[pixel[13]] > cb)
																	if( p[pixel[14]] > cb)
																		if( p[pixel[15]] > cb)
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
										goto is_not_a_corner;
								else if( p[pixel[8]] < c_b)
									if( p[pixel[9]] < c_b)
										if( p[pixel[10]] < c_b)
											if( p[pixel[11]] < c_b)
												if( p[pixel[12]] < c_b)
													if( p[pixel[13]] < c_b)
														if( p[pixel[14]] < c_b)
															if( p[pixel[15]] < c_b)
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
							else if( p[pixel[4]] < c_b)
								if( p[pixel[5]] < c_b)
									if( p[pixel[6]] < c_b)
										if( p[pixel[7]] < c_b)
											if( p[pixel[8]] < c_b)
												if( p[pixel[9]] < c_b)
													if( p[pixel[10]] < c_b)
														if( p[pixel[11]] < c_b)
															goto is_a_corner;
														else
															if( p[pixel[15]] < c_b)
																goto is_a_corner;
															else
																goto is_not_a_corner;
													else
														if( p[pixel[14]] < c_b)
															if( p[pixel[15]] < c_b)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
												else
													if( p[pixel[13]] < c_b)
														if( p[pixel[14]] < c_b)
															if( p[pixel[15]] < c_b)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if( p[pixel[12]] < c_b)
													if( p[pixel[13]] < c_b)
														if( p[pixel[14]] < c_b)
															if( p[pixel[15]] < c_b)
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
											if( p[pixel[11]] < c_b)
												if( p[pixel[12]] < c_b)
													if( p[pixel[13]] < c_b)
														if( p[pixel[14]] < c_b)
															if( p[pixel[15]] < c_b)
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
										if( p[pixel[10]] < c_b)
											if( p[pixel[11]] < c_b)
												if( p[pixel[12]] < c_b)
													if( p[pixel[13]] < c_b)
														if( p[pixel[14]] < c_b)
															if( p[pixel[15]] < c_b)
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
									if( p[pixel[9]] < c_b)
										if( p[pixel[10]] < c_b)
											if( p[pixel[11]] < c_b)
												if( p[pixel[12]] < c_b)
													if( p[pixel[13]] < c_b)
														if( p[pixel[14]] < c_b)
															if( p[pixel[15]] < c_b)
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
								if( p[pixel[8]] < c_b)
									if( p[pixel[9]] < c_b)
										if( p[pixel[10]] < c_b)
											if( p[pixel[11]] < c_b)
												if( p[pixel[12]] < c_b)
													if( p[pixel[13]] < c_b)
														if( p[pixel[14]] < c_b)
															if( p[pixel[15]] < c_b)
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
							if( p[pixel[7]] > cb)
								if( p[pixel[4]] > cb)
									if( p[pixel[5]] > cb)
										if( p[pixel[6]] > cb)
											if( p[pixel[8]] > cb)
												if( p[pixel[9]] > cb)
													if( p[pixel[10]] > cb)
														if( p[pixel[11]] > cb)
															if( p[pixel[12]] > cb)
																if( p[pixel[13]] > cb)
																	if( p[pixel[14]] > cb)
																		if( p[pixel[15]] > cb)
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
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else if( p[pixel[7]] < c_b)
								if( p[pixel[8]] < c_b)
									if( p[pixel[9]] < c_b)
										if( p[pixel[10]] < c_b)
											if( p[pixel[11]] < c_b)
												if( p[pixel[12]] < c_b)
													if( p[pixel[13]] < c_b)
														if( p[pixel[14]] < c_b)
															if( p[pixel[15]] < c_b)
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
						if( p[pixel[6]] > cb)
							if( p[pixel[4]] > cb)
								if( p[pixel[5]] > cb)
									if( p[pixel[7]] > cb)
										if( p[pixel[8]] > cb)
											if( p[pixel[9]] > cb)
												if( p[pixel[10]] > cb)
													if( p[pixel[11]] > cb)
														if( p[pixel[12]] > cb)
															if( p[pixel[13]] > cb)
																if( p[pixel[14]] > cb)
																	if( p[pixel[3]] > cb)
																		goto is_a_corner;
																	else
																		if( p[pixel[15]] > cb)
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
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else if( p[pixel[6]] < c_b)
							if( p[pixel[7]] < c_b)
								if( p[pixel[8]] < c_b)
									if( p[pixel[9]] < c_b)
										if( p[pixel[10]] < c_b)
											if( p[pixel[11]] < c_b)
												if( p[pixel[12]] < c_b)
													if( p[pixel[13]] < c_b)
														if( p[pixel[14]] < c_b)
															if( p[pixel[15]] < c_b)
																goto is_a_corner;
															else
																if( p[pixel[3]] < c_b)
																	if( p[pixel[4]] < c_b)
																		if( p[pixel[5]] < c_b)
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
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
				else
					if( p[pixel[5]] > cb)
						if( p[pixel[4]] > cb)
							if( p[pixel[6]] > cb)
								if( p[pixel[7]] > cb)
									if( p[pixel[8]] > cb)
										if( p[pixel[9]] > cb)
											if( p[pixel[10]] > cb)
												if( p[pixel[11]] > cb)
													if( p[pixel[12]] > cb)
														if( p[pixel[13]] > cb)
															if( p[pixel[3]] > cb)
																if( p[pixel[2]] > cb)
																	goto is_a_corner;
																else
																	if( p[pixel[14]] > cb)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
															else
																if( p[pixel[14]] > cb)
																	if( p[pixel[15]] > cb)
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
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
					else if( p[pixel[5]] < c_b)
						if( p[pixel[6]] < c_b)
							if( p[pixel[7]] < c_b)
								if( p[pixel[8]] < c_b)
									if( p[pixel[9]] < c_b)
										if( p[pixel[10]] < c_b)
											if( p[pixel[11]] < c_b)
												if( p[pixel[12]] < c_b)
													if( p[pixel[13]] < c_b)
														if( p[pixel[14]] < c_b)
															if( p[pixel[15]] < c_b)
																goto is_a_corner;
															else
																if( p[pixel[3]] < c_b)
																	if( p[pixel[4]] < c_b)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
														else
															if( p[pixel[2]] < c_b)
																if( p[pixel[3]] < c_b)
																	if( p[pixel[4]] < c_b)
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
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
					else
						goto is_not_a_corner;
			else
				if( p[pixel[4]] > cb)
					if( p[pixel[5]] > cb)
						if( p[pixel[6]] > cb)
							if( p[pixel[7]] > cb)
								if( p[pixel[8]] > cb)
									if( p[pixel[9]] > cb)
										if( p[pixel[10]] > cb)
											if( p[pixel[11]] > cb)
												if( p[pixel[12]] > cb)
													if( p[pixel[3]] > cb)
														if( p[pixel[2]] > cb)
															if( p[pixel[1]] > cb)
																goto is_a_corner;
															else
																if( p[pixel[13]] > cb)
																	goto is_a_corner;
																else
																	goto is_not_a_corner;
														else
															if( p[pixel[13]] > cb)
																if( p[pixel[14]] > cb)
																	goto is_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
													else
														if( p[pixel[13]] > cb)
															if( p[pixel[14]] > cb)
																if( p[pixel[15]] > cb)
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
							goto is_not_a_corner;
					else
						goto is_not_a_corner;
				else if( p[pixel[4]] < c_b)
					if( p[pixel[5]] < c_b)
						if( p[pixel[6]] < c_b)
							if( p[pixel[7]] < c_b)
								if( p[pixel[8]] < c_b)
									if( p[pixel[9]] < c_b)
										if( p[pixel[10]] < c_b)
											if( p[pixel[11]] < c_b)
												if( p[pixel[12]] < c_b)
													if( p[pixel[3]] < c_b)
														if( p[pixel[2]] < c_b)
															if( p[pixel[1]] < c_b)
																goto is_a_corner;
															else
																if( p[pixel[13]] < c_b)
																	goto is_a_corner;
																else
																	goto is_not_a_corner;
														else
															if( p[pixel[13]] < c_b)
																if( p[pixel[14]] < c_b)
																	goto is_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
													else
														if( p[pixel[13]] < c_b)
															if( p[pixel[14]] < c_b)
																if( p[pixel[15]] < c_b)
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
							goto is_not_a_corner;
					else
						goto is_not_a_corner;
				else
					goto is_not_a_corner;

is_a_corner:
			bmin=b;
			goto end_if;

is_not_a_corner:
			bmax=b;
			goto end_if;

end_if:

			if(bmin == bmax - 1 || bmin == bmax)
				return bmin;
			b = (bmin + bmax) / 2;
		}
	}

	void FAST::detect12( const uint8_t* im, size_t stride, size_t xsize, size_t ysize, std::vector<Feature2D> & corners )
	{
		int num_corners=0;
		int rsize=512;
		int pixel[16];
		size_t x, y;

		corners.reserve( rsize );
		make_offsets(pixel, stride);

		for(y=3; y < ysize - 3; y++)
			for(x=3; x < xsize - 3; x++)
			{
				const uint8_t* p = im + y*stride + x;

				int cb = *p + _threshold;
				int c_b= *p - _threshold;
				if(p[pixel[0]] > cb)
					if(p[pixel[1]] > cb)
						if(p[pixel[2]] > cb)
							if(p[pixel[3]] > cb)
								if(p[pixel[4]] > cb)
									if(p[pixel[5]] > cb)
										if(p[pixel[6]] > cb)
											if(p[pixel[7]] > cb)
												if(p[pixel[8]] > cb)
													if(p[pixel[9]] > cb)
														if(p[pixel[10]] > cb)
															if(p[pixel[11]] > cb)
															{}
															else
																if(p[pixel[15]] > cb)
																{}
																else
																	continue;
														else
															if(p[pixel[14]] > cb)
																if(p[pixel[15]] > cb)
																{}
																else
																	continue;
															else
																continue;
													else
														if(p[pixel[13]] > cb)
															if(p[pixel[14]] > cb)
																if(p[pixel[15]] > cb)
																{}
																else
																	continue;
															else
																continue;
														else
															continue;
												else
													if(p[pixel[12]] > cb)
														if(p[pixel[13]] > cb)
															if(p[pixel[14]] > cb)
																if(p[pixel[15]] > cb)
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
												if(p[pixel[11]] > cb)
													if(p[pixel[12]] > cb)
														if(p[pixel[13]] > cb)
															if(p[pixel[14]] > cb)
																if(p[pixel[15]] > cb)
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
											if(p[pixel[10]] > cb)
												if(p[pixel[11]] > cb)
													if(p[pixel[12]] > cb)
														if(p[pixel[13]] > cb)
															if(p[pixel[14]] > cb)
																if(p[pixel[15]] > cb)
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
										if(p[pixel[9]] > cb)
											if(p[pixel[10]] > cb)
												if(p[pixel[11]] > cb)
													if(p[pixel[12]] > cb)
														if(p[pixel[13]] > cb)
															if(p[pixel[14]] > cb)
																if(p[pixel[15]] > cb)
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
								else if(p[pixel[4]] < c_b)
									if(p[pixel[8]] > cb)
										if(p[pixel[9]] > cb)
											if(p[pixel[10]] > cb)
												if(p[pixel[11]] > cb)
													if(p[pixel[12]] > cb)
														if(p[pixel[13]] > cb)
															if(p[pixel[14]] > cb)
																if(p[pixel[15]] > cb)
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
									else if(p[pixel[8]] < c_b)
										if(p[pixel[5]] < c_b)
											if(p[pixel[6]] < c_b)
												if(p[pixel[7]] < c_b)
													if(p[pixel[9]] < c_b)
														if(p[pixel[10]] < c_b)
															if(p[pixel[11]] < c_b)
																if(p[pixel[12]] < c_b)
																	if(p[pixel[13]] < c_b)
																		if(p[pixel[14]] < c_b)
																			if(p[pixel[15]] < c_b)
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
											continue;
									else
										continue;
								else
									if(p[pixel[8]] > cb)
										if(p[pixel[9]] > cb)
											if(p[pixel[10]] > cb)
												if(p[pixel[11]] > cb)
													if(p[pixel[12]] > cb)
														if(p[pixel[13]] > cb)
															if(p[pixel[14]] > cb)
																if(p[pixel[15]] > cb)
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
							else if(p[pixel[3]] < c_b)
								if(p[pixel[15]] > cb)
									if(p[pixel[7]] > cb)
										if(p[pixel[8]] > cb)
											if(p[pixel[9]] > cb)
												if(p[pixel[10]] > cb)
													if(p[pixel[11]] > cb)
														if(p[pixel[12]] > cb)
															if(p[pixel[13]] > cb)
																if(p[pixel[14]] > cb)
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
									else if(p[pixel[7]] < c_b)
										if(p[pixel[4]] < c_b)
											if(p[pixel[5]] < c_b)
												if(p[pixel[6]] < c_b)
													if(p[pixel[8]] < c_b)
														if(p[pixel[9]] < c_b)
															if(p[pixel[10]] < c_b)
																if(p[pixel[11]] < c_b)
																	if(p[pixel[12]] < c_b)
																		if(p[pixel[13]] < c_b)
																			if(p[pixel[14]] < c_b)
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
											continue;
									else
										continue;
								else
									if(p[pixel[4]] < c_b)
										if(p[pixel[5]] < c_b)
											if(p[pixel[6]] < c_b)
												if(p[pixel[7]] < c_b)
													if(p[pixel[8]] < c_b)
														if(p[pixel[9]] < c_b)
															if(p[pixel[10]] < c_b)
																if(p[pixel[11]] < c_b)
																	if(p[pixel[12]] < c_b)
																		if(p[pixel[13]] < c_b)
																			if(p[pixel[14]] < c_b)
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
											continue;
									else
										continue;
							else
								if(p[pixel[7]] > cb)
									if(p[pixel[8]] > cb)
										if(p[pixel[9]] > cb)
											if(p[pixel[10]] > cb)
												if(p[pixel[11]] > cb)
													if(p[pixel[12]] > cb)
														if(p[pixel[13]] > cb)
															if(p[pixel[14]] > cb)
																if(p[pixel[15]] > cb)
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
								else if(p[pixel[7]] < c_b)
									if(p[pixel[4]] < c_b)
										if(p[pixel[5]] < c_b)
											if(p[pixel[6]] < c_b)
												if(p[pixel[8]] < c_b)
													if(p[pixel[9]] < c_b)
														if(p[pixel[10]] < c_b)
															if(p[pixel[11]] < c_b)
																if(p[pixel[12]] < c_b)
																	if(p[pixel[13]] < c_b)
																		if(p[pixel[14]] < c_b)
																			if(p[pixel[15]] < c_b)
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
											continue;
									else
										continue;
								else
									continue;
						else if(p[pixel[2]] < c_b)
							if(p[pixel[6]] > cb)
								if(p[pixel[7]] > cb)
									if(p[pixel[8]] > cb)
										if(p[pixel[9]] > cb)
											if(p[pixel[10]] > cb)
												if(p[pixel[11]] > cb)
													if(p[pixel[12]] > cb)
														if(p[pixel[13]] > cb)
															if(p[pixel[14]] > cb)
																if(p[pixel[15]] > cb)
																{}
																else
																	if(p[pixel[3]] > cb)
																		if(p[pixel[4]] > cb)
																			if(p[pixel[5]] > cb)
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
										continue;
								else
									continue;
							else if(p[pixel[6]] < c_b)
								if(p[pixel[4]] < c_b)
									if(p[pixel[5]] < c_b)
										if(p[pixel[7]] < c_b)
											if(p[pixel[8]] < c_b)
												if(p[pixel[9]] < c_b)
													if(p[pixel[10]] < c_b)
														if(p[pixel[11]] < c_b)
															if(p[pixel[12]] < c_b)
																if(p[pixel[13]] < c_b)
																	if(p[pixel[3]] < c_b)
																	{}
																	else
																		if(p[pixel[14]] < c_b)
																			if(p[pixel[15]] < c_b)
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
										continue;
								else
									continue;
							else
								continue;
						else
							if(p[pixel[6]] > cb)
								if(p[pixel[7]] > cb)
									if(p[pixel[8]] > cb)
										if(p[pixel[9]] > cb)
											if(p[pixel[10]] > cb)
												if(p[pixel[11]] > cb)
													if(p[pixel[12]] > cb)
														if(p[pixel[13]] > cb)
															if(p[pixel[14]] > cb)
																if(p[pixel[15]] > cb)
																{}
																else
																	if(p[pixel[3]] > cb)
																		if(p[pixel[4]] > cb)
																			if(p[pixel[5]] > cb)
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
										continue;
								else
									continue;
							else if(p[pixel[6]] < c_b)
								if(p[pixel[4]] < c_b)
									if(p[pixel[5]] < c_b)
										if(p[pixel[7]] < c_b)
											if(p[pixel[8]] < c_b)
												if(p[pixel[9]] < c_b)
													if(p[pixel[10]] < c_b)
														if(p[pixel[11]] < c_b)
															if(p[pixel[12]] < c_b)
																if(p[pixel[13]] < c_b)
																	if(p[pixel[14]] < c_b)
																		if(p[pixel[3]] < c_b)
																		{}
																		else
																			if(p[pixel[15]] < c_b)
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
										continue;
								else
									continue;
							else
								continue;
					else if(p[pixel[1]] < c_b)
						if(p[pixel[5]] > cb)
							if(p[pixel[6]] > cb)
								if(p[pixel[7]] > cb)
									if(p[pixel[8]] > cb)
										if(p[pixel[9]] > cb)
											if(p[pixel[10]] > cb)
												if(p[pixel[11]] > cb)
													if(p[pixel[12]] > cb)
														if(p[pixel[13]] > cb)
															if(p[pixel[14]] > cb)
																if(p[pixel[15]] > cb)
																{}
																else
																	if(p[pixel[3]] > cb)
																		if(p[pixel[4]] > cb)
																		{}
																		else
																			continue;
																	else
																		continue;
															else
																if(p[pixel[2]] > cb)
																	if(p[pixel[3]] > cb)
																		if(p[pixel[4]] > cb)
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
									continue;
							else
								continue;
						else if(p[pixel[5]] < c_b)
							if(p[pixel[4]] < c_b)
								if(p[pixel[6]] < c_b)
									if(p[pixel[7]] < c_b)
										if(p[pixel[8]] < c_b)
											if(p[pixel[9]] < c_b)
												if(p[pixel[10]] < c_b)
													if(p[pixel[11]] < c_b)
														if(p[pixel[12]] < c_b)
															if(p[pixel[3]] < c_b)
																if(p[pixel[2]] < c_b)
																{}
																else
																	if(p[pixel[13]] < c_b)
																		if(p[pixel[14]] < c_b)
																		{}
																		else
																			continue;
																	else
																		continue;
															else
																if(p[pixel[13]] < c_b)
																	if(p[pixel[14]] < c_b)
																		if(p[pixel[15]] < c_b)
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
									continue;
							else
								continue;
						else
							continue;
					else
						if(p[pixel[5]] > cb)
							if(p[pixel[6]] > cb)
								if(p[pixel[7]] > cb)
									if(p[pixel[8]] > cb)
										if(p[pixel[9]] > cb)
											if(p[pixel[10]] > cb)
												if(p[pixel[11]] > cb)
													if(p[pixel[12]] > cb)
														if(p[pixel[13]] > cb)
															if(p[pixel[14]] > cb)
																if(p[pixel[15]] > cb)
																{}
																else
																	if(p[pixel[3]] > cb)
																		if(p[pixel[4]] > cb)
																		{}
																		else
																			continue;
																	else
																		continue;
															else
																if(p[pixel[2]] > cb)
																	if(p[pixel[3]] > cb)
																		if(p[pixel[4]] > cb)
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
									continue;
							else
								continue;
						else if(p[pixel[5]] < c_b)
							if(p[pixel[4]] < c_b)
								if(p[pixel[6]] < c_b)
									if(p[pixel[7]] < c_b)
										if(p[pixel[8]] < c_b)
											if(p[pixel[9]] < c_b)
												if(p[pixel[10]] < c_b)
													if(p[pixel[11]] < c_b)
														if(p[pixel[12]] < c_b)
															if(p[pixel[13]] < c_b)
																if(p[pixel[3]] < c_b)
																	if(p[pixel[2]] < c_b)
																	{}
																	else
																		if(p[pixel[14]] < c_b)
																		{}
																		else
																			continue;
																else
																	if(p[pixel[14]] < c_b)
																		if(p[pixel[15]] < c_b)
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
									continue;
							else
								continue;
						else
							continue;
				else if(p[pixel[0]] < c_b)
					if(p[pixel[1]] > cb)
						if(p[pixel[5]] > cb)
							if(p[pixel[4]] > cb)
								if(p[pixel[6]] > cb)
									if(p[pixel[7]] > cb)
										if(p[pixel[8]] > cb)
											if(p[pixel[9]] > cb)
												if(p[pixel[10]] > cb)
													if(p[pixel[11]] > cb)
														if(p[pixel[12]] > cb)
															if(p[pixel[3]] > cb)
																if(p[pixel[2]] > cb)
																{}
																else
																	if(p[pixel[13]] > cb)
																		if(p[pixel[14]] > cb)
																		{}
																		else
																			continue;
																	else
																		continue;
															else
																if(p[pixel[13]] > cb)
																	if(p[pixel[14]] > cb)
																		if(p[pixel[15]] > cb)
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
									continue;
							else
								continue;
						else if(p[pixel[5]] < c_b)
							if(p[pixel[6]] < c_b)
								if(p[pixel[7]] < c_b)
									if(p[pixel[8]] < c_b)
										if(p[pixel[9]] < c_b)
											if(p[pixel[10]] < c_b)
												if(p[pixel[11]] < c_b)
													if(p[pixel[12]] < c_b)
														if(p[pixel[13]] < c_b)
															if(p[pixel[14]] < c_b)
																if(p[pixel[15]] < c_b)
																{}
																else
																	if(p[pixel[3]] < c_b)
																		if(p[pixel[4]] < c_b)
																		{}
																		else
																			continue;
																	else
																		continue;
															else
																if(p[pixel[2]] < c_b)
																	if(p[pixel[3]] < c_b)
																		if(p[pixel[4]] < c_b)
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
									continue;
							else
								continue;
						else
							continue;
					else if(p[pixel[1]] < c_b)
						if(p[pixel[2]] > cb)
							if(p[pixel[6]] > cb)
								if(p[pixel[4]] > cb)
									if(p[pixel[5]] > cb)
										if(p[pixel[7]] > cb)
											if(p[pixel[8]] > cb)
												if(p[pixel[9]] > cb)
													if(p[pixel[10]] > cb)
														if(p[pixel[11]] > cb)
															if(p[pixel[12]] > cb)
																if(p[pixel[13]] > cb)
																	if(p[pixel[3]] > cb)
																	{}
																	else
																		if(p[pixel[14]] > cb)
																			if(p[pixel[15]] > cb)
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
										continue;
								else
									continue;
							else if(p[pixel[6]] < c_b)
								if(p[pixel[7]] < c_b)
									if(p[pixel[8]] < c_b)
										if(p[pixel[9]] < c_b)
											if(p[pixel[10]] < c_b)
												if(p[pixel[11]] < c_b)
													if(p[pixel[12]] < c_b)
														if(p[pixel[13]] < c_b)
															if(p[pixel[14]] < c_b)
																if(p[pixel[15]] < c_b)
																{}
																else
																	if(p[pixel[3]] < c_b)
																		if(p[pixel[4]] < c_b)
																			if(p[pixel[5]] < c_b)
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
										continue;
								else
									continue;
							else
								continue;
						else if(p[pixel[2]] < c_b)
							if(p[pixel[3]] > cb)
								if(p[pixel[15]] < c_b)
									if(p[pixel[7]] > cb)
										if(p[pixel[4]] > cb)
											if(p[pixel[5]] > cb)
												if(p[pixel[6]] > cb)
													if(p[pixel[8]] > cb)
														if(p[pixel[9]] > cb)
															if(p[pixel[10]] > cb)
																if(p[pixel[11]] > cb)
																	if(p[pixel[12]] > cb)
																		if(p[pixel[13]] > cb)
																			if(p[pixel[14]] > cb)
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
											continue;
									else if(p[pixel[7]] < c_b)
										if(p[pixel[8]] < c_b)
											if(p[pixel[9]] < c_b)
												if(p[pixel[10]] < c_b)
													if(p[pixel[11]] < c_b)
														if(p[pixel[12]] < c_b)
															if(p[pixel[13]] < c_b)
																if(p[pixel[14]] < c_b)
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
									if(p[pixel[4]] > cb)
										if(p[pixel[5]] > cb)
											if(p[pixel[6]] > cb)
												if(p[pixel[7]] > cb)
													if(p[pixel[8]] > cb)
														if(p[pixel[9]] > cb)
															if(p[pixel[10]] > cb)
																if(p[pixel[11]] > cb)
																	if(p[pixel[12]] > cb)
																		if(p[pixel[13]] > cb)
																			if(p[pixel[14]] > cb)
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
											continue;
									else
										continue;
							else if(p[pixel[3]] < c_b)
								if(p[pixel[4]] > cb)
									if(p[pixel[8]] > cb)
										if(p[pixel[5]] > cb)
											if(p[pixel[6]] > cb)
												if(p[pixel[7]] > cb)
													if(p[pixel[9]] > cb)
														if(p[pixel[10]] > cb)
															if(p[pixel[11]] > cb)
																if(p[pixel[12]] > cb)
																	if(p[pixel[13]] > cb)
																		if(p[pixel[14]] > cb)
																			if(p[pixel[15]] > cb)
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
											continue;
									else if(p[pixel[8]] < c_b)
										if(p[pixel[9]] < c_b)
											if(p[pixel[10]] < c_b)
												if(p[pixel[11]] < c_b)
													if(p[pixel[12]] < c_b)
														if(p[pixel[13]] < c_b)
															if(p[pixel[14]] < c_b)
																if(p[pixel[15]] < c_b)
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
								else if(p[pixel[4]] < c_b)
									if(p[pixel[5]] < c_b)
										if(p[pixel[6]] < c_b)
											if(p[pixel[7]] < c_b)
												if(p[pixel[8]] < c_b)
													if(p[pixel[9]] < c_b)
														if(p[pixel[10]] < c_b)
															if(p[pixel[11]] < c_b)
															{}
															else
																if(p[pixel[15]] < c_b)
																{}
																else
																	continue;
														else
															if(p[pixel[14]] < c_b)
																if(p[pixel[15]] < c_b)
																{}
																else
																	continue;
															else
																continue;
													else
														if(p[pixel[13]] < c_b)
															if(p[pixel[14]] < c_b)
																if(p[pixel[15]] < c_b)
																{}
																else
																	continue;
															else
																continue;
														else
															continue;
												else
													if(p[pixel[12]] < c_b)
														if(p[pixel[13]] < c_b)
															if(p[pixel[14]] < c_b)
																if(p[pixel[15]] < c_b)
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
												if(p[pixel[11]] < c_b)
													if(p[pixel[12]] < c_b)
														if(p[pixel[13]] < c_b)
															if(p[pixel[14]] < c_b)
																if(p[pixel[15]] < c_b)
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
											if(p[pixel[10]] < c_b)
												if(p[pixel[11]] < c_b)
													if(p[pixel[12]] < c_b)
														if(p[pixel[13]] < c_b)
															if(p[pixel[14]] < c_b)
																if(p[pixel[15]] < c_b)
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
										if(p[pixel[9]] < c_b)
											if(p[pixel[10]] < c_b)
												if(p[pixel[11]] < c_b)
													if(p[pixel[12]] < c_b)
														if(p[pixel[13]] < c_b)
															if(p[pixel[14]] < c_b)
																if(p[pixel[15]] < c_b)
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
									if(p[pixel[8]] < c_b)
										if(p[pixel[9]] < c_b)
											if(p[pixel[10]] < c_b)
												if(p[pixel[11]] < c_b)
													if(p[pixel[12]] < c_b)
														if(p[pixel[13]] < c_b)
															if(p[pixel[14]] < c_b)
																if(p[pixel[15]] < c_b)
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
								if(p[pixel[7]] > cb)
									if(p[pixel[4]] > cb)
										if(p[pixel[5]] > cb)
											if(p[pixel[6]] > cb)
												if(p[pixel[8]] > cb)
													if(p[pixel[9]] > cb)
														if(p[pixel[10]] > cb)
															if(p[pixel[11]] > cb)
																if(p[pixel[12]] > cb)
																	if(p[pixel[13]] > cb)
																		if(p[pixel[14]] > cb)
																			if(p[pixel[15]] > cb)
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
											continue;
									else
										continue;
								else if(p[pixel[7]] < c_b)
									if(p[pixel[8]] < c_b)
										if(p[pixel[9]] < c_b)
											if(p[pixel[10]] < c_b)
												if(p[pixel[11]] < c_b)
													if(p[pixel[12]] < c_b)
														if(p[pixel[13]] < c_b)
															if(p[pixel[14]] < c_b)
																if(p[pixel[15]] < c_b)
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
							if(p[pixel[6]] > cb)
								if(p[pixel[4]] > cb)
									if(p[pixel[5]] > cb)
										if(p[pixel[7]] > cb)
											if(p[pixel[8]] > cb)
												if(p[pixel[9]] > cb)
													if(p[pixel[10]] > cb)
														if(p[pixel[11]] > cb)
															if(p[pixel[12]] > cb)
																if(p[pixel[13]] > cb)
																	if(p[pixel[14]] > cb)
																		if(p[pixel[3]] > cb)
																		{}
																		else
																			if(p[pixel[15]] > cb)
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
										continue;
								else
									continue;
							else if(p[pixel[6]] < c_b)
								if(p[pixel[7]] < c_b)
									if(p[pixel[8]] < c_b)
										if(p[pixel[9]] < c_b)
											if(p[pixel[10]] < c_b)
												if(p[pixel[11]] < c_b)
													if(p[pixel[12]] < c_b)
														if(p[pixel[13]] < c_b)
															if(p[pixel[14]] < c_b)
																if(p[pixel[15]] < c_b)
																{}
																else
																	if(p[pixel[3]] < c_b)
																		if(p[pixel[4]] < c_b)
																			if(p[pixel[5]] < c_b)
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
										continue;
								else
									continue;
							else
								continue;
					else
						if(p[pixel[5]] > cb)
							if(p[pixel[4]] > cb)
								if(p[pixel[6]] > cb)
									if(p[pixel[7]] > cb)
										if(p[pixel[8]] > cb)
											if(p[pixel[9]] > cb)
												if(p[pixel[10]] > cb)
													if(p[pixel[11]] > cb)
														if(p[pixel[12]] > cb)
															if(p[pixel[13]] > cb)
																if(p[pixel[3]] > cb)
																	if(p[pixel[2]] > cb)
																	{}
																	else
																		if(p[pixel[14]] > cb)
																		{}
																		else
																			continue;
																else
																	if(p[pixel[14]] > cb)
																		if(p[pixel[15]] > cb)
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
									continue;
							else
								continue;
						else if(p[pixel[5]] < c_b)
							if(p[pixel[6]] < c_b)
								if(p[pixel[7]] < c_b)
									if(p[pixel[8]] < c_b)
										if(p[pixel[9]] < c_b)
											if(p[pixel[10]] < c_b)
												if(p[pixel[11]] < c_b)
													if(p[pixel[12]] < c_b)
														if(p[pixel[13]] < c_b)
															if(p[pixel[14]] < c_b)
																if(p[pixel[15]] < c_b)
																{}
																else
																	if(p[pixel[3]] < c_b)
																		if(p[pixel[4]] < c_b)
																		{}
																		else
																			continue;
																	else
																		continue;
															else
																if(p[pixel[2]] < c_b)
																	if(p[pixel[3]] < c_b)
																		if(p[pixel[4]] < c_b)
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
									continue;
							else
								continue;
						else
							continue;
				else
					if(p[pixel[4]] > cb)
						if(p[pixel[5]] > cb)
							if(p[pixel[6]] > cb)
								if(p[pixel[7]] > cb)
									if(p[pixel[8]] > cb)
										if(p[pixel[9]] > cb)
											if(p[pixel[10]] > cb)
												if(p[pixel[11]] > cb)
													if(p[pixel[12]] > cb)
														if(p[pixel[3]] > cb)
															if(p[pixel[2]] > cb)
																if(p[pixel[1]] > cb)
																{}
																else
																	if(p[pixel[13]] > cb)
																	{}
																	else
																		continue;
															else
																if(p[pixel[13]] > cb)
																	if(p[pixel[14]] > cb)
																	{}
																	else
																		continue;
																else
																	continue;
														else
															if(p[pixel[13]] > cb)
																if(p[pixel[14]] > cb)
																	if(p[pixel[15]] > cb)
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
								continue;
						else
							continue;
					else if(p[pixel[4]] < c_b)
						if(p[pixel[5]] < c_b)
							if(p[pixel[6]] < c_b)
								if(p[pixel[7]] < c_b)
									if(p[pixel[8]] < c_b)
										if(p[pixel[9]] < c_b)
											if(p[pixel[10]] < c_b)
												if(p[pixel[11]] < c_b)
													if(p[pixel[12]] < c_b)
														if(p[pixel[3]] < c_b)
															if(p[pixel[2]] < c_b)
																if(p[pixel[1]] < c_b)
																{}
																else
																	if(p[pixel[13]] < c_b)
																	{}
																	else
																		continue;
															else
																if(p[pixel[13]] < c_b)
																	if(p[pixel[14]] < c_b)
																	{}
																	else
																		continue;
																else
																	continue;
														else
															if(p[pixel[13]] < c_b)
																if(p[pixel[14]] < c_b)
																	if(p[pixel[15]] < c_b)
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
								continue;
						else
							continue;
					else
						continue;
				if( num_corners == rsize ) {
					rsize = rsize << 1;
					corners.reserve( rsize );
				}

				corners.push_back( Feature2D() );
				corners.back()[ 0 ] = x;
				corners.back()[ 1 ] = y;
				num_corners++;

			}
	}

}
