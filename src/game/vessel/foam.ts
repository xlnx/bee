import { glm } from "../../util/glm";
import { Component } from "./component";
import { Shader } from "../../gl/shader";
import { Vessel } from "./vessel";
import { TransformAttrs, TAO } from "../../gl/transformFeedback";
import { gl, Renderer } from "../../renderer/renderer";
import { Viewport } from "../../gl/viewport";
import { Texture2D } from "../../gl/texture";

class Foam extends Component {
	private static shader: Shader;

	private tao: TAO;
	private origin: glm.vec2;
	private lifetime: number;
	private texture: Texture2D = new Texture2D("./assets/foam.png");

	constructor(opts: any, parent: Vessel) {
		super(parent);
		Foam.shader = Foam.shader || Shader.create("foam", false, [
			"WorldPos", "Offset", "Lifetime"
		]);
		let attrs = new TransformAttrs([{
			name: "WorldPos",
			type: gl.FLOAT,
			size: 4
		}, {
			name: "Offset",
			type: gl.FLOAT,
			size: 2
		}, {
			name: "Lifetime",
			type: gl.FLOAT,
			size: 1
		}]);
		this.origin = glm.vec2(opts.position[0], opts.position[1]);
		this.lifetime = opts.lifetime;
		console.log(this.lifetime);
		for (let i = 0; i != opts.count; ++i) {
			attrs.push({
				WorldPos: [1e8, 1e8, 0, 1],
				Offset: [-0.7, 0.7],
				Lifetime: [i / opts.count * this.lifetime],
			});
			attrs.push({
				WorldPos: [1e8, 1e8, 0, 1],
				Offset: [0.7, 0.7],
				Lifetime: [i / opts.count * this.lifetime],
			});
			attrs.push({
				WorldPos: [1e8, 1e8, 0, 1],
				Offset: [-0.7, -0.7],
				Lifetime: [i / opts.count * this.lifetime],
			});
			attrs.push({
				WorldPos: [1e8, 1e8, 0, 1],
				Offset: [0.7, 0.7],
				Lifetime: [i / opts.count * this.lifetime],
			});
			attrs.push({
				WorldPos: [1e8, 1e8, 0, 1],
				Offset: [-0.7, -0.7],
				Lifetime: [i / opts.count * this.lifetime],
			});
			attrs.push({
				WorldPos: [1e8, 1e8, 0, 1],
				Offset: [0.7, -0.7],
				Lifetime: [i / opts.count * this.lifetime],
			});
		}
		this.tao = new TAO(attrs);
	}

	render(viewport: Viewport) {
		this.setBasicUniforms(viewport);
		this.gDt.set(Renderer.dt);
		this.gOrigin.set(this.origin);
		this.gLifetime.set(this.lifetime);
		let tex = Texture2D.genChannel();
		gl.activeTexture(gl.TEXTURE0 + tex);
		this.gOutline.set(tex);
		gl.bindTexture(gl.TEXTURE_2D, this.texture.handle);
		this.tao.bind();
			this.tao.draw(gl.TRIANGLES);
		this.tao.unbind();
		gl.bindTexture(gl.TEXTURE_2D, null);
		Texture2D.restoreChannel(tex);
		this.tao.swap();
	}

	static bindShader() {
		!Foam.shader || Foam.shader.use();
	}
	static unbindShader() {
		!Foam.shader || Foam.shader.unuse();
	}

	private gDt = Shader.uniform("float", "gDt");
	private gLifetime = Shader.uniform("float", "gLifetime");
	private gOrigin = Shader.uniform("vec2", "gOrigin");
	private gOutline = Shader.uniform("int", "gOutline");
}

export {
	Foam
}