import { PostProcess } from "./postProcess";
import { Shader } from "../gl/shader";
import { Renderer } from "../renderer/renderer";

class PerlinWave extends PostProcess {
	constructor() {
		super(Shader.create("perlinWave", false));
	}

	beforeRender() {
		this.gTime.set(Renderer.time);
	}

	private gTime = Shader.uniform("float", "gTime");
}

export {
	PerlinWave
}
