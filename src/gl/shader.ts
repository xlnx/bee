import text from "../util/text"
import { glm } from "../util/glm"
import { gl, gl2 } from "../renderer/renderer"

type UniformType = "int" | "float" | "ivec2" | "vec2" | "ivec3" | "vec3" | "ivec4" | "vec4" |
	"mat2" | "mat3" | "mat4";

class Uniform {
	private setter: any;

	constructor(private valueType: UniformType, private index: number, private isArray: boolean) {
		// console.log(new UniformMap());
		switch (valueType) {
			case "int": this.setter = gl.uniform1i; break;
			case "float": this.setter = gl.uniform1f; break;
			case "ivec2": this.setter = (location: any, value: any) => 
				gl.uniform2iv(<WebGLUniformLocation>location, value.array); break;
			case "vec2": this.setter = (location: any, value: any) => 
				gl.uniform2fv(<WebGLUniformLocation>location, value.array); break;
			case "ivec3": this.setter = (location: any, value: any) => 
				gl.uniform3iv(<WebGLUniformLocation>location, value.array); break;
			case "vec3": this.setter = (location: any, value: any) => 
				gl.uniform3fv(<WebGLUniformLocation>location, value.array); break;
			case "ivec4": this.setter = (location: any, value: any) => 
				gl.uniform4iv(<WebGLUniformLocation>location, value.array); break;
			case "vec4": this.setter = (location: any, value: any) => 
				gl.uniform4fv(<WebGLUniformLocation>location, value.array); break;
			case "mat2": this.setter = (location: any, value: any) => 
				gl.uniformMatrix2fv(<WebGLUniformLocation>location, false, value.array); break;
			case "mat3": this.setter = (location: any, value: any) => 
				gl.uniformMatrix4fv(<WebGLUniformLocation>location, false, value.array); break;
			case "mat4": this.setter = (location: any, value: any) => 
				gl.uniformMatrix4fv(<WebGLUniformLocation>location, false, value.array); break;
			default: throw "undefined uniform type";
		}
	}
	set(value: any) {
		if (this.index < 0) {
			this.setter(<WebGLUniformLocation>-this.index, value);
		} else {
			if (!this.isArray) {
				this.setter(<WebGLUniformLocation>ShaderBase.current.uniforms[this.index], value);
			} else {
				throw "cannot assign value to a uniform array";
			}
		}
	}
	subscribe(offset: number): Uniform {
		if (!this.isArray) {
			throw "cannot subscribe a non-array uniform";
		} else {
			let index = ShaderBase.current.uniformArrays[this.index].base + 
				offset * ShaderBase.current.uniformArrays[this.index].diff;
			return new Uniform(this.valueType, -index, false);
		}
	}
}

// WebGLUniformLocation

class ShaderBase {
	public static current: ShaderBase = null;
	
	public uniforms: number[] = [0];
	public uniformArrays: { base: number, diff: number }[] = [{base:0, diff:0}];
}

class Shader {
	public readonly handle: WebGLProgram;
	private static readonly shaderPath = "./shaders/";

	private base = new ShaderBase();

	private static shaders: { [key: string]: Shader } = {};
	private static uniformIndex: { [key: string]: number } = {}
	private static uniformArrayIndex: { [key: string]: number } = {};
	private static registeredUniforms: string[] = [];
	private static registeredUniformArrays: { first: string, second: string }[] = [];

	constructor(vsfilename: string, fsfilename: string) {
		vsfilename = Shader.shaderPath + (gl2 ? "gl2/" : "gl/") + vsfilename;
		fsfilename = Shader.shaderPath + (gl2 ? "gl2/" : "gl/") + fsfilename;
		console.log(vsfilename);
		console.log(fsfilename);
		let vs = Shader.compileShader(text.getSync(vsfilename), gl.VERTEX_SHADER);
		let fs = Shader.compileShader(text.getSync(fsfilename), gl.FRAGMENT_SHADER);
		this.handle = gl.createProgram();
		gl.attachShader(this.handle, vs);
		gl.attachShader(this.handle, fs);
		gl.linkProgram(this.handle);
		if (!gl.getProgramParameter(this.handle, gl.LINK_STATUS)) {
			throw gl.getProgramInfoLog(this.handle);
		}
		for (let i in Shader.registeredUniforms) {
			this.base.uniforms[i] = <number>gl.getUniformLocation(this.handle, 
				Shader.registeredUniforms[i]);
		}
		for (let i in Shader.registeredUniformArrays) {
			let first = <number>gl.getUniformLocation(this.handle, 
				Shader.registeredUniformArrays[i].first);
			let second = <number>gl.getUniformLocation(this.handle, 
				Shader.registeredUniformArrays[i].second);
			this.base.uniformArrays[i] = { base: first, diff: second - first };
		}
	}

	use() {
		ShaderBase.current = this.base;
		gl.useProgram(this.handle);
	}
	unuse() {
		ShaderBase.current = null;
		gl.useProgram(0);
	}
	static uniform(valueType: UniformType, name: string): Uniform {
		let p = name.indexOf("[]");
		if (p != -1) {
			if (name in this.uniformArrayIndex) {
				return new Uniform(valueType, Shader.uniformArrayIndex[name], true);
			} else {
				let front = name.substr(0, p + 1);
				let back = name.substr(p + 1);
				let name0 = front + "0" + back;
				let name1 = front + "1" + back;
				Shader.registeredUniformArrays.push({ first: name0, second: name1});
				let index = Shader.uniformArrayIndex[name] = Shader.registeredUniformArrays.length;
				for (let i in this.shaders) {
					let first = <number>gl.getUniformLocation(this.shaders[i].handle, 
						Shader.registeredUniformArrays[i].first);
					let second = <number>gl.getUniformLocation(this.shaders[i].handle, 
						Shader.registeredUniformArrays[i].second);
					this.shaders[i].base.uniformArrays[i] = { base: first, diff: second - first };
				}
				return new Uniform(valueType, index, true);
			}
		} else {
			if (name in Shader.uniformIndex) {
				return new Uniform(valueType, Shader.uniformIndex[name], false);
			} else {
				Shader.registeredUniforms.push(name);
				let index = Shader.uniformIndex[name] = Shader.registeredUniforms.length;
				for (let i in this.shaders) {
					this.shaders[i].base.uniforms.push(
							<number>gl.getUniformLocation(this.shaders[i].handle, name));
				}
				return new Uniform(valueType, index, false);
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
	public static create(name: string): Shader {
		if (!(name in Shader.shaders)) {
			Shader.shaders[name] = new Shader(name + ".vs", name + ".fs");
		}
		return Shader.shaders[name];
	}
}

export {
	Uniform,
	Shader
}