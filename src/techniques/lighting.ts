import { Communicator } from "../gl/communicator";
import { glm } from "../util/glm";

class LightBase extends Communicator {
	constructor(type: string = undefined) {
		super(type ? type: "LightBase");
		this.init({
			color: {
				type: "vec3",
				init: glm.vec3(1, 1, 1)
			}, 
			ambientIntensity: {
				type: "float",
				init: .5
			},
			diffuseIntensity: {
				type: "float",
				init: 1
			}
		});
	}
}

class PointLight extends LightBase {
	constructor(type: string = undefined) {
		super(type ? type : "PointLight");
		this.init({
			attenConstant: {
				type: "float",
				init: 1
			},
			attenLinear: {
				type: "float",
				init: .5
			},
			attenExp: {
				type: "float",
				init: .5
			},
			position: {
				type: "vec3",
				init: glm.vec3(0, 0, 1)
			}
		});
	}
}

export {
	PointLight
}
