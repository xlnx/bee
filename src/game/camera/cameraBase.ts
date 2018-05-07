import { PerspectiveViewport } from "../../gl/viewport";
import { Vessel } from "../vessel/vessel";

abstract class CameraBase extends PerspectiveViewport {
	constructor(left?: number, top?: number, width?: number, height?: number) {
		super(left, top, width, height);
	}

	abstract bind();
	abstract unbind();

	private carrier: Vessel = null;

	tie(carrier: Vessel) {
		if (this.carrier) {
			this.carrier.tie(null);
		}
		this.carrier = carrier;
		if (carrier) {
			carrier.tie(this);
		}
	}
}

class Cmaera extends CameraBase {
	bind() {}
	unbind() {}
}

export {
	CameraBase,
	Cmaera
}
