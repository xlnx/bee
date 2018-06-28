import { glm } from "../util/glm"
import { gl, Renderer, RendererEvent } from "../renderer/renderer"
import { Ocean } from "../object/ocean";
import { ulist_elem, ulist } from "../util/ulist";
import { Observer } from "./camera/observer";
import { CameraBase } from "./camera/cameraBase";
import { Vessel, m2screen } from "./vessel/vessel";
import { Submarine } from "./vessel/submarine";
import { Engine3d } from "./engine3d";
import { Splash } from "./2d/splash";
import { Communicators } from "../gl/communicator";
import { Options } from "./2d/options";
import { Offscreen } from "../gl/offscreen";
import { Texture2D } from "../gl/texture";
import { GaussBlur } from "../techniques/gaussBlur";
import { Periscope } from "./camera/periscope";
import { Torpedo } from "./vessel/torpedo";
import { DeferImage } from "../techniques/deferImage";
import { PeriScreen } from "./2d/periscreen";
import { Explode } from "./vessel/explode";
import { missions } from "./mission";

type CameraMode = "observe" | "follow" | "free" | "periscope"

type DisplayMode = "3d" | "periscope" | "menu" | "map" | "splash" | "options"

function inRect(x: number, y: number, x0: number, y0: number, x1: number, y1: number): boolean {
	return x >= x0 && x < x1 && y >= y0 && y < y1;
}

function hit(t: glm.vec4, v: Vessel) {
	let x = glm.inverse(v.getTrans())["*"](t).xy;
	if (x.y < v.collider[1] || 
		x.y > v.collider[v.collider.length - 1]) {
		return false;
	}
	let l = 0, r = (v.collider.length >> 1) - 1;
	while (r - l > 1) {
		let m = (l + r) >> 1;
		if (x.y >= v.collider[(m << 1) | 1]) {
			l = m;
		} else {
			r = m;
		}
	}
	if (l == (v.collider.length >> 1) - 1) {
		l -= 1;
	}
	let lam = (x.y - v.collider[(l << 1) + 1]) / 
		( v.collider[(l << 1) + 3] - v.collider[(l << 1) + 1] );
	let z = (1 - lam) * v.collider[(l << 1) + 0] + 
		lam * v.collider[(l << 1) + 2];
	return Math.abs(x.x) <= z;
}

class Game {
	private renderer = new Renderer(document.body);
	
	private viewports = {
		observe: new Observer(),
		follow: new Observer(),
		periscope: new Periscope()
	};

	private vessels = new ulist<Vessel>();
	private explodes = new ulist<Explode>();
	private uboat: Submarine;

	private battleKeys: RendererEvent;
	private battleKeysUp: RendererEvent;
	private battleMouses: RendererEvent;
	private battleMousesUp: RendererEvent;
	private prevDisplayMode: DisplayMode = undefined;
	private displayMode: DisplayMode = "menu";
	
	private renderOption: boolean = false;
	private inGame: boolean = false;
	private blurTime: number = 0;
	private prevr: number = 1;

	private engine3d = new Engine3d();
	private splash = new Splash("loading.png");
	private options = new Options();
	private periscreen = new PeriScreen();
	private defer = new DeferImage();

	private queryObjects: string[] = [];

	private offscreen = new Offscreen();
	private gaussBlur = new GaussBlur();
	private worldBuffer = new Texture2D({ component: gl.RGB });
	private swapBuffer = new Texture2D({ component: gl.RGB });
	private screenBuffer = new Texture2D({ component: gl.RGB });

	constructor() {
		new Communicators().use();

		this.beginBattle(missions[".narvik"]);
	}

	start() {
		this.renderer.dispatch("render", () => {
			const callbacks = {
				"3d": () => {
					this.engine3d.render(this.vessels, this.explodes, this.screenBuffer);
					gl.disable(gl.DEPTH_TEST);
					this.screenBuffer.use("Image");
						this.defer.render();
					this.screenBuffer.unuse();
				},
				"periscope": () => {
					this.engine3d.render(this.vessels, this.explodes, this.worldBuffer, (r: number) => {
						if (Math.abs(this.prevr - r) > 0.1) {
							this.blurTime = 2.5;
						}
						this.prevr = r;
					});
					this.offscreen.bind();
						if (this.blurTime > 0) {
							let t = this.swapBuffer; this.swapBuffer = this.worldBuffer; this.worldBuffer = t;
							this.gaussBlur.setRadius(12 * Math.sqrt(this.blurTime));
							this.gaussBlur.render(this.swapBuffer, this.worldBuffer);
							this.blurTime -= Renderer.dt;
						}
						this.offscreen.set(gl.COLOR_ATTACHMENT0, this.screenBuffer);
						this.periscreen.render(this.worldBuffer);
					this.offscreen.unbind();
					gl.disable(gl.DEPTH_TEST);
					this.screenBuffer.use("Image");
						this.defer.render();
					this.screenBuffer.unuse();
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
						// this.engine3d.render(this.vessels, this.worldBuffer);
						gl.disable(gl.DEPTH_TEST);
						this.offscreen.bind();
							this.gaussBlur.setRadius(10);
							this.gaussBlur.render(this.screenBuffer, this.options.bg);
						this.offscreen.unbind();
					}
					this.options.render();
				}
			};
			callbacks[this.displayMode] ();
			if (this.inGame && this.displayMode != "options") {
				let d = this.viewports.periscope.update();
				this.periscreen.indicatorState(d);
				this.explodes.visit((e: ulist_elem<Explode>) => {
					if (!e.get().checkRange()) {
						e.remove();
					}
				});
				this.updateVessels();
			}
		});

		this.renderer.start();
	}

	updateVessels() {
		this.vessels.visit((e: ulist_elem<Vessel>) => {
			let v = e.get();
			if (v instanceof Torpedo) {
				let torpedo = v;
				let tpos = torpedo.getTrans()["*"](glm.vec4(0, torpedo.collider[3], 0, 1));
				let ves = null;
				this.vessels.visit((e: ulist_elem<Vessel>) => {
					let v = e.get();
					if (v instanceof Vessel && !(v instanceof Torpedo) 
						&& !(v instanceof Submarine)) {
						if (hit(tpos, v)) {
							ves = v;
						}
					}
				});
				if (ves) {
					this.explodes.push(new Explode(torpedo.position.xy));
					e.remove();
				} else if (torpedo.checkRange()) {
					v.update();
				} else {
					e.remove();
				}
			} else {
				v.update();
			}
		});
	}

	spawnVessel(className: string, opt: { [key: string]: any } = {}) {
		this.queryObjects.push(className);
		let self = this;
		new Vessel(className, (v: Vessel) => {
			self.vessels.push(v);
			// console.log(opt);
			if ("position" in opt) {
				v.position = glm.vec3(-opt.position[0], -opt.position[1], 0)["*"](m2screen);
			}
			if ("target" in opt) {
				v.speedAngle = opt.target;
			}
			if ("speed" in opt) {
				v.initSpeed = opt.speed;
			}
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
			if ("depth" in opt) {
				v.position = v.position["+"](glm.vec3(0, 0, -opt.depth)["*"](m2screen));
			}
			if ("target" in opt) {
				v.speedAngle = opt.target;
			}
			if ("speed" in opt) {
				v.initSpeed = opt.speed;
			}
			self.queryObjects.splice(self.queryObjects.indexOf(className), 1);
			self.checkLoadingFinish();
			self.viewports.periscope.configureUboat(v);
			self.periscreen.configureUboat(v);
		});
	}

	spawnTorpedo(className: string, launcher: number) {
		let self = this;
		new Torpedo(className, (v: Torpedo) => {
			let x = self.uboat.torpedoLauncher[launcher];
			let p = glm.vec4(x.position[0], x.position[1], x.position[2], 1);
			let m = this.uboat.getTrans();
			v.position = m["*"](p).xyz;
			v.speed = this.uboat.speed * 1.5;
			v.targetSpeed = 1;
			let u = glm.vec4(x.orient[0], x.orient[1], x.orient[2], 0);
			v.orient = glm.normalize(m["*"](u).xy);
			self.vessels.push(v);
		});
	}

	checkLoadingFinish() {
		if (!this.queryObjects.length) {
			this.finishLoading();
		}
	}
	
	setMode(mode: string) {
		if (this.displayMode == "periscope" && mode != "periscope") {
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
				this.displayMode = "periscope"; 
				this.engine3d.setCamera(this.viewports.periscope);
			} break;
			default: {
				throw "unknown mode: " + mode;
			}
		}
	}

	beginBattle(opt: { [key: string]: any } = {}) {
		this.displayMode = "splash";
		this.queryObjects = [];

		for (let x of opt.sea) {
			this.spawnVessel(x.className, {
				position: x.position,
				target: x.target,
				speed: x.speed
			});
		} 
		this.resetUboat(opt.uboat.className, {
			target: opt.uboat.target,
			speed: opt.uboat.speed,
			depth: opt.uboat.depth
		});

		Renderer.timescale = 1;
		this.engine3d.ambient.setTime(opt.ambient.time);
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

					"z": () => this.spawnTorpedo("TII_G7e", 0),

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
					"d": () => this.uboat.rudderSignal(1)
				};
				if (e.key.toLowerCase() in lookup) {
					lookup[e.key.toLowerCase()] ();
				}
			}
		});
		this.battleMouses = Renderer.instance.dispatch("mousedown", (e: MouseEvent) => {
			switch (this.displayMode) {
				case "periscope": {
					// console.log(e.clientX, e.clientY);
					if (inRect(e.clientX, e.clientY, 858 + 14, 572 + 42, 938 + 14, 612 + 42)) {
						this.viewports.periscope.rise();
						this.periscreen.periscopeState(1);
					} else if (inRect(e.clientX, e.clientY, 858 + 14, 612 + 42, 938 + 14, 652 + 42)) {
						this.viewports.periscope.down();
						this.periscreen.periscopeState(0);
					}
				} break;
			}
		});
		this.battleMousesUp = Renderer.instance.dispatch("mouseup", (e: MouseEvent) => {
			switch (this.displayMode) {
				case "periscope": {
					this.viewports.periscope.stop();
					this.periscreen.periscopeState(2);
				} break;
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
		this.battleMouses.cancel();
		this.battleMousesUp.cancel();
	}
}

export {
	Game
}
