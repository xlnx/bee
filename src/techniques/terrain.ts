import Obj from "../scene/object";
import { Viewport } from "../scene/viewport";
import { Shader } from "../gl/shader";
import { VAO, VertexAttrs } from "../gl/vertexAttrs";
import { gl } from "../renderer/renderer";


class Terrain extends Obj {
	protected mesh: VAO;

	private static cascadeCount = 1;
	private static cascadeMeshCount = 36;
	private static minimumMeshSize = 0.05;

	constructor() {
		super();
		let vertices = new VertexAttrs(["pos2"]);
		let indices = [];
		let meshSize = Terrain.minimumMeshSize;
		let indiceBase = 0;
		let line = Terrain.cascadeMeshCount * 2 + 1;
		for (let k = 0; k != Terrain.cascadeCount; ++k) {
			for (let i = -Terrain.cascadeMeshCount; 
					i <= Terrain.cascadeMeshCount; ++i) {
				for (let j = -Terrain.cascadeMeshCount; 
						j <= Terrain.cascadeMeshCount; ++j) {
					vertices.push({
						pos2: [ j * meshSize, i * meshSize ]
					});
				}
			}
			for (let i = 1; i != line - 2; ++i) {
				for (let j = 1; j != line - 2; ++j) {
					let lt = indiceBase + i * line + j;
					indices.push(lt, lt + 1, lt + line + 1, 
							lt, lt + line + 1, lt + line);
				}
			}
			for (let j = 2; j != line - 2; ++j) {
				let lt = indiceBase + 0 * line + j;
				indices.push(lt, lt + line, lt + line - 1,
						lt, lt + line + 1, lt + line,
							lt, lt + 2, lt + 1 + line);
			}
			for (let j = 2; j != line - 2; ++j) {
				let lt = indiceBase + (line - 1) * line + j;
				indices.push(lt, lt - line - 1, lt - line,
						lt, lt - line, lt - line + 1,
							lt, lt + 1 - line, lt + 2);
			}
			for (let i = 2; i != line - 2; ++i) {
				let lt = indiceBase + i * line + 0;
				indices.push(lt, lt + 1 - line, lt + 1,
						lt, lt + 1, lt + 1 + line,
							lt, lt + 1 + line, lt + 2 * line);
			}
			for (let i = 2; i != line - 2; ++i) {
				let lt = indiceBase + i * line + (line - 1);
				indices.push(lt, lt - 1, lt - 1 - line,
						lt, lt - 1 + line, lt - 1,
							lt, lt + 2 * line, lt - 1 + line);
			}
			let lt = indiceBase;
			indices.push(lt, lt + 2, lt + 1 + line, lt, lt + 1 + line, lt + 2 * line);
			lt = indiceBase + (line - 1);
			indices.push(lt, lt - 1 + line, lt - 2, lt, lt + 2 * line, lt - 1 + line);
			lt = indiceBase + line * (line - 1);
			indices.push(lt, lt - 2 * line, lt + 1 - line, lt, lt + 1 - line, lt + 2);
			lt = indiceBase + line * (line - 1) + (line - 1);
			indices.push(lt, lt - 2, lt - 1 - line, lt, lt - 1 - line, lt - 2 * line);
			
			meshSize *= 2;
			indiceBase += (Terrain.cascadeMeshCount * 2 + 1) *
				(Terrain.cascadeMeshCount * 2 + 1);
		}
		this.mesh = new VAO(vertices, indices);
	}
	
	render(viewport: Viewport, shader: Shader) {
		shader.use();
			this.setBasicUniforms(viewport);
			this.mesh.bind();
				this.mesh.draw();
			this.mesh.unbind();
		shader.unuse();
	}
}

export {
	Terrain
}