import { glm } from "../util/glm"
import { gl, Renderer, RendererEvent, gl2 } from "../renderer/renderer"
import { Offscreen, RenderBuffer } from "../gl/offscreen";
import { Texture2D } from "../gl/texture";
import { SSR } from "../techniques/SSR";
import { DeferImage } from "../techniques/deferImage";
import { UV } from "../techniques/uv";
import { Noise } from "../techniques/noise";
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

class Engine3d {
	private worldCom = new Communicators();
	private renderCom = new Communicators();
	
	private mainImage = new Texture2D({ component: gl.RGBA });
	private normalDepthImage = new Texture2D({ component: gl.RGBA, type: gl.FLOAT });
	private positionTypeImage = new Texture2D({ component: gl2.RGBA, type: gl.FLOAT });
	// private ssrImage = new Texture2D({ component: gl.RGBA });
	private uvImage = new Texture2D({ component: gl.RGB });
	private noiseImage = new Texture2D({ component: gl.RGB });
	private phillipsImage = new Texture2D({ component: gl2.RG, type: gl.FLOAT, filter: gl.NEAREST, wrap: gl.CLAMP_TO_EDGE }, 256, 256);

	private channel: Texture2D;

	private offscreen = new Offscreen();

	public readonly ambient = new AmbientCube();
	private main = new GameRenderer();

	private ssr = new SSR();
	private uv = new UV();
	private noise = new Noise();
	private defer = new DeferImage();

	private phillips = new Phillips();
	private fftWave = new FFTWave(this.phillipsImage);

	private skybox = new Skybox();
	public readonly ocean = new Ocean();

	constructor() {
		gl.disable(gl.DEPTH_TEST);
		this.offscreen.bind();
			this.offscreen.set(gl.COLOR_ATTACHMENT0, this.phillipsImage);
			this.phillips.render();
		this.offscreen.unbind();
		gl.enable(gl.DEPTH_TEST);

		this.offscreen.bind();
			this.offscreen.set(gl.DEPTH_ATTACHMENT, new RenderBuffer(gl.DEPTH_COMPONENT16));
		this.offscreen.unbind();

		if (!gl2) {
			throw "webgl 2.0 required.";
		}
	}

	renderDebug(vessels: ulist<Vessel>, target: Texture2D = null) {
		// reder ambient cube - offscreen
		this.ambient.render();

		// render main image into mainImage
		this.offscreen.bind();

		this.worldCom.use();

		this.main.viewport.use();
		
			this.offscreen.set(gl.COLOR_ATTACHMENT0, this.mainImage);
			gl.clear(gl.DEPTH_BUFFER_BIT);
			this.ambient.texture.use("Ambient");
				this.ocean.bindShader();
					this.ocean.render(this.main.viewport);
				this.ocean.unbindShader();
				Vessel.bindShader();
					vessels.visit((e: ulist_elem<Vessel>) => {
						e.get().render(this.main.viewport);
					});
				Vessel.unbindShader();
				this.skybox.bindShader();
					this.skybox.render(this.main.viewport);
				this.skybox.unbindShader();
			this.ambient.texture.unuse();
			
			this.offscreen.set(gl.COLOR_ATTACHMENT0, this.normalDepthImage);
			gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
			Shader.specify("NormalDepth");
				this.ocean.bindShader();
					this.ocean.render(this.main.viewport);
				this.ocean.unbindShader();
				Vessel.bindShader();
					vessels.visit((e: ulist_elem<Vessel>) => {
						e.get().render(this.main.viewport);
					});
				Vessel.unbindShader();
			Shader.unspecify();

		this.main.viewport.unuse();

		this.worldCom.unuse();

		// end world pass
		gl.disable(gl.DEPTH_TEST);
		this.renderCom.use();
		
		this.offscreen.set(gl.COLOR_ATTACHMENT0, this.noiseImage);
		this.noise.render();
		
		// draw uv image
		this.offscreen.set(gl.COLOR_ATTACHMENT0, this.uvImage);
		this.uv.render();
		
		this.offscreen.unbind();
		
		// render defer image
		this.channel.use("Image");
		this.defer.render();
		this.channel.unuse();
		
		gl.enable(gl.DEPTH_TEST);
		this.renderCom.unuse();
	}

	renderRelease(vessels: ulist<Vessel>, target: Texture2D = null) {
		// set gl state
		gl.clearColor(0, 0, 0, 0);
		gl.clearDepth(1);

		// reder ambient cube - offscreen
		this.ambient.render();

		this.fftWave.render();

		// render main image into mainImage
		this.offscreen.bind();

		gl.enable(gl.DEPTH_TEST);

		this.worldCom.use();

		this.main.viewport.use();
		
			this.offscreen.set(gl2.COLOR_ATTACHMENT0, this.mainImage);
			this.offscreen.set(gl2.COLOR_ATTACHMENT1, this.normalDepthImage);
			this.offscreen.set(gl2.COLOR_ATTACHMENT2, this.positionTypeImage);
			gl2.drawBuffers([
				gl2.COLOR_ATTACHMENT0, 
				gl2.COLOR_ATTACHMENT1, 
				gl2.COLOR_ATTACHMENT2
			]);

			gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

			this.ambient.texture.use("Ambient");

			this.fftWave.texture.use("Displacement");
			this.ocean.bindShader();
				this.ocean.render(this.main.viewport);
			this.ocean.unbindShader();
			this.fftWave.texture.unuse();

			Vessel.bindShader();
				vessels.visit((e: ulist_elem<Vessel>) => {
					e.get().render(this.main.viewport);
				});
			Vessel.unbindShader();
			
			this.skybox.bindShader();
				this.skybox.render(this.main.viewport);
			this.skybox.unbindShader();

			this.ambient.texture.unuse();
			
			gl2.drawBuffers([
				gl2.COLOR_ATTACHMENT0
			]);

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
			this.normalDepthImage.use("NormalDepth");
			this.positionTypeImage.use("PositionType");
			this.ambient.texture.use("Ambient");

				this.main.render();

			this.ambient.texture.unuse();
			this.positionTypeImage.unuse();
			this.normalDepthImage.unuse();
			this.mainImage.unuse();
		
		this.renderCom.unuse();

		if (target != null) {
			this.offscreen.unbind();
		}
	}

	render(vessels: ulist<Vessel>, target: Texture2D = null) {
		const debug = 
			false;
			// true;
		if (debug) {
			this.renderDebug(vessels, target);
		} else {
			this.renderRelease(vessels, target);
		}
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
