import { PostProcess } from "../../techniques/postProcess";
import { Texture2D } from "../../gl/texture";
import { gl } from "../../renderer/renderer";
import { Shader } from "../../gl/shader";

class Options extends PostProcess {
	public readonly bg: Texture2D = new Texture2D({ component: gl.RGB });

	constructor() {
		super(Shader.create("options", false));
	}

	beforeRender() {
		this.bg.use("Image");
	}
	afterRender() {
		this.bg.unuse();
	}
}

export {
	Options
}
