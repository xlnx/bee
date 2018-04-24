import { glm } from "./util/glm"
import { gl, Renderer } from "./renderer/renderer"
import Scene from "./scene/scene"
import { PerspectiveViewport } from "./scene/viewport"
import Model from "./gl/model"
import ModelObj from "./object/modelObject"
import { PointLight } from "./lighting/lighting";

let renderer = new Renderer(document.body);

let scene = new Scene();

let camera = scene.createViewport<PerspectiveViewport>(new PerspectiveViewport(
		0, 0, renderer.canvas.width, renderer.canvas.height)).get();

let light = scene.createCommunicator<PointLight>(new PointLight()).get();
light.set("position", glm.vec3(0, 0, 2));
light.set("diffuseIntensity", 5);
light.set("ambientIntensity", 0.7);

camera.position = glm.vec3(1, -0.5, 1.2);
camera.target = glm.vec3(-1, 0.5, -0.8);

gl.clearColor(0, 0, 0, 0);

let model = scene.createObject<ModelObj>(new ModelObj(
		Model.create("black-bishop.json"))).get();
model.scale(0.01);

gl.enable(gl.DEPTH_TEST);

renderer.dispatch("render", () => {
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
	
	scene.renderPass();
})

renderer.render();
