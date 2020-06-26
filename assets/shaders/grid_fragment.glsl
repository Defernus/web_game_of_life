precision mediump float;

uniform sampler2D grid;
uniform vec2 offset;

varying vec2 uv;

void main()
{
	int nc = 0;

	if(texture2D(grid, uv + offset*vec2(-1., -1.)).x > .5) {
		++nc;
	}
	if(texture2D(grid, uv + offset*vec2(-1., 0.)).x > .5) {
		++nc;
	}
	if(texture2D(grid, uv + offset*vec2(-1., 1.)).x > .5) {
		++nc;
	}
	if(texture2D(grid, uv + offset*vec2(0., -1.)).x > .5) {
		++nc;
	}
	if(texture2D(grid, uv + offset*vec2(0., 1.)).x > .5) {
		++nc;
	}
	if(texture2D(grid, uv + offset*vec2(1., -1.)).x > .5) {
		++nc;
	}
	if(texture2D(grid, uv + offset*vec2(1., 0.)).x > .5) {
		++nc;
	}
	if(texture2D(grid, uv + offset*vec2(1., 1.)).x > .5) {
		++nc;
	}
	
	vec4 this_cell = texture2D(grid, uv);
	if(nc == 3 || (this_cell.x>.5 && nc==2 )) {
		float g = this_cell.y+.25;
		gl_FragColor = vec4(1., min(g, 1.), 0., 1.);
		return;
	}

	float g = this_cell.y-.005;
	gl_FragColor = vec4(0., max(g, 0.), 0., 1.);
}
