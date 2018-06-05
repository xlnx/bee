import { RBO, FBO, PDBO } from "../gl/buffer";
import { gl, Renderer, ext, gl2 } from "../renderer/renderer";
import { Texture2D, TextureCube, Texture } from "../gl/texture";
import { Viewport } from "../gl/viewport";
import { glm } from "../util/glm"

// class SingleChannelFBRB {
// 	private rbo = new RBO();
// 	private dbo = new RBO();
// 	private fbo = new FBO();

// 	constructor() {
// 		this.fbo.bind();
// 			this.rbo.bind();
// 				if (gl2) {
// 					gl.renderbufferStorage(gl.RENDERBUFFER, gl2.R32F, 
// 						Renderer.instance.canvas.width, Renderer.instance.canvas.height);
// 				} else {
// 					gl.renderbufferStorage(gl.RENDERBUFFER, gl2.RGB, 
// 						Renderer.instance.canvas.width, Renderer.instance.canvas.height);
// 				}
// 				this.fbo.setRenderBuffer(this.rbo, gl.COLOR_ATTACHMENT0);
// 			this.rbo.unbind();
// 			this.dbo.bind();
// 				gl.renderbufferStorage(gl.RENDERBUFFER, gl.DEPTH_COMPONENT,
// 					Renderer.instance.canvas.width, Renderer.instance.canvas.height);
// 				this.fbo.setRenderBuffer(this.dbo, gl.DEPTH_ATTACHMENT);
// 			this.dbo.unbind();
// 		this.fbo.unbind();
// 	}

// 	bind() {
// 		this.fbo.bind();
// 	}
// 	unbind() {
// 		this.rbo.bind();
// 	}
// }

class RenderBuffer {
	public readonly type = "renderbuffer";
	public readonly rbo = new RBO();

	constructor(component: number)
	constructor(component: number, width: number, height: number)
	constructor(component: number, width?: number, height?: number) {
		if (width == undefined) {
			width = Renderer.instance.canvas.width;
			height = Renderer.instance.canvas.height;
		}
		this.rbo.bind();
			gl.renderbufferStorage(gl.RENDERBUFFER, component, width, height);
		this.rbo.unbind();
	}
}

class PixelRetriver {
	public readonly type = "pixelRetriever";

	public readonly pbo = new PDBO();

	constructor(public readonly pixels: any) {
		if (!Renderer.require("WEBGL_get_buffer_sub_data_async")) {
			throw "extension WEBGL_get_buffer_sub_data_async not supported";
		}
		this.pbo.bind();
		this.pbo.data(pixels.byteLength);
	}
	getData() {
		gl2.getBufferSubData(gl2.PIXEL_PACK_BUFFER, 0, this.pixels, 0, 0);		
	}
	getDataAsync(callback: (buffer?) => void) {
		// console.log(ext.getBufferSubDataAsync);
		ext["WEBGL_get_buffer_sub_data_async"].getBufferSubDataAsync(gl2.PIXEL_PACK_BUFFER, 0, this.pixels, 0, 0).
		then((buffer) => {
			gl.deleteBuffer(this.pbo.handle);
			callback(buffer);
		});
	}
}

class Offscreen {
	private static offscreens = [];

	private fbo = new FBO();

	set(channel: number, texture: TextureCube, face: number);
	set(channel: number, texture: Texture2D)
	set(channel: number, renderbuffer: RenderBuffer);
	set(channel: number, second: any, face?: number) {
		if (second == null) {
			gl.framebufferTexture2D(gl.FRAMEBUFFER, channel, gl.TEXTURE_2D, null, 0);
		} else if (second.type == "renderbuffer") {
			let renderbuffer: RenderBuffer = second;
			gl.framebufferRenderbuffer(gl.FRAMEBUFFER, channel, gl.RENDERBUFFER, renderbuffer.rbo.handle);
		} else {
			let texture: Texture = second;
			switch (texture.type) {
				case gl.TEXTURE_2D: {
					gl.framebufferTexture2D(gl.FRAMEBUFFER, channel, gl.TEXTURE_2D, texture.handle, 0);
				} break;
				case gl.TEXTURE_CUBE_MAP: {
					gl.framebufferTexture2D(gl.FRAMEBUFFER, channel, 
						gl.TEXTURE_CUBE_MAP_POSITIVE_X + face, texture.handle, 0);
				} break;
			}
		}
	}

	bind() {
		this.fbo.bind();
		Offscreen.offscreens.push(this.fbo);
	}
	unbind() {
		if (Offscreen.offscreens.length) {
			Offscreen.offscreens.pop();
			this.fbo.unbind();
			if (Offscreen.offscreens.length) {
				Offscreen.offscreens[Offscreen.offscreens.length - 1].bind();
			}
		}
	}
	check() {
		this.fbo.check();
	}
}

export {
	RenderBuffer,
	PixelRetriver,
	Offscreen
}
