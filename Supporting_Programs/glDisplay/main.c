#include <stdio.h>
#include <stdlib.h>
#include "glDisplay.h"

float square[] = {
	1, 1, -1, 1,
	1, 1, 1, -1,
	-1, -1, -1, 1,
	-1, -1, 1, -1,
};

int main(int argc, char** argv)
{
	int i;
	float edges1[50*4];
	float edges2[50*4];
	
	setWindowDimensions(720, 480);
	setPlotDimensions(1.5, 3);
	setPlotZoom(1.0);
	setPlotCenter(0.0, 0.0);
	setBgColor(1.0, 1.0, 1.0);
	
	setPlotData(square, sizeof(square)/sizeof(float)/4, 0.2, 0.8, 0.8, 1.0, 2.0);
	
	for (i=0; i < 50*4; i+=4)
	{
		edges1[i+0] = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
		edges1[i+1] = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
		edges1[i+2] = edges1[i+1];
		edges1[i+3] = edges1[i+0];
		
		edges2[i+0] = edges1[i+0];
		edges2[i+1] = edges1[i+0];
		edges2[i+2] = edges1[i+0];
		edges2[i+3] = edges1[i+1];
	}
	
	setPlotData(edges1, 50, 0.2, 0.2, 1.0, 0.3, 3.0);
	setPlotData(edges2, 50, 0.0, 0.2, 0.6, 0.3, 1.0);
	displayPlot();
}
