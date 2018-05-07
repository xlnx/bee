import { Vessel, m2screen } from "./vessel";
import { glm } from "../../util/glm"

class Submarine extends Vessel {
	private fdepth: number = 0;
	private fdiveSpeed: number = 0;

	get diveSpeed(): number {
		return this.fdiveSpeed / m2screen;
	}
	set diveSpeed(meters: number) {
		this.fdiveSpeed = m2screen * meters;
	}

	update(dt: number) {
		if (this.fdiveSpeed || this.fspeed) {
			let diff = glm.vec3(this.speedVec["*"](this.fspeed * dt), -this.fdiveSpeed * dt);
			this.translate(diff);
			if (this.camera) {
				this.camera.position = this.camera.position["+"](diff);
			}
		}
	}
}

export {
	Submarine
}
