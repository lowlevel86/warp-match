#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void freeFileBuffer(char *buffer)
{
	if (buffer != NULL)
	free(buffer);
}

int loadFile(char *filename, char **buffer)
{
	FILE *fp;
	int bufferSize;
	
	fp = fopen(filename, "rb");
	if (fp == NULL)
	{
		printf("Error opening file\n");
		return 1;
	}

	fseek(fp, 0, SEEK_END); // get file size
	bufferSize = ftell(fp);
	rewind(fp);

	*buffer = malloc(sizeof(char) * (bufferSize + 1));
	if (*buffer == NULL)
	{
		printf("Memory allocation failed\n");
		fclose(fp);
		return 1;
	}

	// read file into buffer
	int readSize = fread(*buffer, sizeof(char), bufferSize, fp);
	if (ferror(fp) != 0)
	{
		fputs("Error reading file", stderr);
		fclose(fp);
		free(*buffer);
		return 1;
	}
	
	(*buffer)[readSize] = '\0'; // null-terminate string
	return 0;
}

void readTilAfter(char **ptr, char *str, int strSize)
{
	if (*ptr == NULL)
	return;
	
	if (strSize == 2)
	*ptr = strchr(*ptr, str[0]);
	else
	*ptr = strstr(*ptr, str);
	
	if (*ptr != NULL)
	*ptr += strSize-1;
}

int getEdgeCnt(char *ptr)
{
	char *end;
	int edgeCnt = 0;
	
	while (ptr != NULL)
	{
		readTilAfter(&ptr, "<path", sizeof("<path"));
		readTilAfter(&ptr, "d=\"", sizeof("d=\""));
		
		end = ptr;
		readTilAfter(&end, "\"", sizeof("\""));
		
		while ((ptr != NULL) && (ptr < end))
		{
			readTilAfter(&ptr, "M", sizeof("M"));
			
			if (ptr != NULL)
			edgeCnt++;
		}
	}
	
	return edgeCnt;
}

int stringToHex(char *str, int *hex)
{
	char *endptr;
	*hex = strtol(str, &endptr, 16);
	
	if (str == endptr)
	{
		printf("Hex string conversion failed\n");
		return 1;
	}
	
	return 0;
}

int stringToFloat(char *str, float *value)
{
	char *endptr;
	*value = strtof(str, &endptr);
	
	if (str == endptr)
	{
		printf("Float string conversion failed\n");
		return 1;
	}
	
	return 0;
}

int getEdges(char *ptr, float *edges, int *color, int flip_Y)
{
	char *end;
	int iEdge = 0;
	char *colorPtr, *colorEnd;
	int iColor = 0;
	
	while (ptr != NULL)
	{
		readTilAfter(&ptr, "<path", sizeof("<path"));
		readTilAfter(&ptr, "d", sizeof("d"));
		readTilAfter(&ptr, "\"", sizeof("\""));
		
		end = ptr;
		readTilAfter(&end, "\"", sizeof("\""));
		
		while ((ptr != NULL) && (ptr < end))
		{
			readTilAfter(&ptr, "M", sizeof("M"));
			
			if (ptr != NULL)
			{
				if (stringToFloat(ptr, &edges[iEdge]))
				return 1;
				iEdge++;
				
				readTilAfter(&ptr, " ", sizeof(" "));
				if (stringToFloat(ptr, &edges[iEdge]))
				return 1;
				iEdge++;
				
				readTilAfter(&ptr, "L", sizeof("L"));
				if (stringToFloat(ptr, &edges[iEdge]))
				return 1;
				iEdge++;
				
				readTilAfter(&ptr, " ", sizeof(" "));
				if (stringToFloat(ptr, &edges[iEdge]))
				return 1;
				iEdge++;
				
				if (flip_Y)
				{
					edges[iEdge-1] = -edges[iEdge-1];
					edges[iEdge-3] = -edges[iEdge-3];
				}
				
				colorPtr = ptr;
				colorEnd = ptr;
				readTilAfter(&colorPtr, "stroke", sizeof("stroke"));
				readTilAfter(&colorPtr, "#", sizeof("#"));
				readTilAfter(&colorEnd, "/>", sizeof("/>"));
				
				if (colorPtr < colorEnd)
				{
					if (stringToHex(colorPtr, &color[iColor]))
					return 1;
				}
				else
				color[iColor] = 0;
				
				iColor++;
			}
		}
	}
	
	return 0;
}

int loadSvgData(char *filename, float **edges, int **color, int *edgeCnt, int flip_Y)
{
	char *buffer;
	
	if (loadFile(filename, &buffer))
	return 1;
	
	*edgeCnt = getEdgeCnt(buffer);
	
	*edges = malloc(*edgeCnt * 4 * sizeof(float));
	if (*edges == NULL)
	{
		printf("Memory allocation failed\n");
		freeFileBuffer(buffer);
		return 1;
	}
	*color = malloc(*edgeCnt * sizeof(int));
	if (*edges == NULL)
	{
		printf("Memory allocation failed\n");
		freeFileBuffer(buffer);
		return 1;
	}
	
	if (getEdges(buffer, *edges, *color, flip_Y))
	return 1;
	
	freeFileBuffer(buffer);
	return 0;
}

void freeSvgData(float *edges, int *color)
{
	if (edges != NULL)
	free(edges);
	
	if (color != NULL)
	free(color);
}
