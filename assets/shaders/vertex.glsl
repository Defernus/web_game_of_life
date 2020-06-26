attribute vec2 aPosition;

varying vec2 uv;

void main() {
	gl_Position = vec4(aPosition.xy, 0., 1.);
	uv = aPosition*.5+.5;
}
