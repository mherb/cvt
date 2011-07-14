#include <cvt/vision/FAST.h>

namespace cvt {

	int FAST::score12( const uint8_t* p, const int * offsets, uint8_t threshold )
	{
		int bmin = threshold;
		int bmax = 255;
		int b = (bmax + bmin)/2;

		/*Compute the score using binary search*/
		for(;;)
		{
			int cb = *p + b;
			int c_b= *p - b;


			if( p[offsets[0]] > cb)
				if( p[offsets[1]] > cb)
					if( p[offsets[2]] > cb)
						if( p[offsets[3]] > cb)
							if( p[offsets[4]] > cb)
								if( p[offsets[5]] > cb)
									if( p[offsets[6]] > cb)
										if( p[offsets[7]] > cb)
											if( p[offsets[8]] > cb)
												if( p[offsets[9]] > cb)
													if( p[offsets[10]] > cb)
														if( p[offsets[11]] > cb)
															goto is_a_corner;
														else
															if( p[offsets[15]] > cb)
																goto is_a_corner;
															else
																goto is_not_a_corner;
													else
														if( p[offsets[14]] > cb)
															if( p[offsets[15]] > cb)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
												else
													if( p[offsets[13]] > cb)
														if( p[offsets[14]] > cb)
															if( p[offsets[15]] > cb)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if( p[offsets[12]] > cb)
													if( p[offsets[13]] > cb)
														if( p[offsets[14]] > cb)
															if( p[offsets[15]] > cb)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
										else
											if( p[offsets[11]] > cb)
												if( p[offsets[12]] > cb)
													if( p[offsets[13]] > cb)
														if( p[offsets[14]] > cb)
															if( p[offsets[15]] > cb)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
									else
										if( p[offsets[10]] > cb)
											if( p[offsets[11]] > cb)
												if( p[offsets[12]] > cb)
													if( p[offsets[13]] > cb)
														if( p[offsets[14]] > cb)
															if( p[offsets[15]] > cb)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
								else
									if( p[offsets[9]] > cb)
										if( p[offsets[10]] > cb)
											if( p[offsets[11]] > cb)
												if( p[offsets[12]] > cb)
													if( p[offsets[13]] > cb)
														if( p[offsets[14]] > cb)
															if( p[offsets[15]] > cb)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
							else if( p[offsets[4]] < c_b)
								if( p[offsets[8]] > cb)
									if( p[offsets[9]] > cb)
										if( p[offsets[10]] > cb)
											if( p[offsets[11]] > cb)
												if( p[offsets[12]] > cb)
													if( p[offsets[13]] > cb)
														if( p[offsets[14]] > cb)
															if( p[offsets[15]] > cb)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else if( p[offsets[8]] < c_b)
									if( p[offsets[5]] < c_b)
										if( p[offsets[6]] < c_b)
											if( p[offsets[7]] < c_b)
												if( p[offsets[9]] < c_b)
													if( p[offsets[10]] < c_b)
														if( p[offsets[11]] < c_b)
															if( p[offsets[12]] < c_b)
																if( p[offsets[13]] < c_b)
																	if( p[offsets[14]] < c_b)
																		if( p[offsets[15]] < c_b)
																			goto is_a_corner;
																		else
																			goto is_not_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								if( p[offsets[8]] > cb)
									if( p[offsets[9]] > cb)
										if( p[offsets[10]] > cb)
											if( p[offsets[11]] > cb)
												if( p[offsets[12]] > cb)
													if( p[offsets[13]] > cb)
														if( p[offsets[14]] > cb)
															if( p[offsets[15]] > cb)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
						else if( p[offsets[3]] < c_b)
							if( p[offsets[15]] > cb)
								if( p[offsets[7]] > cb)
									if( p[offsets[8]] > cb)
										if( p[offsets[9]] > cb)
											if( p[offsets[10]] > cb)
												if( p[offsets[11]] > cb)
													if( p[offsets[12]] > cb)
														if( p[offsets[13]] > cb)
															if( p[offsets[14]] > cb)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else if( p[offsets[7]] < c_b)
									if( p[offsets[4]] < c_b)
										if( p[offsets[5]] < c_b)
											if( p[offsets[6]] < c_b)
												if( p[offsets[8]] < c_b)
													if( p[offsets[9]] < c_b)
														if( p[offsets[10]] < c_b)
															if( p[offsets[11]] < c_b)
																if( p[offsets[12]] < c_b)
																	if( p[offsets[13]] < c_b)
																		if( p[offsets[14]] < c_b)
																			goto is_a_corner;
																		else
																			goto is_not_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								if( p[offsets[4]] < c_b)
									if( p[offsets[5]] < c_b)
										if( p[offsets[6]] < c_b)
											if( p[offsets[7]] < c_b)
												if( p[offsets[8]] < c_b)
													if( p[offsets[9]] < c_b)
														if( p[offsets[10]] < c_b)
															if( p[offsets[11]] < c_b)
																if( p[offsets[12]] < c_b)
																	if( p[offsets[13]] < c_b)
																		if( p[offsets[14]] < c_b)
																			goto is_a_corner;
																		else
																			goto is_not_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
						else
							if( p[offsets[7]] > cb)
								if( p[offsets[8]] > cb)
									if( p[offsets[9]] > cb)
										if( p[offsets[10]] > cb)
											if( p[offsets[11]] > cb)
												if( p[offsets[12]] > cb)
													if( p[offsets[13]] > cb)
														if( p[offsets[14]] > cb)
															if( p[offsets[15]] > cb)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else if( p[offsets[7]] < c_b)
								if( p[offsets[4]] < c_b)
									if( p[offsets[5]] < c_b)
										if( p[offsets[6]] < c_b)
											if( p[offsets[8]] < c_b)
												if( p[offsets[9]] < c_b)
													if( p[offsets[10]] < c_b)
														if( p[offsets[11]] < c_b)
															if( p[offsets[12]] < c_b)
																if( p[offsets[13]] < c_b)
																	if( p[offsets[14]] < c_b)
																		if( p[offsets[15]] < c_b)
																			goto is_a_corner;
																		else
																			goto is_not_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
					else if( p[offsets[2]] < c_b)
						if( p[offsets[6]] > cb)
							if( p[offsets[7]] > cb)
								if( p[offsets[8]] > cb)
									if( p[offsets[9]] > cb)
										if( p[offsets[10]] > cb)
											if( p[offsets[11]] > cb)
												if( p[offsets[12]] > cb)
													if( p[offsets[13]] > cb)
														if( p[offsets[14]] > cb)
															if( p[offsets[15]] > cb)
																goto is_a_corner;
															else
																if( p[offsets[3]] > cb)
																	if( p[offsets[4]] > cb)
																		if( p[offsets[5]] > cb)
																			goto is_a_corner;
																		else
																			goto is_not_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else if( p[offsets[6]] < c_b)
							if( p[offsets[4]] < c_b)
								if( p[offsets[5]] < c_b)
									if( p[offsets[7]] < c_b)
										if( p[offsets[8]] < c_b)
											if( p[offsets[9]] < c_b)
												if( p[offsets[10]] < c_b)
													if( p[offsets[11]] < c_b)
														if( p[offsets[12]] < c_b)
															if( p[offsets[13]] < c_b)
																if( p[offsets[3]] < c_b)
																	goto is_a_corner;
																else
																	if( p[offsets[14]] < c_b)
																		if( p[offsets[15]] < c_b)
																			goto is_a_corner;
																		else
																			goto is_not_a_corner;
																	else
																		goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
					else
						if( p[offsets[6]] > cb)
							if( p[offsets[7]] > cb)
								if( p[offsets[8]] > cb)
									if( p[offsets[9]] > cb)
										if( p[offsets[10]] > cb)
											if( p[offsets[11]] > cb)
												if( p[offsets[12]] > cb)
													if( p[offsets[13]] > cb)
														if( p[offsets[14]] > cb)
															if( p[offsets[15]] > cb)
																goto is_a_corner;
															else
																if( p[offsets[3]] > cb)
																	if( p[offsets[4]] > cb)
																		if( p[offsets[5]] > cb)
																			goto is_a_corner;
																		else
																			goto is_not_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else if( p[offsets[6]] < c_b)
							if( p[offsets[4]] < c_b)
								if( p[offsets[5]] < c_b)
									if( p[offsets[7]] < c_b)
										if( p[offsets[8]] < c_b)
											if( p[offsets[9]] < c_b)
												if( p[offsets[10]] < c_b)
													if( p[offsets[11]] < c_b)
														if( p[offsets[12]] < c_b)
															if( p[offsets[13]] < c_b)
																if( p[offsets[14]] < c_b)
																	if( p[offsets[3]] < c_b)
																		goto is_a_corner;
																	else
																		if( p[offsets[15]] < c_b)
																			goto is_a_corner;
																		else
																			goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
				else if( p[offsets[1]] < c_b)
					if( p[offsets[5]] > cb)
						if( p[offsets[6]] > cb)
							if( p[offsets[7]] > cb)
								if( p[offsets[8]] > cb)
									if( p[offsets[9]] > cb)
										if( p[offsets[10]] > cb)
											if( p[offsets[11]] > cb)
												if( p[offsets[12]] > cb)
													if( p[offsets[13]] > cb)
														if( p[offsets[14]] > cb)
															if( p[offsets[15]] > cb)
																goto is_a_corner;
															else
																if( p[offsets[3]] > cb)
																	if( p[offsets[4]] > cb)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
														else
															if( p[offsets[2]] > cb)
																if( p[offsets[3]] > cb)
																	if( p[offsets[4]] > cb)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
					else if( p[offsets[5]] < c_b)
						if( p[offsets[4]] < c_b)
							if( p[offsets[6]] < c_b)
								if( p[offsets[7]] < c_b)
									if( p[offsets[8]] < c_b)
										if( p[offsets[9]] < c_b)
											if( p[offsets[10]] < c_b)
												if( p[offsets[11]] < c_b)
													if( p[offsets[12]] < c_b)
														if( p[offsets[3]] < c_b)
															if( p[offsets[2]] < c_b)
																goto is_a_corner;
															else
																if( p[offsets[13]] < c_b)
																	if( p[offsets[14]] < c_b)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
														else
															if( p[offsets[13]] < c_b)
																if( p[offsets[14]] < c_b)
																	if( p[offsets[15]] < c_b)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
					else
						goto is_not_a_corner;
				else
					if( p[offsets[5]] > cb)
						if( p[offsets[6]] > cb)
							if( p[offsets[7]] > cb)
								if( p[offsets[8]] > cb)
									if( p[offsets[9]] > cb)
										if( p[offsets[10]] > cb)
											if( p[offsets[11]] > cb)
												if( p[offsets[12]] > cb)
													if( p[offsets[13]] > cb)
														if( p[offsets[14]] > cb)
															if( p[offsets[15]] > cb)
																goto is_a_corner;
															else
																if( p[offsets[3]] > cb)
																	if( p[offsets[4]] > cb)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
														else
															if( p[offsets[2]] > cb)
																if( p[offsets[3]] > cb)
																	if( p[offsets[4]] > cb)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
					else if( p[offsets[5]] < c_b)
						if( p[offsets[4]] < c_b)
							if( p[offsets[6]] < c_b)
								if( p[offsets[7]] < c_b)
									if( p[offsets[8]] < c_b)
										if( p[offsets[9]] < c_b)
											if( p[offsets[10]] < c_b)
												if( p[offsets[11]] < c_b)
													if( p[offsets[12]] < c_b)
														if( p[offsets[13]] < c_b)
															if( p[offsets[3]] < c_b)
																if( p[offsets[2]] < c_b)
																	goto is_a_corner;
																else
																	if( p[offsets[14]] < c_b)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
															else
																if( p[offsets[14]] < c_b)
																	if( p[offsets[15]] < c_b)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
					else
						goto is_not_a_corner;
			else if( p[offsets[0]] < c_b)
				if( p[offsets[1]] > cb)
					if( p[offsets[5]] > cb)
						if( p[offsets[4]] > cb)
							if( p[offsets[6]] > cb)
								if( p[offsets[7]] > cb)
									if( p[offsets[8]] > cb)
										if( p[offsets[9]] > cb)
											if( p[offsets[10]] > cb)
												if( p[offsets[11]] > cb)
													if( p[offsets[12]] > cb)
														if( p[offsets[3]] > cb)
															if( p[offsets[2]] > cb)
																goto is_a_corner;
															else
																if( p[offsets[13]] > cb)
																	if( p[offsets[14]] > cb)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
														else
															if( p[offsets[13]] > cb)
																if( p[offsets[14]] > cb)
																	if( p[offsets[15]] > cb)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
					else if( p[offsets[5]] < c_b)
						if( p[offsets[6]] < c_b)
							if( p[offsets[7]] < c_b)
								if( p[offsets[8]] < c_b)
									if( p[offsets[9]] < c_b)
										if( p[offsets[10]] < c_b)
											if( p[offsets[11]] < c_b)
												if( p[offsets[12]] < c_b)
													if( p[offsets[13]] < c_b)
														if( p[offsets[14]] < c_b)
															if( p[offsets[15]] < c_b)
																goto is_a_corner;
															else
																if( p[offsets[3]] < c_b)
																	if( p[offsets[4]] < c_b)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
														else
															if( p[offsets[2]] < c_b)
																if( p[offsets[3]] < c_b)
																	if( p[offsets[4]] < c_b)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
					else
						goto is_not_a_corner;
				else if( p[offsets[1]] < c_b)
					if( p[offsets[2]] > cb)
						if( p[offsets[6]] > cb)
							if( p[offsets[4]] > cb)
								if( p[offsets[5]] > cb)
									if( p[offsets[7]] > cb)
										if( p[offsets[8]] > cb)
											if( p[offsets[9]] > cb)
												if( p[offsets[10]] > cb)
													if( p[offsets[11]] > cb)
														if( p[offsets[12]] > cb)
															if( p[offsets[13]] > cb)
																if( p[offsets[3]] > cb)
																	goto is_a_corner;
																else
																	if( p[offsets[14]] > cb)
																		if( p[offsets[15]] > cb)
																			goto is_a_corner;
																		else
																			goto is_not_a_corner;
																	else
																		goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else if( p[offsets[6]] < c_b)
							if( p[offsets[7]] < c_b)
								if( p[offsets[8]] < c_b)
									if( p[offsets[9]] < c_b)
										if( p[offsets[10]] < c_b)
											if( p[offsets[11]] < c_b)
												if( p[offsets[12]] < c_b)
													if( p[offsets[13]] < c_b)
														if( p[offsets[14]] < c_b)
															if( p[offsets[15]] < c_b)
																goto is_a_corner;
															else
																if( p[offsets[3]] < c_b)
																	if( p[offsets[4]] < c_b)
																		if( p[offsets[5]] < c_b)
																			goto is_a_corner;
																		else
																			goto is_not_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
					else if( p[offsets[2]] < c_b)
						if( p[offsets[3]] > cb)
							if( p[offsets[15]] < c_b)
								if( p[offsets[7]] > cb)
									if( p[offsets[4]] > cb)
										if( p[offsets[5]] > cb)
											if( p[offsets[6]] > cb)
												if( p[offsets[8]] > cb)
													if( p[offsets[9]] > cb)
														if( p[offsets[10]] > cb)
															if( p[offsets[11]] > cb)
																if( p[offsets[12]] > cb)
																	if( p[offsets[13]] > cb)
																		if( p[offsets[14]] > cb)
																			goto is_a_corner;
																		else
																			goto is_not_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else if( p[offsets[7]] < c_b)
									if( p[offsets[8]] < c_b)
										if( p[offsets[9]] < c_b)
											if( p[offsets[10]] < c_b)
												if( p[offsets[11]] < c_b)
													if( p[offsets[12]] < c_b)
														if( p[offsets[13]] < c_b)
															if( p[offsets[14]] < c_b)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								if( p[offsets[4]] > cb)
									if( p[offsets[5]] > cb)
										if( p[offsets[6]] > cb)
											if( p[offsets[7]] > cb)
												if( p[offsets[8]] > cb)
													if( p[offsets[9]] > cb)
														if( p[offsets[10]] > cb)
															if( p[offsets[11]] > cb)
																if( p[offsets[12]] > cb)
																	if( p[offsets[13]] > cb)
																		if( p[offsets[14]] > cb)
																			goto is_a_corner;
																		else
																			goto is_not_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
						else if( p[offsets[3]] < c_b)
							if( p[offsets[4]] > cb)
								if( p[offsets[8]] > cb)
									if( p[offsets[5]] > cb)
										if( p[offsets[6]] > cb)
											if( p[offsets[7]] > cb)
												if( p[offsets[9]] > cb)
													if( p[offsets[10]] > cb)
														if( p[offsets[11]] > cb)
															if( p[offsets[12]] > cb)
																if( p[offsets[13]] > cb)
																	if( p[offsets[14]] > cb)
																		if( p[offsets[15]] > cb)
																			goto is_a_corner;
																		else
																			goto is_not_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else if( p[offsets[8]] < c_b)
									if( p[offsets[9]] < c_b)
										if( p[offsets[10]] < c_b)
											if( p[offsets[11]] < c_b)
												if( p[offsets[12]] < c_b)
													if( p[offsets[13]] < c_b)
														if( p[offsets[14]] < c_b)
															if( p[offsets[15]] < c_b)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else if( p[offsets[4]] < c_b)
								if( p[offsets[5]] < c_b)
									if( p[offsets[6]] < c_b)
										if( p[offsets[7]] < c_b)
											if( p[offsets[8]] < c_b)
												if( p[offsets[9]] < c_b)
													if( p[offsets[10]] < c_b)
														if( p[offsets[11]] < c_b)
															goto is_a_corner;
														else
															if( p[offsets[15]] < c_b)
																goto is_a_corner;
															else
																goto is_not_a_corner;
													else
														if( p[offsets[14]] < c_b)
															if( p[offsets[15]] < c_b)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
												else
													if( p[offsets[13]] < c_b)
														if( p[offsets[14]] < c_b)
															if( p[offsets[15]] < c_b)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if( p[offsets[12]] < c_b)
													if( p[offsets[13]] < c_b)
														if( p[offsets[14]] < c_b)
															if( p[offsets[15]] < c_b)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
										else
											if( p[offsets[11]] < c_b)
												if( p[offsets[12]] < c_b)
													if( p[offsets[13]] < c_b)
														if( p[offsets[14]] < c_b)
															if( p[offsets[15]] < c_b)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
									else
										if( p[offsets[10]] < c_b)
											if( p[offsets[11]] < c_b)
												if( p[offsets[12]] < c_b)
													if( p[offsets[13]] < c_b)
														if( p[offsets[14]] < c_b)
															if( p[offsets[15]] < c_b)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
								else
									if( p[offsets[9]] < c_b)
										if( p[offsets[10]] < c_b)
											if( p[offsets[11]] < c_b)
												if( p[offsets[12]] < c_b)
													if( p[offsets[13]] < c_b)
														if( p[offsets[14]] < c_b)
															if( p[offsets[15]] < c_b)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
							else
								if( p[offsets[8]] < c_b)
									if( p[offsets[9]] < c_b)
										if( p[offsets[10]] < c_b)
											if( p[offsets[11]] < c_b)
												if( p[offsets[12]] < c_b)
													if( p[offsets[13]] < c_b)
														if( p[offsets[14]] < c_b)
															if( p[offsets[15]] < c_b)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
						else
							if( p[offsets[7]] > cb)
								if( p[offsets[4]] > cb)
									if( p[offsets[5]] > cb)
										if( p[offsets[6]] > cb)
											if( p[offsets[8]] > cb)
												if( p[offsets[9]] > cb)
													if( p[offsets[10]] > cb)
														if( p[offsets[11]] > cb)
															if( p[offsets[12]] > cb)
																if( p[offsets[13]] > cb)
																	if( p[offsets[14]] > cb)
																		if( p[offsets[15]] > cb)
																			goto is_a_corner;
																		else
																			goto is_not_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else if( p[offsets[7]] < c_b)
								if( p[offsets[8]] < c_b)
									if( p[offsets[9]] < c_b)
										if( p[offsets[10]] < c_b)
											if( p[offsets[11]] < c_b)
												if( p[offsets[12]] < c_b)
													if( p[offsets[13]] < c_b)
														if( p[offsets[14]] < c_b)
															if( p[offsets[15]] < c_b)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
					else
						if( p[offsets[6]] > cb)
							if( p[offsets[4]] > cb)
								if( p[offsets[5]] > cb)
									if( p[offsets[7]] > cb)
										if( p[offsets[8]] > cb)
											if( p[offsets[9]] > cb)
												if( p[offsets[10]] > cb)
													if( p[offsets[11]] > cb)
														if( p[offsets[12]] > cb)
															if( p[offsets[13]] > cb)
																if( p[offsets[14]] > cb)
																	if( p[offsets[3]] > cb)
																		goto is_a_corner;
																	else
																		if( p[offsets[15]] > cb)
																			goto is_a_corner;
																		else
																			goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else if( p[offsets[6]] < c_b)
							if( p[offsets[7]] < c_b)
								if( p[offsets[8]] < c_b)
									if( p[offsets[9]] < c_b)
										if( p[offsets[10]] < c_b)
											if( p[offsets[11]] < c_b)
												if( p[offsets[12]] < c_b)
													if( p[offsets[13]] < c_b)
														if( p[offsets[14]] < c_b)
															if( p[offsets[15]] < c_b)
																goto is_a_corner;
															else
																if( p[offsets[3]] < c_b)
																	if( p[offsets[4]] < c_b)
																		if( p[offsets[5]] < c_b)
																			goto is_a_corner;
																		else
																			goto is_not_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
				else
					if( p[offsets[5]] > cb)
						if( p[offsets[4]] > cb)
							if( p[offsets[6]] > cb)
								if( p[offsets[7]] > cb)
									if( p[offsets[8]] > cb)
										if( p[offsets[9]] > cb)
											if( p[offsets[10]] > cb)
												if( p[offsets[11]] > cb)
													if( p[offsets[12]] > cb)
														if( p[offsets[13]] > cb)
															if( p[offsets[3]] > cb)
																if( p[offsets[2]] > cb)
																	goto is_a_corner;
																else
																	if( p[offsets[14]] > cb)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
															else
																if( p[offsets[14]] > cb)
																	if( p[offsets[15]] > cb)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
					else if( p[offsets[5]] < c_b)
						if( p[offsets[6]] < c_b)
							if( p[offsets[7]] < c_b)
								if( p[offsets[8]] < c_b)
									if( p[offsets[9]] < c_b)
										if( p[offsets[10]] < c_b)
											if( p[offsets[11]] < c_b)
												if( p[offsets[12]] < c_b)
													if( p[offsets[13]] < c_b)
														if( p[offsets[14]] < c_b)
															if( p[offsets[15]] < c_b)
																goto is_a_corner;
															else
																if( p[offsets[3]] < c_b)
																	if( p[offsets[4]] < c_b)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
														else
															if( p[offsets[2]] < c_b)
																if( p[offsets[3]] < c_b)
																	if( p[offsets[4]] < c_b)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
					else
						goto is_not_a_corner;
			else
				if( p[offsets[4]] > cb)
					if( p[offsets[5]] > cb)
						if( p[offsets[6]] > cb)
							if( p[offsets[7]] > cb)
								if( p[offsets[8]] > cb)
									if( p[offsets[9]] > cb)
										if( p[offsets[10]] > cb)
											if( p[offsets[11]] > cb)
												if( p[offsets[12]] > cb)
													if( p[offsets[3]] > cb)
														if( p[offsets[2]] > cb)
															if( p[offsets[1]] > cb)
																goto is_a_corner;
															else
																if( p[offsets[13]] > cb)
																	goto is_a_corner;
																else
																	goto is_not_a_corner;
														else
															if( p[offsets[13]] > cb)
																if( p[offsets[14]] > cb)
																	goto is_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
													else
														if( p[offsets[13]] > cb)
															if( p[offsets[14]] > cb)
																if( p[offsets[15]] > cb)
																	goto is_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
					else
						goto is_not_a_corner;
				else if( p[offsets[4]] < c_b)
					if( p[offsets[5]] < c_b)
						if( p[offsets[6]] < c_b)
							if( p[offsets[7]] < c_b)
								if( p[offsets[8]] < c_b)
									if( p[offsets[9]] < c_b)
										if( p[offsets[10]] < c_b)
											if( p[offsets[11]] < c_b)
												if( p[offsets[12]] < c_b)
													if( p[offsets[3]] < c_b)
														if( p[offsets[2]] < c_b)
															if( p[offsets[1]] < c_b)
																goto is_a_corner;
															else
																if( p[offsets[13]] < c_b)
																	goto is_a_corner;
																else
																	goto is_not_a_corner;
														else
															if( p[offsets[13]] < c_b)
																if( p[offsets[14]] < c_b)
																	goto is_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
													else
														if( p[offsets[13]] < c_b)
															if( p[offsets[14]] < c_b)
																if( p[offsets[15]] < c_b)
																	goto is_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
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

	void FAST::detect12( const Image & img, uint8_t threshold, std::vector<Feature2Df> & corners, size_t border )
	{
		size_t stride;
        const uint8_t * im = img.map( &stride );
        
		int num_corners=0;
		int rsize=512;
		size_t x, y;
        size_t xsize = img.width() - border;
        size_t ysize = img.height() - border;
        
		corners.reserve( rsize );
        
        int offsets[ 16 ];
		make_offsets( offsets, stride );
        
		for( y=border; y < ysize; y++ )
			for( x=border; x < xsize; x++ ) {
				const uint8_t* p = im + y*stride + x;

				int cb = *p + threshold;
				int c_b= *p - threshold;
				if(p[offsets[0]] > cb)
					if(p[offsets[1]] > cb)
						if(p[offsets[2]] > cb)
							if(p[offsets[3]] > cb)
								if(p[offsets[4]] > cb)
									if(p[offsets[5]] > cb)
										if(p[offsets[6]] > cb)
											if(p[offsets[7]] > cb)
												if(p[offsets[8]] > cb)
													if(p[offsets[9]] > cb)
														if(p[offsets[10]] > cb)
															if(p[offsets[11]] > cb)
															{}
															else
																if(p[offsets[15]] > cb)
																{}
																else
																	continue;
														else
															if(p[offsets[14]] > cb)
																if(p[offsets[15]] > cb)
																{}
																else
																	continue;
															else
																continue;
													else
														if(p[offsets[13]] > cb)
															if(p[offsets[14]] > cb)
																if(p[offsets[15]] > cb)
																{}
																else
																	continue;
															else
																continue;
														else
															continue;
												else
													if(p[offsets[12]] > cb)
														if(p[offsets[13]] > cb)
															if(p[offsets[14]] > cb)
																if(p[offsets[15]] > cb)
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
												if(p[offsets[11]] > cb)
													if(p[offsets[12]] > cb)
														if(p[offsets[13]] > cb)
															if(p[offsets[14]] > cb)
																if(p[offsets[15]] > cb)
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
											if(p[offsets[10]] > cb)
												if(p[offsets[11]] > cb)
													if(p[offsets[12]] > cb)
														if(p[offsets[13]] > cb)
															if(p[offsets[14]] > cb)
																if(p[offsets[15]] > cb)
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
										if(p[offsets[9]] > cb)
											if(p[offsets[10]] > cb)
												if(p[offsets[11]] > cb)
													if(p[offsets[12]] > cb)
														if(p[offsets[13]] > cb)
															if(p[offsets[14]] > cb)
																if(p[offsets[15]] > cb)
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
								else if(p[offsets[4]] < c_b)
									if(p[offsets[8]] > cb)
										if(p[offsets[9]] > cb)
											if(p[offsets[10]] > cb)
												if(p[offsets[11]] > cb)
													if(p[offsets[12]] > cb)
														if(p[offsets[13]] > cb)
															if(p[offsets[14]] > cb)
																if(p[offsets[15]] > cb)
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
									else if(p[offsets[8]] < c_b)
										if(p[offsets[5]] < c_b)
											if(p[offsets[6]] < c_b)
												if(p[offsets[7]] < c_b)
													if(p[offsets[9]] < c_b)
														if(p[offsets[10]] < c_b)
															if(p[offsets[11]] < c_b)
																if(p[offsets[12]] < c_b)
																	if(p[offsets[13]] < c_b)
																		if(p[offsets[14]] < c_b)
																			if(p[offsets[15]] < c_b)
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
									if(p[offsets[8]] > cb)
										if(p[offsets[9]] > cb)
											if(p[offsets[10]] > cb)
												if(p[offsets[11]] > cb)
													if(p[offsets[12]] > cb)
														if(p[offsets[13]] > cb)
															if(p[offsets[14]] > cb)
																if(p[offsets[15]] > cb)
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
							else if(p[offsets[3]] < c_b)
								if(p[offsets[15]] > cb)
									if(p[offsets[7]] > cb)
										if(p[offsets[8]] > cb)
											if(p[offsets[9]] > cb)
												if(p[offsets[10]] > cb)
													if(p[offsets[11]] > cb)
														if(p[offsets[12]] > cb)
															if(p[offsets[13]] > cb)
																if(p[offsets[14]] > cb)
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
									else if(p[offsets[7]] < c_b)
										if(p[offsets[4]] < c_b)
											if(p[offsets[5]] < c_b)
												if(p[offsets[6]] < c_b)
													if(p[offsets[8]] < c_b)
														if(p[offsets[9]] < c_b)
															if(p[offsets[10]] < c_b)
																if(p[offsets[11]] < c_b)
																	if(p[offsets[12]] < c_b)
																		if(p[offsets[13]] < c_b)
																			if(p[offsets[14]] < c_b)
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
									if(p[offsets[4]] < c_b)
										if(p[offsets[5]] < c_b)
											if(p[offsets[6]] < c_b)
												if(p[offsets[7]] < c_b)
													if(p[offsets[8]] < c_b)
														if(p[offsets[9]] < c_b)
															if(p[offsets[10]] < c_b)
																if(p[offsets[11]] < c_b)
																	if(p[offsets[12]] < c_b)
																		if(p[offsets[13]] < c_b)
																			if(p[offsets[14]] < c_b)
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
								if(p[offsets[7]] > cb)
									if(p[offsets[8]] > cb)
										if(p[offsets[9]] > cb)
											if(p[offsets[10]] > cb)
												if(p[offsets[11]] > cb)
													if(p[offsets[12]] > cb)
														if(p[offsets[13]] > cb)
															if(p[offsets[14]] > cb)
																if(p[offsets[15]] > cb)
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
								else if(p[offsets[7]] < c_b)
									if(p[offsets[4]] < c_b)
										if(p[offsets[5]] < c_b)
											if(p[offsets[6]] < c_b)
												if(p[offsets[8]] < c_b)
													if(p[offsets[9]] < c_b)
														if(p[offsets[10]] < c_b)
															if(p[offsets[11]] < c_b)
																if(p[offsets[12]] < c_b)
																	if(p[offsets[13]] < c_b)
																		if(p[offsets[14]] < c_b)
																			if(p[offsets[15]] < c_b)
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
						else if(p[offsets[2]] < c_b)
							if(p[offsets[6]] > cb)
								if(p[offsets[7]] > cb)
									if(p[offsets[8]] > cb)
										if(p[offsets[9]] > cb)
											if(p[offsets[10]] > cb)
												if(p[offsets[11]] > cb)
													if(p[offsets[12]] > cb)
														if(p[offsets[13]] > cb)
															if(p[offsets[14]] > cb)
																if(p[offsets[15]] > cb)
																{}
																else
																	if(p[offsets[3]] > cb)
																		if(p[offsets[4]] > cb)
																			if(p[offsets[5]] > cb)
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
							else if(p[offsets[6]] < c_b)
								if(p[offsets[4]] < c_b)
									if(p[offsets[5]] < c_b)
										if(p[offsets[7]] < c_b)
											if(p[offsets[8]] < c_b)
												if(p[offsets[9]] < c_b)
													if(p[offsets[10]] < c_b)
														if(p[offsets[11]] < c_b)
															if(p[offsets[12]] < c_b)
																if(p[offsets[13]] < c_b)
																	if(p[offsets[3]] < c_b)
																	{}
																	else
																		if(p[offsets[14]] < c_b)
																			if(p[offsets[15]] < c_b)
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
							if(p[offsets[6]] > cb)
								if(p[offsets[7]] > cb)
									if(p[offsets[8]] > cb)
										if(p[offsets[9]] > cb)
											if(p[offsets[10]] > cb)
												if(p[offsets[11]] > cb)
													if(p[offsets[12]] > cb)
														if(p[offsets[13]] > cb)
															if(p[offsets[14]] > cb)
																if(p[offsets[15]] > cb)
																{}
																else
																	if(p[offsets[3]] > cb)
																		if(p[offsets[4]] > cb)
																			if(p[offsets[5]] > cb)
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
							else if(p[offsets[6]] < c_b)
								if(p[offsets[4]] < c_b)
									if(p[offsets[5]] < c_b)
										if(p[offsets[7]] < c_b)
											if(p[offsets[8]] < c_b)
												if(p[offsets[9]] < c_b)
													if(p[offsets[10]] < c_b)
														if(p[offsets[11]] < c_b)
															if(p[offsets[12]] < c_b)
																if(p[offsets[13]] < c_b)
																	if(p[offsets[14]] < c_b)
																		if(p[offsets[3]] < c_b)
																		{}
																		else
																			if(p[offsets[15]] < c_b)
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
					else if(p[offsets[1]] < c_b)
						if(p[offsets[5]] > cb)
							if(p[offsets[6]] > cb)
								if(p[offsets[7]] > cb)
									if(p[offsets[8]] > cb)
										if(p[offsets[9]] > cb)
											if(p[offsets[10]] > cb)
												if(p[offsets[11]] > cb)
													if(p[offsets[12]] > cb)
														if(p[offsets[13]] > cb)
															if(p[offsets[14]] > cb)
																if(p[offsets[15]] > cb)
																{}
																else
																	if(p[offsets[3]] > cb)
																		if(p[offsets[4]] > cb)
																		{}
																		else
																			continue;
																	else
																		continue;
															else
																if(p[offsets[2]] > cb)
																	if(p[offsets[3]] > cb)
																		if(p[offsets[4]] > cb)
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
						else if(p[offsets[5]] < c_b)
							if(p[offsets[4]] < c_b)
								if(p[offsets[6]] < c_b)
									if(p[offsets[7]] < c_b)
										if(p[offsets[8]] < c_b)
											if(p[offsets[9]] < c_b)
												if(p[offsets[10]] < c_b)
													if(p[offsets[11]] < c_b)
														if(p[offsets[12]] < c_b)
															if(p[offsets[3]] < c_b)
																if(p[offsets[2]] < c_b)
																{}
																else
																	if(p[offsets[13]] < c_b)
																		if(p[offsets[14]] < c_b)
																		{}
																		else
																			continue;
																	else
																		continue;
															else
																if(p[offsets[13]] < c_b)
																	if(p[offsets[14]] < c_b)
																		if(p[offsets[15]] < c_b)
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
						if(p[offsets[5]] > cb)
							if(p[offsets[6]] > cb)
								if(p[offsets[7]] > cb)
									if(p[offsets[8]] > cb)
										if(p[offsets[9]] > cb)
											if(p[offsets[10]] > cb)
												if(p[offsets[11]] > cb)
													if(p[offsets[12]] > cb)
														if(p[offsets[13]] > cb)
															if(p[offsets[14]] > cb)
																if(p[offsets[15]] > cb)
																{}
																else
																	if(p[offsets[3]] > cb)
																		if(p[offsets[4]] > cb)
																		{}
																		else
																			continue;
																	else
																		continue;
															else
																if(p[offsets[2]] > cb)
																	if(p[offsets[3]] > cb)
																		if(p[offsets[4]] > cb)
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
						else if(p[offsets[5]] < c_b)
							if(p[offsets[4]] < c_b)
								if(p[offsets[6]] < c_b)
									if(p[offsets[7]] < c_b)
										if(p[offsets[8]] < c_b)
											if(p[offsets[9]] < c_b)
												if(p[offsets[10]] < c_b)
													if(p[offsets[11]] < c_b)
														if(p[offsets[12]] < c_b)
															if(p[offsets[13]] < c_b)
																if(p[offsets[3]] < c_b)
																	if(p[offsets[2]] < c_b)
																	{}
																	else
																		if(p[offsets[14]] < c_b)
																		{}
																		else
																			continue;
																else
																	if(p[offsets[14]] < c_b)
																		if(p[offsets[15]] < c_b)
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
				else if(p[offsets[0]] < c_b)
					if(p[offsets[1]] > cb)
						if(p[offsets[5]] > cb)
							if(p[offsets[4]] > cb)
								if(p[offsets[6]] > cb)
									if(p[offsets[7]] > cb)
										if(p[offsets[8]] > cb)
											if(p[offsets[9]] > cb)
												if(p[offsets[10]] > cb)
													if(p[offsets[11]] > cb)
														if(p[offsets[12]] > cb)
															if(p[offsets[3]] > cb)
																if(p[offsets[2]] > cb)
																{}
																else
																	if(p[offsets[13]] > cb)
																		if(p[offsets[14]] > cb)
																		{}
																		else
																			continue;
																	else
																		continue;
															else
																if(p[offsets[13]] > cb)
																	if(p[offsets[14]] > cb)
																		if(p[offsets[15]] > cb)
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
						else if(p[offsets[5]] < c_b)
							if(p[offsets[6]] < c_b)
								if(p[offsets[7]] < c_b)
									if(p[offsets[8]] < c_b)
										if(p[offsets[9]] < c_b)
											if(p[offsets[10]] < c_b)
												if(p[offsets[11]] < c_b)
													if(p[offsets[12]] < c_b)
														if(p[offsets[13]] < c_b)
															if(p[offsets[14]] < c_b)
																if(p[offsets[15]] < c_b)
																{}
																else
																	if(p[offsets[3]] < c_b)
																		if(p[offsets[4]] < c_b)
																		{}
																		else
																			continue;
																	else
																		continue;
															else
																if(p[offsets[2]] < c_b)
																	if(p[offsets[3]] < c_b)
																		if(p[offsets[4]] < c_b)
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
					else if(p[offsets[1]] < c_b)
						if(p[offsets[2]] > cb)
							if(p[offsets[6]] > cb)
								if(p[offsets[4]] > cb)
									if(p[offsets[5]] > cb)
										if(p[offsets[7]] > cb)
											if(p[offsets[8]] > cb)
												if(p[offsets[9]] > cb)
													if(p[offsets[10]] > cb)
														if(p[offsets[11]] > cb)
															if(p[offsets[12]] > cb)
																if(p[offsets[13]] > cb)
																	if(p[offsets[3]] > cb)
																	{}
																	else
																		if(p[offsets[14]] > cb)
																			if(p[offsets[15]] > cb)
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
							else if(p[offsets[6]] < c_b)
								if(p[offsets[7]] < c_b)
									if(p[offsets[8]] < c_b)
										if(p[offsets[9]] < c_b)
											if(p[offsets[10]] < c_b)
												if(p[offsets[11]] < c_b)
													if(p[offsets[12]] < c_b)
														if(p[offsets[13]] < c_b)
															if(p[offsets[14]] < c_b)
																if(p[offsets[15]] < c_b)
																{}
																else
																	if(p[offsets[3]] < c_b)
																		if(p[offsets[4]] < c_b)
																			if(p[offsets[5]] < c_b)
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
						else if(p[offsets[2]] < c_b)
							if(p[offsets[3]] > cb)
								if(p[offsets[15]] < c_b)
									if(p[offsets[7]] > cb)
										if(p[offsets[4]] > cb)
											if(p[offsets[5]] > cb)
												if(p[offsets[6]] > cb)
													if(p[offsets[8]] > cb)
														if(p[offsets[9]] > cb)
															if(p[offsets[10]] > cb)
																if(p[offsets[11]] > cb)
																	if(p[offsets[12]] > cb)
																		if(p[offsets[13]] > cb)
																			if(p[offsets[14]] > cb)
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
									else if(p[offsets[7]] < c_b)
										if(p[offsets[8]] < c_b)
											if(p[offsets[9]] < c_b)
												if(p[offsets[10]] < c_b)
													if(p[offsets[11]] < c_b)
														if(p[offsets[12]] < c_b)
															if(p[offsets[13]] < c_b)
																if(p[offsets[14]] < c_b)
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
									if(p[offsets[4]] > cb)
										if(p[offsets[5]] > cb)
											if(p[offsets[6]] > cb)
												if(p[offsets[7]] > cb)
													if(p[offsets[8]] > cb)
														if(p[offsets[9]] > cb)
															if(p[offsets[10]] > cb)
																if(p[offsets[11]] > cb)
																	if(p[offsets[12]] > cb)
																		if(p[offsets[13]] > cb)
																			if(p[offsets[14]] > cb)
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
							else if(p[offsets[3]] < c_b)
								if(p[offsets[4]] > cb)
									if(p[offsets[8]] > cb)
										if(p[offsets[5]] > cb)
											if(p[offsets[6]] > cb)
												if(p[offsets[7]] > cb)
													if(p[offsets[9]] > cb)
														if(p[offsets[10]] > cb)
															if(p[offsets[11]] > cb)
																if(p[offsets[12]] > cb)
																	if(p[offsets[13]] > cb)
																		if(p[offsets[14]] > cb)
																			if(p[offsets[15]] > cb)
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
									else if(p[offsets[8]] < c_b)
										if(p[offsets[9]] < c_b)
											if(p[offsets[10]] < c_b)
												if(p[offsets[11]] < c_b)
													if(p[offsets[12]] < c_b)
														if(p[offsets[13]] < c_b)
															if(p[offsets[14]] < c_b)
																if(p[offsets[15]] < c_b)
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
								else if(p[offsets[4]] < c_b)
									if(p[offsets[5]] < c_b)
										if(p[offsets[6]] < c_b)
											if(p[offsets[7]] < c_b)
												if(p[offsets[8]] < c_b)
													if(p[offsets[9]] < c_b)
														if(p[offsets[10]] < c_b)
															if(p[offsets[11]] < c_b)
															{}
															else
																if(p[offsets[15]] < c_b)
																{}
																else
																	continue;
														else
															if(p[offsets[14]] < c_b)
																if(p[offsets[15]] < c_b)
																{}
																else
																	continue;
															else
																continue;
													else
														if(p[offsets[13]] < c_b)
															if(p[offsets[14]] < c_b)
																if(p[offsets[15]] < c_b)
																{}
																else
																	continue;
															else
																continue;
														else
															continue;
												else
													if(p[offsets[12]] < c_b)
														if(p[offsets[13]] < c_b)
															if(p[offsets[14]] < c_b)
																if(p[offsets[15]] < c_b)
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
												if(p[offsets[11]] < c_b)
													if(p[offsets[12]] < c_b)
														if(p[offsets[13]] < c_b)
															if(p[offsets[14]] < c_b)
																if(p[offsets[15]] < c_b)
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
											if(p[offsets[10]] < c_b)
												if(p[offsets[11]] < c_b)
													if(p[offsets[12]] < c_b)
														if(p[offsets[13]] < c_b)
															if(p[offsets[14]] < c_b)
																if(p[offsets[15]] < c_b)
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
										if(p[offsets[9]] < c_b)
											if(p[offsets[10]] < c_b)
												if(p[offsets[11]] < c_b)
													if(p[offsets[12]] < c_b)
														if(p[offsets[13]] < c_b)
															if(p[offsets[14]] < c_b)
																if(p[offsets[15]] < c_b)
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
									if(p[offsets[8]] < c_b)
										if(p[offsets[9]] < c_b)
											if(p[offsets[10]] < c_b)
												if(p[offsets[11]] < c_b)
													if(p[offsets[12]] < c_b)
														if(p[offsets[13]] < c_b)
															if(p[offsets[14]] < c_b)
																if(p[offsets[15]] < c_b)
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
								if(p[offsets[7]] > cb)
									if(p[offsets[4]] > cb)
										if(p[offsets[5]] > cb)
											if(p[offsets[6]] > cb)
												if(p[offsets[8]] > cb)
													if(p[offsets[9]] > cb)
														if(p[offsets[10]] > cb)
															if(p[offsets[11]] > cb)
																if(p[offsets[12]] > cb)
																	if(p[offsets[13]] > cb)
																		if(p[offsets[14]] > cb)
																			if(p[offsets[15]] > cb)
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
								else if(p[offsets[7]] < c_b)
									if(p[offsets[8]] < c_b)
										if(p[offsets[9]] < c_b)
											if(p[offsets[10]] < c_b)
												if(p[offsets[11]] < c_b)
													if(p[offsets[12]] < c_b)
														if(p[offsets[13]] < c_b)
															if(p[offsets[14]] < c_b)
																if(p[offsets[15]] < c_b)
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
							if(p[offsets[6]] > cb)
								if(p[offsets[4]] > cb)
									if(p[offsets[5]] > cb)
										if(p[offsets[7]] > cb)
											if(p[offsets[8]] > cb)
												if(p[offsets[9]] > cb)
													if(p[offsets[10]] > cb)
														if(p[offsets[11]] > cb)
															if(p[offsets[12]] > cb)
																if(p[offsets[13]] > cb)
																	if(p[offsets[14]] > cb)
																		if(p[offsets[3]] > cb)
																		{}
																		else
																			if(p[offsets[15]] > cb)
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
							else if(p[offsets[6]] < c_b)
								if(p[offsets[7]] < c_b)
									if(p[offsets[8]] < c_b)
										if(p[offsets[9]] < c_b)
											if(p[offsets[10]] < c_b)
												if(p[offsets[11]] < c_b)
													if(p[offsets[12]] < c_b)
														if(p[offsets[13]] < c_b)
															if(p[offsets[14]] < c_b)
																if(p[offsets[15]] < c_b)
																{}
																else
																	if(p[offsets[3]] < c_b)
																		if(p[offsets[4]] < c_b)
																			if(p[offsets[5]] < c_b)
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
						if(p[offsets[5]] > cb)
							if(p[offsets[4]] > cb)
								if(p[offsets[6]] > cb)
									if(p[offsets[7]] > cb)
										if(p[offsets[8]] > cb)
											if(p[offsets[9]] > cb)
												if(p[offsets[10]] > cb)
													if(p[offsets[11]] > cb)
														if(p[offsets[12]] > cb)
															if(p[offsets[13]] > cb)
																if(p[offsets[3]] > cb)
																	if(p[offsets[2]] > cb)
																	{}
																	else
																		if(p[offsets[14]] > cb)
																		{}
																		else
																			continue;
																else
																	if(p[offsets[14]] > cb)
																		if(p[offsets[15]] > cb)
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
						else if(p[offsets[5]] < c_b)
							if(p[offsets[6]] < c_b)
								if(p[offsets[7]] < c_b)
									if(p[offsets[8]] < c_b)
										if(p[offsets[9]] < c_b)
											if(p[offsets[10]] < c_b)
												if(p[offsets[11]] < c_b)
													if(p[offsets[12]] < c_b)
														if(p[offsets[13]] < c_b)
															if(p[offsets[14]] < c_b)
																if(p[offsets[15]] < c_b)
																{}
																else
																	if(p[offsets[3]] < c_b)
																		if(p[offsets[4]] < c_b)
																		{}
																		else
																			continue;
																	else
																		continue;
															else
																if(p[offsets[2]] < c_b)
																	if(p[offsets[3]] < c_b)
																		if(p[offsets[4]] < c_b)
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
					if(p[offsets[4]] > cb)
						if(p[offsets[5]] > cb)
							if(p[offsets[6]] > cb)
								if(p[offsets[7]] > cb)
									if(p[offsets[8]] > cb)
										if(p[offsets[9]] > cb)
											if(p[offsets[10]] > cb)
												if(p[offsets[11]] > cb)
													if(p[offsets[12]] > cb)
														if(p[offsets[3]] > cb)
															if(p[offsets[2]] > cb)
																if(p[offsets[1]] > cb)
																{}
																else
																	if(p[offsets[13]] > cb)
																	{}
																	else
																		continue;
															else
																if(p[offsets[13]] > cb)
																	if(p[offsets[14]] > cb)
																	{}
																	else
																		continue;
																else
																	continue;
														else
															if(p[offsets[13]] > cb)
																if(p[offsets[14]] > cb)
																	if(p[offsets[15]] > cb)
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
					else if(p[offsets[4]] < c_b)
						if(p[offsets[5]] < c_b)
							if(p[offsets[6]] < c_b)
								if(p[offsets[7]] < c_b)
									if(p[offsets[8]] < c_b)
										if(p[offsets[9]] < c_b)
											if(p[offsets[10]] < c_b)
												if(p[offsets[11]] < c_b)
													if(p[offsets[12]] < c_b)
														if(p[offsets[3]] < c_b)
															if(p[offsets[2]] < c_b)
																if(p[offsets[1]] < c_b)
																{}
																else
																	if(p[offsets[13]] < c_b)
																	{}
																	else
																		continue;
															else
																if(p[offsets[13]] < c_b)
																	if(p[offsets[14]] < c_b)
																	{}
																	else
																		continue;
																else
																	continue;
														else
															if(p[offsets[13]] < c_b)
																if(p[offsets[14]] < c_b)
																	if(p[offsets[15]] < c_b)
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

				corners.push_back( Feature2Df( x, y ) );
				num_corners++;
			}
        img.unmap( im );
	}

}
