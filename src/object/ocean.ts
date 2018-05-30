import { Terrain } from "./terrain";
import { Viewport } from "../gl/viewport";
import { Shader } from "../gl/shader";
import { GerstnerWave } from "./gerstnerWave";
import { Communicators, Communicator } from "../gl/communicator";
import { ulist_elem } from "../util/ulist";
import Material from "../gl/material";
import { VAO, VertexAttrs } from "../gl/vertexAttrs";


class Ocean extends Terrain {
	constructor() {
		super(6, 64, 0.02);
		this.defaultShader = Shader.create("ocean", true);
		
		let M = 64, w = 0.02 * (1 << 6), W = w * M / 2;
		let vertices = new VertexAttrs(["pos2"]);
		let indices = [];
		let pos2 = [];
		let ipos = 0;
		let dx = [1, 1, -1, -1], dy = [1, 0, 1, 0];

		let y = -W;
		while (M >>= 2) {
			for (let j = 0; j < 4; ++j) {
				for (let x = -W; x < W; x += 4 * w) {
					pos2[ipos + 0] = x + 0 * w; pos2[ipos + 1] = y - 0 * w;
					pos2[ipos + 2] = x + 1 * w; pos2[ipos + 3] = y - 0 * w;
					pos2[ipos + 4] = x + 2 * w; pos2[ipos + 5] = y - 0 * w;
					pos2[ipos + 6] = x + 3 * w; pos2[ipos + 7] = y - 0 * w;
					pos2[ipos + 8] = x + 4 * w; pos2[ipos + 9] = y - 0 * w;
					pos2[ipos + 10] = x + 0 * w; pos2[ipos + 11] = y - 1 * w;
					pos2[ipos + 12] = x + 4 * w; pos2[ipos + 13] = y - 1 * w;

					if (dy[j]) {
						for (let k = 0; k < 14; k += 2) {
							pos2[ipos + k + 1] *= dx[j]; 
						}
					} else {
						for (let k = 0; k < 14; k += 2) {
							let t = pos2[ipos + k + 1] * dx[j];
							pos2[ipos + k + 1] = pos2[ipos + k + 0];
							pos2[ipos + k + 0] = t;
						}
					}

					let p = ipos >> 1;
					indices.push(
						p + 0, p + 1, p + 5,
						p + 1, p + 2, p + 5,
						p + 2, p + 6, p + 5,
						p + 2, p + 3, p + 6,
						p + 3, p + 4, p + 6
					);
					ipos += 14;
				}
			}

			let x = -W;
			let fx = [1, 1, -1, -1], fy = [1, -1, 1, -1];
			for (let i = 0; i < 4; ++i) {
				pos2[ipos + 0] = x - 0 * w; pos2[ipos + 1] = y - 0 * w;
				pos2[ipos + 2] = x - 0 * w; pos2[ipos + 3] = y - 1 * w;
				pos2[ipos + 4] = y - 0 * w; pos2[ipos + 5] = x - 0 * w;
				pos2[ipos + 6] = y - 1 * w; pos2[ipos + 7] = x - 0 * w;

				for (let k = 0; k < 8; k += 2) {
					pos2[ipos + k + 0] *= fx[i]; 
					pos2[ipos + k + 1] *= fy[i]; 
				}

				let p = ipos >> 1;
				indices.push(
					p + 0, p + 1, p + 2,
					p + 1, p + 2, p + 3
				);
				ipos += 8;
			}

			y -= w;
			w *= 4;
		}

		let x = -W, inf = 5e4;
		pos2[ipos + 0] = -inf; pos2[ipos + 1] = -inf;
		pos2[ipos + 2] = inf; pos2[ipos + 3] = -inf;
		pos2[ipos + 4] = inf; pos2[ipos + 5] = inf;
		pos2[ipos + 6] = -inf; pos2[ipos + 7] = inf;

		pos2[ipos + 8] = x; pos2[ipos + 9] = y;
		pos2[ipos + 10] = -x; pos2[ipos + 11] = y;
		pos2[ipos + 12] = -x; pos2[ipos + 13] = -y;
		pos2[ipos + 14] = x; pos2[ipos + 15] = -y;

		pos2[ipos + 16] = y; pos2[ipos + 17] = x;
		pos2[ipos + 18] = -y; pos2[ipos + 19] = x;
		pos2[ipos + 20] = -y; pos2[ipos + 21] = -x;
		pos2[ipos + 22] = y; pos2[ipos + 23] = -x;

		let p = ipos >> 1;
		indices.push(
			p + 0, p + 4, p + 8,
			p + 0, p + 4, p + 5,
			p + 1, p + 5, p + 0,
			p + 1, p + 5, p + 9,
			p + 1, p + 9, p + 10,
			p + 1, p + 10, p + 2,
			p + 2, p + 6, p + 10,
			p + 2, p + 6, p + 7,
			p + 2, p + 3, p + 7,
			p + 3, p + 0, p + 8,
			p + 3, p + 7, p + 11,
			p + 3, p + 8, p + 11
		);
		
		vertices.set("pos2", pos2);
		this.vao = new VAO(vertices, indices);
	}

	render(viewport: Viewport) {
		this.setBasicUniforms(viewport);
		this.gN.set(256);
		this.mesh.bind();
			this.mesh.draw();
		this.mesh.unbind();
		this.vao.bind();
			this.vao.draw();
		this.vao.unbind();
	}

	private gN = Shader.uniform("float", "gN");
	private vao: VAO;
}

export {
	Ocean
}
