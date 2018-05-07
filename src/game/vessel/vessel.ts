import ModelObj from "../../object/modelObject";
import Obj from "../../object/object";
import Model from "../../gl/model";
import { Viewport } from "../../gl/viewport";

class Vessel extends Obj {
	private model: Model;
	
	constructor(name: string) {
		if (!Vessel.dummy) {
			Vessel.dummy = new ModelObj(null);
		}

		super();
		this.scale(.5);
		this.model = Model.create(name + ".json");
	}

	render(viewport: Viewport) {
		this.setBasicUniforms(viewport);
		this.model.render();
	}
	
	static bindShader() {
		Vessel.dummy.bindShader();
	}
	static unbindShader() {
		Vessel.dummy.unbindShader();
	}

	private static dummy: ModelObj = null;
}

export {
	Vessel
}
