import { gl, Renderer } from "../../renderer/renderer";
import { PostProcess } from "../../techniques/postProcess";
import { Shader } from "../../gl/shader";
import { Texture2D } from "../../gl/texture";

class Splash extends PostProcess {
	private bg: Texture2D;

	constructor(image: string) {
		super(Shader.create("splash", false));
		this.bg = new Texture2D("./assets/splash/" + image);
	}

	beforeRender() {
		gl.viewport(0, 0, Renderer.instance.canvas.width, 
			Renderer.instance.canvas.height);
		this.bg.use("Image");
	}
	afterRender() {
		this.bg.unuse();
	}
}

export {
	Splash
}
