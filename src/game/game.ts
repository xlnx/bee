import { glm } from "../util/glm"
import { gl, Renderer, RendererEvent } from "../renderer/renderer"
import { Ocean } from "../object/ocean";
import { ulist_elem, ulist } from "../util/ulist";
import { Observer } from "./camera/observer";
import { CameraBase, Cmaera } from "./camera/cameraBase";
import { Vessel } from "./vessel/vessel";
import { Submarine } from "./vessel/submarine";
import { Engine3d } from "./engine3d";
import { Splash } from "./2d/splash";
import { Communicators } from "../gl/communicator";
import { Options } from "./2d/options";
import { Offscreen } from "../techniques/offscreen";
import { Texture2D } from "../gl/texture";
import { GaussBlur } from "../techniques/gaussBlur";

type CameraMode = "observe" | "follow" | "free" | "periscope"

type DisplayMode = "3d" | "6d" | "menu" | "map" | "splash" | "options"

class Game {
	private renderer = new Renderer(document.body);
	
	private viewports = {
		observe: new Observer(),
		follow: new Observer(),
		periscope: new Cmaera()
	};

	private vessels = new ulist<Vessel>();
	private uboat: Submarine;

	private battleKeys: RendererEvent;
	private prevDisplayMode: DisplayMode = undefined;
	private displayMode: DisplayMode = "menu";
	private renderOption: boolean = false;
	private inGame = false;

	private engine3d = new Engine3d();
	private splash = new Splash("loading.png");
	private options = new Options();

	private queryObjects: string[] = [];

	private offscreen = new Offscreen();
	private gaussBlur = new GaussBlur(5);
	private screenBuffer = new Texture2D(gl.RGB);

	constructor() {
		new Communicators().use();

		// this.mainViewport.rotate(glm.vec3(0, 0, glm.radians(180)));
		this.viewports.periscope.fov = glm.radians(5);

		// Renderer.instance.dispatch("keydown", (e: KeyboardEvent) => {
		// 	const lookup = {
		// 		"1": this.mainImage,
		// 		"2": this.normalDepthImage,
		// 		"3": this.depthDecodeImage,
		// 		"4": this.uvImage,
		// 		"5": this.noiseImage,
		// 	};
		// 	if (e.key.toLowerCase() in lookup) {
		// 		this.channel = lookup[e.key.toLowerCase()];
		// 	}
		// });

		// this.channel = this.mainImage;

		this.beginBattle();
	}

	start() {
		this.renderer.dispatch("render", () => {
			if (this.inGame && this.displayMode != "options") {
				this.updateVessels(Renderer.time - Renderer.prevTime);
			}
			const callbacks = {
				"3d": () => {
					this.engine3d.render(this.vessels);
				},
				"6d": () => { 
					this.engine3d.render(this.vessels);
				},
				"menu": () => {
					// this.engine3d.render(this.vessels); 
				},
				"map": () => {
					
				},
				"splash": () => {
					this.splash.render();
				},
				"options": () => {
					if (this.renderOption) {
						this.renderOption = false;
						this.engine3d.render(this.vessels, this.screenBuffer);
						gl.disable(gl.DEPTH_TEST);
						this.offscreen.bind();
							this.offscreen.set(gl.COLOR_ATTACHMENT0, this.options.bg);
							this.screenBuffer.use("Image");
								this.gaussBlur.render();
							this.screenBuffer.unuse();
						this.offscreen.unbind();
					}
					this.options.render();
				}
			};
			callbacks[this.displayMode] ();
		});

		this.renderer.start();
	}

	updateVessels(dt: number) {
		this.vessels.visit((e: ulist_elem<Vessel>) => {
			e.get().update(dt);
		});
	}

	spawnVessel(className: string, opt: { [key: string]: any } = {}) {
		this.queryObjects.push(className);
		let self = this;
		new Vessel(className, (v: Vessel) => {
			self.vessels.push(v);
			self.queryObjects.splice(self.queryObjects.indexOf(className), 1);
			self.checkLoadingFinish();
		});
		// clemson.speed = 35.5;
	}

	resetUboat(className: string, opt: { [key: string]: any } = {}) {
		this.queryObjects.push(className);
		let self = this;
		new Submarine(className, (v: Submarine) => {
			self.uboat = v;
			self.vessels.push(self.uboat);
			self.queryObjects.splice(self.queryObjects.indexOf(className), 1);
			self.checkLoadingFinish();
		});
	}

	checkLoadingFinish() {
		if (!this.queryObjects.length) {
			this.finishLoading();
		}
	}
	
	periscopeMode() {
		this.displayMode = "6d"; 
		this.engine3d.setCamera(this.viewports.periscope);
	}

	turmMode() {
		this.displayMode = "3d";
		this.engine3d.setCamera(this.viewports.follow);
	}

	mapMode() {
		this.displayMode = "map";
	}

	gunMode() {
		this.displayMode = "3d";
		this.engine3d.setCamera(this.viewports.follow);
	}

	freeMode() {
		this.displayMode = "3d";
		this.engine3d.setCamera(this.viewports.observe);
	}

	beginBattle(opt: { [key: string]: any } = {}) {
		this.displayMode = "splash";
		
		this.queryObjects = [];
		this.spawnVessel("clemson");
		this.resetUboat("7c");

		Renderer.timescale = 1;
		this.engine3d.ambient.setTime(6);
	}

	finishLoading() {
		this.battleKeys = Renderer.instance.dispatch("keydown", (e: KeyboardEvent) => {
			
			if (this.displayMode != "options" || e.key.toLowerCase() == "escape") {
				const lookup = {
					"f3": this.periscopeMode.bind(this),
					"f4": this.turmMode.bind(this),
					"f5": this.mapMode.bind(this),
					"f10": this.gunMode.bind(this),
					"f12": this.freeMode.bind(this),

					"`": this.uboat.setSpeedMode.bind(this.uboat, "stop"),
					"1": this.uboat.setSpeedMode.bind(this.uboat, "slowFw"),
					"2": this.uboat.setSpeedMode.bind(this.uboat, "oneThirdFw"),
					"3": this.uboat.setSpeedMode.bind(this.uboat, "normalFw"),
					"4": this.uboat.setSpeedMode.bind(this.uboat, "fullFw"),
					"5": this.uboat.setSpeedMode.bind(this.uboat, "extraFw"),
					"6": this.uboat.setSpeedMode.bind(this.uboat, "slowBw"),
					"7": this.uboat.setSpeedMode.bind(this.uboat, "oneThirdBw"),
					"8": this.uboat.setSpeedMode.bind(this.uboat, "normalBw"),
					"9": this.uboat.setSpeedMode.bind(this.uboat, "extraBw"),

					"[": this.uboat.setRudderMode.bind(this.uboat, "fullLw"),
					"]": this.uboat.setRudderMode.bind(this.uboat, "fullRw"),
					"'": this.uboat.setRudderMode.bind(this.uboat, "reset"),

					"s": this.uboat.setDiveMode.bind(this.uboat, "Uw"),
					"d": this.uboat.setDiveMode.bind(this.uboat, "Dw"),
					"a": this.uboat.setDiveMode.bind(this.uboat, "keep"),
					"p": () => {  },
					";": () => {  },
					"e": this.uboat.setDiveMode.bind(this.uboat, "extraUw"),
					"c": this.uboat.setDiveMode.bind(this.uboat, "extraDw"),
					"z": () => {  },

					"u": () => {  },
					"o": () => {  },
					"b": () => {  },
					"f": () => {  },

					"x": () => {  },

					"escape": () => {
						if (this.displayMode != "options") {
							this.prevDisplayMode = this.displayMode;
							this.displayMode = "options";
							this.renderOption = true;
							Renderer.pauseClock();
						} else {
							this.displayMode = this.prevDisplayMode;
							Renderer.continueClock();
						}
					}
				};
				if (e.key.toLowerCase() in lookup) {
					lookup[e.key.toLowerCase()] ();
				}
			}
		});

		this.inGame = true;

		this.viewports.observe.tie(this.uboat);
		this.uboat.diveSpeed = 1;

		this.freeMode();
	}
	
	endBattle() {
		this.inGame = false;
		this.vessels.clear();
		this.uboat = null;
		this.battleKeys.cancel();
	}
}

export {
	Game
}
