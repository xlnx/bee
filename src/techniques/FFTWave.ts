import { PostProcess } from "./postProcess";
import { Shader } from "../gl/shader";
import { Texture2D } from "../gl/texture";
import { gl2, gl, Renderer } from "../renderer/renderer";
import { Offscreen } from "./offscreen";

class FFTWave extends PostProcess {
	private N: number;

	constructor(private spectrum: Texture2D) {
		super(Shader.create("fftsrc", false));
		console.log(spectrum);
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
		this.displacement = new Texture2D({
			component: gl.RGBA, type: gl.FLOAT, wrap: gl.REPEAT, filter: gl.LINEAR
		}, this.N, this.N);
		for (let i = 0; i != 3; ++i) {
			this.textureA[i] = new Texture2D({ 
				component: gl2.RG, type: gl.FLOAT, wrap: gl.REPEAT, filter: gl.NEAREST
			}, this.N, this.N);
		}
		for (let i = 0; i != 3; ++i) {
			this.textureB[i] = new Texture2D({ 
				component: gl2.RG, type: gl.FLOAT, wrap: gl.REPEAT, filter: gl.NEAREST
			}, this.N, this.N);
		}
	}
	private bindTextures() {
		for (let i = 0; i != 3; ++i) {
			this.offscreen.set(gl.COLOR_ATTACHMENT0 + i, this.textureB[i]);
			gl.activeTexture(gl.TEXTURE0 + this.channel[i]);
			gl.bindTexture(gl.TEXTURE_2D, this.textureA[i].handle);
			this.gPrev[i].set(this.channel[i]);
		}
	}
	render() {
		gl.disable(gl.DEPTH_TEST);
		gl.viewport(0, 0, this.N, this.N);
		this.offscreen.bind();
			this.channel = [ 
				Texture2D.genChannel(), 
				Texture2D.genChannel(), 
				Texture2D.genChannel()
			];
			this.vao.bind();
				// open MRT
				gl2.drawBuffers([
					gl.COLOR_ATTACHMENT0, 
					gl2.COLOR_ATTACHMENT1,
					gl2.COLOR_ATTACHMENT2
				]);
				// generate channels: H(t), Dx(t), Dy(t)
				this.fsrc.use();
					for (let i = 0; i != 3; ++i) {
						this.offscreen.set(gl.COLOR_ATTACHMENT0 + i, this.textureA[i]);
					}
					gl.activeTexture(gl.TEXTURE0 + this.channel[0]);
					gl.bindTexture(gl.TEXTURE_2D, this.spectrum.handle);
					this.gSpectrum.set(this.channel[0]);
					this.gTime.set(Renderer.time);
					this.vao.draw();
				this.fsrc.unuse();
				// vertical bit-reversal copy
				this.vrs.use();
					this.gN.set(this.N);
					this.bindTextures();
					this.vao.draw();
					this.swapTextures();
				this.vrs.unuse();
				// vertical FFT
				this.vs.use();
					this.gN.set(this.N);
					for (let i = 1; i < this.N; i *= 2) {
						this.gStep.set(i);
						this.bindTextures();
						this.vao.draw();
						this.swapTextures();
					}
				this.vs.unuse();
				// horizontal bit-reversal copy
				this.hrs.use();
					this.gN.set(this.N);
					this.bindTextures();
					this.vao.draw();
					this.swapTextures();
				this.hrs.unuse();
				// horizontal FFT
				this.hs.use();
					this.gN.set(this.N);
					for (let i = 1; i < this.N; i *= 2) {
						this.gStep.set(i);
						this.bindTextures();
						this.vao.draw();
						this.swapTextures();
					}
				this.hs.unuse();
				// combine H, Dx, Dy into displacement map
				this.fend.use();
					this.gN.set(this.N);
					this.bindTextures();
					this.offscreen.set(gl.COLOR_ATTACHMENT0, this.displacement);
					this.vao.draw();
				this.fend.unuse();
				// close MRT
				gl2.drawBuffers([
					gl.COLOR_ATTACHMENT0
				]);
			this.vao.unbind();
			for (let i = 0; i != 3; ++i) {
				Texture2D.restoreChannel(this.channel[i]);
			}
		this.offscreen.unbind();
		gl.viewport(0, 0, Renderer.instance.canvas.width, Renderer.instance.canvas.height);
		gl.enable(gl.DEPTH_TEST);
	}

	get texture(): Texture2D {
		return this.displacement;
	}

	get textures(): Texture2D[] {
		return this.textureA;
	}

	private swapTextures() {
		let t = this.textureA; this.textureA = this.textureB; this.textureB = t;
	}

	private textureA: Texture2D[] = [];
	private textureB: Texture2D[] = [];

	private displacement: Texture2D;

	private channel: number[];

	private offscreen = new Offscreen();

	private gN = Shader.uniform("float", "gN");
	private gStep = Shader.uniform("float", "gStep");
	private gPrev = [ 
		Shader.uniform("int", "gPrevH"), 
		Shader.uniform("int", "gPrevDx"),
		Shader.uniform("int", "gPrevDy")
	];
	private gSpectrum = Shader.uniform("int", "gSpectrum");

	private fsrc = Shader.create("fftsrc", false);
	private vs = Shader.create("fftv", false);
	private hs = Shader.create("ffth", false);
	private vrs = Shader.create("fftvr", false);
	private hrs = Shader.create("ffthr", false);
	private fend = Shader.create("fftend", false);
}

export {
	FFTWave
}