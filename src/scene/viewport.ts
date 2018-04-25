import { glm } from "../util/glm"
import { gl, Renderer } from "../renderer/renderer"

class Viewport {
	private N = glm.vec3(0, 1, 0);
	private P = glm.vec3(0, 0, 0);
	private Up = glm.vec3(0, 0, 1);
	
	private rotateModified = true;
	private translateModified = true;

	private rotateTrans: glm.mat4;
	private translateTrans: glm.mat4;

	private trans: glm.mat4;

	constructor();
	constructor(left: number, top: number, width: number, height: number);
	constructor(public readonly left?: number, public readonly top?: number,
		public readonly width?: number, public readonly height?: number)
	{
		if (left != undefined) {
			gl.viewport(left, top, width, height);
		} else {
			this.left = 0; this.top = 0;
			this.width = Renderer.instance.canvas.width;
			this.height = Renderer.instance.canvas.height;
		}
	}

	set target(target: glm.vec3) {
		this.N = glm.normalize(target);
		this.rotateModified = true;
	}
	get target(): glm.vec3 {
		return this.N;
	}
	set up(up: glm.vec3) {
		this.Up = glm.normalize(up);
		this.rotateModified = true;
	}
	get up(): glm.vec3 {
		return this.Up;
	}
	translate(diff: glm.vec3) {
		this.P["+="](diff);
		this.translateModified = true;
	}
	set position(position: glm.vec3) {
		this.P = position;
		this.translateModified = true;
	}
	get position(): glm.vec3 {
		return this.P;
	}
	getTrans(): glm.mat4 {
		if (this.translateModified || this.rotateModified) {
			if (this.translateModified) {
				this.translateModified = false;
				this.translateTrans = glm.mat4(
					1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, 1, 0,
					-this.P.x, -this.P.y, -this.P.z, 1
				);
			}
			if (this.rotateModified) {
				this.rotateModified = false;
				let U = glm.normalize(glm.cross(this.N, this.Up));
				let V = glm.cross(U, this.N);
				this.rotateTrans = glm.mat4(
					U.x, V.x, -this.N.x, 0, 
					U.y, V.y, -this.N.y, 0,
					U.z, V.z, -this.N.z, 0,
					0, 0, 0, 1
				);
			}
			this.trans = this.rotateTrans["*"](this.translateTrans);
		}
		return this.trans;
	}
	protected modified(): boolean {
		return this.translateModified || this.rotateModified;
	}
}

class PerspectiveViewport extends Viewport {
	private perspectiveTrans: glm.mat4;
	private viewportTrans: glm.mat4;

	private perspectiveModified = true;

	private ffov = glm.radians(60);
	private fzNear = 1e-3;
	private fzFar = 1e5;

	constructor();
	constructor(left: number, top: number, width: number, height: number);
	constructor(left?: number, top?: number, width?: number, height?: number) {
		super(left, top, width, height);
	}

	getTrans(): glm.mat4 {
		if (this.perspectiveModified || super.modified()) {
			if (this.perspectiveModified) {
				this.perspectiveModified = false;
				this.perspectiveTrans = glm.perspective(this.ffov, 
						this.width / this.height, this.fzNear, this.fzFar);
			}
			this.viewportTrans = this.perspectiveTrans["*"](super.getTrans());
		}
		return this.viewportTrans;
	}
	set fov(angle: number) {
		this.ffov = angle;
		this.perspectiveModified = true;
	}
	get fov(): number {
		return this.ffov;
	}
	set zNear(z: number) {
		this.fzNear = z;
		this.perspectiveModified = true;
	}
	get zNear(): number {
		return this.fzNear;
	}
	set zFar(z: number) {
		this.fzFar = z;
		this.perspectiveModified = true;
	}
	get zFar(): number {
		return this.fzFar;
	}
}

export {
	Viewport,
	PerspectiveViewport
}
