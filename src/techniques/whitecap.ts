import { PostProcess } from "./postProcess";
import { Shader } from "../gl/shader";
import { Renderer } from "../renderer/renderer";

class Whitecap extends PostProcess {
	constructor() {
		super(Shader.create("playground/whitecap", false));
	}

	beforeRender() {
		// this.gTime.set(Renderer.time);
	}

	// private gTime = Shader.uniform("int", "gTime");
}

export {
	Whitecap
}