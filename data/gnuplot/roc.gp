set term aqua 0 
plot 'boatROC1to2.txt' with lines, 'boat_ocv_ROC1to2.txt' with lines,\
	 'boatROC1to3.txt' with lines, 'boat_ocv_ROC1to3.txt' with lines,\
	 'boatROC1to4.txt' with lines, 'boat_ocv_ROC1to4.txt' with lines,\
	 'boatROC1to5.txt' with lines, 'boat_ocv_ROC1to5.txt' with lines

set term aqua 1 
plot 'grafROC1to2.txt' with lines, 'graf_ocv_ROC1to2.txt' with lines,\
	 'grafROC1to3.txt' with lines, 'graf_ocv_ROC1to3.txt' with lines,\
	 'grafROC1to4.txt' with lines, 'graf_ocv_ROC1to4.txt' with lines,\
	 'grafROC1to5.txt' with lines, 'graf_ocv_ROC1to5.txt' with lines

set term aqua 2 
plot 'wallROC1to2.txt' with lines, 'wall_ocv_ROC1to2.txt' with lines,\
	 'wallROC1to3.txt' with lines, 'wall_ocv_ROC1to3.txt' with lines,\
	 'wallROC1to4.txt' with lines, 'wall_ocv_ROC1to4.txt' with lines,\
	 'wallROC1to5.txt' with lines, 'wall_ocv_ROC1to5.txt' with lines
