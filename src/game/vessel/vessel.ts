import Obj from "../../object/object";
import Model from "../../gl/model";
import { Viewport } from "../../gl/viewport";
import { glm } from "../../util/glm"
import { CameraBase } from "../camera/cameraBase";
import xhr from "../../util/xhr";
import { Shader } from "../../gl/shader";
import { Smoke } from "./smoke";
import { Track } from "./track";
import { Renderer } from "../../renderer/renderer";

const m2screen = .1 * .5;
const knots2mpers = 0.514444;

const EPS = .01;

abstract class VesselBase extends Obj {
	// protected speed = glm.vec2(1, 0);
	protected fmaxSpeed: number;
	protected fspeed: number = 0;
	protected speedVec = glm.vec2(0, 1);
	protected fspeedAngle: number = glm.radians(0);
	protected frudderOrient: number = 0;
	protected fmaxRudderOrient: number;
	protected frudderSpeed: number;

	protected fdtSpeed: number = -1;
	protected foriginalSpeed: number = 0;
	protected ftargetSpeed: number = 0;

	protected camera: CameraBase = null;

	get speedAngle(): number {
		return glm.degree(this.fspeedAngle);
	}
	set speedAngle(degree: number) {
		this.fspeedAngle = glm.radians(degree);
		this.speedVec = glm.vec2(Math.cos(this.fspeedAngle), Math.sin(this.fspeedAngle));
	}

	set speed(knots: number) {
		this.fspeed = m2screen * knots * knots2mpers;
	}
	get speed(): number {
		return this.fspeed / knots2mpers / m2screen;
	}

	set targetSpeed(perc: number) {
		this.fdtSpeed = 0;
		this.foriginalSpeed = this.fspeed;
		this.ftargetSpeed = perc * this.fmaxSpeed;
	}

	set rudderOrient(orient: number) {
		this.frudderOrient = orient;
		if (Math.abs(this.frudderOrient) > this.fmaxRudderOrient) {
			this.frudderOrient = Math.sign(this.frudderOrient) * this.fmaxRudderOrient;
		}
	}

	get rudderOrient(): number {
		return this.frudderOrient;
	}

	rudderRight() {
		this.rudderOrient += Renderer.dt * this.frudderSpeed;
	}

	rudderLeft() {
		this.rudderOrient -= Renderer.dt * this.frudderSpeed;
	}

	rudderMiddle() {
		if (this.frudderOrient != 0) {
			let sgn = Math.sign(this.frudderOrient);
			this.rudderOrient -= sgn * Renderer.dt * this.frudderSpeed;
			if (Math.sign(this.frudderOrient) != sgn) {
				this.frudderOrient = 0;
			}
		}
	}

	protected updateState() {
		if (this.fdtSpeed >= 0) {
			// console.log(this.fdtSpeed);
			this.fdtSpeed += Renderer.dt;
			this.fspeed = (this.ftargetSpeed - this.foriginalSpeed) * 
				Math.tanh(this.fdtSpeed / 5. * Math.sqrt(Math.abs(this.ftargetSpeed - this.foriginalSpeed))) +
					this.foriginalSpeed;
			if (Math.abs(this.fspeed - this.ftargetSpeed) < EPS) {
				this.fspeed = this.ftargetSpeed;
				this.fdtSpeed = -1;
			}
		}
		if (this.frudderOrient && this.fspeed) {
			let theta = - this.fspeed / this.fmaxSpeed * .1 * this.frudderOrient * Renderer.dt;
			let sinv = Math.sin(theta);
			let cosv = Math.cos(theta);
			this.speedVec = glm.vec2(
				this.speedVec.x * cosv - this.speedVec.y * sinv,
				this.speedVec.x * sinv + this.speedVec.y * cosv
			);
			this.rotate(glm.vec3(0, 0, theta));
		}
	}

	protected updatePosition() {
		if (this.fspeed) {
			let diff = glm.vec3(this.speedVec["*"](this.fspeed * Renderer.dt), 0);
			this.translate(diff);
			if (this.camera) {
				this.camera.position = this.camera.position["+"](diff);
			}
		}
	}

	update() {
		// console.log(this);
		this.updateState();
		this.updatePosition();
	}
	
	tie(camera: CameraBase) {
		this.camera = camera;
	}
}

class Vessel extends VesselBase {
	private static modelPath = "./assets/";
	private static shader: Shader;
	private model: Model;

	public smokes: Smoke[] = [];
	public tracks: Track[] = [];
	
	constructor(name: string)
	constructor(name: string, callback: (v: Vessel) => void);
	constructor(name: string, callback?: (v: Vessel) => void) {
		super();

		this.scale(.5);
		let self = this;
		xhr.getAsync(Vessel.modelPath + name + "/property.json", "text", (err: any, data: any) => {
			self.processProperty(JSON.parse(data));
		});
		Vessel.shader = Vessel.shader || Shader.create("vessel", false);
		if (callback == undefined) {
			this.model = Model.create(Vessel.modelPath + name + "/", name + ".json");
		} else {
			Model.create(Vessel.modelPath + name + "/", name + ".json", (v: Model) => {
				self.model = v;
				callback(self);
			});
		}
	}

	protected processProperty(data: { [key: string]: any}) {
		this.fmaxSpeed = m2screen * knots2mpers * data.maxSpeed;
		this.fmaxRudderOrient = data.maxRudderOrient;
		this.frudderSpeed = data.rudderSpeed;
		if ("particleGenerator" in data) {
			for (let emitter of data.particleGenerator) {
				switch (emitter.type) {
				case "smoke": {
					this.smokes.push(new Smoke(emitter, this));
				} break;
				case "track": {
					this.tracks.push(new Track(emitter, this));
				}
				} 
			}
		}
	}

	render(viewport: Viewport) {
		this.setBasicUniforms(viewport);
		this.model.render();
	}
	
	static bindShader() {
		Vessel.shader.use();
	}
	static unbindShader() {
		Vessel.shader.unuse();
	}
}

export {
	knots2mpers,
	m2screen,
	Vessel
}
