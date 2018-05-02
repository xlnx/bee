import { glm } from "../util/glm"
import { Communicator } from "../gl/communicator";

class GerstnerWave extends Communicator {
	constructor() {
		super("GerstnerWave");
		this.init({
			steepness: {
				type: "float",
				init: .5,
				set: (value: number) => {
					return value / (this.get("frequency") * this.get("amplitude"));
				}
			},
			amplitude: {
				type: "float",
				init: .2
			},
			frequency: {
				type: "float",
				init: 5
			},
			direction: {
				type: "vec2",
				init: glm.vec2(1, 0),
				set: (value: glm.vec2) => {
					return glm.normalize(value);
				}
			},
			speed: {
				type: "float",
				init: 1
			}
		});
	}
}

export {
	GerstnerWave
}

