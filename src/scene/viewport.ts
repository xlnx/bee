import { glm } from "../util/glm"
import { gl, Renderer } from "../renderer/renderer"

class Viewport {
	private static viewports: Viewport[] = [];

	protected N = glm.vec3(0, 1, 0);
	protected P = glm.vec3(0, 0, 0);
	protected Up = glm.vec3(0, 0, 1);
	
	protected rotateModified = true;
	protected translateModified = true;

	protected rotateTrans: glm.mat4;
	protected translateTrans: glm.mat4;

	protected trans: glm.mat4;

	constructor();
	constructor(left: number, top: number, width: number, height: number);
	constructor(public readonly left?: number, public readonly top?: number,
		public readonly width?: number, public readonly height?: number)
	{
		if (left != undefined) {
			// gl.viewport(left, top, width, height);
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
	rotate(angle: glm.vec3) {
		this.N = glm.rotate(angle.z, this.Up)["*"](glm.vec4(this.N, 0)).xyz;
		let U = glm.cross(this.N, this.Up);
		this.N = glm.rotate(angle.y, U)["*"](glm.vec4(this.N, 0)).xyz;
		this.Up = glm.rotate(angle.x, this.N)["*"](glm.vec4(this.Up, 0)).xyz;
		this.rotateModified = true;
	}
	translate(diff: glm.vec3) {
		let U = glm.normalize(glm.cross(this.N, this.Up));
		let V = glm.cross(U, this.N);
		this.P["+="](this.N["*"](diff.x)["+"](U["*"](-diff.y))["+"](V["*"](diff.z)));
		this.translateModified = true;
	}
	set position(position: glm.vec3) {
		this.P = position;
		this.translateModified = true;
	}
	get position(): glm.vec3 {
		return this.P;
	}
	getView(): glm.mat4 {
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
	getTrans(): glm.mat4 {
		return this.getView();
	}
	use() {
		Viewport.viewports.push(this);
		gl.viewport(this.left, this.top, this.width, this.height);
	}
	unuse() {
		if (Viewport.viewports.length) {
			Viewport.viewports.pop();
			if (Viewport.viewports.length) {
				let self = Viewport.viewports[Viewport.viewports.length - 1];
				gl.viewport(self.left, self.top, self.width, self.height);
			}
		}
	}
	protected viewModified(): boolean {
		return this.translateModified || this.rotateModified;
	}
	protected modified(): boolean {
		return this.viewModified();
	}
}

class PerspectiveViewport extends Viewport {
	protected perspectiveTrans: glm.mat4;
	protected viewportTrans: glm.mat4;
	
	protected perspectiveModified = true;
	
	protected fwhratio: number;
	protected ffov = glm.radians(45);
	protected fzNear = 2e-1;
	protected fzFar = 1e3;

	constructor();
	constructor(left: number, top: number, width: number, height: number);
	constructor(left?: number, top?: number, width?: number, height?: number) {
		super(left, top, width, height);
		this.fwhratio = this.width / this.height;
	}

	getPers(): glm.mat4 {
		if (this.persModified()) {
			this.perspectiveModified = false;
			this.perspectiveTrans = glm.perspective(this.ffov, 
					this.fwhratio, this.fzNear, this.fzFar);
		}
		return this.perspectiveTrans;
	}
	getTrans(): glm.mat4 {
		if (this.modified()) {
			this.viewportTrans = this.getPers()["*"](this.getView());
		}
		return this.viewportTrans;
	}
	set whratio(value: number) {
		this.fwhratio = value;
		this.perspectiveModified = true;
	}
	get whratio(): number {
		return this.fwhratio;
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
	protected persModified(): boolean {
		return this.perspectiveModified;
	}
	protected modified(): boolean {
		return this.viewModified() || this.persModified();
	}
}

export {
	Viewport,
	PerspectiveViewport
}
