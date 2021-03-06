import { PostProcess } from "./postProcess";
import { Shader } from "../gl/shader";

class Phillips extends PostProcess {
	constructor() {
		super(Shader.create("phillips", false));
	}
}

export {
	Phillips
}