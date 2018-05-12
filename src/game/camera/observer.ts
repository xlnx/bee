import { CameraBase } from "./cameraBase";
import { Renderer, RendererEvent } from "../../renderer/renderer";
import { glm } from "../../util/glm"

function clamp(x: number, min: number, max: number): number {
	return Math.max(min, Math.min(max, x));
}

class Observer extends CameraBase {
	private threshord: number = 200;

	private oTrans: glm.mat4;
	private rTrans: glm.mat4;
	private qTrans: glm.mat4;
	private tTrans: glm.mat4;
	private mTrans: glm.mat4;
	private wTrans: glm.mat4;
	private pTrans: glm.mat4;

	private pos: glm.vec3;

	private rModified: boolean = true;
	private tModified: boolean = true;

	private R = glm.vec3(glm.radians(0), glm.radians(-15), glm.radians(0));

	private dist: number = 4;
	private minDist: number = 4 * 0.4;
	private maxDist: number = 4 * 2;

	private mousewheel: RendererEvent;
	private mousemove: RendererEvent;
	private mouseup: RendererEvent;
	private mousedown: RendererEvent;

	private prevx: number = 0;
	private prevy: number = 0;
	private isDragging: boolean = false;

	constructor(left?: number, top?: number, width?: number, height?: number) {
		super(left, top, width, height);
		this.N = glm.vec3(-1, 0, 0);
		this.Up = glm.vec3(0, 0, 1);
	}

	getCameraPosition(): glm.vec3 {
		if (this.rModified || this.tModified || this.translateModified) {
			this.updatePTrans();
		}
		return this.pos;
	}
	get target(): glm.vec3 {
		return glm.vec3(0, 0, 0);
	}
	set target(value: glm.vec3) {
	}
	get up(): glm.vec3 {
		return glm.vec3(0, 0, 0);
	}
	set up(value: glm.vec3) {
	}
	viewModified(): boolean {
		return this.rModified || this.tModified || this.translateModified ||
			this.rotateModified;
	}
	private updatePTrans() {
		if (this.rModified || this.tModified) {
			if (this.rModified) {
				this.rModified = false;
				let sina = Math.sin(this.R.y - Math.PI / 2), 
				sinz = Math.sin(this.R.z),
				cosa = Math.cos(this.R.y - Math.PI / 2), 
				cosz = Math.cos(this.R.z),
				cosy = Math.cos(this.R.y),
				siny = Math.sin(this.R.y);
				this.rTrans = glm.mat4(
					-sina * cosz, -sinz , cosa * cosz, 0,
					-sina * sinz, cosz  , cosa * sinz, 0,
					-cosa       , 0     , -sina      , 0,
					0           , 0     , 0          , 1
				);
				this.qTrans = glm.mat4(
					cosy * cosz, cosy * sinz, -siny , 0,
					-sinz      , cosz       , 0     , 0,
					siny * cosz, siny * sinz, cosy  , 0,
					0          , 0          , 0     , 1
				);
			}
			if (this.tModified) {
				this.tModified = false;
				this.tTrans = glm.mat4(
					1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, 1, 0,
					-this.dist, 0, 0, 1
				);
			}
			this.pTrans = this.tTrans["*"](this.rTrans);
		}
		this.pos = this.P["+"](this.qTrans["*"](glm.vec4(this.dist, 0, 0, 1)).xyz);
	}
	getView(): glm.mat4 {
		if (this.viewModified()) {
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
			if (this.translateModified || this.rModified || this.tModified) {
				this.updatePTrans();
				if (this.translateModified) {
					this.translateModified = false;
					this.translateTrans = glm.mat4(
						1, 0, 0, 0,
						0, 1, 0, 0,
						0, 0, 1, 0,
						-this.P.x, -this.P.y, -this.P.z, 1
					);
				}
				this.wTrans = this.pTrans["*"](this.translateTrans);
			}
			this.mTrans = this.rotateTrans["*"](this.wTrans);
		}
		return this.mTrans;
	}
	rotate(angle: glm.vec3) {
		this.R["+="](angle);
		this.R.y = clamp(this.R.y, -Math.PI / 2, Math.PI / 2);
		this.rModified = true;
	}
	set rotation(angle: glm.vec3) {
		this.R = angle;
		this.R.y = clamp(this.R.y, -Math.PI / 2, Math.PI / 2);
		this.rModified = true;
	}
	zoom(fact: number) {
		this.dist *= fact;
		this.dist = clamp(this.dist, this.minDist, this.maxDist);
		this.tModified = true;
	}

	bind() {
		this.mousewheel = Renderer.instance.dispatch("mousewheel", (e: MouseWheelEvent) => {
			if (!Renderer.pause) {
				let det = clamp(e.deltaY, -this.threshord, this.threshord);
				det = det / (2 * this.threshord) + 1;
				this.zoom(det);
			}
		});
		this.mousemove = Renderer.instance.dispatch("mousemove", (e: MouseEvent) => {
			if (this.isDragging) {
				if (Renderer.pause) {
					this.isDragging = false;
				} else {
					let c1 = Renderer.instance.canvas.height / Math.tan(this.ffov/2);
					let c2 = Renderer.instance.canvas.width;
					let h = 0.5 * Math.sqrt(c1 * c1 - c2 * c2);
					let dx1 = e.clientX - Renderer.instance.canvas.width / 2;
					let dx2 = this.prevx - Renderer.instance.canvas.width / 2;
					let dy1 = e.clientY - Renderer.instance.canvas.height / 2;
					let dy2 = this.prevy - Renderer.instance.canvas.height / 2;
					let thetax = Math.atan(dx1/h) - Math.atan(dx2/h);
					let thetay = Math.atan(dy1/h) - Math.atan(dy2/h);
					this.rotate(glm.vec3(0, -thetay, -thetax));
					this.prevx = e.clientX; this.prevy = e.clientY;
				}
			}
		});
		this.mouseup = Renderer.instance.dispatch("mouseup", (e: MouseEvent) => {
			this.isDragging = false;
		});
		this.mousedown = Renderer.instance.dispatch("mousedown", (e: MouseEvent) => {
			this.prevx = e.clientX;
			this.prevy = e.clientY;
			this.isDragging = true;
		});
	}
	unbind() {
		this.mousewheel.cancel();
		this.mousemove.cancel();
		this.mouseup.cancel();
		this.mousedown.cancel();
		this.prevx = 0;
		this.prevy = 0;
		this.isDragging = false;
	}
}

export {
	Observer
}
