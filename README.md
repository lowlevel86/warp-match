# warp-match
This project is about warping one vector graphic image to another. The image matching program will be multi-threaded and written in C.
___
svgGridGen.py is a program that can generate grid patterns. It can be used to create test images.

![](./imgs/grid1.png)

![](./imgs/grid2.png)

![](./imgs/grid3.png)
___

Note that it is possible to animate from one seed to another by using a floating point value for the seed parameter.

![](./imgs/animate.gif)
___

Code and small programs that help explain how the main image matching program operates will be in the Supporting_Programs folder.

glDisplay contains code that can plot or draw line data to help visualize the inner workings and results of functions.
![](./imgs/glDisplay.png)
