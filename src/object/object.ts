import { glm } from "../util/glm"
import { Viewport } from "../gl/viewport";
import { Shader, Uniform } from "../gl/shader";
import { gl, Renderer } from "../renderer/renderer";

class ObjBase {
	private P = glm.vec3(0, 0, 0);
	private S = glm.vec3(1, 1, 1);
	private R = glm.vec3(0, 0, 0);
	
	private translateModified = true;
	private scaleModified = true;
	private rotateModified = true;
	
	private translateTrans: glm.mat4;
	private scaleTrans: glm.mat4;
	private rotateTrans: glm.mat4;

	private trans: glm.mat4;
	private itrans: glm.mat4;
	private ntrans: glm.mat4;

	scaleIdentity() {
		this.S = glm.vec3(1, 1, 1); 
		this.scaleModified = true;		
	}
	scale(factor: number);
	scale(factor: glm.vec3);
	scale(factor: any) {
		this.S["*="](factor);
		this.scaleModified = true;
	}
	set scaleFactor(factor: glm.vec3) {
		if (typeof factor == "number") {
			this.S = glm.vec3(factor, factor, factor);
		} else {
			this.S = factor;
		}
		this.scaleModified = true;
	}
	translateIdentity() {
		this.P = glm.vec3(0, 0, 0);
		this.translateModified = true;
	}
	translate(diff: glm.vec3) {
		this.P["+="](diff);
		this.translateModified = true;
	}
	get position(): glm.vec3 {
		return this.P;
	}
	set position(pos: glm.vec3) {
		this.P = pos;
		this.translateModified = true;
	}
	rotateIdentity();
	rotateIdentity(mask: glm.vec3);
	rotateIdentity(mask?: glm.vec3) {
		if (mask) {
			this.R = this.R["*"](mask);
		} else {
			this.R = glm.vec3(0, 0, 0);
		}
		this.rotateModified = true;
	}
	rotate(angle: glm.vec3) {
		this.R["+="](angle);
		this.rotateModified = true;
	}
	private updateTrans() {
		if (this.translateModified || this.scaleModified || this.rotateModified) {
			if (this.translateModified) {
				this.translateModified = false;
				this.translateTrans = glm.mat4(
					1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, 1, 0,
					this.P.x, this.P.y, this.P.z, 1
				);
			}
			if (this.rotateModified) {
				this.rotateModified = false;
				let sinx = Math.sin(this.R.x), 
					siny = Math.sin(this.R.y), 
					sinz = Math.sin(this.R.z),
					cosx = Math.cos(this.R.x), 
					cosy = Math.cos(this.R.y), 
					cosz = Math.cos(this.R.z),
					sycz = siny * cosz, sysz = siny * sinz;
				this.rotateTrans = glm.mat4(
					cosz * cosy         , sinz * cosy         , siny     , 0,
					-cosx*sinz-sinx*sycz, cosx*cosz-sinx*sysz , sinx*cosy, 0,
					sinx*sinz-cosx*sycz , -sinx*cosz-cosx*sysz, cosx*cosy, 0,
					0                   , 0                   , 0        , 1
				);
			}
			if (this.scaleModified) {
				this.scaleModified = false;
				this.scaleTrans = glm.mat4(
					this.S.x, 0, 0, 0,
					0, this.S.y, 0, 0,
					0, 0, this.S.z, 0,
					0, 0, 0, 1
				);
			}
			this.ntrans = this.translateTrans["*"](this.rotateTrans);
			this.trans = this.ntrans["*"](this.scaleTrans);
			this.itrans = glm.inverse(this.ntrans);
		}
	}
	getTrans(): glm.mat4 {
		this.updateTrans();
		return this.trans;
	} 
	getSpace(): glm.mat4 {
		this.updateTrans();
		return this.itrans;
	} 
	getTrival(): glm.mat4 {
		this.updateTrans();
		return this.ntrans;
	}
}

export default abstract class Obj extends ObjBase {
	static gWVP: Uniform = Shader.uniform("mat4", "gWVP");
	static gWorld: Uniform = Shader.uniform("mat4", "gWorld");
	static gV: Uniform = Shader.uniform("mat4", "gV");
	static gWV: Uniform = Shader.uniform("mat4", "gWV");
	static gVP: Uniform = Shader.uniform("mat4", "gVP");
	static gCameraWorldPos: Uniform = Shader.uniform("vec3", "gCameraWorldPos");
	static gTime: Uniform = Shader.uniform("float", "gTime");

	protected defaultShader: Shader = null;

	setBasicUniforms(viewport: Viewport) {
		Obj.gWVP.set(viewport.getTrans()["*"](this.getTrans()));
		Obj.gWorld.set(this.getTrans());
		Obj.gWV.set(viewport.getView()["*"](this.getTrans()));
		Obj.gV.set(viewport.getView());
		Obj.gVP.set(viewport.getTrans());
		Obj.gCameraWorldPos.set(viewport.getCameraPosition());
		Obj.gTime.set(Renderer.time);
	}
	abstract render(viewport: Viewport);
	bindShader() {
		if (this.defaultShader) {
			this.defaultShader.use();
		}
	}
	unbindShader() {
		if (this.defaultShader) {
			this.defaultShader.unuse();
		}
	}
}
