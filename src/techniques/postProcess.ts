import { VAO, VertexAttrs } from "../gl/vertexAttrs";
import { Shader } from "../gl/shader";
import { gl } from "../renderer/renderer";

class PostProcess {
	protected vao: VAO;

	constructor(protected shader: Shader) {
		let vertices = new VertexAttrs(["pos2"]);
		vertices.set("pos2", [
			-1, 1, 1, 1, 1, -1, -1, -1
		]);
		this.vao = new VAO(vertices, [
			0, 1, 2, 0, 2, 3
		]);
	}
	render() {
		this.shader.use();
			this.vao.bind();
				this.vao.draw();
			this.vao.unbind();
		this.shader.unuse();
	}
}

export {
	PostProcess
}
