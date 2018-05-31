import { glm } from "../util/glm"
import { gl, Renderer, RendererEvent } from "../renderer/renderer"
import { Ocean } from "../object/ocean";
import { ulist_elem, ulist } from "../util/ulist";
import { Observer } from "./camera/observer";
import { CameraBase } from "./camera/cameraBase";
import { Vessel } from "./vessel/vessel";
import { Submarine } from "./vessel/submarine";
import { Engine3d } from "./engine3d";
import { Splash } from "./2d/splash";
import { Communicators } from "../gl/communicator";
import { Options } from "./2d/options";
import { Offscreen } from "../gl/offscreen";
import { Texture2D } from "../gl/texture";
import { GaussBlur } from "../techniques/gaussBlur";
import { Periscope } from "./camera/periscope";

type CameraMode = "observe" | "follow" | "free" | "periscope"

type DisplayMode = "3d" | "6d" | "menu" | "map" | "splash" | "options"

class Game {
	private renderer = new Renderer(document.body);
	
	private viewports = {
		observe: new Observer(),
		follow: new Observer(),
		periscope: new Periscope()
	};

	private vessels = new ulist<Vessel>();
	private uboat: Submarine;

	private battleKeys: RendererEvent;
	private battleKeysUp: RendererEvent;
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
	private screenBuffer = new Texture2D({ component: gl.RGB });

	constructor() {
		new Communicators().use();

		// this.mainViewport.rotate(glm.vec3(0, 0, glm.radians(180)));

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
				this.viewports.periscope.update();
				this.updateVessels();
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

	updateVessels() {
		this.vessels.visit((e: ulist_elem<Vessel>) => {
			e.get().update();
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
	
	setMode(mode: string) {
		if (this.displayMode == "6d" && mode != "periscope") {
			this.viewports.periscope.stop();
		}
		switch (mode) {
			case "free": {
				this.displayMode = "3d";
				this.engine3d.setCamera(this.viewports.observe);
				this.viewports.observe.tie(this.uboat);
			} break;
			case "gun": {
				this.displayMode = "3d";
				this.engine3d.setCamera(this.viewports.follow);
			} break;
			case "map": {
				this.displayMode = "map";
			} break;
			case "turm": {
				this.displayMode = "3d";
				this.engine3d.setCamera(this.viewports.follow);
			} break;
			case "periscope": {
				this.displayMode = "6d"; 
				this.engine3d.setCamera(this.viewports.periscope);
				this.viewports.periscope.tie(this.uboat);
			} break;
			default: {
				throw "unknown mode: " + mode;
			}
		}
	}

	beginBattle(opt: { [key: string]: any } = {}) {
		this.displayMode = "splash";
		
		this.queryObjects = [];
		this.spawnVessel("torpedo");
		this.spawnVessel("clemson");
		this.resetUboat("7c");

		Renderer.timescale = 1;
		this.engine3d.ambient.setTime(6);
	}

	finishLoading() {
		this.battleKeys = Renderer.instance.dispatch("keydown", (e: KeyboardEvent) => {
			if (this.displayMode != "options" || e.key.toLowerCase() == "escape") {
				const lookup = {
					"f3": this.setMode.bind(this, "periscope"),
					"f4": this.setMode.bind(this, "turm"),
					"f5": this.setMode.bind(this, "map"),
					"f10": this.setMode.bind(this, "gun"),
					"f12": this.setMode.bind(this, "free"),

					"`": () => this.uboat.targetSpeed = 0,
					"1": () => this.uboat.targetSpeed = 1/5,
					"2": () => this.uboat.targetSpeed = 1/3,
					"3": () => this.uboat.targetSpeed = 1/2,
					"4": () => this.uboat.targetSpeed = 3/4,
					"5": () => this.uboat.targetSpeed = 1,
					"6": () => this.uboat.targetSpeed = -1/6,
					"7": () => this.uboat.targetSpeed = -1/4,
					"8": () => this.uboat.targetSpeed = -1/2,
					"9": () => this.uboat.targetSpeed = -3/4,

					"q": () => { if (!e.repeat) this.uboat.diveSignal(-1); },
					"w": () => { if (!e.repeat) this.uboat.diveSignal(0); },
					"e": () => { if (!e.repeat) this.uboat.diveSignal(1); },

					"a": () => { if (!e.repeat) this.uboat.rudderSignal(-1); },
					"s": () => { if (!e.repeat) this.uboat.rudderSignal(0); },
					"d": () => { if (!e.repeat) this.uboat.rudderSignal(1); },

					",": () => { if (this.displayMode == "6d") this.viewports.periscope.rise(); },
					".": () => { if (this.displayMode == "6d") this.viewports.periscope.down(); },

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
		this.battleKeysUp = Renderer.instance.dispatch("keyup", (e: KeyboardEvent) => {
			if (this.displayMode != "options") {
				const lookup = {

					"q": () => this.uboat.diveSignal(-1),
					"w": () => this.uboat.diveSignal(0),
					"e": () => this.uboat.diveSignal(1),

					"a": () => this.uboat.rudderSignal(-1),
					"s": () => this.uboat.rudderSignal(0),
					"d": () => this.uboat.rudderSignal(1),

					",": () => { if (this.displayMode == "6d") this.viewports.periscope.stop(); },
					".": () => { if (this.displayMode == "6d") this.viewports.periscope.stop(); },
				};
				if (e.key.toLowerCase() in lookup) {
					lookup[e.key.toLowerCase()] ();
				}
			}
		});

		this.inGame = true;

		this.setMode("free");
	}
	
	endBattle() {
		this.inGame = false;
		this.vessels.clear();
		this.uboat = null;
		this.battleKeys.cancel();
		this.battleKeysUp.cancel();

	}
}

export {
	Game
}
