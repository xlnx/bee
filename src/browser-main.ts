import { glm } from "./util/glm"
import { gl, Renderer } from "./renderer/renderer"
import Scene from "./scene/scene"
import { PerspectiveViewport } from "./scene/viewport"
import Model from "./gl/model"
import ModelObj from "./object/modelObject"
import { PointLight } from "./techniques/lighting";
import { ShadowTechnique } from "./techniques/shadow";
import { Ocean } from "./techniques/ocean";
import { Shader } from "./gl/shader";
import { GerstnerWave } from "./techniques/gerstnerWave";

let renderer = new Renderer(document.body);

let scene = new Scene();

let camera = scene.createViewport<PerspectiveViewport>(new PerspectiveViewport()).get();
camera.position = glm.vec3(4, 0, 1.2);
camera.target = glm.vec3(-4, 0, -0.8);

let light = scene.createCommunicator<PointLight>(new PointLight()).get();
light.set("position", glm.vec3(0, 0, 2));
light.set("diffuseIntensity", 5);
light.set("ambientIntensity", 0.7);

let shadow = scene.createTechnique<ShadowTechnique>(new ShadowTechnique()).get();
shadow.setLight(light);

gl.clearColor(0, 0, 0, 0);
gl.clearDepth(1);

let model = scene.createObject<ModelObj>(new ModelObj(
		Model.create("clemson.json"))).get();
		// Model.create("black-bishop.json"))).get();
model.scale(0.5);
model.translate(glm.vec3(0, 0, 0.8));

let ocean = scene.createObject<Ocean>(new Ocean()).get();
let wave = new GerstnerWave();
ocean.add(wave);

gl.enable(gl.DEPTH_TEST);

let forward = false, backward = false, leftward = false, rightward = false;

renderer.dispatch("keydown", (e: KeyboardEvent) => {
	if (!e.altKey && !e.shiftKey && !e.ctrlKey && !e.metaKey) {
		switch (e.key.toLowerCase()) {
			case "w": forward = true; break;
			case "a": leftward = true; break;
			case "s": backward = true; break;
			case "d": rightward = true; break;
		}
	}
	// console.log(e);
});

renderer.dispatch("keyup", (e: KeyboardEvent) => {
	// if (!e.altKey && !e.shiftKey && !e.ctrlKey && !e.metaKey) {
	switch (e.key.toLowerCase()) {
		case "w": forward = false; break;
		case "a": leftward = false; break;
		case "s": backward = false; break;
		case "d": rightward = false; break;
	}
	// }
	// console.log(e);
});

let prevx = 0, prevy = 0, isdragging = false;

renderer.dispatch("mousedown", (e: MouseEvent) => {
	console.log("down");
	isdragging = true;
	prevx = e.clientX; prevy = e.clientY;
});

renderer.dispatch("mousemove", (e: MouseEvent) => {
	if (isdragging) {
		let c1 = Renderer.instance.canvas.height / Math.tan(camera.fov/2);
		let c2 = Renderer.instance.canvas.width;
		let h = 0.5 * Math.sqrt(c1 * c1 - c2 * c2);
		let dx1 = e.clientX - Renderer.instance.canvas.width / 2;
		let dx2 = prevx - Renderer.instance.canvas.width / 2;
		let dy1 = e.clientY - Renderer.instance.canvas.height / 2;
		let dy2 = prevy - Renderer.instance.canvas.height / 2;
		let thetax = Math.atan(dx1/h) - Math.atan(dx2/h);
		let thetay = Math.atan(dy1/h) - Math.atan(dy2/h);
		// console.log(h, dx1, dx2, c1, c2, theta);
		camera.rotate(glm.vec3(0, thetay, thetax));
		prevx = e.clientX; prevy = e.clientY;
	}
});

renderer.dispatch("mouseup", (e: MouseEvent) => {
	console.log("up");
	isdragging = false;
});

let velocity = 3, prevTime = 0;

renderer.dispatch("render", (time: number) => {
	let f = - - forward + - backward;
	let l = - - leftward + - rightward;
	camera.translate(glm.vec3(f, l, 0)["*"]((time - prevTime) / 1000.0 * velocity));
	prevTime = time;
	scene.render();
});

renderer.start();
