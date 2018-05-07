import { glm } from "../util/glm"
import { gl, Renderer } from "../renderer/renderer"
import { PerspectiveViewport, Viewport } from "../gl/viewport"
import Model from "../gl/model"
import ModelObj from "../object/modelObject"
import { PointLight } from "../techniques/lighting";
import { Ocean } from "../object/ocean";
import { Shader } from "../gl/shader";
import { GerstnerWave } from "../object/gerstnerWave";
import { AmbientCube } from "../techniques/ambientCube";
import { TestScreen } from "../techniques/offscreenTest";
import { Skybox } from "../object/skybox";
import { ulist_elem, ulist } from "../util/ulist";
import { Observer } from "./camera/observer";
import { CameraBase } from "./camera/cameraBase";
import { Communicators } from "../gl/communicator";
import Obj from "../object/object";
import { currentId } from "async_hooks";
import { Offscreen, RenderBuffer } from "../techniques/offscreen";
import { Texture2D } from "../gl/texture";
import { SSR } from "../techniques/SSR";
import { DeferImage } from "../techniques/deferImage";
import { UV_UDP_REUSEADDR } from "constants";
import { UV } from "../techniques/uv";
import { DepthDecode } from "../techniques/depthDecode";
import { Noise } from "../techniques/noise";
import { Vessel } from "./vessel/vessel";

type CameraMode = "observe" | "follow" | "free" | "periscope"

class Game {
	private renderer = new Renderer(document.body);

	private skybox = new Skybox();
	private ocean = new Ocean();
	
	private mainViewport: CameraBase;
	
	private viewports = {
		observe: new Observer(),
		follow: new Observer(),
		free: new Observer(),
		periscope: new Observer()
	};
	
	private vessels = new ulist<Vessel>();
	private worldCom = new Communicators();
	private renderCom = new Communicators();
	
	private mainImage = new Texture2D(gl.RGBA);
	private normalDepthImage = new Texture2D(gl.RGBA);
	private normalImage = new Texture2D(gl.RGB);
	private depthImage = new Texture2D(gl.RGBA);
	private ssrImage = new Texture2D(gl.RGBA);
	private uvImage = new Texture2D(gl.RGB);
	private noiseImage = new Texture2D(gl.RGB);
	private depthDecodeImage = new Texture2D(gl.RGB);
	private channel: Texture2D;

	private offscreen = new Offscreen();

	private ambient = new AmbientCube();
	private ssr = new SSR(true);
	private depthDecode = new DepthDecode();
	private uv = new UV();
	private noise = new Noise();
	private defer = new DeferImage();

	constructor() {
		// set gl state
		gl.clearColor(0, 0, 0, 0);
		gl.clearDepth(1);
		gl.enable(gl.DEPTH_TEST);

		this.mainViewport = this.viewports.observe;
		this.ssr.viewport = this.mainViewport;
		this.mainViewport.bind();
				
		this.viewports.periscope.fov = glm.radians(5);

		this.offscreen.bind();
			this.offscreen.set(gl.DEPTH_ATTACHMENT, new RenderBuffer(gl.DEPTH_COMPONENT16));
		this.offscreen.unbind();

		Shader.require({
			NormalDepth: {
				frag: "normalDepth"
			},
			Normal: {
				frag: "normal"
			},
			Depth: {
				frag: "depth"
			}
		});

		this.channel = this.mainImage;
		Renderer.instance.dispatch("keydown", (e: KeyboardEvent) => {
			const lookup = {
				"1": this.mainImage,
				"2": this.normalDepthImage,
				"3": this.normalImage,
				"4": this.depthImage,
				"5": this.ssrImage,
				"6": this.depthDecodeImage,
				"7": this.uvImage,
				"8": this.noiseImage,
			};
			if (e.key.toLowerCase() in lookup) {
				this.channel = lookup[e.key.toLowerCase()];
			}
		});

		this.ambient.scale = 1;
		this.sea();
		// this.cube();
	}

	start() {
		this.renderer.dispatch("render", (time: number) => {
			// reder ambient cube - offscreen
			this.ambient.render();

			// render main image into mainImage
			this.offscreen.bind();

			this.worldCom.use();

			this.mainViewport.use();
			
				this.offscreen.set(gl.COLOR_ATTACHMENT0, this.mainImage);
				gl.clear(gl.DEPTH_BUFFER_BIT);
				this.ambient.texture.use("Ambient");
					this.ocean.bindShader();
						this.ocean.render(this.mainViewport);
					this.ocean.unbindShader();
					Vessel.bindShader();
						this.vessels.visit((e: ulist_elem<Vessel>) => {
							e.get().render(this.mainViewport);
						});
					Vessel.unbindShader();
					this.skybox.bindShader();
						this.skybox.render(this.mainViewport);
					this.skybox.unbindShader();
				this.ambient.texture.unuse();
				
				this.offscreen.set(gl.COLOR_ATTACHMENT0, this.normalDepthImage);
				gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
				Shader.specify("NormalDepth");
					this.ocean.bindShader();
						this.ocean.render(this.mainViewport);
					this.ocean.unbindShader();
					Vessel.bindShader();
						this.vessels.visit((e: ulist_elem<Obj>) => {
							e.get().render(this.mainViewport);
						});
					Vessel.unbindShader();
				Shader.unspecify();
				
				// this.offscreen.set(gl.COLOR_ATTACHMENT0, this.normalImage);
				// gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
				// Shader.specify("Normal");
				// 	this.objects.visit((e: ulist_elem<Obj>) => {
				// 		e.get().bindShader();
				// 			e.get().render(this.mainViewport);
				// 		e.get().unbindShader();
				// 	});
				// Shader.unspecify();

				// this.offscreen.set(gl.COLOR_ATTACHMENT0, this.depthImage);
				// gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
				// Shader.specify("Depth");
				// 	this.objects.visit((e: ulist_elem<Obj>) => {
				// 		e.get().bindShader();
				// 			e.get().render(this.mainViewport);
				// 		e.get().unbindShader();
				// 	});
				// Shader.unspecify();

			this.mainViewport.unuse();

			this.worldCom.unuse();

			// end world pass
			gl.disable(gl.DEPTH_TEST);
			this.renderCom.use();

			// do SSR
			this.offscreen.set(gl.COLOR_ATTACHMENT0, this.ssrImage);
			this.mainImage.use("Image");
			this.normalDepthImage.use("NormalDepth");
			// this.normalImage.use("Normal");
			// this.depthImage.use("Depth");
				this.ssr.render();
			this.normalDepthImage.unuse();
			// this.normalImage.unuse();
			// this.depthImage.unuse();
			this.mainImage.unuse();

			// decode depth
			// this.offscreen.set(gl.COLOR_ATTACHMENT0, this.depthDecodeImage);
			// this.depthImage.use("Depth");
			// 	this.depthDecode.render();
			// this.depthImage.unuse();

			// this.offscreen.set(gl.COLOR_ATTACHMENT0, this.noiseImage);
			// this.noise.render();

			// draw uv image
			// this.offscreen.set(gl.COLOR_ATTACHMENT0, this.uvImage);
			// this.uv.render();
			
			this.offscreen.unbind();

			// render defer image
			this.channel.use("Image");
				this.defer.render();
			this.channel.unuse();

			gl.enable(gl.DEPTH_TEST);
			this.renderCom.unuse();
		});

		this.renderer.start();
	}

	setCameraMode(mode: CameraMode) {
		this.mainViewport.unbind();
		this.mainViewport = this.viewports[mode];
		this.ssr.viewport = this.mainViewport;
		this.viewports[mode].bind();
	}

	sea() {
		this.ocean = new Ocean();
		let wave = new GerstnerWave();
		let wave1 = new GerstnerWave();
		wave.set("amplitude", 0.03);
		wave1.set("amplitude", 0.04);
		wave1.set("direction", glm.vec2(1, 2));
		this.ocean.add(wave);
		this.ocean.add(wave1);

		this.vessels.push(new Vessel("clemson"));
	}
	cube() {
		let off = new TestScreen();

		// this.objects.push(off);
	
		let cameraPositions = [
			glm.vec3(1, 0, 0),
			glm.vec3(-1, 0, 0),
			glm.vec3(0, 1, 0),
			glm.vec3(0, -1, 0),
			glm.vec3(0, 0, 1),
			glm.vec3(0, 0, -1)
		];
	
		this.renderer.dispatch("keydown", (e: KeyboardEvent) => {
			if (!e.altKey && !e.shiftKey && !e.ctrlKey && !e.metaKey) {
				if ("012345".indexOf(e.key) >= 0) {
					off.face = cameraPositions[+e.key];
				}
			}
		});
	}
}

export {
	Game
}
