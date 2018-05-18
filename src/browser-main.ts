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
import { FFT } from "./techniques/FFT";
import { Phillips } from "./techniques/phillips";
import { DecodeImage } from "./techniques/decodeImage";

let renderer = new Renderer(document.body);

let mainImage = new Texture2D({ type: gl.UNSIGNED_BYTE });
let noiseImage = new Texture2D({ component: gl2.RED, type: gl.FLOAT });
let causticsImage = new Texture2D({ component: gl2.RED, type: gl.FLOAT });
let normalHeightImage = new Texture2D({ component: gl2.RGBA, type: gl.FLOAT });

let defer = new DeferImage();
let uv = new UV();
let noise = new Noise();
let causticsRenderer = new Caustics();
let normalHeight = new NormalHeight();

let offscreen = new Offscreen();
// let transformFeedback = new TransformFeedback();
offscreen.bind();
	offscreen.set(gl.DEPTH_ATTACHMENT, new RenderBuffer(gl.DEPTH_COMPONENT16, 512, 512));
offscreen.unbind();

// class Global extends Communicator {
// 	constructor() {
// 		super("Global", false);
// 		this.init({
// 			Light: {
// 				type: "vec3",
// 				init: glm.vec3(0.5, 0.5, -1)
// 			}
// 		});
// 	}
// }

let com = new Communicators();
// com.add(new Global());
com.use();

// let test = new PostProcess(Shader.create("playground/testMRT", false));

let water = new SquareMesh("playground/water", 50);
// let pool = new Pool();

// let tiles = new Texture2D("./assets/tiles.jpg");

// let camera = new PerspectiveViewport();
// camera.position = glm.vec3(3, 0, 1.);
// camera.target = glm.vec3(-3, 0, -1.5);

// let vs = new TransformAttrs([{
// 	name: "Position",
// 	type: gl.FLOAT,
// 	size: 3
// }, {
// 	name: "Color",
// 	type: gl.FLOAT,
// 	size: 3
// }]);
// vs.set("Position", [
// 	0, 0, 0
// ]);
// vs.set("Color", [
// 	1, 1, 0
// ]);
// let tao = new TAO(vs);

// let ts = Shader.create("playground/transformParticle", false, ["Position", "Color"]);


let gaussianImage = new Texture2D("./assets/gaussian.jpg");
let phillipsImage = new Texture2D({ component: gl2.RG, type: gl.FLOAT }, 512, 512);
let hImage = new Texture2D({ component: gl2.RG, type: gl.FLOAT }, 512, 512);
let dxImage = new Texture2D({ component: gl2.RG, type: gl.FLOAT }, 512, 512);
let dyImage = new Texture2D({ component: gl2.RG, type: gl.FLOAT }, 512, 512);

let phillips = new Phillips();
let fft = new FFT();

let decode = new DecodeImage();

gl.disable(gl.DEPTH_TEST);

renderer.dispatch("render", () => {
	gl.viewport(0, 0, 512, 512);
	offscreen.bind();
		offscreen.set(gl.COLOR_ATTACHMENT0, phillipsImage);
		gaussianImage.use("Gaussian");
			phillips.render();
		gaussianImage.unuse();

		offscreen.set(gl.COLOR_ATTACHMENT0, hImage);
		offscreen.set(gl2.COLOR_ATTACHMENT1, dxImage);
		offscreen.set(gl2.COLOR_ATTACHMENT2, dyImage);
		gl2.drawBuffers([
			gl.COLOR_ATTACHMENT0, 
			gl2.COLOR_ATTACHMENT1,
			gl2.COLOR_ATTACHMENT2
		]);
		phillipsImage.use("Spectrum");
			fft.render();
		phillipsImage.unuse();
		gl2.drawBuffers([
			gl.COLOR_ATTACHMENT0
		]);

	offscreen.unbind();
	gl.viewport(0, 0, Renderer.instance.canvas.width, Renderer.instance.canvas.height);

	// phillipsImage.use("Image");
	hImage.use("Image");
		// defer.render();
		decode.render();
	hImage.unuse();
	// phillipsImage.unuse();
});

renderer.start();

// function test_transform_feedback() {
// 	gl.clear(gl.COLOR_BUFFER_BIT);
// 	// offscreen.bind();
// 	// offscreen.unbind();
// 	transformFeedback.bind();
// 		ts.use();
// 		tao.bind();
// 			tao.draw();
// 		tao.unbind();
// 		ts.unuse();
// 	transformFeedback.unbind();

// 	tao.swap();

// 	// gl.disable(gl.DEPTH_TEST);

// 	// // mainImage.use("Image");
// 	// // causticsImage.use("Image");
// 	// normalHeightImage.use("Image");
// 	// // noiseImage.use("Image");
// 	// 	defer.render();
// 	// // noiseImage.unuse();
// 	// normalHeightImage.unuse();
// 	// // causticsImage.unuse();
// 	// // mainImage.unuse();

// 	// gl.enable(gl.DEPTH_TEST);
// }

// function render_old() {
// 	offscreen.bind();
// 		gl.disable(gl.DEPTH_TEST);

// 		// offscreen.set(gl.COLOR_ATTACHMENT0, noiseImage);
// 		// noise.render();

// 		offscreen.set(gl.COLOR_ATTACHMENT0, normalHeightImage);
// 		// noiseImage.use("Noise");
// 		normalHeight.render();
// 		// noiseImage.unuse();

// 		offscreen.set(gl.COLOR_ATTACHMENT0, causticsImage);
// 		gl.clear(gl.COLOR_BUFFER_BIT);
// 		normalHeightImage.use("NormalHeight");
// 			causticsRenderer.render(water);
// 		normalHeightImage.unuse();

// 		gl.enable(gl.DEPTH_TEST);

// 		offscreen.set(gl.COLOR_ATTACHMENT0, mainImage);
// 		gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

// 		normalHeightImage.use("NormalHeight");
// 		causticsImage.use("Caustics");

// 		water.bindShader();
// 			water.render(camera);
// 		water.unbindShader();

// 		tiles.use("Tile");
// 		pool.bindShader();
// 			pool.render(camera);
// 		pool.unbindShader();
// 		tiles.unuse();

// 		causticsImage.unuse();
// 		normalHeightImage.unuse();

// 		// gl.disable(gl.DEPTH_TEST);
// 		// 	offscreen.set(gl2.COLOR_ATTACHMENT0, mainImage);
// 		// 	offscreen.set(gl2.COLOR_ATTACHMENT1, causticsImage);
// 		// 	gl2.drawBuffers([gl.COLOR_ATTACHMENT0, gl2.COLOR_ATTACHMENT1]);
// 		// 	test.render();
// 		// 	gl2.drawBuffers([gl.COLOR_ATTACHMENT0]);
// 		// gl.enable(gl.DEPTH_TEST);
// 	offscreen.unbind();

// 	gl.disable(gl.DEPTH_TEST);

// 	mainImage.use("Image");
// 	// causticsImage.use("Image");
// 	// normalHeightImage.use("Image");
// 	// noiseImage.use("Image");
// 		defer.render();
// 	// noiseImage.unuse();
// 	// normalHeightImage.unuse();
// 	// causticsImage.unuse();
// 	mainImage.unuse();

// 	gl.enable(gl.DEPTH_TEST);
// }