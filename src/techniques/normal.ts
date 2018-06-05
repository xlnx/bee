import { PostProcess } from "./postProcess";
import { Shader } from "../gl/shader";

class Normal extends PostProcess {
	constructor(public readonly N: number = 256) {
		super(Shader.create("normal", false));
	}

	beforeRender() {
		this.gN.set(this.N);
	}

	private gN = Shader.uniform("float", "gN");
}

export {
	Normal
}