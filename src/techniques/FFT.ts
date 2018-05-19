import { PostProcess } from "./postProcess";
import { Shader } from "../gl/shader";
import { Texture2D } from "../gl/texture";
import { gl2, gl } from "../renderer/renderer";
import { Offscreen } from "./offscreen";

class FFT extends PostProcess {
	private N: number;

	constructor(private spectrum: Texture2D) {
		super(Shader.create("playground/fftsrc", false));
		if (spectrum.width != spectrum.height) {
			throw "FFT spectrum must be square.";
		}
		this.N = spectrum.width;
		if (Math.pow(2, Math.log2(this.N)) != this.N) {
			throw "FFT size must be 2^k";
		}
		if (!gl2) {
			throw "webgl 2.0 required.";
		}
		this.textureA = new Texture2D({ 
			component: gl2.RG, type: gl.FLOAT, wrap: gl.REPEAT, filter: gl.NEAREST
		}, this.N, this.N);
		this.textureB = new Texture2D({ 
			component: gl2.RG, type: gl.FLOAT, wrap: gl.REPEAT, filter: gl.NEAREST
		}, this.N, this.N);
	}
	render() {
		this.offscreen.bind();
			this.vao.bind();
				let ch = Texture2D.genChannel();
				gl.activeTexture(gl.TEXTURE0 + ch);
				this.vrs.use();
					this.gN.set(this.N);
					this.gPrevStep.set(ch);

					this.offscreen.set(gl.COLOR_ATTACHMENT0, this.textureA);
					gl.bindTexture(gl.TEXTURE_2D, this.spectrum.handle);
					this.vao.draw();
				this.vrs.unuse();
				this.vs.use();
					this.gN.set(this.N);
					this.gPrevStep.set(ch);

					for (let i = 1; i < this.N; i *= 2) {
						this.offscreen.set(gl.COLOR_ATTACHMENT0, this.textureB);
						this.gStep.set(i);
						gl.bindTexture(gl.TEXTURE_2D, this.textureA.handle);
						this.vao.draw();
						let t = this.textureA; this.textureA = this.textureB; this.textureB = t;
					}
				this.vs.unuse();
				this.hrs.use();
					this.gN.set(this.N);
					this.gPrevStep.set(ch);

					this.offscreen.set(gl.COLOR_ATTACHMENT0, this.textureB);
					gl.bindTexture(gl.TEXTURE_2D, this.textureA.handle);
					this.vao.draw();
					let t = this.textureA; this.textureA = this.textureB; this.textureB = t;
				this.hrs.unuse();
				this.hs.use();
					this.gN.set(this.N);
					this.gPrevStep.set(ch);

					for (let i = 1; i < this.N; i *= 2) {
						this.offscreen.set(gl.COLOR_ATTACHMENT0, this.textureB);
						this.gStep.set(i);
						gl.bindTexture(gl.TEXTURE_2D, this.textureA.handle);
						this.vao.draw();
						let t = this.textureA; this.textureA = this.textureB; this.textureB = t;
					}
				this.hs.unuse();
				this.fend.use();
					this.gN.set(this.N);
					this.gPrevStep.set(ch);

					this.offscreen.set(gl.COLOR_ATTACHMENT0, this.textureB);
					gl.bindTexture(gl.TEXTURE_2D, this.textureA.handle);
					this.vao.draw();
					t = this.textureA; this.textureA = this.textureB; this.textureB = t;
				this.fend.unuse();
				Texture2D.restoreChannel(ch);
			this.vao.unbind();
		this.offscreen.unbind();
	}

	get texture(): Texture2D {
		return this.textureA;
	}

	private textureA: Texture2D;
	private textureB: Texture2D;

	private offscreen = new Offscreen();

	private gN = Shader.uniform("float", "gN");
	private gStep = Shader.uniform("float", "gStep");
	private gPrevStep = Shader.uniform("int", "gPrevStep");

	private vs = Shader.create("playground/fftv", false);
	private hs = Shader.create("playground/ffth", false);
	private vrs = Shader.create("playground/fftvr", false);
	private hrs = Shader.create("playground/ffthr", false);
	private fend = Shader.create("playground/fftend", false);
}

export {
	FFT
}