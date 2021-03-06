import { glm } from "../../util/glm";
import Obj from "../../object/object";
import { Viewport, PerspectiveViewport } from "../../gl/viewport";
import { Shader } from "../../gl/shader";
import { TAO, TransformAttrs } from "../../gl/transformFeedback";
import { gl, Renderer } from "../../renderer/renderer";

class Explode extends Obj {
	private static shader: Shader;

	private tao: TAO;
	private pos: glm.vec3;
	private speed: number = 2.0;
	private lifetime: number = 14;
	private time: number = 14 - 0.1;
	

	constructor(position: glm.vec2) {
		super();
		Explode.shader || (Explode.shader = Shader.create("explode", false, [
			"WorldPos", 
			"Lifetime", 
			"Velocity", 
			"Acceleration",
			"GroupId", 
			"InstanceId", 
			"StartTime"
		]));
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
		}, {
			name: "Acceleration",
			type: gl.FLOAT,
			size: 1
		}, {
			name: "GroupId",
			type: gl.FLOAT,
			size: 1
		}, {
			name: "InstanceId",
			type: gl.FLOAT,
			size: 1
		}, {
			name: "StartTime",
			type: gl.FLOAT,
			size: 1
		}]);
		this.pos = glm.vec3(position, 0.01);
		let ntracks = 16;
		let nparticles = 10;
		for (let i = 0; i != ntracks; ++i) {
			for (let j = 0; j != nparticles; ++j) {
				attrs.push({
					WorldPos: [1e8, 1e8, 0, 1],
					Lifetime: [this.lifetime],
					Velocity: [0, 0, 0],
					Acceleration: [0],
					GroupId: [i],
					InstanceId: [nparticles - j],
					StartTime: [Renderer.time]
				});
			}
		}
		this.tao = new TAO(attrs);
	}
	
	render(viewport: Viewport) {
		this.setBasicUniforms(viewport);
		this.gDt.set(Renderer.dt);
		this.gOrigin.set(this.pos);
		this.gLifetime.set(this.lifetime);
		this.gSpeed.set(this.speed);
		this.gIP.set(glm.inverse((<PerspectiveViewport>viewport).getPers()));
		this.tao.bind();
			this.tao.draw();
		this.tao.unbind();
		this.tao.swap();
	}

	checkRange(): boolean {
		this.time -= Renderer.dt;
		return this.time > 0;
	}

	static bindShader() {
		!Explode.shader || Explode.shader.use();
	}
	static unbindShader() {
		!Explode.shader || Explode.shader.unuse();
	}

	private gDt = Shader.uniform("float", "gDt");
	private gOrigin = Shader.uniform("vec3", "gOrigin");
	private gLifetime = Shader.uniform("float", "gLifetime");
	private gSpeed = Shader.uniform("float", "gSpeed");
	private gIP = Shader.uniform("mat4", "gIP");
}

export {
	Explode
}