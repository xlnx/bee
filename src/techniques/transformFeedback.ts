import { gl2 } from "../renderer/renderer";
import { TBO } from "../gl/buffer";

class TransformFeedback {
	public readonly handle: WebGLTransformFeedback;
	private vbo = new TBO();

	constructor() {
		if (!gl2) {
			throw "webgl 2.0 required.";
		}
		this.handle = gl2.createTransformFeedback();
	}

	bind() {
		gl2.bindTransformFeedback(gl2.TRANSFORM_FEEDBACK, this.handle);
	}
	unbind() {
		gl2.bindTransformFeedback(gl2.TRANSFORM_FEEDBACK, null);
	}
}

export {
	TransformFeedback
}