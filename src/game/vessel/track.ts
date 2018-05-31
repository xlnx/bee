import { glm } from "../../util/glm";
import { Component } from "./component";
import { Shader } from "../../gl/shader";
import { TAO, TransformAttrs } from "../../gl/transformFeedback";
import { Vessel } from "./vessel";
import { Texture2D } from "../../gl/texture";
import { Renderer, gl } from "../../renderer/renderer";
import { Viewport } from "../../gl/viewport";

class Track extends Component {
	private static shader: Shader;

	private tao: TAO;
	private origin: glm.vec3;
	private lifetime: number;

	constructor(opts: any, parent: Vessel) {
		super(parent);
		Track.shader = Track.shader || Shader.create("track", false, [
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
		this.gVelocity.set(glm.vec3(0, 0, 1));
		this.texture.use("Track");
		this.tao.bind();
			this.tao.draw();
		this.tao.unbind();
		this.texture.unuse();
		this.tao.swap();
	}

	static bindShader() {
		!Track.shader || Track.shader.use();
	}
	static unbindShader() {
		!Track.shader || Track.shader.unuse();
	}

	private gDt = Shader.uniform("float", "gDt");
	private gOrigin = Shader.uniform("vec3", "gOrigin");
	private gVelocity = Shader.uniform("vec3", "gVelocity");
	private gLifetime = Shader.uniform("float", "gLifetime");
	private texture = new Texture2D("./assets/track.png");
}

export {
	Track
}