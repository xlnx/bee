import { PostProcess } from "./postProcess";
import { Shader } from "../gl/shader";

class EncodeImage extends PostProcess {
	constructor() {
		super(Shader.create("encodeImage", false));
	}
}

export {
	EncodeImage
}