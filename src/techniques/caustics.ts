import { SquareMesh } from "../object/squareMesh";
import { Viewport } from "../gl/viewport";
import { Shader } from "../gl/shader";

class Caustics {
	constructor() {
		Shader.require({
			Caustics: {
				vert: "playground/caustics",
				frag: "playground/caustics"
			}
		});
	}

	render(caustics: SquareMesh) {
		Shader.specify("Caustics");
			caustics.bindShader();
				caustics.render(this.fakeViewport);
			caustics.unbindShader();
		Shader.unspecify();
	}

	private fakeViewport = new Viewport();
}

export {
	Caustics
}