import Obj from "../scene/object";
import { Viewport } from "../scene/viewport";
import { Shader } from "../gl/shader";
import { VAO, VertexAttrs } from "../gl/vertexAttrs";

class Skybox extends Obj {
	private vao: VAO;

	public readonly isSkybox = true;
	
	constructor() {
		super();
		let vertices = new VertexAttrs(["pos3"]);
		let t = 9e3;
		vertices.set("pos3",[
			-t, -t, -t,
			-t, -t, t,
			-t, t, -t, 
			-t, t, t,
			t, -t, -t,
			t, -t, t,
			t, t, -t,
			t, t, t
		]);
		this.vao = new VAO(vertices, [
			0, 1, 2, 
			1, 3, 2,
			1, 7, 3,
			1, 5, 7,
			1, 0, 4,
			1, 4, 5,
			3, 7, 6,
			3, 6, 2,
			0, 2, 6,
			0, 6, 4,
			5, 4, 6,
			5, 6, 7
		]);
	}

	render(viewport: Viewport, shader: Shader) {
		this.shader.use();
			this.setBasicUniforms(viewport);
			this.vao.bind();
				this.vao.draw();
			this.vao.unbind();
		this.shader.unuse();
	}
	
	private shader = Shader.create("skybox");
}

export {
	Skybox
}
