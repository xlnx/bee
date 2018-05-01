import { glm } from "../util/glm"
import { gl, Renderer } from "../renderer/renderer"
import { PerspectiveViewport, Viewport } from "../scene/viewport"
import Model from "../gl/model"
import ModelObj from "../object/modelObject"
import { PointLight } from "../techniques/lighting";
import { Ocean } from "../techniques/ocean";
import { Shader } from "../gl/shader";
import { GerstnerWave } from "../techniques/gerstnerWave";
import { AmbientCube } from "../techniques/ambientCube";
import { TestScreen } from "../techniques/offscreenTest";
import { Skybox } from "../techniques/skybox";
import { ulist_elem, ulist } from "../util/ulist";
import { Observer } from "./camera/observer";
import { CameraBase } from "./camera/cameraBase";
import { Communicators } from "../gl/communicator";
import Obj from "../scene/object";
import { currentId } from "async_hooks";
import { Offscreen, RenderBuffer } from "../techniques/offscreen";
import { Texture2D } from "../gl/texture";
import { ScreenSpaceReflection } from "../techniques/screenSpaceReflection";
import { DeferImage } from "../techniques/deferImage";

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
	
	private objects = new ulist<Obj>();
	private worldCom = new Communicators();
	private renderCom = new Communicators();
	
	private mainImage = new Texture2D(gl.RGBA);
	private normalImage = new Texture2D(gl.RGB);
	// private mainImage = new Texture2D(gl.RGB);

	private offscreen = new Offscreen();

	private ambient = new AmbientCube();
	private ssr = new ScreenSpaceReflection();
	private defer = new DeferImage();

	constructor() {
		// set gl state
		gl.clearColor(0, 0, 0, 0);
		gl.clearDepth(1);
		gl.enable(gl.DEPTH_TEST);

		this.mainViewport = this.viewports.observe;
		this.viewports.observe.bind();
				
		this.viewports.periscope.fov = glm.radians(5);

		this.offscreen.bind();
			this.offscreen.set(gl.DEPTH_ATTACHMENT, new RenderBuffer(gl.DEPTH_COMPONENT16));
		this.offscreen.unbind();

		Shader.require({
			Normal: {
				fs: "normal"
			}
		});

		this.sea();
		// this.cube();
	}

	start() {
		this.renderer.dispatch("render", (time: number) => {
			// reder ambient cube - offscreen
			this.ambient.render();

			// render main image into mainImage
			this.worldCom.use();

			this.offscreen.bind();

				this.mainViewport.use();
				
				this.offscreen.set(gl.COLOR_ATTACHMENT0, this.mainImage);
				gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
				this.ambient.texture.use("Ambient");
					this.objects.visit((e: ulist_elem<Obj>) => {
						e.get().bindShader();
							e.get().render(this.mainViewport);
						e.get().unbindShader();
					});
					this.skybox.bindShader();
						this.skybox.render(this.mainViewport);
					this.skybox.unbindShader();
				this.ambient.texture.unuse();
				
				this.offscreen.set(gl.COLOR_ATTACHMENT0, this.normalImage);
				gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
				Shader.specify("Normal");
					this.objects.visit((e: ulist_elem<Obj>) => {
						e.get().bindShader();
							e.get().render(this.mainViewport);
						e.get().unbindShader();
					});
				Shader.unspecify();

				this.mainViewport.unuse();

			this.offscreen.unbind();

			this.worldCom.unuse();

			// end world pass
			gl.disable(gl.DEPTH_TEST);
			this.renderCom.use();

			// do SSR
			// this.mainImage.use("World");
			// 	this.ssr.render();
			
			// render defer image
			// this.mainImage.use("Image");
			// 	this.defer.render();
			// this.mainImage.unuse();

			this.normalImage.use("Image");
				this.defer.render();
			this.normalImage.unuse();

			gl.enable(gl.DEPTH_TEST);
			this.renderCom.unuse();
		});

		this.renderer.start();
	}

	setCameraMode(mode: CameraMode) {
		this.mainViewport.unbind();
		this.mainViewport = this.viewports[mode];
		this.viewports[mode].bind();
	}

	sea() {
		let model = new ModelObj(Model.create("clemson.json"));
		model.scale(0.5);

		let ocean = new Ocean();
		let wave = new GerstnerWave();
		let wave1 = new GerstnerWave();
		wave.set("amplitude", 0.04);
		wave1.set("amplitude", 0.06);
		wave1.set("direction", glm.vec2(1, 2));
		ocean.add(wave);
		ocean.add(wave1);

		this.objects.push(ocean);
		this.objects.push(model);
	}
	cube() {
		let off = new TestScreen();

		this.objects.push(off);
	
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
