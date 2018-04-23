import Obj from "../scene/object"
import { Viewport } from "../scene/viewport";
import { Shader } from "../gl/shader";
import Model from "../gl/model";

export default class ModelObj extends Obj {
	constructor(private model: Model) {
		super();
	}

	render(viewport: Viewport, shader: Shader) {
		shader.use();
			this.setViewMatrices(viewport);
			this.model.render();
		shader.unuse();
	}
}
