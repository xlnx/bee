import { PostProcess } from "./postProcess";
import { Shader } from "../gl/shader";

class ScreenSpaceReflection extends PostProcess {
	constructor() {
		super(Shader.create("SSR", false));
	}
}

export {
	ScreenSpaceReflection
}
