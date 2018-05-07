import { PostProcess } from "./postProcess";
import { Shader } from "../gl/shader";
import { glm } from "../util/glm"
import { Renderer } from "../renderer/renderer";

class GaussBlur extends PostProcess {
	constructor(public range: number, public step = 2) {
		super(Shader.create("gaussBlur", false));
	}

	beforeRender() {
		this.gRange.set(this.range);
		this.gStep.set(this.step);
		this.iResolution.set(glm.vec2(Renderer.instance.canvas.width,
			Renderer.instance.canvas.height));
	}

	private gRange = Shader.uniform("int", "gRange");
	private gStep = Shader.uniform("float", "gStep");
	private iResolution = Shader.uniform("vec2", "iResolution");
}

export {
	GaussBlur
}
