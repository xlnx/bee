import { glm } from "../util/glm"
import { gl } from "../renderer/renderer";
import { VBO, EBO } from "./buffer";

const attrLocation = {
	pos: 0, color: 1, norm: 2, tg: 3, bitg: 4, tex: 5, ibone: 6, wbone: 7
};

type Extra = "pos2" | "pos3" | "pos4"
type ColorType = "color3" | "color4"
type NormalType = "norm3" | "norm4"
type TangentType = "tg3" | "tg4"
type BitangentType = "bitg3" | "bitg4"
type TexcoordType = "tex2" | "tex3" | "tex4"
type BoneIndexType = "ibone3" | "ibone4"
type BoneWeightType = "wbone3" | "wbone4"

type VertexAttrType = Extra | ColorType | NormalType |
	TangentType | BitangentType | TexcoordType | BoneIndexType | BoneWeightType;

class TypeMap {
	pos2: glm.vec2;
	pos3: glm.vec3;
	pos4: glm.vec4;
	color3: glm.vec3;
	color4: glm.vec4;
	norm3: glm.vec3;
	norm4: glm.vec4;
	tg3: glm.vec3;
	tg4: glm.vec4;
	bitg3: glm.vec3;
	bitg4: glm.vec4;
	tex2: glm.vec2;
	tex3: glm.vec3;
	tex4: glm.vec4;
	ibone3: glm.vec3;
	ibone4: glm.vec4;
	wbone3: glm.vec3;
	wbone4: glm.vec4;
}

type VertexAttr<T extends VertexAttrType> = {
	[P in T]: TypeMap[P];
}

type AttrRecord = {
	index: number;
	type: number;
	size: number;
	offset: number;
}

class VAO {
	private attrs: AttrRecord[];
	private attrSize: number;
	private numVertices: number;
	private numIndices: number;
	private vbo = new VBO();
	private ebo: EBO;

	constructor(attr: VertexAttrs, indices: number[] = undefined) {
		this.attrs = attr.attrs;
		this.attrSize = attr.attrSize;
		this.numVertices = attr.vertices.length / attr.component;
		this.vbo.bind();
			this.vbo.data(attr.vertices);
		this.vbo.unbind();
		if (indices != undefined) {
			this.ebo = new EBO();
			this.ebo.bind();
				this.ebo.data(indices);
			this.ebo.unbind();
			this.numIndices = indices.length;
		}
	}
	bind() {
		this.vbo.bind();
		if (this.ebo) {
			this.ebo.bind();
		}
		for (let x of this.attrs) {
			gl.enableVertexAttribArray(x.index);
			// console.log(x.index, x.size, x.type, false, this.attrSize, x.offset);
			gl.vertexAttribPointer(x.index, x.size, x.type, false, this.attrSize, x.offset);
		}
	}
	unbind() {
		for (let x of this.attrs) {
			gl.disableVertexAttribArray(x.index);
		}
		if (this.ebo) {
			this.ebo.unbind();
		}
		this.vbo.unbind();
	}
	draw();
	draw(mode: number, first: number, count: number); 
	draw(mode: number = undefined, first: number = undefined, count: number = undefined) {
		if (this.ebo) {
			gl.drawElements(gl.TRIANGLES, this.numIndices, gl.UNSIGNED_SHORT, 0);
		} else {
			if (mode != undefined) {
				gl.drawArrays(mode, first, count);
			} else {
				gl.drawArrays(gl.TRIANGLES, 0, this.numVertices);
			}
		}
	}
}

class VertexAttrs {
	public readonly attrs: AttrRecord[] = [];
	public readonly attrSize: number = 0;
	public vertices: number[] = [];
	public readonly component: number = 0;
	private readonly attrLookup: { [key: string]: AttrRecord } = {};

	constructor(private attrNames: VertexAttrType[]) {
		let offset = 0;
		for (let x of attrNames) {
			let attr = {
				index: <number>null,
				type: <number>null,
				size: <number>null,
				offset: <number>null
			};
			attr.index = attrLocation[x.substr(0, x.length - 1)];
			if (x[0] == "i") {
				attr.type = gl.INT;
			} else {
				attr.type = gl.FLOAT;
			}
			attr.size = +x.substr(-1);
			attr.offset = offset;
			offset += attr.size * VertexAttrs.getSize(attr.type);
			this.attrs.push(attr);
			this.attrLookup[x] = attr;
			this.component += attr.size;
		}
		this.attrSize = offset;
	}
	push<T extends VertexAttrType>(vertex: VertexAttr<T>) {
		for (let x of this.attrNames) {
			if (x in vertex) {
				this.vertices = this.vertices.concat(vertex[x]);
			} else {
				throw "lack of vertex attribute: " + x;
			}
		}
	}
	set(type: VertexAttrType, vs: number[]) {
		let i = 0;
		for (let x of this.attrNames) {
			if (x == type) break;
			i += this.attrLookup[x].size;
		}
		let dt = this.attrLookup[type].size;
		for (let j = 0; j < vs.length; j += dt) {
			for (let k = 0; k != dt; ++k) {
				this.vertices[i + k] = vs[j + k];
			}
			i += this.component;
		}
	}

	static getSize(type: number): number {
		switch (type) {
			case gl.FLOAT: return 4;
			case gl.BYTE: case gl.UNSIGNED_BYTE: return 1;
			case gl.SHORT: case gl.UNSIGNED_SHORT: return 2;
			case gl.INT: case gl.UNSIGNED_INT: return 4;
			default: throw "unknown type";
		}
	}
}

export {
	VertexAttr,
	VertexAttrs,
	VAO
}
