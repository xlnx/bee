import Obj from "../../object/object";
import Model from "../../gl/model";
import { Viewport } from "../../gl/viewport";
import { glm } from "../../util/glm"
import { CameraBase } from "../camera/cameraBase";
import xhr from "../../util/xhr";
import { Shader } from "../../gl/shader";
import { Smoke } from "./smoke";

const m2screen = .5 * .1;
const knots2mpers = 0.514444;

abstract class VesselBase extends Obj {
	// protected speed = glm.vec2(1, 0);
	protected fspeed: number = 0;
	protected speedVec = glm.vec2(0, 1);
	protected fspeedAngle: number = glm.radians(0);
	protected fangleSpeed: number = glm.radians(0);

	protected camera: CameraBase = null;

	get speedAngle(): number {
		return glm.degree(this.fspeedAngle);
	}
	set speedAngle(degree: number) {
		this.fspeedAngle = glm.radians(degree);
		this.speedVec = glm.vec2(Math.cos(this.fspeedAngle), Math.sin(this.fspeedAngle));
	}

	get angleSpeed(): number {
		return this.fangleSpeed;
	}
	set angleSpeed(omega: number) {
		this.angleSpeed = omega;
	}

	set speed(knots: number) {
		this.fspeed = m2screen * knots * knots2mpers;
	}
	get speed(): number {
		return this.fspeed / knots2mpers / m2screen;
	}

	update(dt: number) {
		if (this.fspeed) {
			let diff = glm.vec3(this.speedVec["*"](this.fspeed * dt), 0);
			this.translate(diff);
			if (this.camera) {
				this.camera.position = this.camera.position["+"](diff);
			}
		}
	}
	
	tie(camera: CameraBase) {
		this.camera = camera;
	}
}

class Vessel extends VesselBase {
	private static modelPath = "./assets/";
	private static shader: Shader;
	private model: Model;

	public particles: Smoke[] = [];
	
	constructor(name: string)
	constructor(name: string, callback: (v: Vessel) => void);
	constructor(name: string, callback?: (v: Vessel) => void) {
		super();

		let self = this;
		xhr.getAsync(Vessel.modelPath + name + "/property.json", "text", (err: any, data: any) => {
			data = JSON.parse(data);
			for (let emitter of data.particleGenerator) {
				self.particles.push(new Smoke(emitter, this));
			}
		});
		// this.scale(.5);
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
