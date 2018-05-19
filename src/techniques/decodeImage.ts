import { PostProcess } from "./postProcess";
import { Shader } from "../gl/shader";


class DecodeImage extends PostProcess {
	constructor() {
		super(Shader.create("decodeImage", false));
	}
}

export {
	DecodeImage
}
