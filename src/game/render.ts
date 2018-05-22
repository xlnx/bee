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
		this.gV.set(this.viewport.getView());
		this.gIVP.set(glm.inverse(this.viewport.getTrans()));
		this.gCameraWorldPos.set(this.viewport.getCameraPosition());
	}

	private gP = Shader.uniform("mat4", "gP");
	private gV = Shader.uniform("mat4", "gV");
	private gIVP = Shader.uniform("mat4", "gIVP");
	private gCameraWorldPos = Shader.uniform("vec3", "gCameraWorldPos");

	public viewport: CameraBase = null;
}

export {
	GameRenderer
}
