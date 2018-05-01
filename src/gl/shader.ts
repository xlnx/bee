import xhr from "../util/xhr"
import { glm } from "../util/glm"
import { gl, gl2 } from "../renderer/renderer"
import { Communicators } from "./communicator";

type UniformType = "int" | "float" | "ivec2" | "vec2" | "ivec3" | "vec3" | "ivec4" | "vec4" |
	"mat2" | "mat3" | "mat4";

class Uniform {
	private setter: (loc: WebGLUniformLocation, value: any) => void;

	constructor(private name: string, private valueType: UniformType, private isArray: boolean) {
		// console.log(new UniformMap());
		switch (valueType) {
			case "int": this.setter = (location: WebGLUniformLocation, value: any) => 
				gl.uniform1iv(location, new Int32Array([value])); break;
			case "float": this.setter = (location: WebGLUniformLocation, value: any) => 
				gl.uniform1fv(location, new Float32Array([value])); break;
			case "ivec2": this.setter = (location: WebGLUniformLocation, value: any) => 
				gl.uniform2iv(location, value.array); break;
			case "vec2": this.setter = (location: WebGLUniformLocation, value: any) => 
				gl.uniform2fv(location, value.array); break;
			case "ivec3": this.setter = (location: WebGLUniformLocation, value: any) => 
				gl.uniform3iv(location, value.array); break;
			case "vec3": this.setter = (location: WebGLUniformLocation, value: any) => 
				gl.uniform3fv(location, value.array); break;
			case "ivec4": this.setter = (location: WebGLUniformLocation, value: any) => 
				gl.uniform4iv(location, value.array); break;
			case "vec4": this.setter = (location: WebGLUniformLocation, value: any) => 
				gl.uniform4fv(location, value.array); break;
			case "mat2": this.setter = (location: WebGLUniformLocation, value: any) => 
				gl.uniformMatrix2fv(location, false, value.array); break;
			case "mat3": this.setter = (location: WebGLUniformLocation, value: any) => 
				gl.uniformMatrix4fv(location, false, value.array); break;
			case "mat4": this.setter = (location: WebGLUniformLocation, value: any) => 
				gl.uniformMatrix4fv(location, false, value.array); break;
			default: throw "undefined uniform type";
		}
	}
	set(value: any) {
		try {
			if (!this.isArray) {
				this.setter(gl.getUniformLocation(Shader.current(), this.name), value);
			} else {
				throw 1;
			}
		} catch (e) {
			if (e == 1) {
				throw "cannot assign value to a uniform array.";
			} else {
				// console.warn("uniform variable does not exist: " + this.name);
			}
		}
	}
	subscribe(offset: number): Uniform {
		if (!this.isArray) {
			throw "cannot subscribe a non-array uniform";
		} else {
			return new Uniform(this.name + "[" + offset + "]", this.valueType, false);
		}
	}
}

class Shader {
	private static readonly shaderPath = "./shaders/";
	private static stack: Shader[] = [];
	private static shaders: { [key: string]: Shader } = {};
	private static requirements: { [label: string]: { [type: string]: WebGLShader } } = {};

	private static partials: string[] = [];
	
	private vert: WebGLShader;
	private frag: WebGLShader;
	public readonly handle: WebGLProgram;
	private partial: { [label: string]: WebGLProgram } = {};

	constructor(vsfilename: string, fsfilename: string, private readonly specify: boolean) {
		vsfilename = Shader.shaderPath + (gl2 ? "gl2/" : "gl/") + vsfilename;
		fsfilename = Shader.shaderPath + (gl2 ? "gl2/" : "gl/") + fsfilename;
		this.vert = Shader.compileShader(xhr.getSync(vsfilename), gl.VERTEX_SHADER);
		this.frag = Shader.compileShader(xhr.getSync(fsfilename), gl.FRAGMENT_SHADER);
		this.handle = Shader.createProgram(this.vert, this.frag);
		if (specify) {
			for (let label in Shader.requirements) {
				this.partial[label] = Shader.createProgram(
						"vert" in Shader.requirements[label] ? Shader.requirements[label].vert : this.vert,
						"frag" in Shader.requirements[label] ? Shader.requirements[label].frag : this.frag);
			}
		}
	}

	use() {
		Shader.stack.push(this);
		if (this.specify && Shader.partials.length) {
			gl.useProgram(this.partial[Shader.partials[Shader.partials.length - 1]]);
		} else {
			gl.useProgram(this.handle);
		}
		Communicators.invoke();
	}
	unuse() {
		if (Shader.stack.length) {
			Shader.stack.pop();
		}
		if (Shader.stack.length) {
			let self = Shader.stack[Shader.stack.length - 1];
			if (self.specify && Shader.partials.length) {
				gl.useProgram(self.partial[Shader.partials[Shader.partials.length - 1]]);
			} else {
				gl.useProgram(self.handle);
			}
			Communicators.invoke();
		} else {
			gl.useProgram(null);
		}
	}
	static uniform(valueType: UniformType, name: string): Uniform {
		let p = name.indexOf("[]");
		if (p != -1) {
			return new Uniform(name.substring(0, name.indexOf("[]")), valueType, true);
		} else {
			return new Uniform(name, valueType, false);
		}
	}
	static require(list: { [label: string]: { [type: string]: string }}) {
		for (let label in list) {
			if (!(label in Shader.requirements)) {
				let data: { [type: string]: WebGLShader } = {};
				if ("frag" in list[label]) {
					let fsfilename = Shader.shaderPath + (gl2 ? "gl2/" : "gl/") + list[label].frag + ".frag";
					data.frag = Shader.compileShader(xhr.getSync(fsfilename), gl.FRAGMENT_SHADER);
				}
				if ("vert" in list[label]) {
					let vsfilename = Shader.shaderPath + (gl2 ? "gl2/" : "gl/") + list[label].vert + ".vert";
					data.vert = Shader.compileShader(xhr.getSync(vsfilename), gl.VERTEX_SHADER);
				}
				// console.log(data);
				Shader.requirements[label] = data;
				for (let id in Shader.shaders) {
					if (Shader.shaders[id].specify) {
						Shader.shaders[id].partial[label] = Shader.createProgram(
								"vert" in data ? data.vert : Shader.shaders[id].vert,
								"frag" in data ? data.frag : Shader.shaders[id].frag);
					}
				}
			}
		}
	}

	private static compileShader(source: string, type: number): WebGLShader {
		let shader = gl.createShader(type);
		gl.shaderSource(shader, source);
		gl.compileShader(shader);
		if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
			throw gl.getShaderInfoLog(shader);
		}
		return shader;
	}
	public static create(name: string, specify: boolean): Shader {
		if (!(name in Shader.shaders)) {
			Shader.shaders[name] = new Shader(name + ".vert", name + ".frag", specify);
		}
		return Shader.shaders[name];
	}
	static current(): WebGLProgram {
		if (!Shader.stack.length) {
			return null;
		} else {
			if (Shader.stack[Shader.stack.length - 1].specify && Shader.partials.length) {
				return Shader.stack[Shader.stack.length - 1].partial[Shader.partials[Shader.partials.length - 1]];
			} else {
				return Shader.stack[Shader.stack.length - 1].handle;
			}
		}
	}
	static specify(label: string) {
		this.partials.push(label);
	}
	static unspecify() {
		if (this.partials.length) {
			this.partials.pop();
		}
	}

	private static createProgram(vert: WebGLShader, frag: WebGLShader): WebGLProgram {
		let program = gl.createProgram();
		gl.bindAttribLocation(program, 0, "Position");
		gl.bindAttribLocation(program, 1, "Color");
		gl.bindAttribLocation(program, 2, "Normal");
		gl.bindAttribLocation(program, 3, "Tangent");
		gl.bindAttribLocation(program, 4, "Bitangent");
		gl.bindAttribLocation(program, 5, "TexCoord");
		gl.bindAttribLocation(program, 6, "BoneIndex");
		gl.bindAttribLocation(program, 7, "BoneWeight");
		gl.attachShader(program, vert);
		gl.attachShader(program, frag);
		gl.linkProgram(program);
		if (!gl.getProgramParameter(program, gl.LINK_STATUS)) {
			throw gl.getProgramInfoLog(program);
		}
		return program;
	}
}

export {
	Uniform,
	UniformType,
	Shader
}