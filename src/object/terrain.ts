import Obj from "./object";
import { Viewport } from "../gl/viewport";
import { Shader } from "../gl/shader";
import { VAO, VertexAttrs } from "../gl/vertexAttrs";
import { gl } from "../renderer/renderer";


abstract class Terrain extends Obj {
	protected mesh: VAO;

	public readonly isTerrain = true;

	constructor(C: number, M: number, w0: number) {
		super();
		let vertices = new VertexAttrs(["pos2"]);
		let indices = [];
		let w = w0;
		let offset = 0;
		let line = M * 2 + 1;
		let e = M;
		let b = M;
		let pos2 = []; pos2.length = C * (2 * M + 1) * (2 * M + 1) * 2;
		let ipos = 0;
		for (let k = 0; k != C; ++k) {
			for (let i = -M; i <= M; ++i) {
				for (let j = -M; j <= M; ++j) {
					pos2[ipos++] = j * w; pos2[ipos++] = i * w;
				}
			}
			for (let i = 1; i != line - 2; ++i) {
				for (let j = 1; j != line - 2; ++j) {
					if (i < e || i >= b || j < e || j >= b) {
						let lt = offset + i * line + j;
						indices.push(lt, lt + 1, lt + line + 1, 
								lt, lt + line + 1, lt + line);
					}
				}
			}
			for (let j = 2; j < line - 2; j += 2) {
				let lt = offset + 0 * line + j;
				indices.push(lt, lt + line, lt + line - 1,
						lt, lt + line + 1, lt + line,
							lt, lt + 2, lt + 1 + line);
			}
			for (let j = 2; j < line - 2; j += 2) {
				let lt = offset + (line - 1) * line + j;
				indices.push(lt, lt - line - 1, lt - line,
						lt, lt - line, lt - line + 1,
							lt, lt + 1 - line, lt + 2);
			}
			for (let i = 2; i < line - 2; i += 2) {
				let lt = offset + i * line + 0;
				indices.push(lt, lt + 1 - line, lt + 1,
						lt, lt + 1, lt + 1 + line,
							lt, lt + 1 + line, lt + 2 * line);
			}
			for (let i = 2; i < line - 2; i += 2) {
				let lt = offset + i * line + (line - 1);
				indices.push(lt, lt - 1, lt - 1 - line,
						lt, lt - 1 + line, lt - 1,
							lt, lt + 2 * line, lt - 1 + line);
			}
			let lt = offset;
			indices.push(lt, lt + 2, lt + 1 + line, 
				lt, lt + 1 + line, lt + 2 * line);
			lt = offset + (line - 1);
			indices.push(lt, lt - 1 + line, lt + 2 * line);
			lt = offset + line * (line - 1);
			indices.push(lt, lt + 1 - line, lt + 2);
			
			w *= 2;
			offset += (M * 2 + 1) * (M * 2 + 1);
			e = M / 2;
			b = 3 * M / 2;
		}
		
		vertices.set("pos2", pos2);
		
		this.mesh = new VAO(vertices, indices);
	}
	
	abstract render(viewport: Viewport);
}

export {
	Terrain
}
