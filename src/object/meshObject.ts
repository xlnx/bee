import Obj from "../scene/object"
import { Viewport } from "../scene/viewport";
import { Shader } from "../gl/shader";

export default class MeshObj extends Obj {

	render(viewport: Viewport, shader: Shader) {
		shader.use();
		this.setBasicUniforms(viewport);
		// mesh.render();
		shader.unuse();
	}
}

