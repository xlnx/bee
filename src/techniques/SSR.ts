import { PostProcess } from "./postProcess";
import { Shader } from "../gl/shader";
import { PerspectiveViewport } from "../gl/viewport";
import { glm } from "../util/glm"

class SSR extends PostProcess {
	constructor() {
		super(Shader.create("SSR", false));
	}
	
	setUniforms() {
		this.gP.set(this.viewport.getPers());
		this.gIP.set(glm.inverse(this.viewport.getPers()));
	}

	private gP = Shader.uniform("mat4", "gP");
	private gIP = Shader.uniform("mat4", "gIP");
	public viewport: PerspectiveViewport = null;
}

export {
	SSR
}
