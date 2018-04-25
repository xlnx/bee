import { gl, Renderer, gl2 } from "../renderer/renderer";
import { FBO } from "./buffer";

abstract class Texture {
	public readonly handle: number;

	constructor(public readonly type: number) {
		this.handle = <number>gl.createTexture();
	}
	invoke(channel: number) {
		gl.activeTexture(gl.TEXTURE0 + channel);
		this.bind();
	}
	bind() {
		gl.bindTexture(this.type, this.handle);
	}
	unbind() {
		gl.bindTexture(this.type, null);
	}
}

class Texture2D extends Texture {
	private w: number;
	private h: number;

	constructor(component: number);
	constructor(component: number, width: number, height: number);
	constructor(filename: string);
	constructor(filename: string, usemipmap: boolean);
	constructor(first?: any, second?: any, third?: any) {
		super(gl.TEXTURE_2D);
		if (typeof first == "string") {
			let filename: string = first;
			let usemipmap: boolean = second == undefined ? true : second;
			this.bind();
				gl.texImage2D(this.type, 0, gl.RGBA, 1, 1, 0, gl.RGBA, gl.UNSIGNED_BYTE, new Uint8Array([0, 0, 0, 0]));
			this.unbind();
			let img = new Image();
			img.onload = () => {
				this.bind();
					gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
					gl.texImage2D(this.type, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, img);
					// gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
					// gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
					gl.texParameteri(this.type, gl.TEXTURE_WRAP_S, gl.REPEAT);
					gl.texParameteri(this.type, gl.TEXTURE_WRAP_T, gl.REPEAT);
					gl.texParameteri(this.type, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
					if (usemipmap) {
						gl.texParameteri(this.type, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_LINEAR);
						gl.generateMipmap(this.type);
					} else {
						gl.texParameteri(this.type, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
					}
				this.unbind();
				this.w = img.width; this.h = img.height;
			}
			img.src = filename;
		} else {
			let component: number = first;
			this.w = second;
			this.h = third;
			if (second == undefined) {
				this.w = Renderer.instance.canvas.width;
				this.h = Renderer.instance.canvas.height;
			}
			this.bind();
				switch (component) {
					case gl.DEPTH_COMPONENT: {
						gl.texImage2D(this.type, 0, component, this.w, this.h, 0, component, gl.FLOAT, null);
					} break;
					default: {
						gl.texImage2D(this.type, 0, component, this.w, this.h, 0, component, gl.UNSIGNED_BYTE, null);
					} break;
				}
				// } else {
				// 	gl.texImage2D(this.type, 0, component, 1, 1, 0, component, gl.FLOAT, null);
				// }
				gl.texParameterf(this.type, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
				gl.texParameterf(this.type, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
				gl.texParameterf(this.type, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
				gl.texParameterf(this.type, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
			this.unbind();
		}
	}

	get width() {
		return this.w;
	}
	get height() {
		return this.h;
	}
}

class TextureCube extends Texture {
	constructor(component: number);
	constructor(component: number, width?: number, height?: number) {
		super(gl.TEXTURE_CUBE_MAP);
		if (width == undefined) {
			width = height = Renderer.instance.canvas.height;
		}
		this.bind();
			gl.texParameterf(this.type, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
			gl.texParameterf(this.type, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
			gl.texParameterf(this.type, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
			gl.texParameterf(this.type, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
			if (gl2) {
				gl.texParameterf(this.type, gl2.TEXTURE_WRAP_R, gl.CLAMP_TO_EDGE);
			}

			for (let i = 0; i != 6; ++i) {
				gl.texImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, component, 
						width, height, 0, component, gl.UNSIGNED_BYTE, null);
			}
		this.unbind();
	}
}

export {
	Texture,
	Texture2D,
	TextureCube
}
