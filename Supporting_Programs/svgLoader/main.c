#include <stdio.h>
#include <stdlib.h>
#include "glDisplay.h"
#include "svgLoader.h"

#define TRUE 1
#define FALSE 0

int main(int argc, char** argv)
{
	float *edges = NULL;
	int *color = NULL;
	int edgeCnt = 0;
	
	if (loadSvgData("test.svg", &edges, &color, &edgeCnt, TRUE))
	return 1;
	
	setWindowDimensions(640, 480);
	setPlotDimensions(480, 480);
	setPlotCenter(320.0, -240.0);
	setPlotZoom(1.0);
	setBgColor(1.0, 1.0, 1.0);
	setPlotData(edges, edgeCnt, 0.5, 0.3, 0.3, 1.0, 1.0);
	
	freeSvgData(edges, color);
	displayPlot();
	
	return 0;
}
