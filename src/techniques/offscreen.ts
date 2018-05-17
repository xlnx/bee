import { RBO, FBO } from "../gl/buffer";
import { gl, Renderer } from "../renderer/renderer";
import { Texture2D, TextureCube, Texture } from "../gl/texture";
import { Viewport } from "../gl/viewport";
import { glm } from "../util/glm"

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
	private static offscreens = [];

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
	Offscreen
}
