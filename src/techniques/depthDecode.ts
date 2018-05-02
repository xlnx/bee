import { PostProcess } from "./postProcess";
import { Shader } from "../gl/shader";

class DepthDecode extends PostProcess {
	constructor() {
		super(Shader.create("depthDecode", false));
	}
}

export {
	DepthDecode
}
