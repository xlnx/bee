import Obj from "./object";
import { Shader } from "../gl/shader";
import { Viewport } from "../gl/viewport";
import Mesh from "../gl/mesh";
import { VertexAttrs } from "../gl/vertexAttrs";

class SquareMesh extends Obj {
	constructor(shader: string, n: number) {
		super();
		this.defaultShader = Shader.create(shader, true);
		let vertices = new VertexAttrs(["pos2"]);
		for (let i = 0; i <= n; ++i) {
			for (let j = 0; j <= n; ++j) {
				vertices.push({
					pos2: [i * 2 / n - 1, j * 2 / n - 1]
				});
			}
		}
		let indices = [];
		for (let i = 0; i != n; ++i) {
			for (let j = 0; j != n; ++j) {
				indices.push(
					i * (n + 1) + j, 
					i * (n + 1) + j + 1, 
					(i + 1) * (n + 1) + j
				);
				indices.push(
					i * (n + 1) + j + 1, 
					(i + 1) * (n + 1) + (j + 1), 
					(i + 1) * (n + 1) + j
				);
			}
		}
		this.mesh = new Mesh(vertices, indices);
	}

	render(viewport: Viewport) {
		this.setBasicUniforms(viewport);
		this.mesh.render();
	}

	private mesh: Mesh;
}

export {
	SquareMesh
}