set key right bottom

set term wxt 0 
plot 'barkROC1to2.txt' with lines, 'bark_ocv_ROC1to2.txt' with lines,\
	 'barkROC1to3.txt' with lines, 'bark_ocv_ROC1to3.txt' with lines,\
	 'barkROC1to4.txt' with lines, 'bark_ocv_ROC1to4.txt' with lines,\
	 'barkROC1to5.txt' with lines, 'bark_ocv_ROC1to5.txt' with lines,\
	 'barkROC1to6.txt' with lines, 'bark_ocv_ROC1to6.txt' with lines,\
	 'barkAVG_ROC.txt' with linespoints

set term wxt 1 
plot 'bikesROC1to2.txt' with lines, 'bikes_ocv_ROC1to2.txt' with lines,\
	 'bikesROC1to3.txt' with lines, 'bikes_ocv_ROC1to3.txt' with lines,\
	 'bikesROC1to4.txt' with lines, 'bikes_ocv_ROC1to4.txt' with lines,\
	 'bikesROC1to5.txt' with lines, 'bikes_ocv_ROC1to5.txt' with lines,\
	 'bikesROC1to6.txt' with lines, 'bikes_ocv_ROC1to6.txt' with lines,\
	 'bikesAVG_ROC.txt' with linespoints

set term wxt 2 
plot 'boatROC1to2.txt' with lines, 'boat_ocv_ROC1to2.txt' with lines,\
	 'boatROC1to3.txt' with lines, 'boat_ocv_ROC1to3.txt' with lines,\
	 'boatROC1to4.txt' with lines, 'boat_ocv_ROC1to4.txt' with lines,\
	 'boatROC1to5.txt' with lines, 'boat_ocv_ROC1to5.txt' with lines,\
	 'boatROC1to6.txt' with lines, 'boat_ocv_ROC1to6.txt' with lines,\
	 'boatAVG_ROC.txt' with linespoints

set term wxt 3 
plot 'grafROC1to2.txt' with lines, 'graf_ocv_ROC1to2.txt' with lines,\
	 'grafROC1to3.txt' with lines, 'graf_ocv_ROC1to3.txt' with lines,\
	 'grafROC1to4.txt' with lines, 'graf_ocv_ROC1to4.txt' with lines,\
	 'grafROC1to5.txt' with lines, 'graf_ocv_ROC1to5.txt' with lines,\
	 'grafROC1to6.txt' with lines, 'graf_ocv_ROC1to6.txt' with lines,\
	 'grafAVG_ROC.txt' with linespoints

set term wxt 4 
plot 'leuvenROC1to2.txt' with lines, 'leuven_ocv_ROC1to2.txt' with lines,\
	 'leuvenROC1to3.txt' with lines, 'leuven_ocv_ROC1to3.txt' with lines,\
	 'leuvenROC1to4.txt' with lines, 'leuven_ocv_ROC1to4.txt' with lines,\
	 'leuvenROC1to5.txt' with lines, 'leuven_ocv_ROC1to5.txt' with lines,\
	 'leuvenROC1to6.txt' with lines, 'leuven_ocv_ROC1to6.txt' with lines,\
	 'leuvenAVG_ROC.txt' with linespoints

set term wxt 5 
plot 'treesROC1to2.txt' with lines, 'trees_ocv_ROC1to2.txt' with lines,\
	 'treesROC1to3.txt' with lines, 'trees_ocv_ROC1to3.txt' with lines,\
	 'treesROC1to4.txt' with lines, 'trees_ocv_ROC1to4.txt' with lines,\
	 'treesROC1to5.txt' with lines, 'trees_ocv_ROC1to5.txt' with lines,\
	 'treesROC1to6.txt' with lines, 'trees_ocv_ROC1to6.txt' with lines,\
	 'treesAVG_ROC.txt' with linespoints

set term wxt 6 
plot 'ubcROC1to2.txt' with lines, 'ubc_ocv_ROC1to2.txt' with lines,\
	 'ubcROC1to3.txt' with lines, 'ubc_ocv_ROC1to3.txt' with lines,\
	 'ubcROC1to4.txt' with lines, 'ubc_ocv_ROC1to4.txt' with lines,\
	 'ubcROC1to5.txt' with lines, 'ubc_ocv_ROC1to5.txt' with lines,\
	 'ubcROC1to6.txt' with lines, 'ubc_ocv_ROC1to6.txt' with lines,\
	 'ubcAVG_ROC.txt' with linespoints 

set term wxt 7 
plot 'wallROC1to2.txt' with lines, 'wall_ocv_ROC1to2.txt' with lines,\
	 'wallROC1to3.txt' with lines, 'wall_ocv_ROC1to3.txt' with lines,\
	 'wallROC1to4.txt' with lines, 'wall_ocv_ROC1to4.txt' with lines,\
	 'wallROC1to5.txt' with lines, 'wall_ocv_ROC1to5.txt' with lines,\
	 'wallROC1to6.txt' with lines, 'wall_ocv_ROC1to6.txt' with lines,\
	 'wallAVG_ROC.txt' with linespoints 
