import { Shader } from "../gl/shader";
import { VertexAttrs, VAO } from "../gl/vertexAttrs";

class DeferImage {
	private vao: VAO;

	constructor() {
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
	render() {
		this.shader.use();
			this.vao.bind();
				this.vao.draw();
			this.vao.unbind();
		this.shader.unuse();
	}

	private shader = Shader.create("deferImage", false);
}

export {
	DeferImage
}
