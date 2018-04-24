import { gl } from "../renderer/renderer"

class BufferBase {
	constructor(public readonly handle: any)
	{
	}
}

class VBO extends BufferBase {
	constructor() {
		super(gl.createBuffer());
	}
	bind() {
		gl.bindBuffer(gl.ARRAY_BUFFER, this.handle);
	}
	unbind() {
		gl.bindBuffer(gl.ARRAY_BUFFER, null);
	}
	data(data: any[]) {
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(data), gl.STATIC_DRAW);
	}
}

class EBO extends BufferBase {
	constructor() {
		super(gl.createBuffer());
	}
	bind() {
		gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.handle);
	}
	unbind() {
		gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, null);
	}
	data(data: any[]) {
		gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(data), gl.STATIC_DRAW);
	}
}

class FBO extends BufferBase {
	constructor() {
		super(gl.createFramebuffer());
	}
	bind() {
		gl.bindFramebuffer(gl.FRAMEBUFFER, this.handle);
	}
	unbind() {
		gl.bindFramebuffer(gl.FRAMEBUFFER, null);
	}
}

class RBO extends BufferBase {
	constructor() {
		super(gl.createRenderbuffer());
	}
	bind() {
		gl.bindRenderbuffer(gl.RENDERBUFFER, this.handle);
	}
	unbind() {
		gl.bindRenderbuffer(gl.RENDERBUFFER, null);
	}
}

export {
	VBO,
	EBO,
	FBO,
	RBO
}
