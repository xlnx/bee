import { gl, gl2 } from "../renderer/renderer";
import { TBO } from "./buffer";

type AttrRecord = {
	index: number;
	type: number;
	size: number;
	offset: number;
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
	draw(mode: number);
	draw(mode: number, first: number, count: number); 
	draw(mode?: number, first: number = undefined, count: number = undefined) {
		if (mode == undefined) {
			mode = gl.POINTS;
		}
		gl2.beginTransformFeedback(mode);
		if (first != undefined) {
			gl.drawArrays(mode, first, count);
		} else {
			gl.drawArrays(mode, 0, this.numVertices);
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

class TransformFeedback {
	public readonly handle: WebGLTransformFeedback;
	private vbo = new TBO();

	constructor() {
		if (!gl2) {
			throw "webgl 2.0 required.";
		}
		this.handle = gl2.createTransformFeedback();
	}

	bind() {
		gl2.bindTransformFeedback(gl2.TRANSFORM_FEEDBACK, this.handle);
	}
	unbind() {
		gl2.bindTransformFeedback(gl2.TRANSFORM_FEEDBACK, null);
	}
}

export {
	TransformAttrs,
	TAO,
	TransformFeedback
}