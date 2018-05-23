import { glm } from "./util/glm"
import { Renderer, gl, gl2 } from "./renderer/renderer";
import { Texture2D } from "./gl/texture";
import { DeferImage } from "./techniques/deferImage";
import { Offscreen, RenderBuffer } from "./techniques/offscreen";
import { UV } from "./techniques/uv";
import { Communicators, Communicator } from "./gl/communicator";
import { SquareMesh } from "./object/squareMesh";
import { PerspectiveViewport } from "./gl/viewport";
import { Noise } from "./techniques/noise";
import { Caustics } from "./techniques/caustics";
import { Pool } from "./techniques/pool";
import { NormalHeight } from "./techniques/normalHeight";
import { PostProcess } from "./techniques/postProcess";
import { Shader } from "./gl/shader";
import { TransformFeedback } from "./techniques/transformFeedback";
import { TBO } from "./gl/buffer";
import { TAO, TransformAttrs } from "./gl/vertexAttrs";
import { FFTWave } from "./techniques/FFTWave";
import { Phillips } from "./techniques/phillips";
import { DecodeImage } from "./techniques/decodeImage";
import { EncodeImage } from "./techniques/encodeImage";

let renderer = new Renderer(document.body);

let mainImage = new Texture2D({ type: gl.UNSIGNED_BYTE });

let defer = new DeferImage();

let offscreen = new Offscreen();
let transformFeedback = new TransformFeedback();

let com = new Communicators();
// com.add(new Global());
com.use();

let camera = new PerspectiveViewport();
camera.position = glm.vec3(3, 0, 1.4)["*"](.1);
camera.target = glm.vec3(-3, 0, -1.5);

let vs = new TransformAttrs([{
	name: "Position",
	type: gl.FLOAT,
	size: 3
}, {
	name: "Lifetime",
	type: gl.FLOAT,
	size: 1
}]);

function genetateTrack(v: any, p: any, num: number, maxt: number): { [key: string]: number[] } {
	let vec = {
		Position: [],
		Lifetime: []
	};
	let dv = v["*"](maxt / num);
	for (let i = 0; i != num; ++i) {
		let p1 = p["-"](dv["*"](i));
		vec.Position = vec.Position.concat([p1.x, p1.y, 0]);
		vec.Lifetime = vec.Lifetime.concat([maxt * i / num]);
	}
	return vec;
}

let gDt = Shader.uniform("float", "gDt");
let gMaxLifetime = Shader.uniform("float", "gMaxLifetime");
let gVelocity = Shader.uniform("vec2", "gVelocity");
let gPosition = Shader.uniform("vec2", "gPosition");

let v = glm.vec2(0, .1);
let p = glm.vec2(0, 0);
let rot = 0.;
let t = 4.;
const omega = 3.;

let track = genetateTrack(v, p, 100, t);

console.log(track);

vs.set("Position", track.Position);
vs.set("Lifetime", track.Lifetime);

let tao = new TAO(vs);

let ts = Shader.create("playground/transformParticle", false, 
	["Position", "Lifetime"]);

gl.viewport(0, 0, Renderer.instance.canvas.width, Renderer.instance.canvas.height);

renderer.dispatch("keydown", (e: KeyboardEvent) => {
	switch (e.key.toLowerCase()) {
		case "a": {
			rot = 1.;
		} break;
		case "d": {
			rot = -1.;
		} break;
	}
});

renderer.dispatch("keyup", (e: KeyboardEvent) => {
	switch (e.key.toLowerCase()) {
		case "a": case "d": {
			rot = 0.;
		} break;
	}
	console.log(v);
});

renderer.dispatch("render", () => {
	let cosv = Math.cos(omega * rot * Renderer.dt);
	let sinv = Math.sin(omega * rot * Renderer.dt);
	let t1 = glm.vec2(cosv, -sinv);
	let t2 = glm.vec2(sinv, cosv);
	// let t = glm.mat2(
	// 	cosv, sinv, 
	// 	-sinv, cosv
	// );

	v = glm.vec2(glm.dot(t1, v), glm.dot(t2, v));
	p["+="](v["*"](Renderer.dt));

	gl.clear(gl.COLOR_BUFFER_BIT);
	transformFeedback.bind();
		ts.use();
		gDt.set(Renderer.dt);
		gMaxLifetime.set(t);
		gVelocity.set(v);
		gPosition.set(p);
		tao.bind();
			tao.draw();
		tao.unbind();
		ts.unuse();
	transformFeedback.unbind();

	tao.swap();
});

renderer.start();
