import { PostProcess } from "../techniques/postProcess";
import { Shader } from "../gl/shader";
import { glm } from "../util/glm"
import { CameraBase } from "./camera/cameraBase";

class GameRenderer extends PostProcess {
	constructor() {
		super(Shader.create("game", false));
	}

	beforeRender() {
		this.gP.set(this.viewport.getPers());
		this.gIP.set(glm.inverse(this.viewport.getPers()));
	}

	private gP = Shader.uniform("mat4", "gP");
	private gIP = Shader.uniform("mat4", "gIP");
	public viewport: CameraBase = null;
}

export {
	GameRenderer
}
