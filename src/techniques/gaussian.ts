import { PostProcess } from "./postProcess";
import { Shader } from "../gl/shader";

class Gaussian extends PostProcess {
	constructor() {
		super(Shader.create("gaussian", false));
	}
}

export {
	Gaussian
}