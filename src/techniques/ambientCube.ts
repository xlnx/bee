import { Viewport, PerspectiveViewport } from "../gl/viewport";
import { Communicator } from "../gl/communicator";
import { Offscreen, RenderBuffer } from "../gl/offscreen";
import { Shader, Uniform } from "../gl/shader";
import { gl, Renderer } from "../renderer/renderer";
import { TextureCube } from "../gl/texture";
import { glm } from "../util/glm"
import { VAO, VertexAttrs } from "../gl/vertexAttrs";

class AmbientCube {
	public readonly texture = new TextureCube({ component: gl.RGBA }, 
		Renderer.instance.canvas.height / 2, Renderer.instance.canvas.height / 2);
	private viewport = new Viewport(0, 0, 
		Renderer.instance.canvas.height / 2, Renderer.instance.canvas.height / 2);
	private flightDir: glm.vec3;

	constructor() {
		let vertices = new VertexAttrs(["pos2"]);
		vertices.set("pos2", [
			-1, 1, 1, 1, 1, -1, -1, -1
		]);
		this.vao = new VAO(vertices, [
			0, 1, 2, 0, 2, 3
		]);
	}

	setTime(hour: number) {
		this.dayTimeSec = 3600 * (hour - 12);
	}

	render() {
		let refFactor = this.dayScale * Renderer.timescale / (24 * 60 * 0.08);
		if (refFactor > 6) {
			refFactor = 6;
		} else if (refFactor < 1 / 60) {
			refFactor = 1 / 60;
		}
		let prevRenderFace = Math.floor(this.currRenderFace);
		this.currRenderFace += refFactor;

		this.dayTimeSec += this.dayScale * Renderer.dt; 

		if (Math.floor(this.currRenderFace) > prevRenderFace) {

			this.offscreen.bind();
				gl.disable(gl.DEPTH_TEST);
					this.viewport.use();
						this.shader.use();
							let alpha = this.dayTimeSec * 2 * Math.PI / 24 / 3600;

							this.gTime.set(Renderer.time);
							this.gSunPos.set(glm.vec3(Math.sin(alpha), 0, Math.cos(alpha)));
							this.flightDir = glm.vec3(-Math.sin(alpha), 0, -Math.cos(alpha));

							// for (let i = 0; i != 6; ++i) {
							for (let i = prevRenderFace; i != Math.floor(this.currRenderFace); ++i) {
								this.offscreen.set(gl.COLOR_ATTACHMENT0, this.texture, i % 6);
								// gl.clear(gl.COLOR_BUFFER_BIT);
								this.gSpace.set(AmbientCube.spaceTrans[i % 6]);
								
								this.vao.bind();
								this.vao.draw();
								this.vao.unbind();
							}
							
						this.shader.unuse();
					this.viewport.unuse();
				gl.enable(gl.DEPTH_TEST);
			this.offscreen.unbind();
		}

		this.currRenderFace %= 6;
	}

	get lightDir(): glm.vec3 {
		return this.flightDir;
	}

	private offscreen = new Offscreen();
	private vao: VAO;
	
	private time: number = 0;
	private dayTimeSec: number = 0;
	private currRenderFace: number = 0;
	
	private gSpace: Uniform = Shader.uniform("mat4", "gSpace");
	private gSunPos: Uniform = Shader.uniform("vec3", "gSunPos");
	private gTime: Uniform = Shader.uniform("float", "gTime");
	
	private shader = Shader.create("ambientCube", false);
	
	private static spaceTrans = [
		glm.mat4( 0, 0, -1, 0, 0, -1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 ),
		glm.mat4( 0, 0, 1, 0, 0, -1, 0, 0, 0, 0, 0, 0, -1, 0, 0, 1 ),
		glm.mat4( 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1 ),
		glm.mat4( 1, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, -1, 0, 1 ),
		glm.mat4( 1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1 ),
		glm.mat4( -1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, -1, 1 )
	];

	private dayScale: number = 24;
}

export {
	AmbientCube
}
