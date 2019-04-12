attribute vec2 position;
attribute float alpha;
attribute float size;

uniform mat4 mvp_matrix;

varying vec4 vColor;

void main(void)
{
	vColor = vec4(1.0, 1.0, 1.0, alpha);
	gl_PointSize = size;
	gl_Position = mvp_matrix * vec4(position, 0.0, 1.0);
}
