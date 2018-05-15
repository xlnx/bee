import { PerspectiveViewport } from "../../gl/viewport";
import { Vessel } from "../vessel/vessel";
import { glm } from "../../util/glm"

abstract class CameraBase extends PerspectiveViewport {
	constructor(left?: number, top?: number, width?: number, height?: number) {
		super(left, top, width, height);
	}

	abstract bind();
	abstract unbind();

	protected carrier: Vessel = null;

	tie(carrier: Vessel) {
		if (this.carrier) {
			this.carrier.tie(null);
		}
		this.carrier = carrier;
		if (carrier) {
			this.locate(carrier.position);
			carrier.tie(this);
		}
	}

	abstract locate(pos: glm.vec3);
}

export {
	CameraBase
}
