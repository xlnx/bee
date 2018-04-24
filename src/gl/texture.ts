import { gl } from "../renderer/renderer";

class Texture {
	public readonly handle: number;

	constructor(protected readonly type: number) {
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
	constructor(filename: string, usemipmap: boolean = true) {
		super(gl.TEXTURE_2D);
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
		}
		img.src = filename;
	}
}

export {
	Texture2D
}
