import { glm } from "./util/glm"
import { gl, Renderer } from "./renderer/renderer"
import { ulist_elem, ulist } from "./util/ulist"
import Scene from "./scene/scene"
import Obj from "./scene/object"
import { PerspectiveViewport, Viewport } from "./scene/viewport"
import { Shader, Uniform } from "./gl/shader";
import { VertexAttrs, VAO } from "./gl/vertexAttrs";
import Model from "./gl/model";
import ModelObj from "./object/modelObject";
import Mesh from "./gl/mesh";

let renderer = new Renderer(document.body);

// let scene = new Scene();

// scene.createViewport(new PerspectiveViewport(0, 0, 512, 512));

let shader = Shader.create("test");

let view = new PerspectiveViewport(0, 0, renderer.canvas.width, renderer.canvas.height);

view.position = glm.vec3(1, -0.5, 1.2);
view.target = glm.vec3(-1, 0.5, -0.8);

let WVP = Shader.uniform("mat4", "gWVP");

gl.clearColor(1, 1, 1, 1);

let model = new ModelObj(Model.create("black-bishop.json"));
model.scale(0.01);

gl.enable(gl.DEPTH_TEST);

renderer.dispatch("render", () => {
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
	
	model.render(view, shader);
	// scene.renderPass();
})

renderer.render();
