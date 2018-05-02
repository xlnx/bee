import { PostProcess } from "./postProcess";
import { Shader } from "../gl/shader";

class UV extends PostProcess {
	constructor() {
		super(Shader.create("uv", false));
	}
}

export {
	UV
}
