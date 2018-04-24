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
			// console.log(this.name, gl.getUniformLocation(Shader.current, this.name), value);
			if (!this.isArray) {
				console.log(this.name, gl.getUniformLocation(
						Shader.current.handle, this.name), value);
				this.setter(gl.getUniformLocation(
						Shader.current.handle, this.name), value);
			} else {
				throw 1;
			}
		} catch (e) {
			if (e == 1) {
				throw "cannot assign value to a uniform array.";
			} else {
				// throw e;
				console.warn(e);
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
	
	public readonly handle = gl.createProgram();

	constructor(vsfilename: string, fsfilename: string) {
		vsfilename = Shader.shaderPath + (gl2 ? "gl2/" : "gl/") + vsfilename;
		fsfilename = Shader.shaderPath + (gl2 ? "gl2/" : "gl/") + fsfilename;
		// console.log(vsfilename);
		// console.log(fsfilename);
		let vs = Shader.compileShader(xhr.getSync(vsfilename), gl.VERTEX_SHADER);
		let fs = Shader.compileShader(xhr.getSync(fsfilename), gl.FRAGMENT_SHADER);
		gl.attachShader(this.handle, vs);
		gl.attachShader(this.handle, fs);
		gl.linkProgram(this.handle);
		if (!gl.getProgramParameter(this.handle, gl.LINK_STATUS)) {
			throw gl.getProgramInfoLog(this.handle);
		}
	}

	use() {
		Shader.current = this;
		gl.useProgram(this.handle);
		if (Communicators.current) {
			Communicators.current.invoke();
		}
	}
	unuse() {
		Shader.current = null;
		if (Shader.current && Communicators.current) {
			Communicators.current.invoke();
		}
		gl.useProgram(Shader.current);
	}
	static uniform(valueType: UniformType, name: string): Uniform {
		let p = name.indexOf("[]");
		if (p != -1) {
			return new Uniform(name.substring(0, name.indexOf("[]")), valueType, true);
		} else {
			return new Uniform(name, valueType, false);
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
	public static create(name: string): Shader {
		if (!(name in Shader.shaders)) {
			Shader.shaders[name] = new Shader(name + ".vs", name + ".fs");
		}
		return Shader.shaders[name];
	}
	static get current(): Shader {
		return Shader.stack.length ? Shader.stack[Shader.stack.length - 1] : null;
	}
	static set current(value: Shader) {
		if (value != null) {
			Shader.stack.push(value);
		} else {
			if (Shader.stack.length) {
				Shader.stack.pop();
			}
		}
	}
}

export {
	Uniform,
	UniformType,
	Shader
}