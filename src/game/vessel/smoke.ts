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
	private velocity: glm.vec3;
	private up: glm.vec3;
	private color: glm.vec3;
	private scatter: number;
	private lifetime: number;
	private opacity: number;
	private texture: Texture2D;

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
		this.velocity = parent.getTrans()["*"](
				glm.vec4(opts.velocity[0], opts.velocity[1], opts.velocity[2], 0)).xyz;
		this.up = glm.vec3(opts.up[0], opts.up[1], opts.up[2]);
		this.scatter = opts.scatter;
		this.color = glm.vec3(opts.color[0], opts.color[1], opts.color[2])["/"](255);
		this.opacity = opts.opacity;
		this.texture = new Texture2D("./assets/" + opts.texture + ".png");
		for (let i = 0; i != opts.count; ++i) {
			attrs.push({
				WorldPos: [1e8, 1e8, 0, 1],
				Lifetime: [i / opts.count * this.lifetime],
				Velocity: opts.velocity
			});
		}
		this.tao = new TAO(attrs);
	}

	render(viewport: Viewport) {
		this.setBasicUniforms(viewport);
		this.gDt.set(Renderer.dt);
		this.gOrigin.set(this.origin);
		this.gLifetime.set(this.lifetime);
		this.gScatter.set(this.scatter);
		this.gVelocity.set(this.velocity);
		this.gColor.set(this.color);
		this.gUp.set(this.up);
		this.gOpacity.set(this.opacity);
		let tex = Texture2D.genChannel();
		// this.texture.use("Smoke");
		gl.activeTexture(gl.TEXTURE0 + tex);
		this.gSmoke.set(tex);
		gl.bindTexture(gl.TEXTURE_2D, this.texture.handle);
		this.tao.bind();
			this.tao.draw();
		this.tao.unbind();
		gl.bindTexture(gl.TEXTURE_2D, null);
		Texture2D.restoreChannel(tex);
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
	private gVelocity = Shader.uniform("vec3", "gVelocity");
	private gUp = Shader.uniform("vec3", "gUp");
	private gLifetime = Shader.uniform("float", "gLifetime");
	private gScatter = Shader.uniform("float", "gScatter");
	private gSmoke = Shader.uniform("int", "gSmoke");
	private gColor = Shader.uniform("vec3", "gColor");
	private gOpacity = Shader.uniform("float", "gOpacity");
}

export {
	Smoke
}