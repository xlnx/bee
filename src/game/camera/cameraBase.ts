import { PerspectiveViewport } from "../../scene/viewport";

abstract class CameraBase extends PerspectiveViewport {
	constructor(left?: number, top?: number, width?: number, height?: number) {
		super(left, top, width, height);
	}

	abstract bind();
	abstract unbind();
}

export {
	CameraBase
}
