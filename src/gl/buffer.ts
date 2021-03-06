import { gl, gl2 } from "../renderer/renderer"

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
		gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint32Array(data), gl.STATIC_DRAW);
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
	setRenderBuffer(rbo: RBO, channel: number) {
		gl.framebufferRenderbuffer(gl.FRAMEBUFFER, channel, gl.RENDERBUFFER, rbo);
	}
	check() {
		switch (gl.checkFramebufferStatus(gl.FRAMEBUFFER)) {
			case gl.FRAMEBUFFER_COMPLETE: break;
			case gl.FRAMEBUFFER_INCOMPLETE_ATTACHMENT: 
				throw "framebuffer incomplete attachment.";
			case gl.FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
				throw "framebuffer incomplete missing attachment.";
			case gl.FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
				throw "framebuffer incomplete dimensions.";
			case gl.FRAMEBUFFER_UNSUPPORTED:
				throw "framebuffer unsupported.";
			default: 
				throw "unknown framebuffer error.";
		}
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

class PDBO extends BufferBase {
	constructor() {
		if (!gl2) {
			throw "webgl 2.0 is not supported";
		}
		super(gl2.createBuffer());
	}
	bind() {
		gl2.bindBuffer(gl2.PIXEL_PACK_BUFFER, this.handle);
	}
	unbind() {
		gl2.bindBuffer(gl2.PIXEL_PACK_BUFFER, null);
	}
	data(bytelength: number) {
		gl.bufferData(gl2.PIXEL_PACK_BUFFER, bytelength, gl2.STATIC_READ);
	}
}

class PUBO extends BufferBase {
	constructor() {
		if (!gl2) {
			throw "webgl 2.0 is not supported";
		}
		super(gl2.createBuffer());
	}
	bind() {
		gl2.bindBuffer(gl2.PIXEL_UNPACK_BUFFER, this.handle);
	}
	unbind() {
		gl2.bindBuffer(gl2.PIXEL_UNPACK_BUFFER, null);
	}
}

class TBO {
	private handle1;
	private handle0;

	constructor() {
		if (!gl2) {
			throw "webgl 2.0 required.";
		}
		this.handle0 = gl2.createBuffer();
		this.handle1 = gl2.createBuffer();
	}
	bind() {
		gl.bindBuffer(gl.ARRAY_BUFFER, this.handle0);
		gl2.bindBufferBase(gl2.TRANSFORM_FEEDBACK_BUFFER, 0, this.handle1);
	}
	unbind() {
		gl2.bindBufferBase(gl2.TRANSFORM_FEEDBACK_BUFFER, 0, null);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);
	}
	data(data: any[]) {
		gl.bufferData(gl.ARRAY_BUFFER, 4 * data.length, gl2.DYNAMIC_COPY);
		this.unbind();
		this.swap();
		this.bind();
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(data), gl2.DYNAMIC_COPY);
	}
	swap() {
		let h = this.handle1; this.handle1 = this.handle0; this.handle0 = h;
	}
	get handle(): any {
		return this.handle0;
	}
}

export {
	VBO,
	EBO,
	FBO,
	RBO,
	TBO,
	PUBO,
	PDBO
}
