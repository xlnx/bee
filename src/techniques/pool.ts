import Obj from "../object/object";
import { Shader } from "../gl/shader";
import { Viewport } from "../gl/viewport";
import Mesh from "../gl/mesh";
import { VertexAttrs } from "../gl/vertexAttrs";

class Pool extends Obj {
	constructor() {
		super();
		this.defaultShader = Shader.create("playground/pool", true);
		let vertices = new VertexAttrs(["pos3", "norm3", "tex2"]);
		vertices.set("pos3", [
			1, 1, 1, 
			1, -1, 1, 
			-1, -1, 1, 
			-1, 1, 1, 
			1, 1, -1, 
			1, -1, -1, 
			-1, -1, -1, 
			-1, 1, -1,
			
			1, 1, 1, 
			1, -1, 1, 
			-1, -1, 1, 
			-1, 1, 1, 
			1, 1, -1, 
			1, -1, -1, 
			-1, -1, -1, 
			-1, 1, -1, 

			1, 1, 1, 
			1, -1, 1, 
			-1, -1, 1, 
			-1, 1, 1, 
			1, 1, -1, 
			1, -1, -1, 
			-1, -1, -1, 
			-1, 1, -1, 
		]);
		vertices.set("tex2", [
			1, 1,
			1, 0,
			0, 1,
			0, 0,
			1, 1,
			1, 0,
			0, 1,
			0, 0,

			1, 1,
			1, 0,
			0, 1,
			0, 0,
			1, 1,
			1, 0,
			0, 1,
			0, 0,

			1, 1,
			1, 0,
			0, 1,
			0, 0,
			1, 0,
			1, 1,
			0, 1,
			0, 0,
		]);
		vertices.set("norm3", [
			-1, 0, 0,
			-1, 0, 0,
			1, 0, 0, 
			1, 0, 0,
			-1, 0, 0,
			-1, 0, 0,
			1, 0, 0,
			1, 0, 0,

			0, -1, 0,
			0, 1, 0,
			0, 1, 0, 
			0, -1, 0, 
			0, -1, 0, 
			0, 1, 0,
			0, 1, 0,
			0, -1, 0,
			
			0, 0, -1,
			0, 0, -1,
			0, 0, -1,
			0, 0, -1,
			0, 0, 1,
			0, 0, 1,
			0, 0, 1,
			0, 0, 1,
		]);
		let indices = [
			// 0, 1, 5,
			// 0, 5, 4,
			0 + 8, 4 + 8, 7 + 8,
			0 + 8, 7 + 8, 3 + 8,
			2 + 16, 1 + 16, 0 + 16,
			2 + 16, 0 + 16, 3 + 16,
			6 + 16, 5 + 16, 4 + 16, 
			6 + 16, 4 + 16, 7 + 16,
			2 + 8, 1 + 8, 5 + 8,
			2 + 8, 5 + 8, 6 + 8,
			2, 6, 7,
			2, 7, 3
		];
		this.mesh = new Mesh(vertices, indices);
	}

	render(viewport: Viewport) {
		this.setBasicUniforms(viewport);
		this.mesh.render();
	}

	private mesh: Mesh;
}

export {
	Pool
}