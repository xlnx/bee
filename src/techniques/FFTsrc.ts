import { PostProcess } from "./postProcess";
import { Shader } from "../gl/shader";

class FFTsrc extends PostProcess {
	constructor() {
		super(Shader.create("fftsrc", false));
	}
}

export {
	FFTsrc
}