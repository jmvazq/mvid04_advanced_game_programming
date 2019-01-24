# EJ_03_01
The goal was to draw a triangle from the previous activities, upside down, but doing so by modifying only the vertex shader.

**Introduced in this activity**: Review of vertex position attribute, vertex shaders.

**Notes**:

This was a tricky one. Inverting the geometry in any axis was easy enough. I just had to multiply it by -1. But if I only inverted one axis I would have to either change the order of the indices or disable back face culling. But the exercise required to only change the vertex shader, so, what was there to do?

In the end, I inverted the triangle from AG_03 first, in x and y, which didn't look exactly upside down. So, I went back and used the very first triangle we made in the course: the one with the top at 0 in the x-axis.

Below are the screenshots for the results with both triangles:

Progress tracked in issue #13.


![Demo](demo1.png "DEMO EJ_03_01 (1)")

![Demo](demo2.png "DEMO EJ_03_01 (2)")
