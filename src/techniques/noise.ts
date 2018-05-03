import { PostProcess } from "./postProcess";
import { Shader } from "../gl/shader";

class Noise extends PostProcess {
	constructor() {
		super(Shader.create("noise", false));
	}
}

export {
	Noise
}
