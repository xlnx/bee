import { RBO, FBO } from "../gl/buffer";
import { gl, Renderer } from "../renderer/renderer";
import { Texture2D, TextureCube, Texture } from "../gl/texture";
import { Viewport } from "../scene/viewport";
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

class Offscreen {
	private fbo = new FBO();

	set(channel: number, texture: TextureCube, face: number);
	set(channel: number, texture: Texture2D)
	set(channel: number, renderbuffer: RenderBuffer);
	set(channel: number, second: any, face?: number) {
		if (second.type == "renderbuffer") {
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
	}
	unbind() {
		this.fbo.unbind();
	}
	check() {
		this.fbo.check();
	}
}

export {
	RenderBuffer,
	Offscreen
}
