import { Viewport, PerspectiveViewport } from "../scene/viewport";
import { Communicator } from "../gl/communicator";
import { Offscreen, RenderBuffer } from "./offscreen";
import { Shader, Uniform } from "../gl/shader";
import { gl, Renderer } from "../renderer/renderer";
import { TextureCube } from "../gl/texture";
import { glm } from "../util/glm"
import { VAO, VertexAttrs } from "../gl/vertexAttrs";

class AmbientCube {
	public readonly texture = new TextureCube(gl.RGBA);

	constructor() {
		let vertices = new VertexAttrs(["pos2"]);
		vertices.set("pos2", [
			-1, 1, 1, 1, 1, -1, -1, -1
		]);
		this.vao = new VAO(vertices, [
			0, 1, 2, 0, 2, 3
		]);
	}

	render() {
		let dt = Renderer.instance.time - this.time;
		this.time = Renderer.instance.time;

		this.offscreen.bind();
			gl.disable(gl.DEPTH_TEST);
				this.viewport.use();
					this.shader.use();
						let alpha = Renderer.instance.time * this.dayScale * 2 * Math.PI / 24 / 3600;

						this.gTime.set(Renderer.instance.time);
						this.gSunPos.set(glm.vec3(Math.sin(alpha), 0, Math.cos(alpha)));
						
						for (let i = 0; i != 6; ++i) {
							this.offscreen.set(gl.COLOR_ATTACHMENT0, this.texture, i);

							// gl.clear(gl.COLOR_BUFFER_BIT);
							this.gSpace.set(AmbientCube.spaceTrans[i]);

							this.vao.bind();
								this.vao.draw();
							this.vao.unbind();
						}
					this.shader.unuse();
				this.viewport.unuse();
			gl.enable(gl.DEPTH_TEST);
		this.offscreen.unbind();
	}

	private offscreen = new Offscreen();
	private vao: VAO;
	
	private time: number = 0;
	
	private gSpace: Uniform = Shader.uniform("mat4", "gSpace");
	private gSunPos: Uniform = Shader.uniform("vec3", "gSunPos");
	private gTime: Uniform = Shader.uniform("float", "gTime");
	
	private shader = Shader.create("ambientCube", false);
	private viewport = new Viewport(0, 0, 
		Renderer.instance.canvas.height, Renderer.instance.canvas.height);
	
	static spaceTrans = [
		glm.mat4( 0, 0, -1, 0, 0, -1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 ),
		glm.mat4( 0, 0, 1, 0, 0, -1, 0, 0, 0, 0, 0, 0, -1, 0, 0, 1 ),
		glm.mat4( 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1 ),
		glm.mat4( 1, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, -1, 0, 1 ),
		glm.mat4( 1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1 ),
		glm.mat4( -1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, -1, 1 )
	];

	private dayScale: number = 24 * 60 * 6;
}

export {
	AmbientCube
}
