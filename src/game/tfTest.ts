import { Renderer, gl } from "../renderer/renderer";
import { Viewport, PerspectiveViewport } from "../gl/viewport";
import { TransformFeedback } from "../techniques/transformFeedback";

class TFTest {
	private renderer = new Renderer(document.body);

	private viewport = new PerspectiveViewport();

	private tf = new TransformFeedback();

	constructor() {
		gl.clearColor(0, 0, 0, 0);
		gl.clearDepth(1);
		gl.enable(gl.DEPTH_TEST);
	}

	start() {
		this.viewport.use();

		this.tf.bindShader();
			this.tf.render(this.viewport);
		this.tf.unbindShader();

		this.viewport.unuse();
	}
}

export {
	TFTest
}
