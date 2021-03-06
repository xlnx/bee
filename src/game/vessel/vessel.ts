import Obj from "../../object/object";
import Model from "../../gl/model";
import { Viewport } from "../../gl/viewport";
import { glm } from "../../util/glm"
import { CameraBase } from "../camera/cameraBase";
import xhr from "../../util/xhr";
import { Shader } from "../../gl/shader";
import { Smoke } from "./smoke";
import { Renderer } from "../../renderer/renderer";
import { Foam } from "./foam";
import { VAO, VertexAttrs } from "../../gl/vertexAttrs";

const m2screen = .1 * .5;
const knots2mpers = 0.514444;

const EPS = .01;

abstract class VesselBase extends Obj {
	// protected speed = glm.vec2(1, 0);
	protected fmaxSpeed: number;
	protected fspeed: number = 0;
	protected fspeedVec = glm.vec2(0, 1);
	protected fspeedAngle: number = glm.radians(0);
	protected frudderOrient: number = 0;
	protected fmaxRudderOrient: number;
	protected frudderSpeed: number;

	protected fdtSpeed: number = -1;
	protected foriginalSpeed: number = 0;
	protected ftargetSpeed: number = 0;

	protected fjitterAngle: number;
	protected fjitterCycle: number;

	protected camera: CameraBase = null;

	get speedAngle(): number {
		return glm.degree(this.fspeedAngle);
	}
	set speedAngle(degree: number) {
		this.fspeedAngle = glm.radians(degree);
		this.fspeedVec = glm.vec2(Math.cos(this.fspeedAngle), Math.sin(this.fspeedAngle));
		this.rotateIdentity();
		this.rotate(glm.vec3(0, 0, glm.radians(degree - 90)));
	}

	set speed(knots: number) {
		this.fspeed = m2screen * knots * knots2mpers;
	}
	get speed(): number {
		return this.fspeed / knots2mpers / m2screen;
	}

	set initSpeed(perc: number) {
		this.foriginalSpeed = this.ftargetSpeed = this.fspeed = perc * this.fmaxSpeed;
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
			this.fspeedVec = glm.vec2(
				this.fspeedVec.x * cosv - this.fspeedVec.y * sinv,
				this.fspeedVec.x * sinv + this.fspeedVec.y * cosv
			);
			this.rotate(glm.vec3(0, 0, theta));
		}
	}

	protected updatePosition() {
		if (this.fspeed) {
			let diff = glm.vec3(this.fspeedVec["*"](this.fspeed * Renderer.dt), 0);
			this.translate(diff);
			if (this.camera) {
				this.camera.position = this.camera.position["+"](diff);
			}
		}
	}

	protected jitter() {
		this.rotateIdentity(glm.vec3(1, 0, 1));
		this.rotate(glm.vec3(0, Math.sin(Renderer.time * this.fjitterCycle) * this.fjitterAngle, 0));
	}

	update() {
		// console.log(this);
		this.updateState();
		this.jitter();
		this.updatePosition();
	}
	
	tie(camera: CameraBase) {
		this.camera = camera;
	}
}

class Vessel extends VesselBase {
	private static modelPath = "./assets/";
	private static shader: Shader;
	private static wrapper: Shader;
	private static properties: { [key: string]: any } = {};
	private model: Model;

	// private wao: VAO;

	public smokes: Smoke[] = [];
	public collider: number[] = [];
	public foam: Foam;
	
	constructor(name: string)
	constructor(name: string, callback: (v: Vessel) => void);
	constructor(name: string, callback?: (v: Vessel) => void) {
		super();

		this.scale(.5);
		let self = this;
		if (name in Vessel.properties) {
			self.processProperty(Vessel.properties[name]);
		} else {
			xhr.getAsync(Vessel.modelPath + name + "/property.json", "text", (err: any, data: any) => {
				data = JSON.parse(data);
				Vessel.properties[name] = data;
				self.processProperty(data);
			});
		}
		this.foam = new Foam({
			lifetime: 5,
			count: 50,
			position: [0, -3.1]
		}, this);
		Vessel.shader = Vessel.shader || Shader.create("vessel", false);
		Vessel.wrapper = Vessel.wrapper || Shader.create("wrapper", false);
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
		if ("jitter" in data) {
			this.fjitterAngle = glm.radians(data.jitter.angle);
			this.fjitterCycle = Math.PI * 2 / data.jitter.cycle;
		}
		if ("rudder" in data) {
			this.fmaxRudderOrient = data.rudder.maxOrient;
			this.frudderSpeed = data.rudder.speed;
		}
		if ("particleGenerator" in data) {
			for (let emitter of data.particleGenerator) {
				this.smokes.push(new Smoke(emitter, this));
			}
		}
		if ("collider" in data) {
			this.collider = data.collider;
			// let attrs = new VertexAttrs(["pos3"]);
			// for (let i = 0; i < data.collider.length - 2; i += 2) {
			// 	attrs.push({ pos3: [
			// 		data.collider[i], 
			// 		data.collider[i + 1],
			// 		0.25
			// 	] });
			// 	attrs.push({ pos3: [
			// 		data.collider[i], 
			// 		data.collider[i + 1],
			// 		-0.25
			// 	] });
			// 	attrs.push({ pos3: [
			// 		data.collider[i + 2], 
			// 		data.collider[i + 3],
			// 		0.25
			// 	] });

			// 	attrs.push({ pos3: [
			// 		data.collider[i], 
			// 		data.collider[i + 1],
			// 		-0.25
			// 	] });
			// 	attrs.push({ pos3: [
			// 		data.collider[i + 2], 
			// 		data.collider[i + 3],
			// 		0.25
			// 	] });
			// 	attrs.push({ pos3: [
			// 		data.collider[i + 2], 
			// 		data.collider[i + 3],
			// 		-0.25
			// 	] });

			// 	attrs.push({ pos3: [
			// 		-data.collider[i], 
			// 		data.collider[i + 1],
			// 		0.25
			// 	] });
			// 	attrs.push({ pos3: [
			// 		-data.collider[i], 
			// 		data.collider[i + 1],
			// 		-0.25
			// 	] });
			// 	attrs.push({ pos3: [
			// 		-data.collider[i + 2], 
			// 		data.collider[i + 3],
			// 		0.25
			// 	] });

			// 	attrs.push({ pos3: [
			// 		-data.collider[i], 
			// 		data.collider[i + 1],
			// 		-0.25
			// 	] });
			// 	attrs.push({ pos3: [
			// 		-data.collider[i + 2], 
			// 		data.collider[i + 3],
			// 		0.25
			// 	] });
			// 	attrs.push({ pos3: [
			// 		-data.collider[i + 2], 
			// 		data.collider[i + 3],
			// 		-0.25
			// 	] });
			// }
			// this.wao = new VAO(attrs);
		}
	}

	render(viewport: Viewport) {
		this.setBasicUniforms(viewport);
		this.model.render();
		// Vessel.unbindShader();
		// !Vessel.wrapper || (() => {
		// 	Vessel.wrapper.use();
		// 	this.setBasicUniforms(viewport);
		// 	if (this.wao) {
		// 		this.wao.bind();
		// 			this.wao.draw();
		// 		this.wao.unbind();
		// 	}
		// 	Vessel.wrapper.unuse();
		// }) ();
		// Vessel.bindShader();
	}
	
	static bindShader() {
		!Vessel.shader || Vessel.shader.use();
	}
	static unbindShader() {
		!Vessel.shader || Vessel.shader.unuse();
	}
}

export {
	knots2mpers,
	m2screen,
	Vessel
}
