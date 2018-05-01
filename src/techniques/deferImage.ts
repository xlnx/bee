import { PostProcess } from "./postProcess";
import { Shader } from "../gl/shader";


class DeferImage extends PostProcess {
	constructor() {
		super(Shader.create("deferImage", false));
	}
}

export {
	DeferImage
}
