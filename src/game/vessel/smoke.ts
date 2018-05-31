import { glm } from "../../util/glm";
import { Viewport } from "../../gl/viewport";
import { TAO, TransformAttrs } from "../../gl/transformFeedback";
import { Shader } from "../../gl/shader";
import { gl, Renderer } from "../../renderer/renderer";
import { Vessel } from "./vessel";
import { Component } from "./component";
import { Texture, Texture2D } from "../../gl/texture";

class Smoke extends Component {
	private static shader: Shader;

	private tao: TAO;
	private origin: glm.vec3;
	private lifetime: number;

	constructor(opts: any, parent: Vessel) {
		super(parent);
		Smoke.shader = Smoke.shader || Shader.create("smoke", false, [
			"WorldPos", "Lifetime", "Velocity"
		]);
		// console.log(opts);
		let attrs = new TransformAttrs([{
			name: "WorldPos",
			type: gl.FLOAT,
			size: 4
		}, {
			name: "Lifetime",
			type: gl.FLOAT,
			size: 1
		}, {
			name: "Velocity",
			type: gl.FLOAT,
			size: 3
		}]);
		this.origin = glm.vec3(opts.position[0], opts.position[1], opts.position[2]);
		this.lifetime = opts.lifetime;
		for (let i = 0; i != opts.count; ++i) {
			attrs.push({
				WorldPos: [this.origin.x, this.origin.y, this.origin.z, 1],
				Lifetime: [i / opts.count * this.lifetime],
				Velocity: [0, 0, 1]
			});
		}
		this.tao = new TAO(attrs);
	}

	render(viewport: Viewport) {
		this.setBasicUniforms(viewport);
		this.gDt.set(Renderer.dt);
		this.gOrigin.set(this.origin);
		this.gLifetime.set(this.lifetime);
		this.texture.use("Smoke");
		this.tao.bind();
			this.tao.draw();
		this.tao.unbind();
		this.texture.unuse();
		this.tao.swap();
	}
	
	static bindShader() {
		!Smoke.shader || Smoke.shader.use();
	}
	static unbindShader() {
		!Smoke.shader || Smoke.shader.unuse();
	}

	private gDt = Shader.uniform("float", "gDt");
	private gOrigin = Shader.uniform("vec3", "gOrigin");
	private gLifetime = Shader.uniform("float", "gLifetime");
	private texture = new Texture2D("./assets/smoke.png");
}

export {
	Smoke
}