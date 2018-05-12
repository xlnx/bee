import { Vessel, m2screen } from "./vessel";
import { glm } from "../../util/glm"

type SpeedMode = "stop" | "slowFw" | "oneThirdFw" | "normalFw" | "fullFw" |
	"extraFw" | "slowBw" | "oneThirdBw" | "normalBw" | "extraBw"

type RudderMode = "fullLw" | "fullRw" | "reset"

type DiveMode = "Uw" | "Dw" | "extraUw" | "extraDw" | "keep" | "Pw"

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

	setSpeedMode(mode: SpeedMode) {
		const lookup = {
			"stop": () => this.speed = 0,
			"slowFw": () => this.speed = 1,
			"oneThirdFw": () => this.speed = 2,
			"normalFw": () => this.speed = 6,
			"fullFw": () => this.speed = 8,
			"extraFw": () => this.speed = 10,
			"slowBw": () => this.speed = -.4,
			"oneThirdBw": () => this.speed = -.1,
			"normalBw": () => this.speed = -3,
			"extraBw": () => this.speed = -4
		};
		lookup[mode] ();
	}

	setRudderMode(mode: RudderMode) {
		const lookup = {
			"Lw": () => this.angleSpeed = -1,
			"Rw": () => this.angleSpeed = 1,
			"reset": () => this.angleSpeed = 0,
		};
		lookup[mode] ();
	}

	setDiveMode(mode: DiveMode) {
		const lookup = {
			"Uw": () => this.diveSpeed = -1,
			"Dw": () => this.diveSpeed = 1,
			"keep": () => this.diveSpeed = 0,
			"extraUw": () => this.diveSpeed = -3,
			"extraDw": () => this.diveSpeed = 3
		};
		lookup[mode] ();
	}
}

export {
	Submarine
}
