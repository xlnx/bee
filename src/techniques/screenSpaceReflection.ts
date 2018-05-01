import { PostProcess } from "./postProcess";
import { Shader } from "../gl/shader";
import { PerspectiveViewport } from "../scene/viewport";
import { glm } from "../util/glm"

class ScreenSpaceReflection extends PostProcess {
	constructor() {
		super(Shader.create("SSR", false));
	}

	render() {
		this.shader.use();
			this.gP.set(this.viewport.getPers());
			this.gIP.set(glm.inverse(this.viewport.getPers()));
			this.vao.bind();
				this.vao.draw();
			this.vao.unbind();
		this.shader.unuse();
	}

	private gP = Shader.uniform("mat4", "gP");
	private gIP = Shader.uniform("mat4", "gIP");
	public viewport: PerspectiveViewport = null;
}

export {
	ScreenSpaceReflection
}
