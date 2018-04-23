import { VertexAttrs, VAO } from "./vertexAttrs";

export default class Mesh {
	private readonly vao: VAO;

	constructor(vertices: any, indices: number[] = undefined) {
		if (indices != undefined) {
			this.vao = new VAO(vertices, indices);
		} else {
			this.vao = new VAO(vertices);
		}
	}

	render() {
		this.vao.bind();
			this.vao.draw();
		this.vao.unbind();
	}
}
