# EJ_02_06
Draw a hexagon at the center of the screen, but programatically, using polar coordinates.

**Reviewed in this exercise**: explore other drawing modes

**Notes**: I had difficulty calculating the indices procedurally, so instead of drawing with element buffer object and GL_TRIANGLES, I did it with no EBO and with GL_TRIANGLE_FAN instead, 6 unique vertices passed in. I would like to revisit this challenge when I have more time, and try drawing from indexes.

Progress tracked in issue #11.


![Demo](demo.png "DEMO EJ_02_06")
