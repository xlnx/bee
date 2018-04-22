import { glm } from "../util/glm"
import { gl } from "../renderer/renderer";
import { VBO, EBO } from "./buffer";

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
	private vbo = new VBO();

	constructor(attr: VertexAttrs, private ebo: EBO = null) {
		this.attrs = attr.attrs;
		this.attrSize = attr.attrSize;
		this.vbo.bind();
			this.vbo.data(attr.vertices);
		this.vbo.unbind();
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
	draw(mode: number = gl.TRIANGLES, first: number = 0, count: number = 0) {
		if (this.ebo) {
			// gl.drawElements(gl.TRIANGLES, this.ebo.size() * 3, gl.UNSIGNED_INT, 0);
		} else {
			gl.drawArrays(mode, first, count);
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
}

class VertexAttrs {
	public readonly attrs: AttrRecord[] = [];
	public readonly attrSize: number = 0;
	public vertices: number[] = [];

	constructor(private attrNames: VertexAttrType[]) {
		this.attrs = VertexAttrs.genAttrs(attrNames);
		for (let x of this.attrs) {
			this.attrSize += x.size * VertexAttrs.getSize(x.type);
		}
	}
	push<T extends VertexAttrType>(vertex: VertexAttr<T>) {
		// let row = [];
		for (let x of this.attrNames) {
			if (x in vertex) {
				this.vertices = this.vertices.concat(vertex[x]);
			} else {
				throw "undefined vertex attribute: " + x;
			}
		}
		// this.vertices.push();
	}

	static genAttrs(attrs: VertexAttrType[]): AttrRecord[] {
		let result: AttrRecord[] = [];
		for (let x of attrs) {
			let offset = 0;
			let attr = {
				index: <number>null,
				type: <number>null,
				size: <number>null,
				offset: <number>null
			};
			switch (x.substr(0, x.length)) {
				case "pos": attr.index = 0; break;
				case "color": attr.index = 1; break;
				case "norm": attr.index = 2; break;
				case "tg": attr.index = 3; break;
				case "bitg": attr.index = 4; break;
				case "tex": attr.index = 5; break;
				case "ibone": attr.index = 6; break;
				case "wbone": attr.index = 7; break;
			}
			if (x[0] == "i") {
				attr.type = gl.INT;
			} else {
				attr.type = gl.FLOAT;
			}
			attr.size = +x.substr(-1);
			attr.offset = offset;
			offset += attr.size * VertexAttrs.getSize(attr.type);
			result.push(attr);
		}
		return result;
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
