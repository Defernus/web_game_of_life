precision mediump float;

uniform sampler2D grid;

varying vec2 uv;

void main()
{
	vec4 cell = texture2D(grid, uv);
	if(cell.x > .5) {
		gl_FragColor = vec4(1.);
	} else {
		gl_FragColor = vec4(0., cell.yy, 1.);
	}
}
