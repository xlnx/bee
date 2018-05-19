import { PostProcess } from "./postProcess";
import { Shader } from "../gl/shader";

class FFTsrc extends PostProcess {
	constructor() {
		super(Shader.create("playground/fftsrc", false));
	}
}

export {
	FFTsrc
}