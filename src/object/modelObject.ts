import Obj from "./object"
import { Viewport } from "../gl/viewport";
import { Shader } from "../gl/shader";
import Model from "../gl/model";

export default class ModelObj extends Obj {
	constructor(private model: Model) {
		super();
		this.defaultShader = Shader.create("model", true);
	}

	render(viewport: Viewport) {
		this.setBasicUniforms(viewport);
		this.model.render();
	}
}
