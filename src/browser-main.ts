import { glm } from "./util/glm"
import { gl, Renderer } from "./renderer/renderer"
import Scene from "./scene/scene"
import { PerspectiveViewport } from "./scene/viewport"
import Model from "./gl/model"
import ModelObj from "./object/modelObject"
import { PointLight } from "./techniques/lighting";
import { ShadowTechnique } from "./techniques/shadow";

let renderer = new Renderer(document.body);

let scene = new Scene();

let camera = scene.createViewport<PerspectiveViewport>(new PerspectiveViewport()).get();
camera.position = glm.vec3(3, 0, 1.2);
camera.target = glm.vec3(-3, 0, -0.8);

let light = scene.createCommunicator<PointLight>(new PointLight()).get();
light.set("position", glm.vec3(0, 0, 2));
light.set("diffuseIntensity", 5);
light.set("ambientIntensity", 0.7);

let shadow = scene.createTechnique<ShadowTechnique>(new ShadowTechnique());

gl.clearColor(0, 0, 0, 0);
gl.clearDepth(1);

let model = scene.createObject<ModelObj>(new ModelObj(
		Model.create("clemson.json"))).get();
		// Model.create("black-bishop.json"))).get();
model.scale(0.5);

gl.enable(gl.DEPTH_TEST);

renderer.dispatch("render", () => {
	scene.render();
})

renderer.start();
