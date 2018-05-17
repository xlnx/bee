import { glm } from "../util/glm"
import { gl, gl2 } from "../renderer/renderer";
import { VBO, EBO, TBO } from "./buffer";

const attrLocation = {
	pos: 0, color: 1, norm: 2, tg: 3, bitg: 4, tex: 5, ibone: 6, wbone: 7
};

type PositionType = "pos2" | "pos3" | "pos4"
type ColorType = "color3" | "color4"
type NormalType = "norm3" | "norm4"
type TangentType = "tg3" | "tg4"
type BitangentType = "bitg3" | "bitg4"
type TexcoordType = "tex2" | "tex3" | "tex4"
type BoneIndexType = "ibone3" | "ibone4"
type BoneWeightType = "wbone3" | "wbone4"

type VertexAttrType = PositionType | ColorType | NormalType |
	TangentType | BitangentType | TexcoordType | BoneIndexType | BoneWeightType;

type VertexAttr<T extends VertexAttrType> = {
	[P in T]: number[];
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

class TAO {
	private attrs: AttrRecord[];
	private attrSize: number;
	private numVertices: number;
	private tbo = new TBO();

	constructor(attr: TransformAttrs) {
		this.attrs = attr.attrs;
		this.attrSize = attr.attrSize;
		this.numVertices = attr.vertices.length / attr.component;
		this.tbo.bind();
			this.tbo.data(attr.vertices);
		this.tbo.unbind();
	}
	bind() {
		this.tbo.bind();
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
		this.tbo.unbind();
	}
	draw();
	draw(first: number, count: number); 
	draw(first: number = undefined, count: number = undefined) {
		gl2.beginTransformFeedback(gl.POINTS);
		if (first != undefined) {
			gl.drawArrays(gl.POINTS, first, count);
		} else {
			gl.drawArrays(gl.POINTS, 0, this.numVertices);
		}
		gl2.endTransformFeedback();
	}
	swap() {
		this.tbo.swap();
	}
}

class TransformAttrType {
	name: string;
	type: number;
	size: number;
}

class TransformAttrs {
	public readonly attrs: AttrRecord[] = [];
	public readonly attrSize: number = 0;
	public vertices: number[] = [];
	public readonly component: number = 0;
	private readonly attrLookup: { [key: string]: AttrRecord } = {};

	constructor(private attrNames: TransformAttrType[]) {
		let offset = 0;
		for (let i in attrNames) {
			let attr = {
				index: <number>null,
				type: <number>null,
				size: <number>null,
				offset: <number>null
			};
			attr.index = +i;
			attr.type = attrNames[i].type;
			attr.size = attrNames[i].size;
			attr.offset = offset;
			offset += attr.size * TransformAttrs.getSize(attr.type);
			this.attrs.push(attr);
			this.attrLookup[attrNames[i].name] = attr;
			this.component += attr.size;
		}
		this.attrSize = offset;
	}
	push(vertex: { [key: string]: number [] }) {
		for (let x of this.attrNames) {
			if (x.name in vertex) {
				this.vertices = this.vertices.concat(vertex[x.name]);
			} else {
				throw "lack of vertex attribute: " + x;
			}
		}
	}
	set(type: string, vs: number[]) {
		let i = 0;
		for (let x of this.attrNames) {
			if (x.name == type) break;
			i += this.attrLookup[x.name].size;
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
	VertexAttrs,
	VAO,
	TransformAttrs,
	TAO
}
