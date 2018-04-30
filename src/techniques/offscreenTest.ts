import Obj from "../scene/object";
import { VAO, VertexAttrs } from "../gl/vertexAttrs";
import { Shader, Uniform } from "../gl/shader";
import { Viewport } from "../scene/viewport";
import { gl } from "../renderer/renderer";
import { glm }  from "../util/glm"

class TestScreen extends Obj {
	private vao: VAO;
	
	constructor() {
		super();
		this.defaultShader = Shader.create("offscreenTest");
		let vertices = new VertexAttrs(["pos2", "tex2"]);
		vertices.set("pos2", [
			-1, 1, 1, 1, 1, -1, -1, -1
		]);
		vertices.set("tex2", [
			0, 1, 1, 1, 1, 0, 0, 0
		]);
		this.vao = new VAO(vertices, [
			0, 1, 2, 0, 2, 3
		]);
	}

	render(viewport: Viewport) {
		this.gFace.set(this.face);
		this.vao.bind();
			this.vao.draw();
		this.vao.unbind();
	}

	private gFace: Uniform = Shader.uniform("vec3", "gFace");

	public face: glm.vec3 = glm.vec3(-1, 0, 0);
}

export {
	TestScreen
}
