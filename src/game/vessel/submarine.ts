import { Vessel, m2screen } from "./vessel";
import { glm } from "../../util/glm"
import { Renderer } from "../../renderer/renderer";

type SpeedMode = "stop" | "slowFw" | "oneThirdFw" | "normalFw" | "fullFw" |
	"extraFw" | "slowBw" | "oneThirdBw" | "normalBw" | "extraBw"

type RudderMode = "fullLw" | "fullRw" | "reset"

type DiveMode = "Uw" | "Dw" | "extraUw" | "extraDw" | "keep" | "Pw"

class Submarine extends Vessel {
	private fdepth: number = 0;
	private fdiveSpeed: number = 0;
	private fmaxDiveSpeed: number;
	private frudderState: number = 0;
	private frudderMiddle: boolean = false;

	get diveSpeed(): number {
		return this.fdiveSpeed / m2screen;
	}
	set diveSpeed(meters: number) {
		this.fdiveSpeed = m2screen * meters;
	}

	protected processProperty(data: { [key: string]: any}) {
		super.processProperty(data);
		this.fmaxDiveSpeed = data.maxDiveSpeed * m2screen;
	}

	protected updateState() {
		if (this.frudderMiddle) {
			this.rudderMiddle();
		} else switch (this.frudderState) {
			case 3: case 2: this.rudderLeft(); break;
			case 1: case 4: this.rudderRight(); break;
		}
		super.updateState();
	}

	protected updatePosition() {
		if (this.fdiveSpeed || this.fspeed) {
			let diff = glm.vec3(this.speedVec["*"](this.fspeed * Renderer.dt), -this.fdiveSpeed * Renderer.dt);
			this.translate(diff);
			if (this.camera) {
				this.camera.position = this.camera.position["+"](diff);
			}
		}
	}

	rudderSignal(signal: number) {
		console.log(this.frudderState);
		if (signal) {
			switch (this.frudderState) {
				case 0: this.frudderState = signal == 1 ? 1 : 2; break;
				case 1: this.frudderState = signal == 1 ? 0 : 3; break;
				case 2: this.frudderState = signal == 1 ? 4 : 0; break;
				case 3: case 4: this.frudderState = signal == 1 ? 2 : 1; break;
			}
		} else {
			this.frudderMiddle = !this.frudderMiddle;
		}
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
