#version 110

uniform sampler2D texture;

uniform vec3 innerColor;
uniform vec3 outerColor;
uniform float colorRadius;

float hsl_value(float n1, float n2, float h)
{
	if(h < 0.0)
		h += 6.0;
	else if(h > 6.0)
		h -= 6.0;

	if(h < 1.0)
		return (n2 - n1) * h + n1;
	if(h < 3.0)
		return n2;
	if(h < 4.0)
		return (n2 - n1) * (4.0 - h) + n1;
	return n1;
}

vec3 hsl_to_rgb(vec3 hsl)
{
	if(hsl.g == 0.0)
		return vec3(hsl.b, hsl.b, hsl.b);
	else
	{
		float m2;

		if(hsl.b <= 0.5)
			m2 = hsl.b * (1.0 + hsl.g);
		else
			m2 = hsl.b + hsl.g - hsl.b * hsl.g;

		float m1 = 2.0 * hsl.b - m2;

		float r = hsl_value(m1, m2, 6.0 * hsl.r + 2.0);
		float g = hsl_value(m1, m2, hsl.r * 6.0);
		float b = hsl_value(m1, m2, 6.0 * hsl.r - 2.0);

		return vec3(r, g, b);
	}
}

void main(void)
{
	/* Get lightness from texture */
	vec4 texColor = texture2D(texture, gl_TexCoord[0].xy);

	float a = max(texColor.r, max(texColor.g, texColor.b));
	float b = min(texColor.r, min(texColor.g, texColor.b));

	float l = (a + b) / 2.0;

	/* Convert HSL to RGB */
	vec3 iColor = hsl_to_rgb( vec3(innerColor.r, innerColor.g, l) );
	vec3 oColor = hsl_to_rgb( vec3(outerColor.r, outerColor.g, l) );

	/* Mix between inner and outer color */
	float radius = 2.0 * distance(gl_TexCoord[0].xy, vec2(0.5, 0.5)) * colorRadius;
	gl_FragColor = vec4(mix(iColor, oColor, radius), 1.0);
}

