import { glm } from "../util/glm"
import { PostProcess } from "./postProcess";
import { Shader } from "../gl/shader";
import { Offscreen, RenderBuffer } from "./offscreen";
import { gl, Renderer } from "../renderer/renderer";
import { Texture2D } from "../gl/texture";

class NormalHeight extends PostProcess {
	constructor() {
		super(Shader.create("playground/normalHeight", false));
	}

	beforeRender() {
		this.gStep.set(glm.vec2(
			1/Renderer.instance.canvas.width, 
			1/Renderer.instance.canvas.height));
		super.beforeRender();
	}

	private gStep = Shader.uniform("vec2", "gStep");
}

export {
	NormalHeight
}