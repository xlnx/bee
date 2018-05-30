import { glm } from "../util/glm"
import { gl, Renderer, RendererEvent, gl2 } from "../renderer/renderer"
import { Offscreen, RenderBuffer } from "../gl/offscreen";
import { Texture2D } from "../gl/texture";
import { DeferImage } from "../techniques/deferImage";
import { UV } from "../techniques/uv";
import { PerlinWave } from "../techniques/perlinWave";
import { AmbientCube } from "../techniques/ambientCube";
import { TestScreen } from "../techniques/offscreenTest";
import { Skybox } from "../object/skybox";
import { Communicators } from "../gl/communicator";
import { GameRenderer } from "./render";
import { Vessel } from "./vessel/vessel";
import { Shader } from "../gl/shader";
import { CameraBase } from "./camera/cameraBase";
import { Ocean } from "../object/ocean";
import { ulist_elem, ulist } from "../util/ulist";
import { GerstnerWave } from "../object/gerstnerWave";
import { FFTWave } from "../techniques/FFTWave";
import { Phillips } from "../techniques/phillips";
import { DecodeImage } from "../techniques/decodeImage";
import { Normal } from "../techniques/normal"
import { TransformFeedback } from "../gl/transformFeedback";
import { Smoke } from "./vessel/smoke";
import { Gaussian } from "../techniques/gaussian";

class Engine3d {
	private worldCom = new Communicators();
	private renderCom = new Communicators();
	
	private mainImage = new Texture2D({ component: gl.RGBA, type: gl.FLOAT });
	private bumpImage = new Texture2D("./assets/bump.jpg");
	private normalTypeImage = new Texture2D({ component: gl.RGBA, type: gl.FLOAT });
	private extraImage = new Texture2D({ component: gl2.RGBA, type: gl.FLOAT });
	private uvImage = new Texture2D({ component: gl.RGB });
	private perlinImage = new Texture2D({ component: gl2.RGBA, type: gl.FLOAT, filter: gl.NEAREST, wrap: gl.REPEAT }, 256, 256);
	private phillipsImage = new Texture2D({ component: gl2.RG, type: gl.FLOAT, filter: gl.NEAREST, wrap: gl.CLAMP_TO_EDGE }, 256, 256);
	private normalJImage = new Texture2D({ component: gl2.RGBA, type: gl.FLOAT, filter: gl.LINEAR, wrap: gl.REPEAT }, 256, 256);
	private smokeBallImage = new Texture2D("./assets/smoke.png");
	private smokeImage = new Texture2D({ component: gl.RGBA, type: gl.UNSIGNED_BYTE });
	private gaussianImage = new Texture2D({ component: gl2.RGBA, type: gl.FLOAT, filter: gl.NEAREST, wrap: gl.REPEAT });

	private channel: Texture2D;

	private offscreen = new Offscreen();
	private suboffscreen = new Offscreen();
	private transformFeedback = new TransformFeedback();

	public readonly ambient = new AmbientCube();
	private main = new GameRenderer();

	private uv = new UV();
	private perlin = new PerlinWave();
	private defer = new DeferImage();
	private decode = new DecodeImage();

	private phillips = new Phillips();
	private fftWave = new FFTWave(this.phillipsImage);
	private normal = new Normal();
	private gaussian = new Gaussian();

	private skybox = new Skybox();
	public readonly ocean = new Ocean();

	constructor() {
		gl.disable(gl.DEPTH_TEST);
		gl.viewport(0, 0, this.phillipsImage.width, this.phillipsImage.height);
		this.offscreen.bind();
			this.offscreen.set(gl.COLOR_ATTACHMENT0, this.phillipsImage);
			this.phillips.render();
		this.offscreen.unbind();
		gl.enable(gl.DEPTH_TEST);
		gl.viewport(0, 0, Renderer.instance.canvas.width, Renderer.instance.canvas.height);

		this.offscreen.bind();
			this.offscreen.set(gl.DEPTH_ATTACHMENT, new RenderBuffer(gl.DEPTH_COMPONENT16));
			this.offscreen.set(gl.COLOR_ATTACHMENT0, this.gaussianImage);
			this.gaussian.render();
		this.offscreen.unbind();

		if (!gl2) {
			throw "webgl 2.0 required.";
		}
	}

	render(vessels: ulist<Vessel>, target: Texture2D = null) {
		// set gl state
		gl.clearColor(0, 0, 0, 0);
		gl.clearDepth(1);

		// reder ambient cube - offscreen
		this.ambient.render();

		this.gaussianImage.use("Gaussian");
			this.fftWave.render();
		this.gaussianImage.unuse();

		// render main image into mainImage
		this.offscreen.bind();

		gl.viewport(0, 0, 256, 256);

		gl.disable(gl.DEPTH_TEST);

			this.suboffscreen.bind();

				this.suboffscreen.set(gl.COLOR_ATTACHMENT0, this.perlinImage);
				this.perlin.render();

				this.suboffscreen.set(gl.COLOR_ATTACHMENT0, this.normalJImage);
				this.fftWave.texture.use("Displacement");
				this.normal.render();
				this.fftWave.texture.unuse();

			this.suboffscreen.unbind();

		gl.enable(gl.DEPTH_TEST);

		this.worldCom.use();

		this.main.viewport.use();

			gl.clearColor(.0, .0, .0, -1e8);
		
			this.offscreen.set(gl2.COLOR_ATTACHMENT0, this.mainImage);
			this.offscreen.set(gl2.COLOR_ATTACHMENT1, this.normalTypeImage);
			this.offscreen.set(gl2.COLOR_ATTACHMENT2, this.extraImage);
			gl2.drawBuffers([
				gl2.COLOR_ATTACHMENT0, 
				gl2.COLOR_ATTACHMENT1, 
				gl2.COLOR_ATTACHMENT2
			]);

			gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

			gl2.drawBuffers([
				gl2.COLOR_ATTACHMENT0, 
				gl2.COLOR_ATTACHMENT1
			]);

			Vessel.bindShader();
				vessels.visit((e: ulist_elem<Vessel>) => {
					e.get().render(this.main.viewport);
				});
			Vessel.unbindShader();

			gl2.drawBuffers([
				gl2.NONE,
				gl2.COLOR_ATTACHMENT1, 
				gl2.COLOR_ATTACHMENT2
			]);

			this.fftWave.texture.use("Displacement");
			this.normalJImage.use("NormalJ");
			this.bumpImage.use("Bump");
			this.perlinImage.use("Perlin");
			this.ocean.bindShader();
				this.ocean.render(this.main.viewport);
			this.ocean.unbindShader();
			this.perlinImage.unuse();
			this.bumpImage.unuse();
			this.normalJImage.unuse();
			this.fftWave.texture.unuse();

			gl2.drawBuffers([
				gl2.COLOR_ATTACHMENT0
			]);

			gl.clearColor(.0, .0, .0, .0);

			this.ambient.texture.use("Ambient");
			this.skybox.bindShader();
				this.skybox.render(this.main.viewport);
			this.skybox.unbindShader();
			this.ambient.texture.unuse();

			this.offscreen.set(gl2.COLOR_ATTACHMENT0, this.smokeImage);
			gl.clear(gl.COLOR_BUFFER_BIT);
			gl.enable(gl.BLEND);
			gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);
			gl.disable(gl.DEPTH_TEST);
			this.transformFeedback.bind();
			this.gaussianImage.use("Gaussian");
			this.smokeBallImage.use("SmokeBall");
			Smoke.bindShader();
				vessels.visit((e: ulist_elem<Vessel>) => {
					let parent = e.get();
					for (let s of parent.particles) {
						s.render(this.main.viewport);
					}
				});
			this.smokeBallImage.unuse();
			this.gaussianImage.unuse();
			Smoke.unbindShader();
			this.transformFeedback.unbind();

			gl.disable(gl.BLEND);

		this.main.viewport.unuse();

		this.worldCom.unuse();

		// end world pass
		gl.disable(gl.DEPTH_TEST);
		
		if (target == null) {
			this.offscreen.unbind();
		} else {
			this.offscreen.set(gl.COLOR_ATTACHMENT0, target)
		}
		
		this.renderCom.use();

			// main renderer
			this.mainImage.use("Image");
			this.normalTypeImage.use("NormalType");
			this.extraImage.use("Extra");
			this.ambient.texture.use("Ambient");
			this.smokeImage.use("Smoke");

				this.main.render();

			this.smokeImage.unuse();
			this.ambient.texture.unuse();
			this.extraImage.unuse();
			this.normalTypeImage.unuse();
			this.mainImage.unuse();

			// this.debugWindow(this.normalJImage, true, 0);
			this.debugWindow(this.smokeImage, true, 0);
			// this.debugWindow(this.extraImage, false, 2);
		
		this.renderCom.unuse();

		if (target != null) {
			this.offscreen.unbind();
		}
	}

	debugWindow(texture: Texture2D, normalize: boolean, index: number)
	{
		let w = index % 3;
		let h = Math.floor(index / 3);
		gl.viewport(
			Renderer.instance.canvas.width/3 * w, 
			Renderer.instance.canvas.height/3 * h, 
			Renderer.instance.canvas.width/3, 
			Renderer.instance.canvas.height/3
		);
		texture.use("Image");
		if (normalize) this.decode.render();
		else this.defer.render();
		texture.unuse();
	}

	setCamera(camera: CameraBase) {
		if (this.main.viewport) {
			this.main.viewport.unbind();
		}
		this.main.viewport = camera;
		camera.bind();
	}
}

export {
	Engine3d
}
