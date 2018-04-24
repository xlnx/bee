import { Communicator } from "../gl/communicator";
import { glm } from "../util/glm";

// import { Communicator } from "../gl/communicator";
// import { Shader, Uniform } from "../gl/shader";

// class LightBase extends Communicator {
// 	private gColor: Uniform;
// 	private gAmbientIntensity: Uniform;
// 	private gDiffuseIntensity: Uniform;

// 	constructor(type: string | undefined) {
// 		super(type ? type : "LightBase");
// 		this.gColor = Shader.uniform("vec3", this.type + "_Color[]");
// 		this.gAmbientIntensity = Shader.uniform("vec3", this.type + "_AmbientIntensity[]");
// 		this.gDiffuseIntensity = Shader.uniform("vec3", this.type + "_DiffuseIntensity[]");
// 	}

// 	invoke(index: number): boolean {
// 		this.gColor.subscribe(index).set(this.color);
// 		this.gAmbientIntensity.subscribe(index).set(this.ambientIntensity);
// 		this.gDiffuseIntensity.subscribe(index).set(this.diffuseIntensity);
// 		return true;
// 	}
	
// 	public diffuseIntensity = 1;
// 	public ambientIntensity = .5;
// 	public color = glm.vec3(1, 1, 1);
// }

// class PointLight extends LightBase {
// 	private gPosition: Uniform;
// 	private gAttenConstant: Uniform;
// 	private gAttenLinear: Uniform;
// 	private gAttenExp: Uniform;

// 	constructor(type: string | undefined) {
// 		super(type ? type : "PointLight");
// 		let a: glm.mat4;
// 		a = glm.mat4(12);
// 	}
// }

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
