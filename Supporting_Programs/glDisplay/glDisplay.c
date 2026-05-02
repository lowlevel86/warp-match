#include <stdlib.h>
#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define TRUE 1
#define FALSE 0

int WINDOW_WIDTH = 720, WINDOW_HEIGHT = 480;
double PLOT_WIDTH = 720, PLOT_HEIGHT = 480;
double PLOT_LOC_X = 0.0, PLOT_LOC_Y = 0.0;
double PLOT_ZOOM = 1.0;
double BG_R = 1.0, BG_G = 1.0, BG_B = 1.0; // background color

struct EdgeDataNode {
	float *edges;
	int edgeCnt;
	int memSize;
	float color[4];
	float thickness;
	void *next;
};
struct EdgeDataNode *nodeHead = NULL;
struct EdgeDataNode *nodeTail = NULL;
int EDGES_MEM_SIZE = 0;

void clearPlotData()
{
	struct EdgeDataNode *currNode = nodeHead;
	struct EdgeDataNode *lastNode;
	
	while (currNode != NULL)
	{
		free(currNode->edges);
		lastNode = currNode;
		currNode = currNode->next;
		free(lastNode);
	}
	
	nodeHead = NULL;
	nodeTail = NULL;
	EDGES_MEM_SIZE = 0;
}

void setPlotData(float *edges, int edgeCnt, float r, float g, float b, float a, float thickness)
{
	int i;
	struct EdgeDataNode *currNode;
	
	currNode = malloc(sizeof(struct EdgeDataNode));
	
	if (nodeHead == NULL)
	nodeHead = currNode;
	else
	nodeTail->next = currNode;
	
	currNode->next = NULL;
	nodeTail = currNode;

	// edgeCnt * 2 points per edge * 3 coordinates per point * 4 bytes per float
	currNode->memSize = edgeCnt * 2 * 3 * sizeof(float);
	
	// + edgeCnt * 2 points per edge * 4 channels per color * 4 bytes per float
	currNode->memSize += edgeCnt * 2 * 4 * sizeof(float);
	
	currNode->edges = malloc(currNode->memSize);
	
	EDGES_MEM_SIZE += currNode->memSize;
	
	int j = 0;
	for (i=0; i < edgeCnt*2*7; i+=7) //7 floats per 3D point + rgba
	{
		// edge point
		currNode->edges[i+0] = edges[j+0];
		currNode->edges[i+1] = edges[j+1];
		currNode->edges[i+2] = 0.0;
		
		// color
		currNode->edges[i+3] = r;
		currNode->edges[i+4] = g;
		currNode->edges[i+5] = b;
		currNode->edges[i+6] = a;
		
		j += 2; //2 floats per 2D point
	}
	
	currNode->edgeCnt = edgeCnt;
	currNode->thickness = thickness;
}

void setWindowDimensions(int width, int height)
{
	WINDOW_WIDTH = width;
	WINDOW_HEIGHT = height;
}

void setPlotDimensions(double width, double height)
{
	PLOT_WIDTH = width;
	PLOT_HEIGHT = height;
}

void setPlotZoom(double zoom)
{
	PLOT_LOC_X += PLOT_LOC_X / PLOT_ZOOM * (zoom - 1.0);
	PLOT_LOC_Y += PLOT_LOC_Y / PLOT_ZOOM * (zoom - 1.0);
	PLOT_ZOOM = zoom;
}

void setPlotCenter(double x, double y)
{
	if (WINDOW_WIDTH > WINDOW_HEIGHT)
	{
		PLOT_LOC_X = -x * WINDOW_HEIGHT / PLOT_WIDTH;
		PLOT_LOC_Y = -y * WINDOW_HEIGHT / PLOT_HEIGHT;
	}
	else
	{
		PLOT_LOC_X = -x * WINDOW_WIDTH / PLOT_WIDTH;
		PLOT_LOC_Y = -y * WINDOW_WIDTH / PLOT_HEIGHT;
	}
}

void setBgColor(double r, double g, double b)
{
	BG_R = r;
	BG_G = g;
	BG_B = b;
}

int isDragging = FALSE;
double lastMouseX, lastMouseY;
double mouseX = 0.0, mouseY = 0.0;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
        if (action == GLFW_PRESS)
		{
            isDragging = TRUE;
            glfwGetCursorPos(window, &lastMouseX, &lastMouseY); // get initial position
        }
		else if (action == GLFW_RELEASE)
		isDragging = FALSE;
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (isDragging)
	{
        double deltaX = xpos - lastMouseX;
        double deltaY = ypos - lastMouseY;
		
		PLOT_LOC_X += deltaX;
		PLOT_LOC_Y -= deltaY;
		
        lastMouseX = xpos;
        lastMouseY = ypos;
    }
	
	double x, y;
	char buffer[64];
	mouseX = xpos - WINDOW_WIDTH / 2; // the center is zero
	mouseY = ypos - WINDOW_HEIGHT / 2;
	
	x = (mouseX - PLOT_LOC_X) / PLOT_ZOOM; // apply plot location offset
	y = (mouseY + PLOT_LOC_Y) / PLOT_ZOOM;
	
	// display coordinates relative to window dimensions
	if (WINDOW_WIDTH > WINDOW_HEIGHT)
	snprintf(buffer, sizeof(buffer), "x:%f  y:%f", x / WINDOW_HEIGHT * PLOT_WIDTH, -y / WINDOW_HEIGHT * PLOT_HEIGHT);
	else
	snprintf(buffer, sizeof(buffer), "x:%f  y:%f", x / WINDOW_WIDTH * PLOT_WIDTH, -y / WINDOW_WIDTH * PLOT_HEIGHT);
	
	glfwSetWindowTitle(window, buffer);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	#define scroll_coe 0.025 // scroll coefficient

	PLOT_LOC_X -= mouseX; // normalize to mouse position
	PLOT_LOC_Y -= -mouseY;
	
	if (yoffset > 0) // scroll up
	{
		PLOT_LOC_X += PLOT_LOC_X / PLOT_ZOOM * scroll_coe;
		PLOT_LOC_Y += PLOT_LOC_Y / PLOT_ZOOM * scroll_coe;
		PLOT_ZOOM += scroll_coe;
	}
	else if (yoffset < 0) // scroll down
	{
		PLOT_LOC_X -= PLOT_LOC_X / PLOT_ZOOM * scroll_coe;
		PLOT_LOC_Y -= PLOT_LOC_Y / PLOT_ZOOM * scroll_coe;
		PLOT_ZOOM -= scroll_coe;
	}

	if (PLOT_ZOOM < 1.0) // never scale less than 1
	{
		PLOT_LOC_X += PLOT_LOC_X / PLOT_ZOOM * (1.0 - PLOT_ZOOM);
		PLOT_LOC_Y += PLOT_LOC_Y / PLOT_ZOOM * (1.0 - PLOT_ZOOM);
		PLOT_ZOOM = 1.0;
	}
	
	PLOT_LOC_X += mouseX; // undo normalization
	PLOT_LOC_Y += -mouseY;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
	WINDOW_WIDTH = width;
	WINDOW_HEIGHT = height;
}

void displayPlot()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 8);

    GLFWwindow* window;
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "x:  y:", NULL, NULL);
    if (!window)
	{
        fprintf(stderr, "%s\n", "ERROR: Failed creating GLFW window!");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
	{
        fprintf(stderr, "%s\n", "ERROR: Failed initialize GLAD!");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	
    // shaders
    const char* vShaderCode =
    "#version 410 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec4 aColor;\n"
	"uniform vec3 sz; // scale\n"
	"uniform vec3 loc; // location\n"
    "out vec4 bColor;\n"
    "void main() {\n"
    "    gl_Position = vec4(aPos.x*sz.x+loc.x, aPos.y*sz.y+loc.y, aPos.z, 0.5);\n"
    "    bColor = aColor;\n"
    "}\n";

    const char* fShaderCode =
    "#version 410 core\n"
    "out vec4 FragColor;\n"
    "in vec4 bColor;\n"
    "void main() {\n"
    "    FragColor = vec4(bColor);\n"
    "}\n";

    unsigned int fShader;
    fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &fShaderCode, NULL);
    glCompileShader(fShader);

    unsigned int vShader;
    vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vShaderCode, NULL);
    glCompileShader(vShader);

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, fShader);
    glAttachShader(shaderProgram, vShader);
    glLinkProgram(shaderProgram);

    unsigned int VBO;
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
	
	if (nodeHead != NULL) // if data, load data to GPU
	{
		int offset = 0;
		struct EdgeDataNode *currNode = nodeHead;
		glBufferData(GL_ARRAY_BUFFER, EDGES_MEM_SIZE, NULL, GL_STATIC_DRAW);
		
		while (currNode != NULL)
		{
			glBufferSubData(GL_ARRAY_BUFFER, offset, currNode->memSize, currNode->edges);
			offset += currNode->memSize;
			currNode = currNode->next;
		}
	}
	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

	GLint coors_scale = glGetUniformLocation(shaderProgram, "sz");
	GLint coors_loc = glGetUniformLocation(shaderProgram, "loc");

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glfwSwapInterval(2);
	//float lastFrame = 0.0;

	int prevKey = GLFW_RELEASE;
    while (!glfwWindowShouldClose(window))
    {
		// show FPS
		//float currentFrame = glfwGetTime();
		//float deltaTime = currentFrame - lastFrame;
		//lastFrame = currentFrame;
		//printf("FPS:%f\n", 1.0 / deltaTime);
		
		// exit on ESC key release
		glfwPollEvents();
		
		int curKey = glfwGetKey(window, GLFW_KEY_ESCAPE);
		if (prevKey == GLFW_PRESS && curKey == GLFW_RELEASE)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		
		prevKey = curKey;
		
		
        glClearColor(BG_R, BG_G, BG_B, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
		
        // draw edges
		int offset = 0;
		struct EdgeDataNode *currNode = nodeHead;
		
		while (currNode != NULL)
		{
			glLineWidth(currNode->thickness);
			glDrawArrays(GL_LINES, offset, currNode->edgeCnt*2);
			
			offset += currNode->edgeCnt * 2;
			currNode = currNode->next;
		}
		
		if (WINDOW_WIDTH > WINDOW_HEIGHT)
		glUniform3f(coors_scale, (double)WINDOW_HEIGHT/WINDOW_WIDTH/PLOT_WIDTH*PLOT_ZOOM, 1.0/PLOT_HEIGHT*PLOT_ZOOM, 1.0);
		else
		glUniform3f(coors_scale, 1.0/PLOT_WIDTH*PLOT_ZOOM, (double)WINDOW_WIDTH/WINDOW_HEIGHT/PLOT_HEIGHT*PLOT_ZOOM, 1.0);
		
		glUniform3f(coors_loc, PLOT_LOC_X/WINDOW_WIDTH, PLOT_LOC_Y/WINDOW_HEIGHT, 0.0);
		
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}