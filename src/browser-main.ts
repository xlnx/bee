import { glm } from "./util/glm"
import { gl, Renderer } from "./renderer/renderer"
import { ulist_elem, ulist } from "./util/ulist"
import Scene from "./scene/scene"
import Obj from "./scene/object"
import { PerspectiveViewport, Viewport } from "./scene/viewport"
import { Shader, Uniform } from "./gl/shader";
import { VertexAttrs, VAO } from "./gl/vertexAttrs";

const canvas: HTMLCanvasElement = <HTMLCanvasElement>document.getElementById("wgl-canvas");

let renderer = new Renderer(canvas);

// let scene = new Scene();

// scene.createViewport(new PerspectiveViewport(0, 0, 512, 512));

let shader = Shader.create("test");

let view = new PerspectiveViewport(0, 0, 300, 150);

view.position = glm.vec3(0, 0, 2);
view.target = glm.vec3(0.3, 0, -1);
view.up = glm.vec3(0, 1, 0);

let WVP = Shader.uniform("mat4", "gWVP");

shader.use();

console.log(view.getTrans());
WVP.set(view.getTrans());

gl.clearColor(1, 1, 1, 1);

let vertices = new VertexAttrs(["pos2"]);

let vs = [
	-1, 0.8094,
	-1, 0.5027,
	1, 0.8094,
	-1, 0.5027,
	1, 0.5027,
	1, 0.8094,
	-1, -0.8094,
	-1, -0.5027,
	1, -0.8094,
	-1, -0.5027,
	1, -0.5027,
	1, -0.8094,
	-0.2717, 0.2114,
	0.2717, 0.2114,
	0.1857, 0.1479,
	-0.1857, 0.1479,
	0.1857, 0.1479,
	-0.2717, 0.2114,
	0, -0.42,
	-0.1857, 0.1479,
	-0.2717, 0.2114,
	0, -0.2968,
	0, -0.42,
	-0.1857, 0.1479,
	0, -0.42,
	0.1857, 0.1479,
	0.2717, 0.2114,
	0, -0.2968,
	0, -0.42,
	0.1857, 0.1479,
	-0.2717, -0.2114,
	0.2717, -0.2114,
	0.1857, -0.1479,
	-0.1857, -0.1479,
	0.1857, -0.1479,
	-0.2717, -0.2114,
	0, 0.42,
	-0.1857, -0.1479,
	-0.2717, -0.2114,
	0, 0.2968,
	0, 0.42,
	-0.1857, -0.1479,
	0, 0.42,
	0.1857, -0.1479,
	0.2717, -0.2114,
	0, 0.2968,
	0, 0.42,
	0.1857, -0.1479
];

for (let i = 0; i != vs.length; i += 2) {
	vertices.push({
		pos2: [vs[i], vs[i + 1]]
	})
}

let vao = new VAO(vertices);

renderer.dispatch("render", () => {
	gl.clear(gl.COLOR_BUFFER_BIT)
	
	// gl.enableVertexAttribArray(0)
	// gl.vertexAttribPointer(0, 2, gl.FLOAT, false, 0, 0)
	// 	gl.drawArrays(gl.TRIANGLES, 0, 48)
	vao.bind();
	vao.draw(gl.TRIANGLES, 0, 48);
	vao.unbind();
	// scene.renderPass();
})

renderer.render();
