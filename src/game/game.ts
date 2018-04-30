import { glm } from "../util/glm"
import { gl, Renderer } from "../renderer/renderer"
import Scene from "../scene/scene"
import { PerspectiveViewport, Viewport } from "../scene/viewport"
import Model from "../gl/model"
import ModelObj from "../object/modelObject"
import { PointLight } from "../techniques/lighting";
import { ShadowTechnique } from "../techniques/shadow";
import { Ocean } from "../techniques/ocean";
import { Shader } from "../gl/shader";
import { GerstnerWave } from "../techniques/gerstnerWave";
import { AmbientCube } from "../techniques/ambientCube";
import { TestScreen } from "../techniques/offscreenTest";
import { Skybox } from "../techniques/skybox";
import { ulist_elem } from "../util/ulist";
import { Observer } from "./camera/observer";
import { CameraBase } from "./camera/cameraBase";

type CameraMode = "observe" | "follow" | "free" | "periscope"

class Game {
	private renderer = new Renderer(document.body);

	private scene = new Scene();
	private ambient = new AmbientCube();
	private skybox = new Skybox();

	private mainViewport: ulist_elem<CameraBase>;

	private viewports = {
		observe: new Observer(),
		follow: new Observer(),
		free: new Observer(),
		periscope: new Observer()
	};

	private ocean = new Ocean();

	constructor() {
		// set gl state
		gl.clearColor(0, 0, 0, 0);
		gl.clearDepth(1);
		gl.enable(gl.DEPTH_TEST);

		this.mainViewport = this.scene.createViewport<CameraBase>(
				this.viewports.observe);
		this.viewports.observe.bind();

		this.viewports.observe.position = glm.vec3(4, 0, 1.2);
		this.viewports.observe.target = glm.vec3(-4, 0, -0.8);
				
		this.viewports.periscope.fov = glm.radians(5);
	}

	start() {
		this.renderer.start();
	}

	setCameraMode(mode: CameraMode) {
		this.mainViewport.get().unbind();
		this.mainViewport.set(this.viewports[mode]);
		this.viewports[mode].bind();
	}
}

export {
	Game
}
