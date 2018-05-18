import { PostProcess } from "./postProcess";
import { Shader } from "../gl/shader";

class FFT extends PostProcess {
	constructor() {
		super(Shader.create("playground/fftsrc", false));
	}
}

export {
	FFT
}