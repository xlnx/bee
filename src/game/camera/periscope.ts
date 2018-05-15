import { CameraBase } from "./cameraBase";
import { glm } from "../../util/glm"
import { Renderer } from "../../renderer/renderer";

class Periscope extends CameraBase {
	private static speed: number = .2;
	private static maxH: number = 1.;
	private static minH: number = -1.;

	private risev: number;
	private diff: number = 0;

	constructor() {
		super();
		this.fov = glm.radians(5);
	}

	bind() {

	}

	unbind() {

	}

	rise() {
		this.risev = 1;
	}

	down() {
		this.risev = -1;
	}

	stop() {
		this.risev = 0;
	}

	update() {
		if (this.risev) {
			let d = this.risev * 
				Periscope.speed * Renderer.dt;
			this.diff += d;
			let fix = this.checkRange();
			this.diff += fix;
			d += fix;
			this.translate(glm.vec3(0, 0, d));
		}
	}

	private checkRange(): number { 
		if (this.diff > Periscope.maxH) {
			return Periscope.maxH - this.diff;
		} else if (this.diff < Periscope.minH) {
			return Periscope.minH - this.diff;
		} else {
			return 0;
		}
	}

	locate(pos: glm.vec3) {
		console.log(pos);
		this.position = pos["+"](this.diff);
	}
}

export {
	Periscope
}